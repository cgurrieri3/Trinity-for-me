///
///
/// @brief A new file
///
/// @file client.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 26 nov 2021
///

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <utility> // for std::move
#include <memory> // for std::make_shared, std::shared_ptr
#include <type_traits> // for std::is_same, std::conditional
#include <map> // for std::map, std::pair
#include <mutex> // for std::mutex, std::lock
#include <atomic> // for std::atomic
#include <optional> // for std::optional
#include <functional> // for std::reference_wrapper
#include <iostream>

#include <boost/asio.hpp>

namespace fcutils{
namespace networking{

namespace ba=boost::asio;
namespace baip=ba::ip;

using tcp=baip::tcp;
using udp=baip::udp;

// A lockable map class.
// It is just a map with a mutex added.
// Operation on mutex are forced through the public class interface.
// Just the map contructors are inherited
template <typename Key, typename Value> class lockable_map: public std::map<Key,Value>{
private:
        std::mutex mtx_;
public:
        // Inheriting constructors since C++11
        using std::map<Key,Value>::map;
        using std::map<Key,Value>::operator=;
        void lock(){mtx_.lock();}
        void unlock(){ mtx_.unlock();}
        auto try_lock(){ return mtx_.try_lock();}
};

const int WATCHDOGWAITTIME{2};

using reuse_port=boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>;

template < class Protocol, bool Echo, template<typename,bool> class Session, bool Debug=false > class base_client
{
public:
	using protocol_type=Protocol;
	using session_type=Session<protocol_type,Echo>;
	using session_ptr_type=std::shared_ptr<session_type>;
	using endpoint_type=typename protocol_type::endpoint;
	using socket_type=typename protocol_type::socket;
	using address_type=ba::ip::address;
	using port_type=ba::ip::port_type;
	using boost_error_type=boost::system::error_code;
	using timer_type=ba::steady_timer;
	// Consumer and checker are stored in the session
	using consumer_type=typename session_type::consumer_type;
	using checker_type=typename session_type::checker_type;
	// Maps an endpoint to a session shared pointer and socket pair
	using session_map_type=lockable_map< endpoint_type,std::pair<session_ptr_type,socket_type> >;

	// Just create the client
	// A consumer is needed as well as a checker.
	// The consumer is presumably bound to an external object.
	//
	base_client(ba::io_context& io_context, consumer_type & consumer, const checker_type & checker=checker_type{}, bool sessionid=false )
		: io_{io_context},started_{false}, watchdog_{io_}, checker_{checker}, consumer_{consumer}, sessionid_{sessionid}
	{ }

	void start(){
		// If already started do nothing
		if(started_.load(std::memory_order_consume)) return;
		// Start all the session that are already created and connected
		for( auto & [ep, value]: sessions_){
			// If a session pointer is different from zero
			if(value.first) {
				// start the session
				value.first->start();
			}
		}
		started_.store(true,std::memory_order_release);
	}

	void stop(){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, value]: sessions_){
			if(value.first) {
				value.first->stop();
			}
		}
		started_.store(false,std::memory_order_release);
	}

	bool is_started() const { return started_.load(std::memory_order_consume); }

