///
///
/// @brief A manager for the UDP beacon
///
/// @file udpbeaconmanager.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 17 set 2022 
///

#ifndef UDPBEACONMANAGER_HPP_
#define UDPBEACONMANAGER_HPP_

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <type_traits>
#include <utility>

//#include "fcutils/command/sequence.hpp"
#include "fcutils/concurrency/lockable_container.hpp"

namespace fcutils{
namespace networking{

const std::string UMPROGNAME(" UDPBEACONMANAGER -->> ");

template <typename Beacon, bool Debug = false > class udpbeaconmanager{
public:
	using self_type			= udpbeaconmanager;
	using beacon_type		= Beacon;
	using port_type	 		= typename beacon_type::port_type;
	using message_type		= typename beacon_type::message_type;
	using duration_type		= typename beacon_type::duration_type;
	using key_type			= port_type;
	using beaconmap_type	= fcutils::concurrency::lockable_container<std::unordered_map,key_type, beacon_type>;

	udpbeaconmanager() {};
	udpbeaconmanager & operator=(const udpbeaconmanager &) = default;

	udpbeaconmanager( udpbeaconmanager && ) noexcept = default;
	udpbeaconmanager & operator= (udpbeaconmanager && ) noexcept = default;

	// Remember that the manager owns the sequences. It must delete them. Let's relay on the sharable_pointer for that.
	~udpbeaconmanager(){};
	// Add a new sequence with identifier id
	template <typename ... Arg> void add_beacon(const key_type &id, Arg && ... args ){
		// Search for id and then delete
		auto [iter,flag] = beaconmap_.try_emplace(id,std::forward<Arg>(args)...);
		if(flag){
			std::cout << UMPROGNAME << " Creating the beacon :" << id << std::endl;
		} else {
			std::cout << UMPROGNAME << " Beacon :" << id << " already present  !"<< std::endl;
		}
//		auto i=beaconmap_.find(id);
//		if(i==beaconmap_.end()){
//			// Beacon is not present, create one
//			beaconmap_.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(std::forward<Arg>(args)...));
//			// Found it execute
//			if constexpr (Debug){
//				std::cout << UMPROGNAME << " Creating the beacon :" << id << std::endl;
//			}
//		} else {
//			// Found it execute
//			if constexpr (Debug){
//				std::cout << UMPROGNAME << " Beacon :" << id << " already present  !"<< std::endl;
//			}
//		}
	}

	// Remove a beacon with identifier id
	bool remove_beacon(const key_type & id){
		// Search for id and then delete
		auto i=beaconmap_.find(id);
		if(i!=beaconmap_.end()){
			// Found it delete the pointer and erase it
			beaconmap_.erase(i);
			// Found it execute
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Beacon :" << id << ", erased" << std::endl;
			}

			return true;
		}
		return false;
	}

	// Check if a beacon with a given ID is present
	bool is_stored(const key_type & id) const {
		auto i=beaconmap_.find(id);
		return  i!=beaconmap_.end();
	}

	// Start a given beacon
	bool start(const key_type & id) {
		// Search for it and execute if present
		auto i=beaconmap_.find(id);
		if(i!=beaconmap_.end()) {
			// Found it execute
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Starting the beacon :" << id << std::endl;
			}
			(i->second).start();
			return true;
		}
		return false;
	}

	// Start all beacons
	void start() {
		for( auto & [k,b]: beaconmap_) {
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Starting the beacon :" << k << std::endl;
			}
			b.start();
		}
	}

	// Stop a given beacon
	bool stop(const key_type & id) {
		// Search for it and execute if present
		auto i=beaconmap_.find(id);
		if(i!=beaconmap_.end()) {
			// Found it execute
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Stopping the beacon :" << id << std::endl;
			}
			(i->second).stop();
			return true;
		}
		return false;
	}

	// Stop all beacons
	void stop() {
		for( auto & [k,b]: beaconmap_) {
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Stopping the beacon :" << k << std::endl;
			}
			b.stop();
		}
	}

	template <typename M=message_type> bool set_message(const key_type & id, M && m){
		// Search for it and set message if present
		auto i=beaconmap_.find(id);
		if(i!=beaconmap_.end()) {
			// Found it execute
			if constexpr (Debug){
				std::cout << UMPROGNAME << " Setting the message for the beacon :" << id <<", message: "<<m<< std::endl;
			}
			(i->second).set_message(std::forward<M>(m));
			return true;
		}
		return false;
	}

	template <typename D=duration_type> bool set_wait(const key_type & id, D && d){
			// Search for it and set wait interval  if present
			auto i=beaconmap_.find(id);
			if(i!=beaconmap_.end()) {
				// Found it execute
				if constexpr (Debug){
					std::cout << UMPROGNAME << " Setting the duration for the beacon :" << id <<", message: "<<d<< std::endl;
				}
				(i->second).set_wait(std::forward<D>(d));
				return true;
			}
			return false;
	}


	std::size_t size(){ return beaconmap_.size(); }

	auto find(const key_type & id) {
		return beaconmap_.find(id);
	}

	const auto find(const key_type & id) const {
		return beaconmap_.find(id);
	}

	auto & operator[]( const key_type & id){ return beaconmap_[id]; }
	const auto & operator[]( const key_type & id) const { return beaconmap_[id]; }
	auto & at(const key_type & id) { return beaconmap_.at(id); }
	const auto & at(const key_type & id) const { return beaconmap_.at(id); }

	void print(std::ostream & o=std::cout ) const {
		for( const auto & [id,b]: beaconmap_ ){
			std::cout << UMPROGNAME << " beacon id: "<< id << '\n';
			b.print();
		}
		std::cout << std::endl;
	}

private:
	beaconmap_type beaconmap_;
};

}
}



#endif // UDPBEACONMANAGER_HPP_ 
