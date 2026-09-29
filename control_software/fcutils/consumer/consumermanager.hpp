///
///
/// @brief A new file
///
/// @file guarded_thread.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 20 ott 2021
///

#ifndef CONSUMERMANAGER_HPP_
#define CONSUMERMANAGER_HPP_

#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <forward_list>
#include <future>
#include <thread>
#include <mutex> // For lock_guard
#include <type_traits>
#include <chrono>
#include <iosfwd>
#include <utility> // for std::move and std::forward
#include <functional>

#include "fcutils/concurrency/guarded_thread.hpp" // for joinable thread
#include "fcutils/concurrency/lockable_container.hpp" // For a template lockable container
#include "fcutils/type_traits/type_traits.hpp" // for is_shared_ptr, is_invocable_ovl, has_consume
#include "fcutils/consumer/decode_consumer_command.hpp" // for the consumer command decoder
#include "fcutils/concurrency/sync_queue.hpp"

namespace fcutils{
namespace consumer{


template<typename Data=unsigned char> struct simple_pool{
	using data_type = Data;
	void * allocate(std::size_t l){ return new data_type[l];}
	void deallocate(data_type * pt){ delete [] pt; }
};

template <typename Data, typename ID=uint16_t > struct empty_header_trailer{
	using id_type 	= ID;
	using data_type = Data;
	std::size_t size(){ return 0;}
	void * data(){ return nullptr;}
	void operator()(const Data *, std::size_t ){};
	void consume(const Data *, std::size_t){};
	// No ID present, keep it as a common interface
	void set_id(const id_type & a){};
};

template <typename Data, typename ID=uint16_t > struct id_unixtime_header_trailer{
	using id_type 	= ID;
	using time_type = std::time_t;
	using data_type = Data;
	std::size_t size(){ return SIZE;}
	void * data(){ return &id_utime;}
	void operator()(const Data *d, std::size_t s){
		id_utime.utime=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() );
	};
	void consume(const Data * d, std::size_t s){
		operator()(d,s);
	};
	void set_id(const id_type & id){ id_utime.id=id; }
	struct id_and_time {
		// founding memories
		id_type id{static_cast<id_type>(0xFAB1U)};
		time_type utime{};
	} id_utime;
	enum : std::size_t {SIZE=sizeof(id_and_time)};
};

template <typename Data, typename ID=uint16_t, typename MsgSize=uint16_t  > struct id_size_unixtime_header_trailer{
	using id_type 		= ID;
	using msg_size_type= MsgSize;
	using time_type 	= std::time_t;
	using data_type 	= Data;
	std::size_t size(){ return SIZE;}
	void * data(){ return &id_size_utime;}
	void operator()(const Data *d, std::size_t s){
		id_size_utime.utime=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() );
		id_size_utime.msg_size=static_cast<msg_size_type>(s);
	};
	void consume(const Data * d, std::size_t s){
		operator()(d,s);
	};
	void set_id(const id_type & id){ id_size_utime.id=id; }
	struct id_size_time {
		// founding memories
		id_type id{static_cast<id_type>(0xFAB1U)};
		msg_size_type msg_size{};
		time_type utime{};
	} id_size_utime;
	enum : std::size_t {SIZE=sizeof(id_size_time)};
	enum : msg_size_type {UTSIZE=sizeof(time_type)};
};


template <typename Consumer, template <typename> class Pool=simple_pool, template <typename> class Header=empty_header_trailer, template <typename> class Trailer=empty_header_trailer,
		typename Key=std::string >
class base_consumermanager{
public:
	enum class PSTATE: char {STOP,DATA_CONSUMED,WAIT};
	using self_type				= base_consumermanager<Consumer,Pool,Header, Trailer, Key>;
	using key_type				= Key;
	using consumer_opts_type	= std::string;
	using consumer_type			= Consumer;
	using consumer_ptr_type		= std::shared_ptr<consumer_type>;
	using consumer_map_type		= fcutils::concurrency::lockable_container<std::map,key_type,consumer_ptr_type>;
	using data_type 			= typename consumer_type::data_type;
	using data_ptr_type			= typename consumer_type::data_ptr_type;
	using pool_type				= Pool<data_type>;
	using thread_type			= fcutils::concurrency::joinable_guarded_thread;
	// NOTE!!!! Here you need a pointer to the abstract class: consumer_type, because consume is a pure virtual method
//	using consume_return_type	= typename std::invoke_result<decltype(&consumer_type::consume), consumer_type *,const data_type *,std::size_t>::type;
	using consume_return_type	= void;
	using future_type			= std::future<consume_return_type>;
	using promise_type			= std::promise<PSTATE>;
	using future_list_type		= fcutils::concurrency::lockable_container<std::forward_list,std::future<int>>;
	using header_type			= Header<data_type>;
	using trailer_type			= Trailer<data_type>;
	using buffer_type 			= std::pair<data_type *,std::size_t>;
	using sync_queue_type		= fcutils::concurrency::sync_queue<PSTATE>;