	template<typename Data> void write(const Data * d, std::size_t length){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, value]: sessions_){
			if(value.first) {
				if(value.first->is_started()) value.first->write(d,length);
			}
		}
	}

	template<typename Data> void write(const Data * d, std::size_t length, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( (iter->second).first ) { // Check if the session pointer is different from nullptr
				if( (iter->second).first->is_started()) (iter->second).first->write(d,length);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, typename socket_type::message_flags flags){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, value]: sessions_){
			if(value.first) {
				if(value.first->is_started()) value.first->write(s,flags);
			}
		}
	}
	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, typename socket_type::message_flags flags, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( (iter->second).first ) { // Check if the session pointer is different from nullptr
				if( (iter->second).first->is_started()) (iter->second).first->write(s,flags);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, value]: sessions_){
			if(value.first) {
				if(value.first->is_started()) value.first->write(s);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( (iter->second).first ) { // Check if the session pointer is different from nullptr
				if( (iter->second).first->is_started()) (iter->second).first->write(s);
			}
		}
	}

	// Simply connect to an endpoint, if is available, write and read with a timeout of d seconds
	// Please note this method will not use the session map. So a new session will be created and deleted soon after the read
	template< typename ConstBufferSequence, typename Duration=std::chrono::seconds>
	void write_read_until( const ConstBufferSequence & s, const endpoint_type & ep, const Duration & d=std::chrono::seconds(2)){
		// Is implemented only for the TCP, at the moment
		if constexpr(std::is_same<protocol_type,tcp>::value) {
			// First create the socket
			socket_type socket{io_};
			// Then try to connect
			boost::system::error_code ec;
			socket.connect(ep, ec);
			if (ec)
			{
				// An error occurred.
				std::cout << " base_client::write_read_until. Error in connecting endpoint: " << ep << ", " <<ec.message() << std::endl;
				return;
			}
			// Here connection was sucessfull let's write data to the socket
			boost::asio::write(socket,s,ec);
			if (ec)
			{
				// An error occurred.
				std::cout << " base_client::write_read_until. Error in writing to the endpoint !!!! " << ep << ", " << ec.message() << std::endl;
				return;
			}
			// create a watchdog copying the one used by the client
			std::atomic_bool done{false};
			timer_type watchdog{io_};
			// Re launch the watchdog ...
			watchdog.expires_after(d);
			watchdog.async_wait([&socket,&done](const boost_error_type & ec){
				if(!ec){
					// No error. Check if the read operation  and stop it, in case
					if(done.load(std::memory_order_consume)) {
						// here the read operation was done, just return
						return;
					} else {
						// Watchdog stop
						std::cout << " base_client::write_read_until. Read operation stopped after the wait time. socket will be closed ..." << std::endl;
						// Shudtown the socket and close it.
						boost::system::error_code ec2;
						socket.shutdown(socket_type::shutdown_both,ec2);
						if(ec2) std::cout << " base_client::write_read_until. Watchdog: Error shutting down socket: " << ec2.message() << std::endl;
						socket.close(ec2);
						if(ec2) std::cout << " base_client::write_read_until. Watchdog: Error closing socket: " << ec2.message() << std::endl;
					} // done
				} // ec
			} // lambda
			);
			// Start the read operation. It will block until read is over or the watchdog will close it
			// Create the read buffer
			boost::asio::streambuf buffer;
			// Read until \n
			auto length=boost::asio::read_until(socket,buffer,'\n',ec);
			if (ec)
			{
				// An error occurred.
				std::cout << " base_client::write_read_until. Error in reading from the endpoint !!!! " << ep << ", " << ec.message() << std::endl;
				return;
			}
			// Set the done flag. This is needed by the watchdog.
			done.store(true,std::memory_order_consume);
			// Consume the message
			std::string msg{
			  boost::asio::buffers_begin(buffer.data()),
			  boost::asio::buffers_begin(buffer.data()) + length -sizeof(char)
			};
			consumer_(msg.c_str(),msg.size());
			// Here the read_until was sucessfully completed. So let's shutdown and close the socket
			socket.shutdown(socket_type::shutdown_both,ec);
			if(ec) std::cout << " base_client::write_read_until. Error shutting down socket: " << ec.message() << std::endl;
			socket.close(ec);
			if(ec) std::cout << " base_client::write_read_until. Error closing socket: " << ec.message() << std::endl;
		} else {
			std::cout << " base_client::write_Read_until. Is not implemented for UDP transport type." << std::endl;
		}

	} // Write_read_until

	session_ptr_type get_session( const endpoint_type & ep) {
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			return (iter->second).first;
		}
		return nullptr;
	}

	std::optional< std::reference_wrapper<socket_type> >  get_socket( const endpoint_type & ep){
		// Check if is already present in the map
		std::lock_guard< session_map_type > lck{sessions_};
		// Find the endpoint
		auto epitr=sessions_.find(ep);
		if(epitr!=sessions_.end()){
			// Ep is present, check if the session has been created or not
			// Check if a shared_ptr is set
			if((epitr->second).first){
				// the session has been created so we need to retrive the socket from the session
				return std::optional< std::reference_wrapper<socket_type> >{ (epitr->second).first->get_socket() };
			} else {
				// If the session has not been created yet, then use the socket stored in the map
				return std::optional< std::reference_wrapper<socket_type> >{ (epitr->second).second };
			}
		}
		std::cout << "  base_client::get_socket. Endpoint is not present!! \n";
		return std::nullopt;
	}

	bool is_session_started ( const endpoint_type & ep) {
		if(!is_started()) return false;
		std::lock_guard< session_map_type > lck{sessions_};
		bool started{false};
		if(const auto iter=sessions_.find(ep); iter!=sessions_.end()){
			if((iter->second).first){
				started=(iter->second).first->is_started();
			}
		}
		return started;
	}

	// Add a new endpoint to the map and try connect
	// If connection is sucessfull start the session
	template <typename ... Eppars>  void async_connect(Eppars && ... pars){
		// first create the endpoint
		endpoint_type ep{std::forward<Eppars>(pars) ...};
		{
			// Check if is already present in the map
			std::lock_guard< session_map_type > lck{sessions_};
			auto epitr=sessions_.find(ep);
			if(epitr!=sessions_.end()){
				// Ep is already saved, check if a session already started and the socket status
				// Check if a shared_ptr is set
				if((epitr->second).first){
					// Check if the session is started or not
					if(!( (epitr->second).first->is_started()) ) {
						// if not started, let's suppose that the problem is the socket connection an try to reconnect
						try_connect((epitr->second).second, ep);
					}
				}
			} else {
				// Ep is new, start connection attempts and create the socket, setting the relevant options
				// Start saving the socket in the map, no session yet until connection is successful
				socket_type socket{io_};
				auto [iter,success]=sessions_.insert({std::move(ep),std::make_pair(nullptr,std::move(socket))});
				if(! success){
					std::cout << " base_client::async_connect. Error in adding endpoint !!!! \n";
					return;
				} // if(!success)
				// Try connect the socket to the endpoint
				// The endpoint is copied because we cannot insure the map is not changing ... the end point will be propagated and used as a key, so a value semantic is preferable
				try_connect((iter->second).second, iter->first);
			} // if (epitr)
		}
		// Start a watchdog in any case.
		watchdog();
	}

	// Add a new endpoint to the map, bind it to a socket and create the session.
	// to be used with protocol not supporting the connect operation on socket
	template <typename ... Eppars>  void bind(Eppars && ... pars){
		// first create the endpoint
		endpoint_type ep{std::forward<Eppars>(pars) ...};
		// Add the endpoint to the map
		auto iter=add_endpoint(std::move(ep));
		// Check if an entry is already present
		if((iter->second).first){
			// Check if the session is started or not
			if(( (iter->second).first->is_started()) ) {
				std::cout << " base_client::bind. Section already started ... nothing is done \n";
			} else {
				std::cout << " base_client::bind. Section is stopped ... nothing is done \n";
			}
		} else {
			// Here the section is not created, so we open the socked, bind it to the endpoint and start it in case
			if(!(iter->second).second.is_open()) {
				if constexpr(Debug) {
					std::cout << " base_client::bind. Socket is not open, let's open it ...\n";
				}
				// In case of asio tcp and udp class use the v4 static function
				if constexpr(std::is_same<protocol_type,udp>::value || std::is_same<protocol_type,tcp>::value) {
					(iter->second).second.open(protocol_type::v4());
				} else {
					(iter->second).second.open(protocol_type{});
				}
			}
			// Bind it to the endpoint.
			(iter->second).second.bind(iter->first);
			// Accept the endpoint, creating a new session and start it, in case.
			do_accept(ep);
		}

	}

	template <typename Option> void set_option(const endpoint_type & ep, Option && opt){
		auto socket=get_socket(ep);
		if(socket) (socket.value().get()).set_option(std::forward<Option>(opt));
	}

	template <typename Option> void set_option(const endpoint_type & ep, Option && opt, boost::system::error_code & ec){
		auto socket=get_socket(ep);
		if(socket) (socket.value().get()).set_option(std::forward<Option>(opt),ec);
	}

