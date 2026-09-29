///
///
/// @brief Common process utilities. A struct that handles a sequence and sequencer managers along with logger utilities.
///
/// @file process_common.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 21 mar 2022 
///

#ifndef PROCESS_COMMON_HPP_
#define PROCESS_COMMON_HPP_
#include <string>
#include <chrono>
#include <iosfwd>
#include <type_traits>
#include <thread>
#include <errno.h> // for program_invocation_short_name
#include <string_view>
/// Boost process has some io redirection facility that can be added to std::system
#include <boost/process.hpp>
///
#include "fcutils/chrono/chrono.hpp"
#include "fcutils/string/program_name_and_date.hpp"
#include "fcutils/serialization/deserializer.hpp"
#include "fcutils/utility/print_vals.hpp"
#include "fcutils/iostream/iostream.hpp"
#include "fcutils/type_traits/type_traits.hpp"
#include "fcutils/consumer/decode_consumer_command.hpp"
#include "fcutils/command/decode_process_command.hpp"

namespace {
	using namespace std::chrono_literals;
	namespace proc{
	namespace cmd {
		// Command sizes
		typedef unsigned char byte;
		// The trailer
		const unsigned char EOC=0xEC;
		// All sequencer, sequences, anything is 2 bytes long
		const std::string ALL("0000");
		namespace size{
			// The PROC Command size, in characters  including the leading 0x characters
			const std::size_t PROCCMDCHARSIZE=6;
			// The PROC Header size, in characters  including the leading 0x characters
			const std::size_t PROCHEADCHARSIZE=4;
			// The sequence name size, in bytes
			const std::size_t SEQIDSIZE=2;
			// The sequencer name size, in bytes
			const std::size_t SEQUENCERIDSIZE=2;
			// The time or counter unit size, in bytes
			const std::size_t TIMECNTUNITSIZE=1;
			// The time or counter value size, in bytes
			const std::size_t TIMECNTVALSIZE=2;
			// The Alarm mask size, in bytes
			const std::size_t LALARMMASKSIZE=5;
			const std::size_t HALARMMASKSIZE=1;
			// The Alarm action counter size, in bytes
			const std::size_t ALARMCOUNTERSIZE=1;
			// The header, command, CRC and trailer size, in bytes
			const std::size_t CMDADDRESSSIZE=1;
			const std::size_t CMDACTIONSIZE=1;
			const std::size_t CMDCHANNELSIZE=1;
			const std::size_t CMDCRCSIZE=2;
			const std::size_t CMDEOCSIZE=1;
			const std::size_t CMDDATASIZE=4;
			const std::size_t DATA=2;
			// The SIP link and routing sizes
			const std::size_t SIPLINKSIZE=1;
			const std::size_t SIPROUTINGSIZE=1;
			// The IP address and port size for UDP log.
			// Four (4) bytes are for the IP address 2 for the port
			const std::size_t IPADDRESSSIZE=7;
			const std::size_t ASKCMD=CMDADDRESSSIZE+CMDACTIONSIZE+CMDCHANNELSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 6
			const std::size_t REPLYCMD=8;// It should be 8
			const std::size_t SETCMD=7;// It should be 7
			const std::size_t OPCMD=5;// It should be 5
			const std::size_t LALARMSTRATEGYCMD=CMDADDRESSSIZE+CMDACTIONSIZE
					+LALARMMASKSIZE+SEQIDSIZE+ALARMCOUNTERSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 14
			const std::size_t HALARMSTRATEGYCMD=CMDADDRESSSIZE+CMDACTIONSIZE
					+HALARMMASKSIZE+SEQIDSIZE+ALARMCOUNTERSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 10
			const std::size_t LALARMRESETSTRATEGYCMD=CMDADDRESSSIZE+CMDACTIONSIZE
					+LALARMMASKSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 10
			const std::size_t HALARMRESETSTRATEGYCMD=CMDADDRESSSIZE+CMDACTIONSIZE
					+HALARMMASKSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 6
			const std::size_t SEQUENCERCMD=CMDADDRESSSIZE+CMDACTIONSIZE+SEQUENCERIDSIZE
					+SEQIDSIZE+TIMECNTUNITSIZE+TIMECNTVALSIZE+CMDEOCSIZE+CMDCRCSIZE; // It should be 12
			const std::size_t ENABLENETLOGCMD=CMDADDRESSSIZE+CMDACTIONSIZE+IPADDRESSSIZE+CMDEOCSIZE+CMDCRCSIZE; // it should be 12
			const std::size_t SETSIPLINKCMD=CMDADDRESSSIZE+CMDACTIONSIZE+SIPLINKSIZE+CMDEOCSIZE+CMDCRCSIZE; // it should be 12
			const std::size_t SETSIPROUTECMD=CMDADDRESSSIZE+CMDACTIONSIZE+SIPROUTINGSIZE+CMDEOCSIZE+CMDCRCSIZE; // it should be 12
		} // namespace size
	}// namesapce cmd
	} // namespace proc
}

