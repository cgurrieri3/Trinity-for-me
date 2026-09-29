///
///
/// @brief A new file
///
/// @file consumer.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 9 apr 2022 
///

#ifndef MONITOR_CONSUMER_HPP_
#define MONITOR_CONSUMER_HPP_

#include "fcutils/checker/true_checker.hpp"
#include "fcutils/consumer/consumer_base.hpp"
#include "fcutils/terminal/ansi.hpp"
#include "fcutils/string/program_name_and_date.hpp"

namespace fcutils{
namespace consumer{

template < template <typename >class  Screen, typename OptionTree, typename Data=unsigned char, template <typename > class Checker = fcutils::checker::true_checker, bool Debug = false >
class monitor_consumer: public consumer_base<Data,Checker> {
public:
	using data_type				= Data;
	using self_type 			= monitor_consumer;
	using consumer_base_type	= consumer_base<Data,Checker>;
	using screen_type			= Screen<Data>;
	using tree_type				= OptionTree;
	using checker_type			= Checker<Data>;

	template <typename C=checker_type> monitor_consumer(screen_type & scmon, std::size_t skip=0ul, C && checker=fcutils::checker::true_checker<data_type>{}):
		consumer_base_type{std::forward<checker_type>(checker)}, scmon_{scmon}, skip_{skip}
	{
			counter_.set_modulo(progress_.size());
	};


	void consume(const data_type * c, size_t  length) override {
		operator()(c,length);
	}

	self_type & operator()(const data_type * c, size_t  length) override {
		// Check if the packet is the one handled by the checked
		if(consumer_base_type::checker_(c)){
			// Extract the packet beginning skipping a given number of bytes
			const data_type * pck=c+skip_;
			// Fill with new data only if is an hk packet
			scmon_.set_values(pck);
			// Just for debugging purpose, to avoid terminal resize during debugger session
			if constexpr (!Debug) {
				fcutils::monitor::adapt_terminal(scmon_,2);
				// Clear the screen and position the cursor in the left upper corner
				fcutils::terminal::ansi::clear();
				// Position cursor in the left upper corner
				fcutils::terminal::ansi::gotoxy(0,0);
			}
			// Print the screen values
			std::cout << scmon_ << std::endl;
				consumer_base_type::processed_+=length;
				++consumer_base_type::pckts_;
				std::cout << prg_name_date_ << " Packet processed: " << consumer_base_type::pckts_ << ", bytes processed: " << consumer_base_type::processed_
						<< " " << progress_[++counter_] << std::endl;
		}
		return *this;
	}


private:
	struct modulo_counter{
	private:
		std::size_t count_{0};
		std::size_t modulo_{6};
	public:
		modulo_counter() = default;
		modulo_counter(const modulo_counter & c )= default;
		modulo_counter( modulo_counter && c) = default;
		modulo_counter & operator=(const modulo_counter & c) = default;
		modulo_counter & operator=(modulo_counter && c) = default;
		explicit modulo_counter(std::size_t mod): modulo_{mod}{};
		auto & operator++(){
			++count_;
			count_=count_% modulo_;
			return *this;
		}
		const auto operator++(int){
			modulo_counter temp{*this};
			operator++();
			return temp;
		}
		void reset(){ count_=0;}
		void set_modulo(std::size_t mod){ modulo_=mod;}
		operator std::size_t() const { return count_; }
	};
	modulo_counter counter_;
	screen_type & scmon_;
	std::size_t skip_;
	const std::string progress_{"|+/+\\+"};
	fcutils::string::program_name_and_date prg_name_date_{program_invocation_short_name,""};

	// The time header pointer, handy to get some const ...
	void fill_data( data_type * d){
		for( auto & fav: scmon_.favorites){
			for ( auto & v: fav.values){
				v.set_value(d);
			}
		}
	}


//	std::string extract_string(data_ptr_type c, std::size_t length, std::size_t offset=3){
//		data_type * pckstart=c.get()+th.size();
//		// extract the two bytes in littleendian format
//		int pcksize=iterator::get_value(c.get()+offset, 2, 0, iterator::is_littleendian, int());
//		// check if payload length is the same of the transmitted size. That is an error to be fized on transmission side
//		if(pcksize==length) pcksize=length-th.size()-(origin_consumer_type::TRAILERSIZE+2);
//		std::string temp(pckstart,pcksize);
//		// Remove \n or \r in case
//		temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
//		temp.erase(std::remove(temp.begin(), temp.end(), '\r'), temp.end());
//		return temp;
//	}

};

} // namesapce monitor
} // namespace fcutils



#endif // MONITOR_CONSUMER_HPP_ 
