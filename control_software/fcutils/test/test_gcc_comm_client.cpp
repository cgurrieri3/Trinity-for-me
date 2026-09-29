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

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl.hpp>
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
    beast::ssl_stream<beast::tcp_stream> stream;
    http::request<http::string_body> req;
    http::response<http::dynamic_body> res{};
};

// Set SNI Hostname (many hosts need this to handshake successfully)
void set_host_name(http_io & io_object, std::string & address){
	if(! SSL_set_tlsext_host_name(io_object.stream.native_handle(), address.c_str()))
	{
		beast::error_code ec{static_cast<int>(::ERR_get_error()), ba::error::get_ssl_category()};
		throw beast::system_error{ec};
	}
}
// Perform some header set up to prepare the message on the GCC
void prepare_header(http_io & io_object, std::string & address, std::string & auth64e){
	io_object.req.set(http::field::host, address);
	io_object.req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	// Set the username and password
	io_object.req.set(http::field::authorization, "Basic "+auth64e);
	// Set keep alive to allow multiple connection
	io_object.req.keep_alive(true);
	// In case of POST
	if(io_object.req.method()==http::verb::post) io_object.req.set(http::field::content_type,"application/octet-stream");
}

void stream_shutdown(http_io & io_object){
    // Gracefully close the stream
    beast::error_code ec;
    io_object.stream.shutdown(ec);
    if(ec == ba::error::eof)
    {
        // Rationale:
        // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
        ec = {};
    }
    if(ec)
        throw beast::system_error{ec};

}