namespace fcutils{
namespace command{
// A simple executor to print a command, if possible ...

template<typename Command> struct print_executor{
	using command_type 	= Command;
	using self_type 	= print_executor;
	explicit print_executor(const command_type & c): cmd{c}{};
	print_executor(const self_type & pe): cmd{pe.cmd}{};
	self_type & operator=(const self_type & pe){ cmd=pe.cmd;}
	self_type & operator=(const command_type & c){ cmd=c;}
	void operator()(){
		if constexpr( fcutils::type_traits::is_to_stream_writable_v<command_type>){
			std::cout <<" THis is the command: "<< cmd << std::endl;
		} else {
			std::cout << " Command type doeas not have any stream operator defined !!!" << std::endl;
		}
	}
	command_type cmd;
};

template<typename SequenceManager, typename SequencerManager, typename ConsumerManager, typename Logger=fcutils::iostream::ostream_logger, typename Data= unsigned char,
		typename Duration=std::chrono::milliseconds, typename Id= unsigned char, typename ActionId= unsigned char, template <typename> typename Executor=fcutils::command::print_executor >
struct process_common {
	using message_type					= std::string;
	using self_type						= process_common;
	using data_type						= Data;
	using id_type						= Id;
	using action_type					= ActionId;
	using sequence_manager_type			= SequenceManager;
	using sequence_type					= typename sequence_manager_type::sequence_type;
	using command_type					= typename sequence_type::command_type;
	using sequence_id_type				= typename sequence_manager_type::id_type;
	using sequencer_manager_type		= SequencerManager;
	using sequencer_id_type				= typename sequencer_manager_type::id_type;
	using sequencer_ptr_type			= typename sequencer_manager_type::sequencer_ptr_type;
	using consumer_manager_type			= ConsumerManager;
	using logger_type					= Logger;
	using duration_type					= Duration;
	using deserializer_type				= fcutils::serialization::deserialize_repr;
	using net_duplicator_manager_type	= fcutils::iostream::net_duplicator_manager<>;
	using executor_type					= Executor<message_type>;

	process_common(id_type procid, const char * loggername, const std::string & progname=program_invocation_short_name):
			procid_(procid), proc_byte_helper(procid), progname(progname), logger(loggername), seqdef_(false)
		{
			sequencemgr.set_progname(progname+' '+sequencemgr.get_progname());
			sequencermgr.set_progname(progname+' '+sequencermgr.get_progname());
		}

// What about copy or moving ?
	// Let's start deleting copying ...
	process_common(const process_common &) = delete;
	process_common & operator=(const process_common &) = delete;
// Data Member
	id_type proc_byte_helper;
	fcutils::string::program_name_and_date progname;
	net_duplicator_manager_type net_duplicators;
	// create the logger and immediatelly enable log in case is needed
	logger_type logger;
	sequence_manager_type sequencemgr;
	//// Define the sequencer manager type
	sequencer_manager_type sequencermgr;
	/// Add a consumer manager
	consumer_manager_type consumermgr;
private:
	// The private data part
	id_type procid_;
	// Sequence recording flag. REMEMBER it must be initialized always to false
	bool seqdef_{false};
	// Last Sequence key recorded or recording
	sequence_id_type seqname_;
	// A deserializer to extract data field from the command string
	deserializer_type deserializer_;
	// Create an anonymous sequence to handle sequences not to be recorded
	const std::string ANOMSEQ_{"noname"};