	// Check if the header and trailer do have an operator() or a consume member function
	static_assert( (fcutils::type_traits::is_invocable_ovl_v<header_type,data_type> && fcutils::type_traits::has_consume_v<header_type,data_type>),
			" fcutils::consumer::consumermanager. The header type do not have operator() or consume member functions ");
	static_assert( (fcutils::type_traits::is_invocable_ovl_v<trailer_type,data_type> && fcutils::type_traits::has_consume_v<trailer_type,data_type>),
			" fcutils::consumer::consumermanager. The trailer type do not have operator() or consume member functions ");
	// Default creator
	base_consumermanager(): pool_{},
//			check_{thread_type(&self_type::check_futures,this)},
			header_{}, trailer_{} {};
	// Creator for the pool
	template <typename ... Par > base_consumermanager(Par && ... pars): pool_{std::forward<Par>(pars) ... } //, check_{thread_type(&self_type::check_futures,this)}
	{};
	// Is a base class, so get rid of copy operations
	base_consumermanager(const base_consumermanager & c) = delete;
	auto & operator=(const base_consumermanager & c) = delete;
	// To be done ... move operator ... if any,

	// Remember that the manager could not own the consumers. Let's relay on the sharable_pointer to delete them, in case.
	virtual ~base_consumermanager(){
		// We need to be sure that all the consumer threads are over before we delete them
		// thanks to RAII in the joinable_guarded_thread, all threads are joined upon this class deletion
		// but check_queue_ wil be deleted before execution of the thread ... so we need to join the thread
		// Comunicate to the check_futures that the state is STOP
		check_queue_.put(PSTATE::STOP);
		// Join to avoid to delete the check_queue_ before the thread ....
		if(check_.joinable()) check_.join();
        // Do some clean up of the pool?
	};

	template<typename Ptr=consumer_ptr_type> self_type & add_consumer(const key_type & id, Ptr && c){
		// Lock the map to avoid data races
		std::lock_guard< consumer_map_type > lck{consumermap_};
		// Note if a consumer is already present with that ID it will be deleted ...
		consumermap_[id]=std::forward<consumer_ptr_type>(c);
		return *this;
	}

	template<typename Node=typename consumer_map_type::node_type>
	self_type & add_consumer(Node && n){
		// Check if the pointer is not null
		if(n.second) {
			// Lock the map to avoid data races
			std::lock_guard< consumer_map_type > lck{consumermap_};
			// Note if a consumer is already present with that ID it will be deleted ...
			consumermap_.insert(std::forward<Node>(n));
		}
		return *this;
	}

	template <typename Decoder > self_type & add_consumer(Decoder && decoder, const std::string & s){
		// Add a consumer decoding a string according to a decoder object
		// The decoder must decode a std::string reference and implement an operator() returning a node type
		Decoder dec{std::forward<Decoder>(decoder)};
		return add_consumer(dec(s,*this));
	}

	// Remember. Never overload an universal reference ... Ptr is overloaded in operator()(const char *d, std::size_t ) ....
	// We need to setup tricks in case the overload is needed ....
	template<typename Ptr=consumer_ptr_type, typename = std::enable_if_t<
			!std::is_integral<std::remove_reference_t<Ptr>>::value
			> >
	self_type & operator()(const key_type & id, Ptr && c){
		return add_consumer(id, std::forward<consumer_ptr_type>(c));
	}

	template<typename Node=typename consumer_map_type::node_type>
	self_type & operator()(Node && n){
		return add_consumer(std::forward<Node>(n));
	}

	template <typename Decoder > self_type & operator()(Decoder && decoder, const std::string & s){
		// Add a consumer decoding a string according to a decoder object
		// The decoder must decode a std::string reference and implement an operator() returning a node type
		return add_consumer(std::forward<Decoder>(decoder),s);
	}

