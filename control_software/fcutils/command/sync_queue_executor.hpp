///
///
/// @brief An executor for the process_common. This executor push a message in a sync queue
///
/// @file sync_queue_executor.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 3 ott 2022 
///

#ifndef SYNC_QUEUE_EXECUTOR_HPP_
#define SYNC_QUEUE_EXECUTOR_HPP_
#include "fcutils/concurrency/sync_queue.hpp"

namespace fcutils{
namespace command{
// A simple executor to put a command in a sync_queue

template<typename Command, template <typename> class Queue=fcutils::concurrency::sync_queue > struct sync_queue_executor{
	using command_type 	= Command;
	using queue_type	= Queue<command_type>;
	using self_type 	= sync_queue_executor;
	explicit sync_queue_executor(const command_type & c, queue_type & q): cmd{c}, mq{q}{};
	sync_queue_executor(const self_type & sqe): cmd{sqe.cmd}, mq{sqe.mq}{};
	self_type & operator=(const self_type & pe) = delete;
	void operator()(){
		mq.put(cmd);
	}
	command_type cmd;
	queue_type mq;
};

} // Namespace command
} // Namespace fcutils

#endif // SYNC_QUEUE_EXECUTOR_HPP_ 
