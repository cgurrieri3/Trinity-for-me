///
///
/// @brief To manage more than one message queue
///
/// @file posix_mq_manager.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 26 mag 2022 
///

#ifndef POSIX_MQ_MANAGER_HPP_
#define POSIX_MQ_MANAGER_HPP_

#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <type_traits>

#include "fcutils/concurrency/lockable_container.hpp"

namespace fcutils{
namespace system{


const std::string SPROGNAME(" PMQMANAGER -->> ");

template <typename MessageQueue, bool Debug = false > class posix_mq_manager{
public:
	using self_type			= posix_mq_manager;
	using mq_type			= MessageQueue;
	using id_type	 		= std::string;
	using mq_ptr_type		= std::shared_ptr<mq_type>;
	using mqmap_type		= fcutils::concurrency::lockable_container<std::map,id_type, mq_ptr_type>;
	using message_type		= typename mq_type::message_type;
	using descriptor_type	= typename mq_type::descriptor_type;
	using attribute_type	= typename mq_type::attribute_type;
	using permission_type	= typename mq_type::permission_type;
	using flag_type			= typename mq_type::flag_type;
	using priority_type		= typename mq_type::priority_type;
	using handler_type		= typename mq_type::handler_type;
	using sigevent_type		= typename mq_type::sigevent_type;

	posix_mq_manager() {};
	posix_mq_manager & operator=(const posix_mq_manager &) = default;

	posix_mq_manager( posix_mq_manager && ) noexcept = default;
	posix_mq_manager & operator= (posix_mq_manager && ) noexcept = default;

	// Remember that the manager owns the message queue. It must delete them. Let's relay on the sharable_pointer for that.
	~posix_mq_manager(){};
	// Add a new message queue with identifier id
//	void add_mq(const id_type & id){
//		// remember to add a check if an id already exists
//		// Please note that sharable_pointer will take care of deleting the pointer to the pair as soon all the references has been released.
//		mqmap_[id]=mq_ptr_type(new mq_type(id));
//	}
	template<typename ... Mqpars> void add_mq(const id_type & id, Mqpars && ... pars){
		// remember to add a check if an id already exists
		// Please note that sharable_pointer will take care of deleting the pointer to the pair as soon all the references has been released.
		mqmap_[id]=mq_ptr_type(new mq_type(id,std::forward<Mqpars>(pars) ... ));
	}
	// Remove a mq with identifier id
	bool remove_mq(const id_type & id){
		// Search for id and then delete
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()){
			// Found it delete the pointer and erase it
			mqmap_.erase(i);
			return true;
		}
		return false;
	}
	// Check if a sequence with a given ID is present
	bool is_stored(const id_type & id) const {
		auto i=mqmap_.find(id);
		return  i!=mqmap_.end();
	}


	std::size_t size(){ return mqmap_.size(); }
	auto find(const id_type & id) {
		return mqmap_.find(id);
	}

	const auto find(const id_type & id) const {
		return mqmap_.find(id);
	}

	auto & operator[]( const id_type & id){ return mqmap_[id]; }
	const auto & operator[]( const id_type & id) const { return mqmap_[id]; }
	auto & at(const id_type & id) { return mqmap_.at(id); }
	const auto & at(const id_type & id) const { return mqmap_.at(id); }

	// Open all message queues
	bool open( ) {
		bool test{true},t{false};
		for( const auto & [id,s]: mqmap_ ){
			t=s->open();
			test=t && test;
		}
		return test;
	}

	// open a given message queue
	bool open(const id_type & id) {
		// Search for it and open if present
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()) {
			// Found it. Open it.
			(i->second)->open();
			return true;
		}
		return false;
	}

	// Close all message queues
	bool close( ) {
		bool test{true},t{false};
		for( const auto & [id,s]: mqmap_ ){
			t=s->close();
			test=t && test;
		}
		return test;
	}

	// close a given message queue
	bool close(const id_type & id) {
		// Search for it and open if present
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()) {
			// Found it. Open it.
			(i->second)->close();
			return true;
		}
		return false;
	}
	// Send a message to a given message queue
	bool send(const id_type & id, const char * msg, std::size_t length, int priority=0){
		// Search for it and send message if present
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()) {
			bool success{false};
			// Found it. Send the message.
			success=(i->second)->send(msg,length,priority);
			return success;
		}
		return false;
	}
	// Send a message to a given message queue
	bool send(const id_type & id, const std::string & msg, priority_type priority=0){
		return send(id, msg.c_str(),msg.size(),priority);
	}

	// Send a message to all message queue
	bool broadcast(const char * msg, std::size_t length, int priority=0){
		bool test{true},t{false};
		for( const auto & [id,s]: mqmap_ ){
			t=s->send(msg,length,priority);
			test=t && test;
		}
		return test;
	}
	// Send a message to all message queue
	bool broadcast(const std::string & msg, priority_type priority=0){
		return broadcast(msg.c_str(),msg.size(),priority);
	}

	// Notify a given message queue
	bool notify(const id_type & id, handler_type f){
		// Search for it and notify if present
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()) {
			// Found it. Notify.
			(i->second)->notify(f);
			return true;
		}
		return false;
	}

	// Notify a given message queue
	bool notify(const id_type & id, sigevent_type & sev){
		// Search for it and notify if present
		auto i=mqmap_.find(id);
		if(i!=mqmap_.end()) {
			// Found it. Notify.
			(i->second)->notify(sev);
			return true;
		}
		return false;
	}
	// Notify all message queue
	bool notify(handler_type f){
		bool test{true},t{false};
		for( const auto & [id,s]: mqmap_ ){
			t=s->notify(f);
			test=t && test;
		}
		return test;
	}
	// Notify all message queue
	bool notify(sigevent_type & sev){
		bool test{true},t{false};
		for( const auto & [id,s]: mqmap_ ){
			t=s->notify(sev);
			test=t && test;
		}
		return test;
	}

	void print(std::ostream & o=std::cout){
		o << SPROGNAME << " list of posix message queues: \n";
		for( const auto & [id,s]: mqmap_ ){
			o << " ID: " << id << ", name: " << s->get_name() << '\n';
		}
		std::cout << std::endl;

	}

private:
	mqmap_type mqmap_;

};

} // system
} // fcutils


#endif // POSIX_MQ_MANAGER_HPP_ 