	// Remove a consumer with identifier id
	bool remove_consumer(const key_type & id){
		// Erase the object with key id
		std::lock_guard< consumer_map_type > lck{consumermap_};
		return (consumermap_.erase(id))==0?false:true;
	}

	// Consume data on all consumers
//	template <typename Data>
	bool consume(const  data_type * c, const std::size_t & length) {
		bool executed{false};
		// reset the buffer pointer
		buffer_.first=nullptr;
		buffer_.second=length+header_.size()+trailer_.size();
		buffer_.first=static_cast< data_type *>(pool_.allocate(buffer_.second));
		if(buffer_.first){
			prepare_header(c,length);
			prepare_trailer(c,length);
			// Copy the data into the buffer
			copy_buffer(c,length);
			// Consume the data
			{
				std::lock_guard< consumer_map_type > lck{consumermap_};
				for( auto & [id, consumer_ptr]: consumermap_){
					consumer_ptr->consume(buffer_.first,buffer_.second);
				}
			}
			// Deallocate the buffer
			pool_.deallocate(buffer_.first);
			// reset the buffer
			buffer_.first=nullptr;
			executed=true;
		}
		return executed;
	}

//	// Consume data asynchronously on all consumers
	bool consume_async(const data_type * c, const size_t & length) {
		bool executed{false};
		// Start the thread checking for the futures in the consume threads
		if(!check_.joinable()) check_= thread_type(&self_type::check_futures,this);
		// reset the buffer pointer
		buffer_.first=nullptr;
		buffer_.second=length+header_.size()+trailer_.size();
		buffer_.first=static_cast<data_type*>(pool_.allocate(buffer_.second));
		if(buffer_.first){
			// Prepare header and trailer
			prepare_header(c,length);
			prepare_trailer(c,length);
			// Copy the data into the buffer
			copy_buffer(c,length);
			// Async consume
			std::forward_list<future_type> local_futures;
			{
				std::lock_guard< consumer_map_type > lck{consumermap_};
				// Consume data and store futures in a local map
				for( auto & [id, consumer_ptr]: consumermap_){
	                local_futures.push_front(
	                		std::async(std::launch::async,
	                				[consumer_ptr, data=buffer_.first, length=buffer_.second]() mutable {
	                					consumer_ptr->consume(data,length);
	                				} // closing lambda
	                		) // std::async
					); // push_front
				} // for
			}
			{
				std::lock_guard< future_list_type > lck{futures_};
				// Check when the consume tasks are over, waiting for the future
		        futures_.push_front(
		        		std::async(std::launch::async,
		        				[ftlist=std::move(local_futures),&pool=pool_,&mq=check_queue_, data=buffer_.first] () mutable {
		        				int i{0};
		        				// Get the future from the async consume threads
		        				for(auto & ft: ftlist){
		        					++i;
		        					ft.get();
		        				}
		        				// Release memory when all futures are get
		        				pool.deallocate(data);
		        				// Comunicate to the check_futures that the all data are consumed
		        				mq.put(PSTATE::DATA_CONSUMED);
		        				return i;
		        		} // closing lambda
		        ) // std::async
				); // push_front
			}
			executed = true;
		} // if(executed)
		return executed;
	}

	std::size_t size(){
		std::lock_guard< consumer_map_type > lck{consumermap_};
		return consumermap_.size();
	}

	void reset(){
		std::lock_guard< consumer_map_type > lck{consumermap_};
		for( auto & [id, consumer_ptr]: consumermap_){
			consumer_ptr->reset();
		}
	}

	void reset(const key_type & id){
		std::lock_guard< consumer_map_type > lck{consumermap_};
		typename consumer_map_type::iterator i=consumermap_.find(id);
		if( i!=consumermap_.end() ) {
			i->second->reset();
		}
	}

	bool operator()(const data_type* c, const size_t & length){ return consume_async(c,length); }

	auto & operator[]( const key_type & id){
		std::lock_guard< consumer_map_type > lck{consumermap_};
		return consumermap_[id];
	}
	auto & at(const key_type & id) {
		std::lock_guard< consumer_map_type > lck{consumermap_};
		return consumermap_.at(id);
	}

	void print_stats(std::ostream & os=std::cout) const {
		 os << " Stats for consumermanager: \n";
		 {
			 std::lock_guard< consumer_map_type > lck{consumermap_};
			 for ( const auto & [id, consumer_ptr]: consumermap_){
				 consumer_ptr->print_stats(os,id);
			 }
		 }
		 os << std::endl;
	 }

