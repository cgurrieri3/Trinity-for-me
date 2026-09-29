///
///
/// @brief A new file
///
/// @file test_gcc_comm_client.cpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 21 lug 2022 
///
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <utility>
#include <fstream>

//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/ssl.hpp>
//#include <boost/beast/version.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "fcutils/consumer/print_consumer.hpp"
#include "fcutils/monitor/monitor.hpp"
#include "fcutils/consumer/monitor_consumer.hpp"
#include "fcutils/property_tree/property_tree.hpp"

namespace ba = boost::asio;
namespace po = boost::program_options;
namespace pt = boost::property_tree;

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace ssl = ba::ssl;       // from <boost/asio/ssl.hpp>
using tcp = ba::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// A quick and dirty struct to keep all you need in an object
struct http_io{
    beast::tcp_stream stream;
    http::request<http::string_body> req;
    http::response<http::dynamic_body> res{};
};

// Set SNI Hostname (many hosts need this to handshake successfully)
//void set_host_name(http_io & io_object, std::string & address){
//	if(! SSL_set_tlsext_host_name(io_object.stream.native_handle(), address.c_str()))
//	{
//		beast::error_code ec{static_cast<int>(::ERR_get_error()), ba::error::get_ssl_category()};
//		throw beast::system_error{ec};
//	}
//}
// Perform some header set up to prepare the message on the GCC
void prepare_header(http_io & io_object, std::string & address, std::string & auth64e){
	io_object.req.set(http::field::host, address);
	io_object.req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	// Set the username and password
	io_object.req.set(http::field::authorization, "Basic "+auth64e);
	// Set keep alive to allow multiple connection
	io_object.req.keep_alive(true);
	// In case of POST
	io_object.req.set(http::field::content_type,"application/x-www-form-urlencoded");
}

template <typename Resolver> void connect_write_read(http_io & io_object, const Resolver & results, std::string cmd=""){
    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(io_object.stream).connect(results);
	if(!cmd.empty()) io_object.req.body()=cmd.c_str();
	// Prepare the request payload
	io_object.req.prepare_payload();
    // Send the HTTP request to the remote host
    http::write(io_object.stream, io_object.req);
    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;
    // Receive the HTTP response
    http::read(io_object.stream, buffer, io_object.res);
}

// The screen to be used for the state
const bool DEBUG{false};
template <typename Data> using screen_type = fcutils::monitor::screen<fcutils::monitor::value, Data>;
template <typename Data> using monitor_consumer_type = fcutils::consumer::monitor_consumer<screen_type, pt::ptree, Data, fcutils::checker::true_checker, DEBUG>;

// Performs an HTTP GET and prints the response
int main(int ac, char** av)
{
    try
    {
    	std::string address,dbase,port,auth,auth64e, target;
    	bool cmdmode;
    	int version,wait;
    	using data_type=char;
    	// start adding options
        po::options_description desc("test_gcc_comm_client options ");
        desc.add_options()
            ("help,h", "An help message")
            ("host,o", po::value<std::string>(&address)->default_value("192.168.1.1"), "the IP address to talk to")
            ("port,p", po::value<std::string>(&port)->default_value("8086"), "the port to connect to [8086]")
//			("mode,m", po::value<std::string>(&mode)->default_value("write"), "the ")
			("dbase,d", po::value<std::string>(&dbase)->default_value("fctest"), " the database name [fctest]")
			("version,v", po::value<int>(&version)->default_value(11), " the HTTP version")
			("auth,a", po::value<std::string>(&auth)->default_value(""), " the basic username:pasword for the database ")
        ;
    // Create the option map
        po::variables_map vm;
        // Parse command line for options
        po::store(po::parse_command_line(ac, av, desc), vm);
        // Store user options into the map and the local variables
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        // Set the standard password in case no auth option is present
        auth=auth.empty()?target+":"+target+"password":auth;
        auth64e.resize(boost::beast::detail::base64::encoded_size(auth.size()));
        boost::beast::detail::base64::encode(auth64e.data(),auth.c_str(),auth.size());
        // Set the path just write at the moment
        target="/write?db="+dbase;

        std::cout << " URL: https://" << address <<":"<<port<<"/ "<<target<< ", auth: " << auth << " (base 64 encoded, if different than none) :" << auth64e << '\n'
        		<< " Version:  "<<version<<" DataBase: "<< dbase<< std::endl;
        // The io_context is required for all I/O
        ba::io_context ioc;
        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        // Look up the domain name
        auto const results = resolver.resolve(address.c_str(), port.c_str());
        // Set post verb if command mode, otherwise get the status
        auto v=http::verb::post;
        std::string point;
        std::cout << "Enter a point: ";
        while(std::getline(std::cin,point)) {
        	// Relies in C++17 aggregate estension ...
            beast::tcp_stream stream{ioc};
        	http_io io_object{std::move(stream),{v,target,version}};
//        	// Set SNI Hostname (many hosts need this to handshake successfully)
//        	set_host_name(io_object,address);
        	// Prepare the header
        	prepare_header(io_object,address,auth64e);
        	// Make the connection on the IP address we get from a lookup, write the https request and get the reply
        	connect_write_read(io_object,results, std::move(point));
        	// Write the message to standard out
        	std::cout << " The response: \n"
        			<< io_object.res << std::endl;
        	std::cout << "Enter a command: ";
        } // while


        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
