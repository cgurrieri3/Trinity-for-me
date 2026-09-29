///
///
/// @brief To test a message queue manager
///
/// @file test_posix_messagequeuemanager.cpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 29 may 2022
///

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <functional>

#include "string/char_to_hex.hpp"
#include "system/posix_message_queue.hpp"
#include "system/posix_mq_manager.hpp"
#include "checker/words_checker.hpp"
#include "concurrency/sync_queue.hpp"
#include "concurrency/guarded_thread.hpp"

// Alias: boost
namespace po = boost::program_options;

// Alias fcutils
namespace fcc=fcutils::checker;
namespace fcs=fcutils::string;
namespace fcsy=fcutils::system;
namespace fccon=fcutils::concurrency;

// A message queue
using my_queue=fccon::sync_queue<std::string,false>;
// The posix message queue
using my_posix_queue=fcutils::system::posix_message_queue;
// The posix message queue manager
using my_pmq_manager=fcutils::system::posix_mq_manager<my_posix_queue>;

const char EOTX{0x04};


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
		// Just a guess on the queue name size
		std::size_t mq_name_size{15};
		temp.reserve(numRead+mq_name_size);
		temp=mq_name;
		temp+=':';
		temp.append(msg,numRead);
		uq.put(std::move(temp));
	}
	delete [] msg;
}

// The received message consumer
void pmq_message_handler(my_queue & q){
	std::string msg;
	// Loop forever waiting for a message in the queue
	while(1){
		msg.clear();
		q.get(msg);
		// Check if an end of transmission detected
		if(msg[0]==EOTX) {
			std::cout << " Message consumer end of transmission detected ... \n";
			return;
		}
		std::cout << " Message received in the message consumer: " << msg << std::endl;
	}

}

int main(int ac, char* av[])
{

	// Let's look at the options: ip and port
	// A very basic usage of BOOST program options
	std::vector<std::string> read_mq,write_mq;
	// start adding options
    po::options_description desc("test_posix_messagequeuemanager options ");
    desc.add_options()
        ("help,h", "An help message")
		("read_queues,r", po::value<std::vector<std::string> >(&read_mq)->multitoken(), "the posix message queues to read from")
		("write_queues,w", po::value<std::vector<std::string> >(&write_mq)->multitoken(), "the posix message queues to write into")
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

    std::cout << "Reading from the posix message_queues: ";
    for(auto & mqnames: read_mq) std::cout << mqnames << ' ';
    std::cout << '\n';
    std::cout << "Writing to the posix message_queues: ";
    for(auto & mqnames: write_mq) std::cout << mqnames << ' ';
    std::cout << '\n';

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
	// Notify to all reading queues
	rq.notify(read_posix_mq);
	// Start the message consumer in a separate thread
	fccon::guarded_thread my_receiver{pmq_message_handler,std::ref(uq)};
	std::string msg;
	std::cout << "Enter the message to be sent to the listening queues \n";
	while(std::getline(std::cin,msg)){
		std::cout << " Sending message: " << msg <<", of size: "<< msg.size() << std::endl;
		// Brodcast the message to all the listening queues
		wq.broadcast(msg);
	}
	// Stop the listening thread. Force an end of transmission in the message queue
	msg=EOTX;
	uq.put(msg);
	//
	return 0;
}
