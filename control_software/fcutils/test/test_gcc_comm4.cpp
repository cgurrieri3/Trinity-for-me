///
///
/// @brief To test transmission of a GCC command to a posix_queue manager
///
/// @file test_gcc_comm3.cpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 5 june 2022
///

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <ctime>
#include <cstring>
#include <thread>
#include <cstddef>
#include <vector>
#include <map>

#include "string/char_to_hex.hpp"
#include "system/posix_message_queue.hpp"
#include "system/posix_mq_manager.hpp"
#include "checker/checker.hpp"
#include "concurrency/sync_queue.hpp"
#include "networking/client.hpp"
#include "networking/server.hpp"
#include "networking/session.hpp"


// Alias: std
namespace ch = std::chrono;
// Alias: boost
namespace ba=boost::asio;
namespace po = boost::program_options;

namespace fcc=fcutils::checker;
namespace fcn=fcutils::networking;
namespace fcs=fcutils::string;
namespace fcsy=fcutils::system;

// A message queue
using my_queue=fcutils::concurrency::sync_queue<std::string,false>;
using my_posix_queue=fcutils::system::posix_message_queue;
// The posix message queue manager
using my_pmq_manager=fcutils::system::posix_mq_manager<my_posix_queue>;

const std::size_t GCCHEADER_SIZE{8},GCCHEADER_CHAR_SIZE{GCCHEADER_SIZE*2},GCCSTATUS_MAX_SIZE{104};
const std::size_t SUBSYSHEADER_SIZE{3};

struct message_consumer{
	my_queue & head, & user;
	int i{0};
	const char eotx{0x04};

	message_consumer(my_queue & q, my_queue & u) :head{q}, user{u} {};
	// To read the TCP client or server sessions
	void operator()(){
		++i;
		std::string msg;
		head.get(msg);
		if(msg[0]==eotx) {
			std::cout << " Consumer end of transmission detected ... \n";
			user.put(std::move(msg));
			return;
		}
		// Strip away the GCC header
		std::string_view sv{msg};
		if( msg.size()>GCCHEADER_CHAR_SIZE ) sv.remove_prefix(GCCHEADER_CHAR_SIZE);
		std::cout << "Consumer i: "<< i <<", full GCC message: 0x"<< msg <<", payload: 0x"<< sv<< std::endl;
		// Transmit the message to the user
		user.put(std::move(msg));
		// post back this member function into the thread and wait for another message
		(*this)();
	};

};

template <typename Data, bool Convert=true> struct message_producer{
	using data_type=Data;
	my_queue & tail;
	message_producer(my_queue & q) :tail(q) {};
	void operator()(data_type * d, std::size_t length){
		std::string msg;
		if constexpr(Convert) {
			// Convert the char to hex values
//			std::string msg{fcutils::string::char_to_hex(d,length,"")};
			msg=fcutils::string::char_to_hex(d,length,"");
		} else {
			// Just copy the data in the msg
//			std::string msg{d,length};
			msg.append(d,length);
		}
		std::cout << " A message received: " << msg <<'\n';
		tail.put(std::move(msg));
		return;
	};
	const char eotx{0x04};
};

// We need here the message queues to guarantee usage of a standard function to the posix message queue ...
// To be solved somehow in the future ...

my_queue uq;

// To read the posix message queue
void read_posix_mq(sigval_t sig){
	int numRead{-1};
	// Retrive the posix message queue object from the sigval_t infos.
	my_posix_queue* mqp=static_cast<my_posix_queue*>(sig.sival_ptr);
	auto msg_size=mqp->get_msgsize();
	auto msg=new char[msg_size];
	auto mq_name=mqp->get_name();
	mqp->notify(read_posix_mq);
	numRead = mqp->receive(msg, msg_size);
	if(numRead > 0){
		std::cout << " Queue: " << mq_name <<", message received: " << msg << ", of size: " << numRead << std::endl;
		std::string temp;
		temp.reserve(numRead+2);
		temp="CT";
		temp.append(msg,numRead);
		uq.put(std::move(temp));
	}
	delete [] msg;
}

