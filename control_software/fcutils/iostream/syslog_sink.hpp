///
///
/// @brief A new file
///
/// @file syslog_sink.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 14 giu 2022 
///

#ifndef SYSLOG_SINK_HPP_
#define SYSLOG_SINK_HPP_

#include <iostream>
#include <string>
#include <type_traits>
#include <array>
#include <boost/iostreams/tee.hpp> // tee_stream & tee_device
#include <boost/iostreams/stream.hpp> // iostream
#include <boost/asio.hpp> // UDP & ASIO facilities

#include "fcutils/networking/tcp_connection_tester.hpp"
#include "fcutils/string/syslog_formatter.hpp"

namespace fcutils{
namespace iostream{

// General template
template <class Transport> class syslog_sink {};

// Specialized for udp transport
template <> class syslog_sink<boost::asio::ip::udp>: public boost::iostreams::sink {
public:
	typedef boost::asio::ip::udp transport_type;
	using syslog_formatter_type=fcutils::string::syslog_formatter;

	syslog_sink(const char * ip="127.0.0.1",int port=514): ip_address_(ip), port_(port){};
	std::streamsize write(const char *s, std::streamsize n)
	{
		// Concatenate the output buffers using an array
		std::string temp{syslog_fmt_};
		const std::size_t TERMSIZE{2ul};
		char termination[TERMSIZE]={'\n',0};
		std::array<boost::asio::const_buffer,3> buffers{{{temp.c_str(),temp.size()},{s,static_cast<std::size_t>(n)},{&termination,TERMSIZE}}};
		boost::asio::io_service ios;
		boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string(ip_address_.c_str()), port_);
		boost::asio::ip::udp::socket sock(ios);
		sock.open(boost::asio::ip::udp::v4());
//		sock.send_to( boost::asio::buffer(s, n),ep);
		sock.send_to( buffers,ep);
		sock.close();
		return n;
	}
	const char* get_address() const {return ip_address_.c_str();}
	int get_port() const {return port_;}
	syslog_formatter_type & get_syslog_formatter() { return syslog_fmt_;}
	const syslog_formatter_type & get_syslog_formatter() const { return syslog_fmt_;}
	// In the UDP case the test of the connection is always true.
	bool test_connection() { return true; }
	enum: bool { is_udp=std::is_same_v<transport_type,boost::asio::ip::udp>, is_tcp=std::is_same_v<transport_type,boost::asio::ip::tcp> };

private:
	std::string ip_address_;
	int port_;
	syslog_formatter_type syslog_fmt_;
};


//template <> class net_sink<boost::asio::ip::tcp>: public boost::iostreams::sink {
//public:
//	typedef boost::asio::ip::tcp transport_type;
//	net_sink(const char * ip="127.0.0.1",int port=514): ip_address_(ip), port_(port){};
//	std::streamsize write(const char *s, std::streamsize n)
//	{
//		boost::asio::io_service ios;
//		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip_address_.c_str()), port_);
//		boost::asio::ip::tcp::socket sock(ios);
//		boost::system::error_code ec;
//		std::streamsize nsent{0};
//		sock.connect(ep,ec);
//		if (ec)
//		{
//			// Silently do nothing ....
////			std::cout << " TCP net sink, error connecting the socked on address: "<< ip_address_<<", port: " << port_ << ", error: "<< ec.message() << '\n';
//
//		} else {
//			nsent = sock.send( boost::asio::buffer(s, n));
//		}
//		sock.close();
//		// it must be the same as the write to avoid troubles ...
//		return n;
//	}
//	const char* get_address() const {return ip_address_.c_str();}
//	int get_port() const {return port_;}
//	bool test_connection() { fcutils::networking::tcp_connection_tester t; return t.test_connection(ip_address_.c_str(), port_); }
//	enum: bool { is_udp=std::is_same_v<transport_type,boost::asio::ip::udp>, is_tcp=std::is_same_v<transport_type,boost::asio::ip::tcp> };
//private:
//	std::string ip_address_;
//	int port_;
//};

//typedef net_sink<boost::asio::ip::udp> udp_sink;
//typedef net_sink<boost::asio::ip::tcp> tcp_sink;

using syslog_udp_sink=syslog_sink<boost::asio::ip::udp>;

} // namespace iostream
} // namesapce fcutils




#endif // SYSLOG_SINK_HPP_ 
