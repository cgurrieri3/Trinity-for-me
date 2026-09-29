///
///
/// @brief A map type to wrap the different map types needed to hold values.
///
/// @file value_map.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 9 ago 2022 
///

#ifndef VALUE_MAP_HPP_
#define VALUE_MAP_HPP_
#include <iostream>
#include <unordered_map>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/join.hpp>

#include "fcutils/type_traits/type_traits.hpp"
#include "fcutils/monitor/favorite.hpp"
#include "fcutils/monitor/value.hpp"


namespace fcutils{
namespace monitor{

template <typename Data> class value_map{
public:
	using data_type				= Data;
	using value_type			= fcutils::monitor::value;
	using tree_type 			= boost::property_tree::ptree;
	using favorite_name_type	= std::string;
	using favorite_filename_type= std::string;
	using favorite_vector_type	= std::vector< std::string> ;

	std::string name{"Noname"};
	std::string info{"No info"};
	size_t precision{};
	size_t width{};
    favorite_vector_type favorites;

    void print(std::ostream & os=std::cout) const {
		os << " Favorite list name: " << name <<", info: "<< info <<'\n';
		os << " Floating point precision: " << precision <<", field width :" << width << '\n';
		os << " Favorite variables: "  << favorites.size() << ", names: \n";
		std::copy(favorites.begin(),
				  favorites.end(),
				  std::ostream_iterator<favorite_vector_type::value_type>(os," "));
		os << std::endl;
    }

    void print_all(std::ostream & os=std::cout) const{
    	print(os);
    	for( const auto & [key,v]: ivals_){
    		os<< "Key: " << key<< '\n';
    		v.print(os);
    	}
    	for( const auto & [key,v]: svals_){
    		os<< "Key: " << key<< '\n';
    		v.print(os);
    	}
    	for( const auto & [key,v]: imvals_){
    		os<< "Key: " << key<< '\n';
    		v.print(os);
    	}
    	for( const auto & [key,v]: smvals_){
    		os<< "Key: " << key<< '\n';
    		v.print(os);
    	}
    }

    auto get_values(const std::string & k ){
    	// First search in the map, if is found then return
    	auto v=svals_.equal_range(k);
    	if(v.first!=svals_.end()){ return v; }
    	// Use the equal_range returned object to store the iterator to the single object
    	v=smvals_.equal_range(k);
    	return v;
    }

    auto get_values(const int & k ){
    	// First search in the map, if is found then return
    	auto v=ivals_.equal_range(k);
    	if(v.first!=ivals_.end()){ return v; }
    	// Use the equal_range returned object to store the iterator to the single object
    	v=imvals_.equal_range(k);
    	return v;
    }

    auto set_values(const std::string & k, const data_type *d ){
    	// First search in the map, if is found then set the value and returns
    	auto v=svals_.equal_range(k);
    	if(v.first!=svals_.end()){
    		for(auto it=v.first;it!=v.second;++it) it->second.set_value(d);
    		return v;
    	}
    	// Use the equal_range returned object to store the iterator to the single object
    	v=smvals_.equal_range(k);
		for(auto it=v.first;it!=v.second;++it) it->second.set_value(d);
    	return v;
    }

    auto set_values(const int & k, const data_type *d ){
    	// First search in the map, if is found then return
    	auto v=ivals_.equal_range(k);
    	if(v.first!=ivals_.end()){
    		for(auto it=v.first;it!=v.second;++it) it->second.set_value(d);
    		return v;
    	}
    	// Use the equal_range returned object to store the iterator to the single object
    	v=imvals_.equal_range(k);
		for(auto it=v.first;it!=v.second;++it) it->second.set_value(d);
    	return v;
    }

    bool fetch( tree_type & pt, const std::string & favname=xml::slowcontrol::favorite::value::DEFAULTNAME){
    	bool success=true;
    	auto ptfavorites=fcutils::property_tree::find_by_value_optional(pt,favname);
    	if(ptfavorites) {
    		name=ptfavorites->get_child(xml::slowcontrol::favorite::field::NAMEOPT).get_value("noname");
    		info=ptfavorites->get_child(xml::slowcontrol::favorite::field::INFOOPT).get_value("noinfo");
    		precision=ptfavorites->get_child(xml::slowcontrol::favorite::field::PRECISIONOPT).get_value(PRECISION);
    		width=ptfavorites->get_child(xml::slowcontrol::favorite::field::WIDTHOPT).get_value(FIELDWIDTH);
    		for( auto & [key,fav]: *ptfavorites ){
    			if(auto nodename=fav.get_child_optional(xml::slowcontrol::favorite::field::NAMEOPT)) {
    							// favorite.name found
    				favorites.push_back(nodename->get_value("no name"));
    			}
    		}
    	} // if(ptfavorites)
		return success;
	}


	bool fetch_values( tree_type & pt, const std::string & valroot=xml::slowcontrol::VARIABLESROOT){
		bool success=true;
		auto ptvariables=pt.get_child_optional(valroot);
		if(ptvariables) {
			for( auto & v: favorites){
				boost::optional<tree_type& > ptvalue=fcutils::property_tree::find_by_value_optional(*ptvariables,v);
				if(ptvalue){
					// Value found. Create one, check key format and store it
					value_type temp;
					temp.fetch(*ptvalue);
					if(temp.key_type){
						switch(*temp.key_type){
						case fcutils::monitor::inouttype::INT:
						{
							auto k=std::get<int>(*temp.key_value);
							if(temp.multikey){
								imvals_.insert({k,temp});
							} else {
							    ivals_.insert({k,temp});
							}
						}
						break;
						case fcutils::monitor::inouttype::STRING:
						{
							auto k=std::get<std::string>(*temp.key_value);
							if(temp.multikey){
								smvals_.insert({k,temp});
							} else {
							    svals_.insert({k,temp});
							}
						}
						break;
						default:
							std::cout << " Value_map::fetch_values. Value: " << temp.name <<" Wrong key type: " << fcutils::type_traits::enum_to_underlying(*temp.key_type) <<std::endl;
						}
					} else {
						std::cout << " Value_map::fetch_values. Value: " << temp.name <<" Key type not found!!!! " << std::endl;
					}
				}
			}
		}// if(ptvariables)
		return success;
	}

	bool fetch_all( tree_type & pt, const std::string &favname=xml::slowcontrol::favorite::value::DEFAULTNAME){
		bool success=false;
		success=fetch(pt,favname);
		success=success&&fetch_values(pt);
		return success;
	}

	template<typename Key> auto get_full_range() const {
		if constexpr(std::is_same<Key,int>::value){
			return boost::join(ivals_,imvals_);
		}	else {
			return boost::join(svals_,smvals_);
		}
	}

	template<typename Key,bool Multi=false> auto get_begin() const {
		if constexpr(std::is_same<Key,int>::value){
			if constexpr(Multi){
				return imvals_.begin();
			} else {
				return ivals_.begin();
			}
		} else {
			if constexpr(Multi){
				return smvals_.begin();
			} else {
				return svals_.begin();
			}
		}
	}

	template<typename Key,bool Multi=false> auto get_end() const {
		if constexpr(std::is_same<Key,int>::value){
			if constexpr(Multi){
				return imvals_.end();
			} else {
				return ivals_.end();
			}
		} else {
			if constexpr(Multi){
				return smvals_.end();
			} else {
				return svals_.end();
			}
		}
	}

	template<typename Key,bool Multi=false> auto get_begin() {
		if constexpr(std::is_same<Key,int>::value){
			if constexpr(Multi){
				return imvals_.begin();
			} else {
				return ivals_.begin();
			}
		} else {
			if constexpr(Multi){
				return smvals_.begin();
			} else {
				return svals_.begin();
			}
		}
	}

	template<typename Key,bool Multi=false> auto get_end() {
		if constexpr(std::is_same<Key,int>::value){
			if constexpr(Multi){
				return imvals_.end();
			} else {
				return ivals_.end();
			}
		} else {
			if constexpr(Multi){
				return smvals_.end();
			} else {
				return svals_.end();
			}
		}
	}

private:
	std::unordered_map<int,value_type> ivals_;
	std::unordered_map<std::string,value_type> svals_;
	std::unordered_multimap<int,value_type> imvals_;
	std::unordered_multimap<std::string,value_type> smvals_;
};

} // namespace monitor
} // namespace fcutils


#endif // VALUE_MAP_HPP_ 
