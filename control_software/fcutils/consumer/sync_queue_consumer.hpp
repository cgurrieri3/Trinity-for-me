///
///
/// @brief A consumer that writes a message to a sync_queue.
///        The message is the serialization of the data payload into a std::string
///
/// @file sync_queue_consumer.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 12 lug 2022 
///

#ifndef SYNC_QUEUE_CONSUMER_HPP_
#define SYNC_QUEUE_CONSUMER_HPP_
//#include <iostream>
#include <string>
#include <utility> // for std::move

#include "fcutils/concurrency/sync_queue.hpp"
#include "fcutils/string/char_to_hex.hpp"
#include "fcutils/consumer/consumer_base.hpp"
#include "fcutils/serialization/serializer.hpp"

namespace fcutils{
namespace consumer{

template <typename Data=char, typename Msg=std::string, typename Serializer=fcutils::serialization::serialize_repr,
		template<typename> class Queue=fcutils::concurrency::sync_queue, template <typename > class Checker = fcutils::checker::true_checker>
struct sync_queue_consumer: public consumer_base<Data,Checker>{
	using data_type		=Data;
	using message_type	=Msg;
	using queue_type	=Queue<message_type>;
	using checker_type 	=Checker<data_type>;
	using self_type		=sync_queue_consumer;
	using serializer_type= Serializer;

	sync_queue_consumer(queue_type & q): queue(q) {};

	void consume(const data_type * c, std::size_t  length) override {
		operator()(c,length);
	}

	self_type & operator()(const data_type * d, std::size_t length) override {
		serializer_type serializer;
		serializer.serialize(d,length);
		std::string msg{serializer.data()};
//		std::string msg{fcutils::string::char_to_hex(d,length,"")};
//		std::cout << " A message received: " << msg <<'\n';
		queue.put(std::move(msg));
		return *this;
	};

	queue_type & queue;
//	const char eotx{0x04};
};

using sync_queue_consumer_repr=sync_queue_consumer<char, std::string, fcutils::serialization::serialize_repr, fcutils::concurrency::sync_queue, fcutils::checker::true_checker>;
using sync_queue_consumer_str=sync_queue_consumer<char, std::string, fcutils::serialization::serialize_string, fcutils::concurrency::sync_queue, fcutils::checker::true_checker>;

} // namespace consumer
} // namespace fcutils



#endif // SYNC_QUEUE_CONSUMER_HPP_ 
