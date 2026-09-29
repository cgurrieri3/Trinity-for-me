///
///
/// @brief A new file
///
/// @file TEST_PROCESS.CPP
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 22 mar 2022 
///

#include <iostream>
#include <memory>

#include "fcutils/command/sequence.hpp"
#include "fcutils/command/sequencer.hpp"
#include "fcutils/command/sequencemanager.hpp"
#include "fcutils/command/sequencermanager.hpp"
#include "fcutils/consumer/consumer.hpp"
#include "fcutils/message/time_ht.hpp"
#include "fcutils/checker/checker.hpp"
#include "fcutils/command/sequencer.hpp"
#include "fcutils/command/sequencermanager.hpp"
#include "fcutils/command/process_common.hpp"
#include "fcutils/command/decode_process_command.hpp"
#include "fcutils/serialization/serializer.hpp"
#include "fcutils/command/message.hpp"


struct executor{
	explicit executor( const std::string & s): cmd{s}{};
	executor & operator=(const std::string & s) { cmd=s; return *this;}
	void operator()(){
		std::cout <<" THis is the command: "<< cmd << std::endl;
	}
	std::string cmd;
};

template <typename Data> using time_header_t=fcutils::message::time_ht<Data, 15,fcutils::message::TIME_HT_OPTION::AT_END>;
template<typename Action> using sequencer_type=fcutils::command::sequencer<Action>;


int main(){

//	using data_type 				= char;
	using data_type					= fcutils::command::data_type;
	using sequence_type 			= fcutils::command::sequence<std::string>;
	using manager_type 				= fcutils::command::seqmanager<sequence_type>;
//	using consumer_manager_type=fcutils::consumer::base_consumermanager<base_consumer,fcutils::consumer::simple_pool,time_header_t>;
	using fcutils_consumer_base_type= fcutils::consumer::consumer_base<data_type>;
	using net_consumer_manager_type	= fcutils::consumer::base_consumermanager<fcutils_consumer_base_type>;
	using udp_consumer_type 		= fcutils::consumer::udp_consumer<data_type>;
	using tcp_consumer_type 		= fcutils::consumer::tcp_consumer<data_type>;
	using binaryfile_consumer_type 	= fcutils::consumer::binaryfile_consumer<data_type>;
	using textfile_consumer_type 	= fcutils::consumer::textfile_consumer<data_type,fcutils::checker::true_checker,'\n'>;
	using printbinary_consumer_type = fcutils::consumer::printbinary_consumer<data_type>;
	using printtext_consumer_type	= fcutils::consumer::printtext_consumer<data_type>;
	using count_consumer_type 		= fcutils::consumer::count_consumer<data_type>;
	using print_stats_consumer_type = fcutils::consumer::print_stats_consumer<data_type>;
	using sequencer_base_type 		= fcutils::command::base_sequencer;
	using sequencermanager_type 	= fcutils::command::sequencermanager<sequencer_type, sequencer_base_type>;
	using logger_type				= fcutils::iostream::ostream_logger;
	using id_type					= fcutils::command::header::header_type;
	using action_type				= fcutils::command::data_type;
	using duration_type				= std::chrono::milliseconds;
	using process_type 				= fcutils::command::process_common<manager_type,
																		sequencermanager_type, net_consumer_manager_type, logger_type, data_type,
																		duration_type, id_type, action_type>;
//	using id_type					= process_type::id_type;
//	using action_type				= process_type::action_type;
	using duration_unit_type		= fcutils::command::duration_unit_type;
	using sequencer_info_type		= fcutils::command::sequencer_info_type;

//	process_type::id_and_action_type test{static_cast<data_type>(0xFA),static_cast<data_type>(0)};
//	fcutils::command::id_and_action<id_type,action_type> test{static_cast<id_type>(0xFAU),static_cast<id_type>(0U)};
	fcutils::command::id_and_action<id_type,action_type>
	test{ {static_cast<data_type>(0xFA),static_cast<data_type>(0xB0),static_cast<data_type>(0xAF),static_cast<data_type>(0xB1)},
		static_cast<data_type>(0)};
	process_type proccommon{test.id,"log"};

	fcutils::serialization::serialize_repr b;

	for( std::uint16_t i=0x50; i<0x86; ++i){
		test.action=static_cast<decltype(test.action)>(i);
		b.clear();
		b+="0x";
		b.serialize(&test);
		if( (i==0x50)||(i==0x53)) b+="Del!";
		if( i==0x61 ){
			std::uint16_t d{123u};
			b.serialize(&d);
		}
		if( i==0x68 ) b+="ls -l";
		if( i == 0x71 ){
			sequencer_info_type sinfo{{'1','2','3','4'},{'D','e','l','!'},duration_unit_type::SEC,100};
			b+=std::string{sinfo.id,fcutils::command::seqinfo<>::label_size};
			b+=std::string{sinfo.seqid,fcutils::command::seqinfo<>::label_size};
			b.serialize(&sinfo.tunit);
			b.serialize(&sinfo.tval);
		}
		if( ( i == 0x75 ) || (i == 0x76) ){
			struct nloginfo{
				std::uint8_t address[4];
				std::uint16_t port{8888};
				char transport{'u'};
				char padding;
			};
			nloginfo nlinfo{{255u,255u,255u,254u}};
			b.serialize(&nlinfo);
		}
		if( ( i == 0x84 ) || (i == 0x85) ){
			struct nloginfo{
				std::uint8_t address[4];
				std::uint16_t port{6514};
				char transport{'l'};
				char padding;
			};
			nloginfo nlinfo{{192u,167u,91u,126u}};
			b.serialize(&nlinfo);
		}
		std::cout << " Command serialization: " << b.data() << ". \n";
//		proccommon.process_msg(b.data());
		auto [cmdexc,cmdfound,cmd]=fcutils::command::decode_process_command(b.data(),proccommon);
		std::cout << std::boolalpha << " Decode process result: cmdexec: " << cmdexc << ", cmd found: " << cmdfound <<", command: " << cmd << std::noboolalpha << std::endl;
	}

//	fcutils::command::decode_process_command cmd_decoder;
	// Test a wrong process id
//	test.id=id_type{0xffU};
//	test.action=id_type{0xffU};
	test.id[fcutils::command::header::DETECTORID]=0xFF;
	test.action=0xFF;
	b.clear();
	b.serialize(&test);
	std::cout << " Command serialization: " << b.data() << ". \n";
//		proccommon.process_msg(b.data());
	auto [cmdexc,cmdfound,cmd]=fcutils::command::decode_process_command(b.data(),proccommon);
	std::cout << std::boolalpha << " Decode process result: cmdexec: " << cmdexc << ", cmd found: " << cmdfound <<", command: " << cmd << std::noboolalpha << std::endl;

	return 0;
}