	template <typename D> void sleep_for_impl(const D & d){
		// If Duration is an integer then create a standard duration, otherwise cast to the standard duration
		if constexpr (std::is_integral<D>::value){
			duration_type wait{d};
			std::cout << progname << "Waiting for "<< wait.count() << fcutils::chrono::suffix<duration_type>() << std::endl;
			std::this_thread::sleep_for(wait);
		} else {
			duration_type wait{std::chrono::duration_cast<duration_type>(d)};
			std::cout << progname << "Waiting for "<< wait.count() << fcutils::chrono::suffix<duration_type>() << std::endl;
			std::this_thread::sleep_for(wait);
		}
	}

	void spawn_cmd_impl(const std::string & s){
		//Launch child process
		// Remember the std::cout flush is needed in case the process will use the standard output
		std::cout << progname << " Spawing command: " << s << std::endl;
//		if(0 != std::system(s.c_str()))
		boost::process::ipstream procout;
		if(0 != boost::process::system(s.c_str(),boost::process::std_out>procout)){
			std::cout << progname << " ERROR ->>> Error spawing command: " << s << std::endl;
		} else {
			std::string line;
		    while (procout && std::getline(procout, line) && !line.empty())
		        std::cout << line << std::endl;
		}
	}

public:
	// Member functions

	template<typename Ret=id_type> Ret get_id() const { return static_cast<Ret>(procid_); }

	template <typename D> void sleep_for(const D & d){
		if(seqdef_){
			// the sleep for command refers to a sequence. Just add it to the sequence being defined
			sequencemgr.add_cmd_to_sequence(seqname_, [d,this](){sleep_for_impl(d);} );
//					boost::bind(&self_type::sleep_for_impl,this,d) );
		} else {
			sleep_for_impl(d);
		}
	}

	void spawn_cmd(const std::string &s){
		if(seqdef_) {
			// the spawn for command refers to a sequence. Just add it to the sequence being defined
			sequencemgr.add_cmd_to_sequence(seqname_, [s,this](){spawn_cmd_impl(s);});
//					boost::bind(&self_type::spawn_cmd_impl,this,s) );
		} else {
			//Launch child process
			spawn_cmd_impl(s);
		}
	}

	void delete_sequence( const typename sequence_manager_type::id_type & seqname) {
			// a sequence DELETE is found.
			seqdef_=false;
			// reset the command size to avoid execution to the serial
			std::cout << progname << " A delete sequence found. Sequence ID: " << seqname << std::endl;
			// remove the sequence
			sequencemgr.remove_sequence(seqname);
			auto cmdsize=0;
	}

