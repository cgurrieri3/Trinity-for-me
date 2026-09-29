///
///
/// @brief Decode a message with process command
/// A process command should contain:
/// - The process id
/// - The process command
/// - Options needed by the command
/// This command coding has:
/// - Process ID: 1 Byte;
/// - Command ID: 1 Byte;
/// - Command options: Variable size, depending on the command
/// The command is supposed to be encoded into a std::string
///
/// @file decode_process_command.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 24 mar 2022 
///

#ifndef DECODE_PROCESS_COMMAND_HPP_
#define DECODE_PROCESS_COMMAND_HPP_
#include <string>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <charconv>

#include "fcutils/serialization/deserializer.hpp"
#include "fcutils/utility/integer.hpp"
#include "fcutils/type_traits/type_traits.hpp"

namespace {
 namespace fctt= fcutils::type_traits;
 const std::size_t BYTE_SIZE_IN_CHAR{2};
 const std::size_t BYTE_SIZE_IN_BIT{8};
}

namespace fcutils{
namespace command{

// The process command list. A Byte identifying the action to be executed on a process
template <typename Data> struct wrapper{
	using data_type = Data;
	enum class action_id: data_type {
				// Remove sequence definition
				SEQDEL=static_cast<data_type>(0x50),
				// Start sequence definition
				SEQSTART=static_cast<data_type>(0x51),
				// End sequence definition
				SEQEND=static_cast<data_type>(0x52),
				// Start sequence definition
				SEQSTARTREC=static_cast<data_type>(0x53),
				// End sequence definition
				SEQENDREC=static_cast<data_type>(0x54),
				// Execute a sequence
				SEQEXEC=static_cast<data_type>(0x55),
				// Set a low priority alarm strategy
				LALSTRATEGY=static_cast<data_type>(0x56),
				// Set an high priority alarm strategy
				HALSTRATEGY=static_cast<data_type>(0x57),
				// Reset the counters for a low priority alarm strategy
				RESETLALSTRATEGY=static_cast<data_type>(0x58),
				// Reset the counters for an high priority alarm strategy
				RESETHALSTRATEGY=static_cast<data_type>(0x59),
				// Execute command or add it to a sequence
				EXECCOMMAND=static_cast<data_type>(0x60),
				// Wait
				WAIT=static_cast<data_type>(0x61),
				// Delete a low priority alarm strategy
				DELLALSTRATEGY=static_cast<data_type>(0x62),
				// Delete an high priority alarm strategy
				DELHALSTRATEGY=static_cast<data_type>(0x63),
				// Enable LOW alarm strategy
				ENABLELSTRATEGY=static_cast<data_type>(0x64),
				// Disable LOW alarm strategy
				DISABLELSTRATEGY=static_cast<data_type>(0x65),
				// Enable HIGH alarm strategy
				ENABLEHSTRATEGY=static_cast<data_type>(0x66),
				// Disable HIGH alarm strategy
				DISABLEHSTRATEGY=static_cast<data_type>(0x67),
				// Spawn a shell command
				SPAWNCMD=static_cast<data_type>(0x68),
				// Enable log file
				ENABLELOG=static_cast<data_type>(0x69),
				// Disable log file
				DISABLELOG=static_cast<data_type>(0x70),
				// Define a sequencer
				SEQUENCERREC=static_cast<data_type>(0x71),
				// Start an already registered sequencer
				SEQUENCERSTART=static_cast<data_type>(0x72),
				// Stop an already registered sequencer
				SEQUENCERSTOP=static_cast<data_type>(0x73),
				// Echo command
				ECHOCMD=static_cast<data_type>(0x74),
				// Enable log on network
				NETENABLELOG=static_cast<data_type>(0x75),
				// Disable log on network
				NETDISABLELOG=static_cast<data_type>(0x76),
				// Remove sequencer definition
				SEQUENCERDEL=static_cast<data_type>(0x77),
				// Start all sequencers
				SEQUENCERSTARTALL=static_cast<data_type>(0x78),
				// Stop all sequencers
				SEQUENCERSTOPALL=static_cast<data_type>(0x79),
				// Delete all sequencers
				SEQUENCERDELALL=static_cast<data_type>(0x80),
				// Change SIP routing
//				CHANGESIPROUTING=static_cast<data_type>(0x81),
				// Add consumer
				CONSUMERADD=static_cast<data_type>(0x82),
				// Remove consumer
				CONSUMERDEL=static_cast<data_type>(0x83),
				// Enable syslog on network
				SYSLOGENABLE=static_cast<data_type>(0x84),
				// Disable log on network
				SYSLOGDISABLE=static_cast<data_type>(0x85),
				// Print sequences
				SEQUENCEPRINT=static_cast<data_type>(0x86),
				// Print sequencers
				SEQUENCERPRINT=static_cast<data_type>(0x87)
	};
};

template <typename D> using enum_action_id_type = typename wrapper<D>::action_id;


template <typename Data> struct wrapper_duration_unit {
	using data_type = Data;
	enum class duration_unit: data_type {
			// Count type, just an unsigned digit
			COUNTS=static_cast<data_type>(0x01),
			// Milliseconds
			MSEC=static_cast<data_type>(0x02),
			// Milliseconds
			SEC=static_cast<data_type>(0x03),
			// Milliseconds
			MIN=static_cast<data_type>(0x04),
			// Milliseconds
			HOUR=static_cast<data_type>(0x05)
	};
};

template <typename D> using enum_duration_unit_type = typename wrapper_duration_unit<D>::duration_unit;
using duration_unit_type	= enum_duration_unit_type<std::uint8_t>;

template <typename Id, typename Action> 	struct id_and_action{
	using id_type=Id;
	using action_type=Action;
	id_type id{};
	action_type action{};
};

const std::size_t SEQ_LABEL_TYPE{4};
// The sequencer info to be recorded.
// Sequence and Sequencer IDs are array of 4 chars
template <typename DurationUnit=std::uint8_t, std::size_t LabelSize=SEQ_LABEL_TYPE> struct seqinfo{
	enum {label_size=LabelSize};
	using duration_unit_type = DurationUnit;
	// 4 chars for the sequencer info
	char id[LabelSize];
	// 4 chars for the sequence ID
	char seqid[LabelSize];
	// A byte for the unit of time
	DurationUnit tunit;
	// Two bytes for the time value
	std::uint16_t tval;
};

using sequencer_info_type	= seqinfo<duration_unit_type,SEQ_LABEL_TYPE>;

// The network logger info type
struct nloginfo{
	std::uint8_t address[4]; // The IP address
	std::uint16_t port; // The port
	char transport; // The transport: t or u
	char padding; // An extra char to insure padding to an even number of bytes
	explicit operator std::string() const {
		std::string ss;
		ss+=std::to_string(address[0]);
		ss+='.';
		ss+=std::to_string(address[1]);
		ss+='.';
		ss+=std::to_string(address[2]);
		ss+='.';
		ss+=std::to_string(address[3]);
		ss+=':';
		ss+=std::to_string(port);
		ss+=' ';
		ss+=transport;
		return ss;
	}
};

using nlog_info_type	= nloginfo;


///
/// Command format: ID, payload
///  Delete sequence 			: 0x50, a null terminated string with the sequence name
///  Start sequence  			: 0x51, no payload. This is just a flag to mark the begining of a sequence to be executed
///  End sequence	 			: 0x52, end of a sequence definition and execute it
///  Start sequence recording	: 0x53, a null terminated string with the sequence name
///  End sequence recording		: 0x54, end sequence recording
///
template <typename Process, bool DEBUG=false> auto decode_process_command(std::string msg, Process & p){
	using data_type 			= typename Process::data_type;
	using id_type				= typename Process::id_type;
	using action_type			= typename Process::action_type;
	using id_and_action_type 	= id_and_action<id_type, action_type>;
	using action_id_type		= enum_action_id_type<action_type>;
	using deserializer_type		= fcutils::serialization::deserialize_repr;
	using sequence_id_type		= typename Process::sequence_id_type;
	using sequencer_id_type		= typename Process::sequencer_id_type;
	using duration_type			= typename Process::duration_type;
	using return_type			= std::tuple<bool,bool,std::string>;

	int cmdsize=-1;
	// A flag to enable command execution. Is true if the command is not a command for the process
	bool cmdexec{false}, cmdfound{false};
	// Deserialize the process ID and the actions. These are the first two bytes of the message
	id_and_action_type dec_id_action;
	deserializer_type deserializer;
	// Deserialize the process ID and the action, use the automatic deserialization option using a representation deserializer
	// lastptr is the first pointer after the bytes used to deserialize dec_id_action
	auto [lastptr,val]=deserializer.deserialize(&dec_id_action,msg.c_str());
	// Check if the process_id is the same as the one in the message
	if constexpr(DEBUG) {
		fcutils::print_vals((std::cout<<std::hex<<std::showbase),static_cast<std::uint32_t>(dec_id_action.id),static_cast<std::uint32_t>(dec_id_action.action));
		std::cout<<std::dec<<std::noshowbase;
	}
	if(p.check_process_id(dec_id_action.id)) {
		// The ID is the same as the process, so we can proceed checking for commands
		if constexpr(DEBUG) { std::cout << p.progname << " right proc_id found !!!" << std::endl; }
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQDEL) )  ){
			// a sequence DELETE is found.
			// Extract the sequence name (ID) from the message and remove it
			// lastptr stores the last pointer after the procid and the
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string seqname{citer,msg.cend()};
			//			sequence_id_type seqname;
			//			deserializer.deserialize()
			if constexpr(DEBUG) {std::cout << p.progname << " A delete sequence found. Sequence ID: " << seqname << std::endl;}
			// remove the sequence
			p.delete_sequence(seqname);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQSTART) ) ){
			// a sequence is starting just start recording a sequence with name ANOMSEQ
			if constexpr(DEBUG) {std::cout << p.progname << " A start of sequence found" << std::endl;}
			p.start_sequence();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQEND) )  ){
			if constexpr(DEBUG){std::cout << p.progname << " An end of sequence found. Executing the sequence ..." << std::endl;}
			// end the sequence recording, execut it and delete
			p.end_sequence();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQSTARTREC) ) ){
			// Extract the sequence name (ID) from the message and mark the start of the sequence recording
			// lastptr stores the last pointer after the procid and the
			// reset the command size to avoid execution to the serial
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string seqname{citer,msg.cend()};
			if constexpr(DEBUG) {std::cout << p.progname << " A start of sequence recording found. Sequence ID: " << seqname << std::endl;}
			p.startrec_sequence(seqname);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQENDREC) )  ){
			if constexpr(DEBUG){ std::cout << p.progname << " An end of sequence recording found. " << std::endl; }
			p.endrec_sequence();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::WAIT) ) ){
			// Check if any duration has been specified
			// Create the converter
			// The wait command has two bytes for the duration, as unsigned integer
			std::uint16_t dint{0u};
			// decode the two bytes after the command
			std::from_chars(lastptr,lastptr+sizeof(dint*BYTE_SIZE_IN_CHAR),dint,16);
//			deserializer.deserialize(&dint,lastptr);
			if constexpr(DEBUG){ std::cout << p.progname << " A wait found for " << dint << " time units" << std::endl;}
			p.sleep_for(dint);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQEXEC) ) ) {
			// execute a sequence
			// Extract the sequence name (ID) from the message and mark the start of the sequence recording
			// lastptr stores the last pointer after the procid and the
			// reset the command size to avoid execution to the serial
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string seqname{citer,msg.cend()};
			if constexpr(DEBUG) {std::cout << p.progname << " An execute sequence found. Sequence ID: " << seqname << std::endl;}
			p.execute_sequence(seqname);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::ENABLELOG) ) ) {
			// Enable a log file to the std output
			// Open a log file with the ID and time stamps
			if constexpr(DEBUG) {std::cout << p.progname << " An enable log found. Duplicating standard output on a log file." << std::endl; }
			p.enable_log(std::cout);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::DISABLELOG) ) ) {
			// Disable a log file of the std output
			p.disable_log(std::cout);
			if constexpr(DEBUG){ std::cout << p.progname << " A disable log found. Disable duplication of standard output on a log file." << std::endl; }
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERREC) ) ) {
			// Create a new sequencer and store it into the map
			// Start defining the sequencer info
			// Use the default label size
			sequencer_info_type sinfo;
			// Deserialize the sequencer information
			std::copy(lastptr,lastptr+sequencer_info_type::label_size,sinfo.id);
			lastptr=lastptr+sequencer_info_type::label_size;
			std::copy(lastptr,lastptr+sequencer_info_type::label_size,sinfo.seqid);
			lastptr=lastptr+sequencer_info_type::label_size;
			auto [ptr,vptr]=deserializer.deserialize(&sinfo.tunit,lastptr);
			lastptr=ptr;
			deserializer.deserialize(&sinfo.tval,lastptr);
			std::string seqcerid{sinfo.id,sequencer_info_type::label_size};
			std::string seqid{sinfo.seqid,sequencer_info_type::label_size};
			if constexpr(DEBUG) {
				std::cout << p.progname << " A record a new sequencer found. ID: "
					<< seqcerid
					<< ", Sequence to be sequenced ID: "
					<< seqid
					<< ", Time type: "<< static_cast<unsigned int>(sinfo.tunit)
					<< ", Time value: " << sinfo.tval <<  std::endl;
			}
			// Create the time unit accordingly. Thanks to C++17 relaxed enum creator
			duration_unit_type tu{sinfo.tunit};
			switch(tu){
			case duration_unit_type::COUNTS:
				p.add_sequencer(seqcerid,sinfo.tval,seqid);
			break;
			case duration_unit_type::MSEC:
			{
				std::chrono::milliseconds d{sinfo.tval};
				p.add_sequencer(seqcerid,d,seqid);
			}
			break;
			case duration_unit_type::SEC:
			{
				std::chrono::seconds d{sinfo.tval};
				p.add_sequencer(seqcerid,d,seqid);
			}
			break;
			case duration_unit_type::MIN:
			{
				std::chrono::minutes d{sinfo.tval};
				p.add_sequencer(seqcerid,d,seqid);
			}
			break;
			case duration_unit_type::HOUR:
			{
				std::chrono::hours d{sinfo.tval};
				p.add_sequencer(seqcerid,d,seqid);
			}
			break;
			default:
				std::cout << p.progname << " Error !!!! Units is not defined for sequencer :"
				<< seqcerid
				<< ", Unit: "
				<< static_cast<unsigned int>(sinfo.tunit)
				<< std::endl;
				break;
			}
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERSTART) ) ) {
			// Start sequencer
			// Decode the sequencer id
			sequencer_id_type sequencerid;
			if constexpr (std::is_same<fcutils::type_traits::remove_cvref_t<sequencer_id_type>, std::string>::value){
				// Create an iterator from the last pointer used fo decode dhe id and action
				std::string::const_iterator citer{lastptr};
				// decode the sequence name supposing is the rest of the message string
				sequencerid.append(citer,msg.cend());
				if constexpr(DEBUG) {std::cout << p.progname << " A start sequencer found. Sequencer ID: " << sequencerid << std::endl; }
			} else {
				// Deserialize the sequencer information
				deserializer.deserialize(&sequencerid,lastptr);
				if constexpr(DEBUG) {std::cout << p.progname << " A start sequencer found. Sequencer ID (hex): " << static_cast<std::size_t>(sequencerid) << std::endl; }
			}
			// Create the converter and transform the string into an array of bytes.
			p.start_sequencer(sequencerid);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERSTOP) ) ) {
			// Stop sequencer
			// Decode the sequencer id
			sequencer_id_type sequencerid;
			if constexpr (std::is_same<fcutils::type_traits::remove_cvref_t<sequencer_id_type>, std::string>::value){
				// Create an iterator from the last pointer used fo decode dhe id and action
				std::string::const_iterator citer{lastptr};
				// decode the sequence name supposing is the rest of the message string
				sequencerid.append(citer,msg.cend());
				if constexpr(DEBUG){ std::cout << p.progname << " A stop sequencer found. Sequencer ID: " << sequencerid << std::endl; }
			} else {
				// Deserialize the sequencer information
				deserializer.deserialize(&sequencerid,lastptr);
				if constexpr(DEBUG) {std::cout << p.progname << " A stop sequencer found. Sequencer ID (hex): " << sequencerid << std::endl; }
			}
			// Create the converter and transform the string into an array of bytes.
			p.stop_sequencer(sequencerid);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERSTARTALL) ) ) {
			// Start all sequencers
			if constexpr(DEBUG) {std::cout << p.progname << " A start all sequencers found." << std::endl; }
			p.start_all_sequencer();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERSTOPALL) ) ) {
			// Stop all sequencers
			if constexpr(DEBUG){ std::cout << p.progname << " A stop all sequencers found." << std::endl; }
			p.stop_all_sequencer();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERDEL) ) ) {
			// Delete sequencer
			// Decode the sequencer id
			sequencer_id_type sequencerid;
			if constexpr (std::is_same<fcutils::type_traits::remove_cvref_t<sequencer_id_type>, std::string>::value){
				// Create an iterator from the last pointer used fo decode dhe id and action
				std::string::const_iterator citer{lastptr};
				// decode the sequence name supposing is the rest of the message string
				sequencerid.append(citer,msg.cend());
				if constexpr(DEBUG) {std::cout << p.progname << " A delete sequencer found. Sequencer ID: " << sequencerid << std::endl; }
			} else {
				// Deserialize the sequencer information
				deserializer.deserialize(&sequencerid,lastptr);
				if constexpr(DEBUG) {std::cout << p.progname << " A delete sequencer found. Sequencer ID (hex): " << sequencerid << std::endl;}
			}
			p.delete_sequencer(sequencerid);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERDELALL) ) ) {
			// Delete all sequencers
			if constexpr(DEBUG){ std::cout << p.progname << " A delete all sequencers found." << std::endl; }
			p.delete_all_sequencer();
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SPAWNCMD) ) ) {
			// Just spawn the command send in the string
			// Store the command in a temporary string
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string cmdspawn{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " A spawn command found. Command: " << cmdspawn << std::endl;}
			p.spawn_cmd(cmdspawn);
			cmdfound=true;
		}
		//
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::ECHOCMD) ) ) {
			//	if(msg.find(PROCECHOCMD)!=std::string::npos ) {
			// Just echo the msg received, as it is, adding the PROC ID
			if constexpr(DEBUG){std::cout << p.progname << " Echo command: " << msg << std::endl;}
			msg=p.process_echo(msg);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::NETENABLELOG) ) ) {
			// Duplicate the std output to an address, port and transport
			// extract the ip address, port and transport
			nlog_info_type nlinfo;
			// Deserialize the info
			deserializer.deserialize(&nlinfo,lastptr);
			// Convert it to a string
			std::string ss{static_cast<std::string>(nlinfo)};
			if constexpr(DEBUG) {std::cout << p.progname << " An enable network duplicator found. Duplicating standard output on address: "<< ss.c_str() << std::endl;}
			p.enable_netlog(ss);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::NETDISABLELOG) ) ) {
			// Disable duplication of the std output to an address, port and transport
			// extract the ip address, port and transport
			nlog_info_type nlinfo;
			// Deserialize the info
			deserializer.deserialize(&nlinfo,lastptr);
			// Convert it to a string
			std::string ss{static_cast<std::string>(nlinfo)};
			if constexpr(DEBUG){
				std::cout << p.progname << " A disable network duplicator found. Duplicating standard output on address: "<< ss.c_str() << std::endl;
			}
			p.disable_netlog(ss);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::CONSUMERADD) ) ) {
			//	if(msg.find(CONSUMERADD)!=std::string::npos ) {
			// Add a consumer
			// String is passed to a specialized function
			// Just strip away the header and the command ID
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			std::string s{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " A new consumer to be added, options: "<< s << std::endl;}
			//		std::pair<std::string, consumermanager_type::consumer_ptr_type> temp=consumer::make_consumer(s);
			p.add_consumer(
					[](const std::string & str, auto & manager){return fcutils::consumer::decode_consumer_command<data_type>(str, manager);},
					s);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::CONSUMERDEL) ) ) {
			//	if(msg.find(CONSUMERADD)!=std::string::npos ) {
			// Delete a consumer
			// Just the key is needed
			// Just strip away the header and the command ID
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			std::string s{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " A delete consumer found, key: "<< s << std::endl;}
			p.delete_consumer(s);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SYSLOGENABLE) ) ) {
			// Duplicate the std output to an address, port and transport
			// extract the ip address, port and transport
			nlog_info_type nlinfo;
			// Deserialize the info
			deserializer.deserialize(&nlinfo,lastptr);
			// Convert it to a string
			std::string ss{static_cast<std::string>(nlinfo)};
			if constexpr(DEBUG){std::cout << p.progname << " An enable SYSLOG duplicator found. Duplicating standard output on address: "<< ss.c_str() <<", using SYSLOG format"<< std::endl;}
			p.enable_syslog(ss);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SYSLOGDISABLE) ) ) {
			// Disable duplication of the std output to an address, port and transport
			// extract the ip address, port and transport
			nlog_info_type nlinfo;
			// Deserialize the info
			deserializer.deserialize(&nlinfo,lastptr);
			// Convert it to a string
			std::string ss{static_cast<std::string>(nlinfo)};
			if constexpr(DEBUG){
				std::cout << p.progname << " A disable SYSLOG duplicator found. Duplicating standard output on address: "<< ss.c_str() <<", using SYSLOG format"<< std::endl;}
			p.disable_syslog(ss);
			cmdfound=true;
		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::EXECCOMMAND) ) ) {
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string to_be_executed{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " An execute command found. Command: "<< to_be_executed << std::endl;}
			p.execute_cmd(to_be_executed);
			cmdfound=true;

		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCEPRINT) ) ) {
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string to_be_executed{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " An print sequence statistic command found. Command: "<< to_be_executed << std::endl;}
			p.print_sequences();
			cmdfound=true;

		}
		if( ( dec_id_action.action==static_cast<data_type>(action_id_type::SEQUENCERPRINT) ) ) {
			// Create an iterator from the last pointer used fo decode dhe id and action
			std::string::const_iterator citer{lastptr};
			// decode the sequence name supposing is the rest of the message string
			std::string to_be_executed{citer,msg.cend()};
			if constexpr(DEBUG){std::cout << p.progname << " An print sequencer statistic command found. Command: "<< to_be_executed << std::endl;}
			p.print_sequencers();
			cmdfound=true;

		}
		return return_type{cmdexec,cmdfound,msg};
	} //	if(p.check_process_id(dec_id_action.id)) {
	// if we are here, the process id is not the same as the process
	// The msg contains no command for the process, so process the cmd
	cmdexec=true;
	return return_type{cmdexec,cmdfound,msg};
}


} // namespace command
} // namespace fcutils




#endif // DECODE_PROCESS_COMMAND_HPP_ 
