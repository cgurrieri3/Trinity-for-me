///
///
/// @brief The message wrapping a command
///
/// @file message.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 24 set 2022 
///

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_
#include <array>

namespace fcutils{
namespace command{
	using data_type=char;
	const std::size_t DATASIZE{sizeof(data_type)};

	namespace header{
		// The header size, in byte
		const std::size_t HEADERSIZE{8};
		/// Define the position in the header
		/// These are the byte index in an array of char
		/// First an header ID
		const std::size_t HEADERID{0};
		/// The the instrument ID
		const std::size_t DETECTORID{HEADERID+DATASIZE};
		/// Then the System ID in a detector
		const std::size_t SYSTEMID{DETECTORID+DATASIZE};
		/// Then the subsystem ID
		const std::size_t SUBSYSTEMID{SYSTEMID+DATASIZE};
		/// Then the command size. Please note that two words are dedicated for this value.
		const std::size_t COMMANDSIZE{SUBSYSTEMID+DATASIZE};
		/// Then the command format. Note two wordsize added
		const std::size_t COMMANDFORMAT{COMMANDSIZE+2*DATASIZE};
		/// Then the command options.
		const std::size_t COMMANDOPTS{COMMANDFORMAT+DATASIZE};
		/// The header type itself. Just a C-style array of HEADERSIZE
		struct header_type{
			data_type header[HEADERSIZE];
			/// Implicit conversion to a pointer of type data_type.
			operator data_type*() { return header;}
			/// Random access operators
			data_type & operator [](std::size_t i){ return header[i]; }
			const data_type & operator [](std::size_t i) const { return header[i]; }
		};

		/// Let's define some operators
		/// An header is considered equal, if HEADERID, DETECTORID and SYSTEMID are equal. The rational is that a process for each system should run.
		bool operator==(const header_type & a, const header_type & b){
			return ( (a[HEADERID]==b[HEADERID]) && (a[DETECTORID]==b[DETECTORID]) && (a[SYSTEMID]==b[SYSTEMID]) );
		}

	} // namespace header

	namespace trailer{
		// The header size, in byte
		const std::size_t TRAILERSIZE{8};
		/// Define the position in the header
		/// These are the byte index in an array of char
		/// First an header ID
		const std::size_t TRAILERID{0};
		/// The the instrument ID
		const std::size_t DETECTORID{TRAILERID+DATASIZE};
		/// Then the System ID in a detector
		const std::size_t SYSTEMID{DETECTORID+DATASIZE};
		/// Then the subsystem ID
		const std::size_t SUBSYSTEMID{SYSTEMID+DATASIZE};
		/// Then the command size. Please note that two words are dedicated for this value.
		const std::size_t COMMANDSIZE{SUBSYSTEMID+DATASIZE};
		/// Then the command format. Note two wordsize added
		const std::size_t CRC16H{COMMANDSIZE+2*DATASIZE};
		/// Then the command options.
		const std::size_t CRC16L{CRC16H+DATASIZE};
		/// The header type itself. Just a C-style array of HEADERSIZE
		struct trailer_type{
			data_type trailer[TRAILERSIZE];
			/// Implicit conversion to a pointer of type data_type.
			operator data_type*() { return trailer;}
			/// Random access operators
			data_type & operator [](std::size_t i){ return trailer[i]; }
			const data_type & operator [](std::size_t i) const { return trailer[i]; }
		};

		/// Let's define some operators
		/// An header is considered equal, if HEADERID, DETECTORID and SYSTEMID are equal. The rational is that a process for each system should run.
		bool operator==(const trailer_type & a, const trailer_type & b){
			return ( (a[TRAILERID]==b[TRAILERID]) && (a[DETECTORID]==b[DETECTORID]) && (a[SYSTEMID]==b[SYSTEMID]) );
		}

	} // namespace header

} // namespace command
} // namespace fcutils




#endif // MESSAGE_HPP_ 