	auto & get_header(){ return header_; }
	auto & get_trailer(){ return trailer_; }
	const auto & get_header() const { return header_; }
	const auto & get_trailer() const { return trailer_; }

protected:
	// Prepare the header in case is needed
	// The header has an operator()
	// Remember, decltype cannot resolve overloding. So you must cast the pointer to a member function, to the right type.
//	template < typename H=header_type,typename std::enable_if< std::is_invocable_v<decltype(static_cast<void (H::*)(const data_type *, std::size_t)>(&H::operator())),H*, data_type *, std::size_t> >::type * =nullptr>
	template < typename H=header_type,typename std::enable_if< fcutils::type_traits::is_invocable_ovl_v<H,data_type> >::type * =nullptr>
	void prepare_header(const data_type * c, std::size_t length){
		header_(c,length);
	}
	// The header has a member function consume, but do not have an operator()
	template < typename H=header_type,
			typename std::enable_if<
			( !fcutils::type_traits::is_invocable_ovl_v<H,data_type> && fcutils::type_traits::has_consume_v<H,data_type>)
			>::type * =nullptr>
	void prepare_header(const data_type * c, std::size_t length){
			header_.consume(c,length);
	}
//
	template < typename H=trailer_type,typename std::enable_if< fcutils::type_traits::is_invocable_ovl_v<H,data_type> >::type * =nullptr>
	void prepare_trailer(const data_type * c, std::size_t length){
		trailer_(c,length);
	}
	// The trailer has a member function consume, but do not have an operator()
	template < typename H=trailer_type,
			typename std::enable_if<
			( !fcutils::type_traits::is_invocable_ovl_v<H,data_type> && fcutils::type_traits::has_consume_v<H,data_type> )
			>::type * =nullptr>
	void prepare_trailer(const data_type * c, std::size_t length){
			trailer_.consume(c,length);
	}

	template <typename Data> void copy_buffer(const Data* c, const std::size_t & length){
		// Start copying the header
		auto hptr=static_cast< data_type *>(header_.data());
		 data_type * last{nullptr};
		// if the  data_type * is a shared_ptr, then use the get method, otherwise, just use the "naked" pointer
		if constexpr (fcutils::type_traits::is_shared_ptr< data_type *>::value) {
			last= std::copy(hptr,hptr+header_.size(),buffer_.first.get() );
			last = std::copy(c.get(),c.get()+length,last );
		} else {
			last= std::copy(hptr,hptr+header_.size(),buffer_.first );
			last = std::copy(c,c+length,last );
		}
		// Now the trailer
		auto tptr=static_cast<data_type *>(trailer_.data());
		std::copy(tptr,tptr+trailer_.size(),last );
	}

	int check_futures(){
		// Loop for ever until stop
		while(true){
			// Check if there is a message in the check_future queue
			typename sync_queue_type::message_type m{PSTATE::WAIT};
			// Wait until a message is pushed into the queue
			check_queue_.get(m);
			// reset the future
			int futures_cnt{0};
			switch(m){
			case PSTATE::STOP:
				{
					std::lock_guard< future_list_type > lck{futures_};
					// Waits for all futures to finish
					for(auto & ft: futures_){
						// Check the future shared state, we do not know if the future has been set
						if(ft.valid()) futures_cnt+=ft.get();
					}
					// Clean the list of futures
					futures_.clear();
				}
				return futures_cnt;
			case PSTATE::DATA_CONSUMED:
				{
					std::lock_guard< future_list_type > lck{futures_};
					// Waits for all futures to finish. Here the futures are supposed to be valid ... but we check anyway !
					for(auto & ft: futures_){
						if(ft.valid()) futures_cnt+=ft.get();
					}
					// Clean the list of futures
					futures_.clear();
				}
				break;
			/// Opps wrong state !!!!
			default:
				return -1;
			} //	 switch(m)
		} // while (true)
	}

	mutable consumer_map_type consumermap_;
	pool_type pool_;
	buffer_type buffer_;
	future_list_type futures_;
	typename std::result_of<decltype(&promise_type::get_future)(promise_type)>::type check_futures_;
	thread_type check_;
	header_type header_;
	trailer_type trailer_;
	sync_queue_type check_queue_;
};

} // namespace consumer
} // namespace fcutils

#endif /* CONSUMERMANAGER_HPP_ */
