///
///
/// @brief A checker for a given char
///
/// @file true_checker.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 28 ott 2021 
///

#ifndef CHAR_CHECKER_HPP_
#define CHAR_CHECKER_HPP_

#include "checker/base_checker.hpp"

namespace fcutils{
namespace checker{

// A char checker. It checks if a char is present. Usefull to check if an EOL, NL or \0 is present at the end of the data stream
template< typename Data=char, Data Checker='\0' , typename Return=bool, typename Key=char, bool Debug=false >
struct char_checker final: public base_checker<Data,Key,Return>{
	using self_type=char_checker;
	using data_type=Data;
	using base_type=base_checker<Data,Key,Return>;
	// Let's just inherit all the base_checker creators
//	using base_checker<Data,Key,Return>::base_checker;
//	using base_checker<Data,Key,Return>::operator=;

	char_checker(): checker_{Checker}{};

	char_checker(const char_checker & c): checker_{c.checker_}{};
	self_type & operator=(const char_checker & c){ checker_=c.checker_;};

	typename base_type::return_type operator()( const typename base_type::data_type * d) override {
		// We check if the last char in the d array is the checker_.
		// If is a checker returns false, i.e. do not read any more chars.
		return !(*d==checker_);
	}


	typename base_type::return_type operator()( const typename base_type::data_type * d, std::size_t l) override {
		// We check if the last char in the d array is the checker_.
		// If is a checker returns false, i.e. do not read any more chars.
		return !(*(d+l-1)==checker_);
	}

	void reset() override {};
	char get_checker()const{ return checker_; }

private:
	data_type checker_{Checker};

};

// Few useful aliases
template<typename Data=char> using nl_checker=char_checker<Data,'\n',bool,char,false>;
template<typename Data=char> using cr_checker=char_checker<Data,'\r',bool,char,false>;
template<typename Data=char> using null_checker=char_checker<Data,'\0',bool,char,false>;

} // namespace checker
} // namespace fcutils



#endif // CHAR_CHECKER_HPP_