//	template <typename ... Opts> void set_option(const endpoint_type & ep, Opts && ... opts){
//		auto & socket=get_socket(ep);
//		// A C++17 feature,
//		(..., socket.set_option( std::forward<Opts>(opts) ));
//	}

//	template <typename Option> boost::system::error_code set_option(const endpoint_type & ep, Option && opt, boost::system::error_code & ec)
//	{
//		auto & socket=get_socket(ep);
//		return socket.set_option( std::forward<Option>(opt),ec);
//	}

	template <typename Option> void get_option(const endpoint_type & ep, Option && opt){
		auto socket=get_socket(ep);
		if(socket) socket.value.get_option( std::forward<Option>(opt) );
	}

	template <typename Option> boost::system::error_code get_option(const endpoint_type & ep, Option && opt, boost::system::error_code & ec)
	{
		auto socket=get_socket(ep);
		if(socket) return socket.value().get_option( std::forward<Option>(opt),ec);
		return ec;
	}

private:
	auto add_endpoint(endpoint_type ep){
		// Check if is already present in the map
		std::lock_guard< session_map_type > lck{sessions_};
		auto epitr=sessions_.find(ep);
		if(epitr!=sessions_.end()){
			return epitr;
		} else {
			// Ep is new, start connection attempts and create the socket, setting the relevant options
			// Start saving the socket in the map, no session yet until connection is successful
			socket_type socket{io_};
			auto [iter,success]=sessions_.insert({std::move(ep),std::make_pair(nullptr,std::move(socket))});
			if(! success){
				std::cout << " base_client::add_endpoint. Error in adding endpoint !!!! \n";
			} // if(!success)
			return iter;
		} // if (epitr)
	}

