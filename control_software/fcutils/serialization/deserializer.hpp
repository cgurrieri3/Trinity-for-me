///
///
/// @brief A simple deserializer
///
/// @file deserializer.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 8 feb 2022 
///

#ifndef DESERIALIZER_HPP_
#define DESERIALIZER_HPP_

#include <string>
#include <algorithm>
#include <type_traits>
#include "string/hex_to_int.hpp" // for int_to_hex functions

namespace fcutils{
namespace serialization{


struct deserialize_representation{
	template<typename ToBeDeserialized, typename Data, typename SizeType > auto operator()(ToBeDeserialized *s,
			const Data * source, SizeType length){
		auto pt=reinterpret_cast<unsigned char*>(s);
//		auto pt=reinterpret_cast<Data*>(s);
		// Return an tuple with the last pointer to the source and first pointer to ToBeSerialized
		return fcutils::string::hex_to_int(pt,source,length);
	}
};

/// Same as above but will use a reverse buffer, i.e. will start deserialization from the last to the first.
struct deserialize_reverse_representation{
	template<typename ToBeDeserialized, typename Data, typename SizeType > auto operator()(ToBeDeserialized *s,
			const Data * source, SizeType length){
		auto pt=reinterpret_cast<unsigned char*>(s);
//		auto pt=reinterpret_cast<Data*>(s);
		// Return an tuple with the last pointer to the source and first pointer to ToBeSerialized
		return fcutils::string::hex_to_int(pt,source,length, true);
	}
};

struct just_copy{

	template<typename ToBeDeserialized, typename Data> auto operator()(ToBeDeserialized *s, const Data * source, fcutils::string::type_size<ToBeDeserialized,2ul> length){
		return operator()(s,source,static_cast<std::size_t>(length));
	}

	template<typename ToBeDeserialized, typename Data> auto operator()(ToBeDeserialized *s, const Data * source, std::size_t length){
		// Special case for string
		if constexpr( std::is_same<ToBeDeserialized,std::string>::value && std::is_same<Data,char>::value){
			// Return the string itself in case s is a std::string and source is a char array
			return s->append(source, length);
		} else {
			// Return the latest output iterator in s
			return std::copy_n(source,length,reinterpret_cast<Data *>(s));
		}
	}
};

template <typename Policy> class deserializer{
public:
	using policy_type=Policy;

	template<typename ToBeDeserialized, typename Data>
	auto deserialize(ToBeDeserialized * s, const Data * source, std::size_t length ){
		return deserialize_(s, source, length);
	}

	template<typename ToBeDeserialized, typename Data, std::size_t ByteCharSize=2ul >
	auto deserialize(ToBeDeserialized * s, const Data * source) {
		return deserialize_(s, source, fcutils::string::type_size<ToBeDeserialized,ByteCharSize>{});
	}
private:
	policy_type deserialize_;
};

using deserialize_repr=deserializer<deserialize_representation>;
using deserialize_reverse_repr=deserializer<deserialize_reverse_representation>;
using deserialize_string=deserializer<just_copy>;

} // namespace serialization
} // namespace fcutils






#endif // DESERIALIZER_HPP_ 