template <typename Resolver> void connect_write_read(http_io & io_object, const Resolver & results, std::string cmd=""){
    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(io_object.stream).connect(results);
    // Perform the SSL handshake
    io_object.stream.handshake(ssl::stream_base::client);
	// set the message body, if any
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
    	std::string address,target,port,auth,auth64e,cmdfile,xmlfile;
    	bool cmdmode;
    	int version,wait;
    	using data_type=char;
    	// start adding options
        po::options_description desc("test_gcc_comm_client options ");
        desc.add_options()
            ("help,h", "An help message")
            ("host,o", po::value<std::string>(&address)->default_value("192.168.1.1"), "the IP address to talk to")
            ("port,p", po::value<std::string>(&port)->default_value("443"), "the port to connect to")
			("system,s", po::value<std::string>(&target)->default_value("ft"), " the subsystem to be addressed in the GCC [ft,ct,ir]")
			("version,v", po::value<int>(&version)->default_value(11), " the HTTP version")
			("auth,a", po::value<std::string>(&auth)->default_value(""), " the basic username:pasword for https target autentication, this will override the default value ${system}:${system}password")
			("cmd,c", po::bool_switch(&cmdmode),"send commands instead of reading status")
			("file,f", po::value<std::string>(&cmdfile)->default_value(""), " a file with a command on every line. Commands are just strings, the file is readout as a stream")
			("wait,w", po::value<int>(&wait)->default_value(30), "the wait time between status readout ( in seconds)")
			("xml,x", po::value<std::string>(&xmlfile)->default_value(""), " an xml file with the configuration for the monitor")
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

        // Check the XML file with the monitor definition if any
        if(!xmlfile.empty()){
        	std::cout << " Using XML files: " << xmlfile <<  std::endl;
        	screen_type<data_type> scrn;
        	//	pt::ptree monitor_tree;
        	//	pt::read_xml(monxmlfile,monitor_tree);
        	//	printtree(monitor_tree);
        	scrn.fetch_all(xmlfile);
        	scrn.print_all();
        	monitor_consumer_type<data_type> mc{scrn};
//        	fcutils::consumer::monitor_consumer<screen_type<data_type>, pt::ptree> mc{scrn};
        	std::size_t blength{104};
        	data_type buffer[blength];
        	int * cnt= reinterpret_cast<int *>(&buffer[0]);
        	double * dcnt= reinterpret_cast<double *>(&buffer[4]);
        	// fcutils::monitor::adapt_terminal(scrn);
        	//	fcutils::monitor::list_values(scrn);
        	//	fcutils::monitor::list_labels(scrn);
        	//	std::cout << scrn << std::endl;
        	//		fcutils::monitor::adapt_terminal(scrn);
        	//		std::cout << scrn << std::endl;
        	int temp{100};
        	while(std::cin>>temp){
        		*cnt=temp;
        		*dcnt=static_cast<double>(temp);
        		mc(buffer,blength);
        	}
        }

        // Set the standard password in case no auth option is present
        auth=auth.empty()?target+":"+target+"password":auth;
        auth64e.resize(boost::beast::detail::base64::encoded_size(auth.size()));
        boost::beast::detail::base64::encode(auth64e.data(),auth.c_str(),auth.size());
        // Set the command mode also if a filename has been set. The file option actually override the standard input
        cmdmode=(cmdfile.empty())?cmdmode:true;
        // Set the path according to the subsystem
        target=(cmdmode)?"/"+target+"/command":"/"+target+"/state";

        std::cout << " URL: https://" << address <<":"<<port<<"/ "<<target<< ", auth: " << auth << " (base 64 encoded, if different than none) :" << auth64e << '\n'
        		<< " Version:  "<<version<< std::endl;
        // The io_context is required for all I/O
        ba::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx(ssl::context::tlsv12_client);

        // Do not verify the remote server's certificate
        ctx.set_verify_mode(ssl::verify_none);

            // These objects perform our I/O
        tcp::resolver resolver(ioc);
        // Look up the domain name
        auto const results = resolver.resolve(address.c_str(), port.c_str());
        // Set post verb if command mode, otherwise get the status
        auto v=(cmdmode)?http::verb::post:http::verb::get;
        if(cmdmode){
        	std::string cmd;
        	std::istream * is{&std::cin};
        	// create the input command file and open it in case a filename is set
        	std::ifstream cmdif;
        	if(!cmdfile.empty()) {
        		std::cout << " Using the file: " << cmdfile << ", for command broadcast" << std::endl;
        		cmdif.open(cmdfile);
        		is=&cmdif;
        	} else {
        		std::cout << "Enter a command: ";
        	}
        	while(std::getline(*is,cmd)) {
              	// Relies in C++17 aggregate estension ...
            	http_io io_object{{ioc,ctx},{v,target,version}};
                // Set SNI Hostname (many hosts need this to handshake successfully)
            	set_host_name(io_object,address);
            	// Prepare the header
            	prepare_header(io_object,address,auth64e);
                // Make the connection on the IP address we get from a lookup, write the https request and get the reply
            	connect_write_read(io_object,results, std::move(cmd));
                // Write the message to standard out
                std::cout << " The response: \n"
                		<< io_object.res << std::endl;
                // Gracefully close the stream
                stream_shutdown(io_object);
            	if(cmdfile.empty()) std::cout << "Enter a command: ";
        	} // while

        } else {
        	std::cout << "Reading the status every " << wait << "seconds \n";
       		fcutils::consumer::printbinary_consumer<char> pb;
           	while(true){
            	// Relies in C++17 aggregate estension ...
            	http_io io_object{{ioc,ctx},{v,target,version}};
                // Set SNI Hostname (many hosts need this to handshake successfully)
            	set_host_name(io_object,address);
            	// Prepare the header
            	prepare_header(io_object,address,auth64e);
                // Make the connection on the IP address we get from a lookup, write the https request and get the reply
            	connect_write_read(io_object,results);
//            std::cout << " The response: \n"
//            		<< res << std::endl;
           		std::cout << " The message status size: "
                            << io_object.res.body().size() << std::endl;
           		for( auto const c: io_object.res.body().cdata() )
           			pb(static_cast<const char*>(c.data()),c.size());
           		std::cout << std::dec << std::endl;
                // Gracefully close the stream
           		stream_shutdown(io_object);
           		std::this_thread::sleep_for(std::chrono::seconds{wait});
           	} // while
        } // if

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
