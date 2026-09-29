///
///
/// @brief A new file
///
/// @file server.h
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 13 ott 2021 
///

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <utility> // for std::move
#include <memory> // for std::make_shared
#include <type_traits> // for std::is_same, std::conditional
#include <iostream>
#include <map>
#include <optional>
// Boost libraries
#include <boost/asio.hpp>
// Fcutils
#include "concurrency/lockable_container.hpp" // for a lockable container.
//
namespace fcutils{
namespace networking{

namespace ba=boost::asio;
namespace baip=ba::ip;

using tcp=baip::tcp;
using udp=baip::udp;
using udp_acceptor=ba::basic_socket_acceptor<udp>;

// A type selector for the acceptor_type. From C++11 we can use the std::conditional!!!!!
// Just return the second template type
template <bool IsUdp, class T> struct acceptor_selector{
	using type=typename T::acceptor;
};
// Specialization for UDP, return the
template<> struct acceptor_selector<false,udp>{
	using type=udp_acceptor;
};

// Primary template
template < class Protocol, bool Echo, template<typename,bool> class Session > class base_server{};

// TCP/IP specialization
template <  bool Echo, template<typename,bool> class Session > class base_server<tcp,Echo,Session>
{
public:
	using protocol_type=tcp;
	using session_type=Session<protocol_type,Echo>;
	using session_ptr_type=std::shared_ptr<session_type>;
	using acceptor_type=typename protocol_type::acceptor;
	using endpoint_type=typename protocol_type::endpoint;
	using socket_type=typename protocol_type::socket;
	using address_type=ba::ip::address;
	using checker_type=typename session_type::checker_type;
	using consumer_type=typename session_type::consumer_type;
	// Maps an endpoint to a session shared pointer and socket pair
	using session_map_type=fcutils::concurrency::lockable_container< std::map, endpoint_type, session_ptr_type >;

	// Accept connection on a local port
	base_server(ba::io_context& io_context, int port, consumer_type & consumer, const checker_type & checker=checker_type{})
		: acceptor_(io_context, endpoint_type(protocol_type::v4(), port)), consumer_{consumer}, checker_{checker}
	{
//		boost::asio::socket_base::reuse_address option;
//		acceptor_.get_option(option);
//		std::cout << " TCP server: Reuse_address option: " << std::boolalpha << option.value() << std::endl;
//
		do_accept();
	}

	void start(){
		// If already started do nothing
		if(started_.load()) return;
		// Start the session if already created.
		for(auto & [ep,session_ptr_]: sessions_ ) session_ptr_->start();
		started_.store(true);
	}

	void stop(){
		// Stop the session if already created.
		for(auto & [ep,session_ptr_]: sessions_ ) session_ptr_->stop();
		started_.store(false);
	}

	bool is_started() const { return started_.load(); }

	template<typename Data> void write(const Data * d, std::size_t length){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, session_ptr_]: sessions_){
			if(session_ptr_) {
				if(session_ptr_->is_started()) session_ptr_->write(d,length);
			}
		}
	}