using data_type=char;
using key_type=char;
using checker_return_type=std::size_t;
using subsys_checker_return_type=bool;
const bool DEBUG{false},SUBSYSDEBUG{false}; // Set this to true for more DEBUG printout from the checker
template<typename Data> using gcc_consumer_type=message_producer<Data>;
template<typename Data> using gcc_checker_type=fcc::words_checker<GCCHEADER_SIZE,checker_return_type,fcc::decode_header,Data,key_type,DEBUG>;
template<typename Protocol, bool Echo> using gcc_session_type=fcn::session<Protocol, Echo, gcc_consumer_type, gcc_checker_type, data_type, DEBUG>;
// The client using the checker for a GCC header
using gcc_client_type=fcn::base_client< fcn::tcp, false, gcc_session_type >;
// An endpoint map. Just associate a string to an endpoint
using endpoint_map_type = std::map<std::string,fcn::tcp::endpoint>;
template<typename Data> using subsys_consumer_type=message_producer<Data,false>;
template<typename Data> using subsys_checker_type=fcc::nl_checker<Data>; //fcc::words_checker<SUBSYSHEADER_SIZE,subsys_checker_return_type,fcc::do_nothing,Data,key_type,SUBSYSDEBUG>;
template<typename Protocol, bool Echo> using subsys_session_type=fcn::session<Protocol, Echo, subsys_consumer_type, subsys_checker_type, data_type, SUBSYSDEBUG>;
// The client using the checker for a subsystem header
using subsys_client_type=fcn::base_client< fcn::tcp, false, subsys_session_type, SUBSYSDEBUG>;