	void start_sequence( ){
		// a sequence definition is starting, just create an anonymous sequence and mark the start of recording
		std::cout << progname << " An start of sequence found. Recording an anonymous ..." << std::endl;
		startrec_sequence(ANOMSEQ_);
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void end_sequence ( ){
		// a sequence definition is over, just stop recording, execute the anonymous sequence and delete it
		std::cout << progname << " An end of sequence found. Executing the sequence ..." << std::endl;
		// execute the sequence
		endrec_sequence();
		execute_sequence(ANOMSEQ_);
		delete_sequence(ANOMSEQ_);
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void startrec_sequence ( const typename sequence_manager_type::id_type & seqname){
		  // Start a sequence recording
		  seqdef_=true;
		  // reset the command size to avoid execution to the serial
		  seqname_=seqname;
		  auto cmdsize=0;
		  std::cout << progname << " A start of sequence recording found. Sequence ID: " << seqname << std::endl;
		  // Add a new empty sequence
		  add_sequence(seqname);
	}

	void endrec_sequence () {
		// an end of recording sequence found. Just flag the end of a sequence definition
		seqdef_=false;
		std::cout << progname << " An end of sequence recording found. " << std::endl;
			// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void add_sequence(const typename sequence_manager_type::id_type & id ){
		  std::cout << progname << " A create a new sequence found. Sequence ID: " << id << std::endl;
		  // Add a new empty sequence
		  sequencemgr.add_sequence(id);

	}

	void add_cmd_to_sequence(const typename sequence_manager_type::id_type & id, const command_type & cmd){ //const typename sequence_manager_type::command_type & cmd) {
		  std::cout << progname << " An add new command found.  Sequence ID: " << id << std::endl;
		  // Add a new empty sequence
		  sequencemgr.add_cmd_to_sequence(id,cmd);

	}

	void execute_cmd(const message_type &s){
		if(seqdef_) {
			// the command have to be registered in a sequence. Just add it to the sequence using an executor
			sequencemgr.add_cmd_to_sequence(seqname_, executor_type{s});
//					boost::bind(&self_type::spawn_cmd_impl,this,s) );
		} else {
			//Execute the command using the executor
			executor_type temp{s};
			temp();
		}
	}

	void execute_cmd(const command_type &s){
		if(seqdef_) {
			// the command have to be registered in a sequence. Just add it to the sequence
			sequencemgr.add_cmd_to_sequence(seqname_, s);
//					boost::bind(&self_type::spawn_cmd_impl,this,s) );
		} else {
			//Execute the command using the executor
			s();
		}
	}

	void execute_sequence( const std::string & seqname){
		// execute a sequence
		sequencemgr.execute_sequence(seqname);
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;

	}

	void enable_log( std::ostream & o = std::cout) {
		  // Enable a log file of the std output
		// Open a log file with the ID and time stamps
		logger.enable(o);
		std::cout << progname << " An enable log found. Duplicating standard output on log file: "<< logger.get_name() << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void disable_log( std::ostream & o = std::cout) {
		// Disable a log file of the std output
		logger.disable(o);
		std::cout << progname << " A disable log found. Disable duplication of standard output on log file: "<< logger.get_name() << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	template<class D, class Action > void add_sequencer(const sequencer_id_type & id, const D &val, std::shared_ptr<Action> a  ){
		  std::cout << progname << " A record a new sequencer found. ID: "
				  << id;
		  if constexpr (std::is_integral<D>::value){
			  std::cout << ", Time value: " << val <<  std::endl;
		  } else {
			  std::cout << ", Time value: " << val.count()  <<
					  fcutils::chrono::suffix<D>() << std::endl;
		  }
		  sequencermgr.add_sequencer(id,val,a);
		  // reset the command size to avoid execution to the serial
		  auto cmdsize=0;
	}

	template<class D>
	void add_sequencer(const sequencer_id_type & id, const D &val,
			const sequence_id_type & seqname  ){
		  std::cout << progname << " A record a new sequencer found. ID: "
				  << id;
		  if constexpr (std::is_integral<D>::value){
			  std::cout << ", Time value: " << val <<  std::endl;
		  } else {
			  std::cout << ", Time value: " << val.count()  <<
					  fcutils::chrono::suffix<D>() << std::endl;
		  }
		  sequencermgr.add_sequencer(id,val,sequencemgr[seqname]);
		  // reset the command size to avoid execution to the serial
		  auto cmdsize=0;
	}

	void start_sequencer(const sequencer_id_type & id){
		  std::cout << progname << " A start sequencer found. Sequencer ID: " << id << std::endl;
		  sequencermgr.start_sequencer(id);
		  auto cmdsize=0;
	}

	void start_all_sequencer(){
		  std::cout << progname << " A start all sequencers found" << std::endl;
		  sequencermgr.start_all_sequencer();
		  auto cmdsize=0;
	}

	void stop_sequencer(const sequencer_id_type & id){
		  std::cout << progname << " A stop sequencer found. Sequencer ID: " << id << std::endl;
		  sequencermgr.stop_sequencer(id);
		  auto cmdsize=0;
	}

	void stop_all_sequencer(){
		  std::cout << progname << " A stop all sequencers found" << std::endl;
		  sequencermgr.stop_all_sequencer();
		  auto cmdsize=0;
	}

	void delete_sequencer(const sequencer_id_type & id){
		  std::cout << progname << " A delete sequencer found. Sequencer ID (hex): " << id << std::endl;
		  // First stop the sequencer
		  sequencermgr.stop_sequencer(id);
		  // Then remove it
		  sequencermgr.remove_sequencer(id);
		  auto cmdsize=0;
	}

	void delete_all_sequencer(){
		  std::cout << progname << " A delete all sequencers found" << std::endl;
		  // First stop all sequencer
		  sequencermgr.stop_all_sequencer();
		  // Then remove them all
		  sequencermgr.remove_all_sequencer();
		  auto cmdsize=0;
	}

	std::string process_echo( std::string & s){
		// Just echo the msg received, as it is, adding the program name
		std::cout << progname << " Echo command. " << s << std::endl;
		// reset the command size to avoid dispatch to the serial port
		auto cmdsize=0;
		std::string temp{s};
		temp+=progname.get_name();
		return temp;

	}

	void enable_netlog(const std::string & cmd, std::ostream & o=std::cout){
		// Duplicate the std output to a net protocol, address and port
		// Is the net_duplicator_manager responsibility to decode the command, that is the transport protocol: UDP or TCP, the IP address and the port.
		net_duplicators.enable(cmd,o);
		std::cout << progname << " An enable network duplicator found. Duplicating standard output on net duplicator: "<< cmd << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}
	void disable_netlog( const std::string & cmd, std::ostream & o = std::cout){
		// Disable a net duplicator.
		// Is the net_duplicator_manager responsibility to decode the command, that is the transport protocol: UDP or TCP, the IP address and the port.
		net_duplicators.disable(cmd,o);
		std::cout << progname << " A disable network duplicator found. Disable net duplication of standard output: "<< cmd << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	template <typename Decoder>
	void add_consumer(Decoder && decoder, const std::string & cmd){
		// Add a consumer
		// Consumer is decoded by the consumer manager, via the decoder object
		std::cout << progname << " A new consumer to be added: "<< cmd << std::endl;
		consumermgr.add_consumer(std::forward<Decoder>(decoder),cmd);
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	  }

	template<typename Ptr=typename consumer_manager_type::consumer_ptr_type>
	void add_consumer(const typename consumer_manager_type::key_type & id, Ptr && c){
		// Add a consumer pointed by c with id
		std::cout << progname << " A new consumer to be added with id: "<< id << std::endl;
		consumermgr.add_consumer(id,std::forward<Ptr>(c));
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	template<typename Node=typename consumer_manager_type::consumer_map_type::node_type>
	void add_consumer(Node && n){
		// Add a consumer pointed by c with id
		std::cout << progname << " A new node in the consumer map to be added with id: "<< n.first << std::endl;
		consumermgr.add_consumer(std::forward<Node>(n));
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void delete_consumer(const typename consumer_manager_type::key_type & id){
		// Delete a consumer
		// Just the key is needed
		std::cout << progname << " A delete consumer found for key: "<< id << std::endl;
		consumermgr.remove_consumer(id);
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}

	void enable_syslog(const std::string & cmd, std::ostream & o=std::cout){
		// Duplicate the std output to a net protocol, address and port
		// Is the net_duplicator_manager responsibility to decode the command, that is the transport protocol: UDP or TCP, the IP address and the port.
		net_duplicators.enable(cmd,o);
		std::cout << progname << " An enable SYSLOG duplicator found. Duplicating standard output on net duplicator: "<< cmd << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}
	void disable_syslog( const std::string & cmd, std::ostream & o = std::cout){
		// Disable a net duplicator.
		// Is the net_duplicator_manager responsibility to decode the command, that is the transport protocol: UDP or TCP, the IP address and the port.
		net_duplicators.disable(cmd,o);
		std::cout << progname << " A disable SYSLOG duplicator found. Disable net duplication of standard output: "<< cmd << std::endl;
		// reset the command size to avoid execution to the serial
		auto cmdsize=0;
	}
	bool check_process_id( const id_type & id){
		return id==procid_;
	}

	bool is_seqdef() const { return seqdef_;}
	const auto & last_seqname() const { return seqname_; }

	void print_sequences ( std::ostream & o=std::cout) const {
		sequencemgr.print(o);
	}

	void print_sequencers ( std::ostream & o=std::cout) const {
		sequencermgr.print(o);
	}
};


} // namesapce command
} // namespace fcutils


#endif // PROCESS_COMMON_HPP_ 