	template<typename Data> void write(const Data * d, std::size_t length, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( iter->second ) { // Check if the session pointer is different from nullptr
				if( (iter->second)->is_started()) (iter->second)->write(d,length);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, typename socket_type::message_flags flags){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, session_ptr_]: sessions_){
			if(session_ptr_) {
				if(session_ptr_->is_started()) session_ptr_->write(s,flags);
			}
		}
	}
	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, typename socket_type::message_flags flags, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( (iter->second) ) { // Check if the session pointer is different from nullptr
				if( (iter->second)->is_started()) (iter->second)->write(s,flags);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s){
		std::lock_guard< session_map_type > lck{sessions_};
		for( auto & [ep, session_ptr_]: sessions_){
			if(session_ptr_) {
				if(session_ptr_->is_started()) session_ptr_->write(s);
			}
		}
	}

	template< typename ConstBufferSequence> void write( const ConstBufferSequence & s, const endpoint_type & ep){
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			// Endpoint is present in the map
			if( (iter->second) ) { // Check if the session pointer is different from nullptr
				if( (iter->second)->is_started()) (iter->second)->write(s);
			}
		}
	}

	session_ptr_type get_session( const endpoint_type & ep) {
		std::lock_guard< session_map_type > lck{sessions_};
		if(auto iter=sessions_.find(ep); iter!=sessions_.end()){
			return (iter->second);
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
			if(epitr->second){
				// the session has been created so we need to retrive the socket from the session
				return std::optional< std::reference_wrapper<socket_type> >{ (epitr->second)->get_socket() };
			} else {
				// If the session has not been created yet, then use the socket stored in the map
				std::cout << " TCP server::get_socket. Endpoint is not present!! \n";
				return std::nullopt;
			}
		}
		std::cout << "  server::get_socket. Endpoint is not present!! \n";
		return std::nullopt;
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(
				[this](boost::system::error_code ec, socket_type socket)
				{
				if (!ec)
				{
					std::lock_guard< session_map_type > lck{sessions_};
					boost::system::error_code ece;
					auto endpoint = socket.remote_endpoint(ece);
					if (ece)
					{
						std::cout << " TCP server: Error retrieving remote endpoint: " << ece.message() << std::endl;
					} else {
						std::cout << " TCP server: a session just created on endpoint: " << endpoint << std::endl;
					}
					auto [iter,success]=sessions_.insert_or_assign(endpoint,std::make_shared<session_type>(std::move(socket),consumer_,checker_));
					if (success)
					{
						std::cout << " TCP server: a session just created on endpoint: " << endpoint << std::endl;
						// Once created the session check if client is started. If so also the session must be started
						if(started_.load() ) iter->second->start();
					} else {
						std::cout << " TCP server: error creating a session on endpoint: " << endpoint << std::endl;
					}
				}
				std::cout << " TCP server: re-running do_accept ..." << std::endl;
				do_accept();
        });
  }

	acceptor_type acceptor_;
	consumer_type & consumer_;
	checker_type checker_;
	session_ptr_type session_ptr_{nullptr};
	std::atomic_bool started_{false};
	session_map_type sessions_;

};

// UDP specialization
template <  bool Echo, template<typename,bool> class Session > class base_server<udp,Echo,Session>
{
public:
	using protocol_type=udp;
	using session_type=Session<protocol_type,Echo>;
	using session_ptr_type=std::shared_ptr<session_type>;
	using endpoint_type=typename protocol_type::endpoint;
	using socket_type=typename protocol_type::socket;
	using address_type=ba::ip::address;
	using consumer_type=typename session_type::consumer_type;
	using checker_type= typename session_type::checker_type;

	base_server(ba::io_context& io_context, int port, consumer_type & consumer, const checker_type& checker=checker_type{})
    : io_context_(io_context), port_(port), consumer_{consumer}, checker_{checker}
  {
    do_accept();
  }

  void do_accept()
  {
	  io_context_.post(
		  [this](){
			session_ptr_=std::make_shared<session_type>(socket_type(io_context_,endpoint_type(udp::v4(),port_)),consumer_,checker_);
			// Once created the session check if client is started. If so also the session must be started
			if(started_.load() ) session_ptr_->start();
			boost::system::error_code ece;
			auto endpoint = session_ptr_->get_socket().remote_endpoint(ece);
			if (ece)
			{
				std::cout << " UDP server: Error retrieving remote endpoint: " << ece.message() << std::endl;

			} else {
				std::cout << " UDP server: a session just created on endpoint: " << endpoint << std::endl;
			}
	  	  }
	  );
  }

  void start(){
	  // If already started do nothing
	  if(started_.load()) return;
		// Start the session if already created.
		if(session_ptr_) session_ptr_->start();
		started_.store(true);
  }

  void stop(){
	  // Stop the session if already created.
	  if(session_ptr_) session_ptr_->stop();
			started_.store(false);
  	  }

  bool is_started() const { return started_.load(); }

private:
  ba::io_context & io_context_;
  udp::endpoint endpoint_;
  int port_;
  consumer_type & consumer_;
  checker_type checker_;
  session_ptr_type session_ptr_{nullptr};
  std::atomic_bool started_{false};
};


} // namespace networking
} // namespace fcutils



#endif // SERVER_HPP_ 