//	void set_socket_options( socket_type & s){
//	// Options activated if protocol type is a network one, i.e. either TCP or UDP
//		if constexpr ( std::is_same<protocol_type,udp>::value || std::is_same<protocol_type,tcp>::value ) {
//			if(!s.is_open()) s.open(protocol_type::v4());
//			// Set reuse address option to insure more than a process listening on the socket
//			boost::asio::socket_base::reuse_address ra{true};
//			reuse_port rp{true};
//			s.set_option(ra);
//			s.set_option(rp);
//			// Check if set is successful
//			s.get_option(ra);
//			s.get_option(rp);
//			std::cout << " Reuse_address option: " << std::boolalpha << ra.value() << ", reuse_port option: " << std::boolalpha << rp.value() << '\n';
//		}
//	}

	// End point is passed by value to insure the reference is not changing because of some asynchronous map insertions.
	void try_connect(socket_type & s, endpoint_type ep ){
		// try connecting the socket
		s.async_connect(ep,[this, ep_=ep](boost_error_type ec){ on_connect(ep_,ec);});
	}

	// End point is passed by value to insure the reference is not changing because of some asynchronous map insertions.
	void on_connect(endpoint_type ep, boost_error_type ec){
		if(!ec){
			if constexpr(Debug){
				std::cout << " base_client::on_connect. Socket connected  ...\n";
			}
			do_accept(ep);
		} else {
			if constexpr(Debug){
				std::cout << " base_client::on_connect. Socket not connected  ... "<< ec.message() << '\n'
						<< "\t Waiting 5 seconds for a new attempt \n";
			}
			io_.post([this,ep_=ep](){
				// Set an expiry time relative to now.
				timer_type timer(io_);
				timer.expires_after(std::chrono::seconds(5));
				timer.wait();
				// Retry connect
				{
					std::lock_guard< session_map_type > lck{sessions_};
					auto & [session_ptr,socket]=sessions_.at(ep_);
					try_connect(socket,ep_);
				}
			}
			);
		}
	}

	void do_accept(endpoint_type & ep)
	{
		// post to the context an instantiation of the session with the created socket.
		// Socket is moved to the session
		// I am here because the socket has been sucessfull connected to the end point.
		// Let's extract it from the map and move into a new session.
//		std::cout << " Posting a Session ... on endpoint: " << ep << std::endl;
		io_.post(
			// Capture list with inizialization ! C++14 feature. We can move object now !!!
			[this, ep_=ep]() {
				std::lock_guard< session_map_type > lck{sessions_};
				auto & [session_ptr,socket]=sessions_.at(ep_);
				// Delete the previous session and create a new one
				session_ptr=std::make_shared<session_type>(std::move(socket),consumer_,checker_,sessionid_);
				std::cout << " base_client::do_accept. A session just created on endpoint: " << ep_ << std::endl;
				// Once createdbase_lclient  the session check if client is started. If so also the session must be started
				if(started_.load(std::memory_order_consume) ) session_ptr->start();
			}
		);
//		std::cout << " Session posted ... on endpoint " << ep << std::endl;
  }

	// Check if any session has been stopped and retry connection
	void watchdog(){
		std::lock_guard< session_map_type > lck{sessions_};
		if(sessions_.empty()) {
			std::cout << " base_client::watchdog. Empty session map ... \n";
		} else {
			// Retrive session pointer and socket pair
			for( auto & [ep, value]: sessions_){
			// Check if a shared_ptr is set
				if(value.first){
					// Check if the session is started or not
					if(!(value.first->is_started())) {
						// Is not, we create a new socket, and set the options,
						// then we reassign the session shared ptr and move the socket in the map
						// After that we retry we retry to connect
						socket_type socket{io_};
						auto [iter,success]=sessions_.insert_or_assign(ep,std::make_pair(nullptr,std::move(socket)));
						if(success){
							std::cout << " base_client::watchdog. Opssss an insertion was done instead of an assignment!! Error in assigning endpoint !!!! \n";
						}
						std::cout << " base_client::watchdog. An old session deleted and a new socket created, start trying to reconnect ..." << std::endl;
						// if not started, let's suppose that the problem is the socket connection an try to reconnect
						try_connect((iter->second).second, ep);
					}
				}
			}

		}
		// Re launch the watchdog ... waiting WATCHDOGWAITTIME seconds.
		watchdog_.expires_after(std::chrono::seconds(WATCHDOGWAITTIME));
		watchdog_.async_wait([this](const boost_error_type & ec){
			if(!ec){
				watchdog();
			} else {
				std::cout << " base_client::watchdog. Error waiting for the watchdog: " << ec.message() << std::endl;
			}
		});

	}

	ba::io_context & io_;
	session_map_type sessions_;
	std::atomic_bool started_{false};
	timer_type watchdog_;
	checker_type checker_;
	consumer_type & consumer_;
	bool sessionid_{false};
}; // class basic_client


} // namespace networking
} // namespace fcutils


#endif // CLIENT_HPP_ 