int main(int ac, char* av[])
{

	// Let's look at the options: ip and port
	// A very basic usage of BOOST program options
	std::vector<std::string> read_mq,write_mq,clients;
	std::string address;
	ba::ip::port_type port;
	int delay{0};
	bool timestamp{false},nostatus{false};
	// start adding options
    po::options_description desc("test_gcc_comm3 options ");
    desc.add_options()
        ("help,h", "An help message")
        ("ip", po::value<std::string>(&address)->default_value("192.168.1.1"), "the IP address to talk to")
        ("port,p", po::value<ba::ip::port_type>(&port)->default_value(20021U), "the port to connect to")
		("delay,d", po::value<int>(&delay)->default_value(30), "the delay between broadcast (seconds)")
		("timestamp,t", po::bool_switch(&timestamp),"use timestamp instead of a counter")
		("nostatus,n", po::bool_switch(&nostatus),"turn off automatic status messages")
		("read_queues,r", po::value<std::vector<std::string> >(&read_mq)->multitoken(), "the posix message queues to read from")
		("write_queues,w", po::value<std::vector<std::string> >(&write_mq)->multitoken(), "the posix message queues to write into")
		("ip_clients,c", po::value<std::vector<std::string> >(&clients)->multitoken(), "The list of clients to use for forward or receiving messages. This must be in the form: key@xxx.xxx.xxx.xxx:port, for example: clkb@192.168.1.64:10020")
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

    std::cout << " IP address: " << address <<":"<<port<<", delay (sec): "<<delay<< '\n';
    std::string s=(timestamp)?"timestamp":"message counter";
    std::cout << "Using "<<s<<"as status message \n";
    std::cout << "Reading from the posix message_queues: ";
    for(auto & mqnames: read_mq) std::cout << mqnames << ' ';
    std::cout << '\n';
    std::cout << "Writing to the posix message_queues: ";
    for(auto & mqnames: write_mq) std::cout << mqnames << ' ';
    std::cout << '\n';
    std::cout << "Connecting with the TCP/IP clients: ";
    for(auto & mqnames: clients) std::cout << mqnames << ' ';
    std::cout << '\n';

    // Here the checker for words of size char, storable in a checker map with a char key.
	// Here the number of words to be checked is the size of the header
    // This object will be copied into a client or a server.
	gcc_checker_type<data_type> gcc_checker{
		'g',
		{ // The header byte constant values to be checked for only 7 of 8 bytes will be used,
			static_cast<data_type>(0xAA), // The header ID, four bytes 0xAA5555AA
			static_cast<data_type>(0x55),
			static_cast<data_type>(0x55),
			static_cast<data_type>(0xAA),
			static_cast<data_type>(0x00), // The message type: 0x0, for received msg; 0x1 for status, i.e. transmitted, message
			static_cast<data_type>(0x00), // Mandatory empty bytes, two bytes
			static_cast<data_type>(0x00)
		},
		fcc::decode_header<bool,std::size_t>{8,8} // The number of words to be checked upon extraction of the size from the offset.
	}; // end of words_checker constructor

//	// Here the checker for the subsystem
//	subsys_checker_type<data_type> subsys_checker{
//		's',
//		{ // The header byte constant values to be checked for only 7 of 8 bytes will be used,
//			's', // The header ID, four bytes 0xAA5555AA
//			'y',
//			's'
//		} //,
////		fcc::do_nothing<bool,std::size_t>{} // No message size is present in the header. Just check if the "sys" is the prefix
//	}; // end of words_checker constructor

	// Create the posix message queue manager
    // read queues
	my_pmq_manager rq;
	// add the queues
    for(auto & mqnames: read_mq) rq.add_mq(mqnames);
    // Open all queue
	rq.open();
	// Print the stored queue names
	rq.print();
    // write queues
	my_pmq_manager wq;
	// add the queues
    for(auto & mqnames: write_mq) wq.add_mq(mqnames);
    // Open all queue
	wq.open();
	// Print the stored queue names
	wq.print();

	// Create a map with all the TCP IP mapping
	endpoint_map_type endpoints;
	for(auto & mqnames: clients) {
        std::string_view caddr{mqnames};
        auto at=caddr.find('@');
        auto colon=caddr.find(':');
        auto key=caddr.substr(0,at);
        auto sport=caddr.substr(colon+1);
        short unsigned int iport{0};
        std::from_chars(sport.data(),sport.data()+sport.size(),iport);
        caddr.remove_suffix(caddr.size()-colon);
        caddr.remove_prefix(at+1);
        endpoints.emplace(key,fcn::tcp::endpoint{boost::asio::ip::make_address(caddr),iport});
    }
	std::cout << "Enpoint map: \n";
	for( auto & [key,v]: endpoints) std::cout <<" Key: " << key <<", endpoint: "<< v <<'\n';
	std::cout << std::endl;

    // Try to deal with the TCP stream objects
    try{
    	// start with an ASIO context
    	ba::io_context ioc;
    	// We need a consumer.
    	// The consumer is supposed to be passed by reference.
    	// This is because a consumer can be also a container that could be filled with new consumers at run time.
    	// For this example we use a syncronized message queue producer that will serialize the message into a string, and put into the queue.
    	my_queue mq;
		gcc_client_type::session_type::consumer_type sq_producer{mq};
    	// Then we need a message receiver
    	message_consumer sq_consumer{mq,uq};
    	// Notify to all reading queues
    	rq.notify(read_posix_mq);
    	// Start the message receiver in a separate thread
    	std::thread my_receiver{sq_consumer};
    	// Construct a signal set registered for process termination.
    	ba::signal_set signals(ioc, SIGINT, SIGTERM);
    	// Start an asynchronous wait for one of the signals to occur.
    	signals.async_wait([&mq](const boost::system::error_code& ec, int signal_number){
    		std::cout << " Signal received: " << signal_number <<'\n';
    		if(!ec) {
    			std::cout << " Signal handler. No error \n";
    		} else {
    			std::cout << " signal handler. Error : " << ec.message() << std::endl;
    		}
    		// Anyway, if we receive a signal means that something went wrong. So let's stop the message queue
    		const char eotx{0x04};
    		mq.put(my_queue::message_type{eotx});
    	});
    	// The client TCP to be used with a session checking for the GCC header
    	gcc_client_type gcc_client{ioc,sq_producer,gcc_checker};
    	auto ep=fcn::tcp::endpoint{boost::asio::ip::make_address(address),port};
    	// Add the address and port and try to connect
//    	gcc_client.async_connect(boost::asio::ip::make_address(address),port);
    	// Set the reuseaddress and reuseports options to the socket
    	gcc_client.async_connect(ep);
    	gcc_client.set_option(ep,boost::asio::socket_base::reuse_address{true});
    	gcc_client.set_option(ep,boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>{true});
    	// Start the client
    	gcc_client.start();
    	// The client with all the endpoints where the message should be addressed
		subsys_client_type::session_type::consumer_type subsys_producer{uq};
		// Use the default check at the momen
    	subsys_client_type subsys_client{ioc,subsys_producer};
    	// Add the endpoints
    	for (auto & [k,sep]: endpoints){
    		subsys_client.async_connect(sep);
        	subsys_client.set_option(ep,boost::asio::socket_base::reuse_address{true});
        	subsys_client.set_option(ep,boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>{true});
    	}
    	// Start the client
    	subsys_client.start();
    	// Start the context on another thread
        // A pointer to the member function run(). Is needed because is an oveloaded function
        ba::io_context::count_type (ba::io_context::*pf_run)(){&ba::io_context::run};
        // Create an launch a thread executing io_context.run()
        std::thread t{pf_run,&ioc};
       	// create a time point converted to time_t (C style) for an easier text conversion
       	std::time_t now=ch::system_clock::to_time_t(ch::system_clock::now());
       	// create a time duration for the wait state
       	auto wd=ch::seconds{delay};
       	int count{0};
       	// Prepare a standard header needed by the GCC message
       	const std::size_t GCCHEADER_SIZE{8},GCCSTATUS_MAX_SIZE{104};
       	unsigned char gccheader[GCCHEADER_SIZE]={static_cast<unsigned char>(0xAA),
       			static_cast<unsigned char>(0x55),static_cast<unsigned char>(0x55),
				static_cast<unsigned char>(0xAA),static_cast<unsigned char>(0x01),
				static_cast<unsigned char>(0x00),static_cast<unsigned char>(0x00) };
       	// Just the null termination char
       	char gcctrailer='\0';
       	// create the buffer sequence
       	std::array<ba::const_buffer,2> status_msg;
       	// A pointer to the message
       	char *cnow{nullptr};
       	// The message length
       	std::size_t length{0};
       	std::string msg;
       	// End of Transmission according to ASCII coding
		const char eotx{0x04};
       	while(1){
       		if(!nostatus){
				++count;
				std::cout<< "Sending status message no. "<<count<<" ...\n";
				if(timestamp){
					// this is in principle a static array of char ...
					auto now=ch::system_clock::to_time_t(ch::system_clock::now());
					cnow=std::ctime(&now);
					// Remember, std::strlen returns the lenght without the null termination
					length=cnow?std::strlen(cnow)+1:0;
					// Set maximum length to 146 bytes
					length=(length<=GCCSTATUS_MAX_SIZE)?length:GCCSTATUS_MAX_SIZE;
				} else {
					cnow=reinterpret_cast<char *>(&count);
					length=sizeof(count);
				}
				// Store the message length into the gcc header
				gccheader[7]=static_cast<char>(length);
	//       		stream<<std::ctime(&now);
	//       		stream << count;
				std::cout << " Writing the message :"<< cnow << ", of length: "<< length <<std::endl;
				// Create the buffer sequence
				status_msg[0]=ba::buffer(gccheader,GCCHEADER_SIZE);
				status_msg[1]=ba::buffer(cnow,length);
				gcc_client.write(status_msg);
				// Put this thread on wait for 30 seconds
	//           	std::this_thread::sleep_for(wd);
				// Instead of sleeping, just check if there is any message in the queue
       		} // if(!nostatus)
       		msg.clear();
       		uq.get(msg,wd);
    		if(msg[0]==eotx) {
    			std::cout << " End of transmission detected ... That's all folks!!!\n";
    			return 0;
    		}
    		// Here we get a timeout or a message was set.
    		if(!msg.empty()) {
    			if( (msg[0]=='C' && msg[1]=='T') || (msg[0]=='s' && msg[1]=='y' && msg[2]=='s') ){
    				// Here we got a message from the CT or sys forward it to the GCC
    				// Store the message length into the gcc header
    				length=msg.size();
    				gccheader[7]=static_cast<char>(length);
    				std::cout << " Writing the message :"<< msg << ", of length: "<< length <<std::endl;
    				// Create the buffer sequence
    				status_msg[0]=ba::buffer(gccheader,GCCHEADER_SIZE);
    				status_msg[1]=ba::buffer(msg.c_str(),length);
    				gcc_client.write(status_msg);
    			} else {
    				// Here we got a message from the GCC
    				// Strip away the GCC header
    				std::string_view sv{msg};
    				if(msg.size()>GCCHEADER_CHAR_SIZE) sv.remove_prefix(GCCHEADER_CHAR_SIZE);
    				std::cout << " A message from GCC. Full GCC message: 0x"<<msg <<", payload: 0x"<< sv<< std::endl;
    				// Transmit the message to the user
    				std::string umsg{sv};
//        			std::cout << " A message was received and sent to the message queue: " << umsg << '\n';
//    				wq.send(umsg.c_str(),umsg.size());
    				// --->>>>>>> HERE You can change the way the message is send to the queues and select a queue according to an header
        			std::cout << " A message was received and broadcast to all message queues: " << umsg << '\n';
    				wq.broadcast(umsg.c_str(),umsg.size());
    				// Compare the endpoints key to the begining of the message and forward to the endpoint, in case of positive matching

    				for(auto & [k,e]: endpoints){
    					if(umsg.find(k)!=std::string::npos){
    						// A key was found in the message
    						subsys_client.write(ba::buffer(umsg.c_str(),umsg.size()),e);
    					}
    				}

    			}
    		} // if(!msg.empty)
       	} // while(1)
    } // try
    // Catch ASIO exceptions here
    catch (std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }//

	return 0;
}

