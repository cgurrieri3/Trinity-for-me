///
///
/// @brief A new file
///
/// @file value.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 26 mar 2022 
///

#ifndef VALUE_HPP_
#define VALUE_HPP_
#include <iostream>
#include <string>
#include <variant>
#include <optional>
#include <type_traits>
#include <utility>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/format.hpp>

#include "fcutils/terminal/ansi.hpp"
#include "fcutils/type_traits/type_traits.hpp"
#include "fcutils/string/change_case.hpp"
#include "fcutils/monitor/string_to_object.hpp"
#include "fcutils/monitor/color_map.hpp"
#include "fcutils/monitor/endianess_map.hpp"
#include "fcutils/monitor/inouttype_map.hpp"
#include "fcutils/monitor/outformat_map.hpp"
#include "fcutils/serialization/deserializer.hpp"
#include "fcutils/utility/overload.hpp"

namespace fcutils{
namespace monitor{

template< class Value> Value mask_and_shift(Value v, std::optional<std::size_t> mask=std::nullopt,
        std::optional<std::size_t> rshift=std::nullopt){
        if constexpr (std::is_integral<Value>::value){
                if(mask) v= v& *mask;
                if(rshift) v= v >> *rshift;
        }
        return v;
};


struct converter{
	converter(double f=1.,double off=0., std::size_t bm=~0, std::size_t bs=0): factor(f), offset(off), bytemask(bm), bitshift(bs) {}
	double factor;
	double offset;
	std::size_t bytemask;
	std::size_t bitshift;
	template <class Return, class Value > Return operator()( Value v){
		v=mask_and_shift(v,bytemask,bitshift);
		return static_cast<Return>(v)*static_cast<Return>(factor)+static_cast<Return>(offset);
	}
	template <class Return, class Value > Return convert( Value v){
		return operator()(v);
	}
};



struct value{
	using tree_type 			= boost::property_tree::ptree;
	using self_type 			= value;
	using value_type			= fcutils::monitor::value_type; // from inouttype_map.hpp
	using converted_value_type	= fcutils::monitor::value_type; // from inouttype_map.hpp
	using color_type			= fcutils::terminal::ansi::color;
	using typelist_type 		= fcutils::monitor::typelist_type; // from inouttype_map.hpp
	using outtype_type			= fcutils::monitor::inouttype;
	using intype_type			= fcutils::monitor::inouttype;
	using endianess_type		= fcutils::monitor::endianess;
	using outformat_type		= std::ios_base::fmtflags;
	using keyvalue_type			= fcutils::monitor::keyvalue_type;
	using keytype_type			= fcutils::monitor::keytype;
	using keytypelist_type		= fcutils::monitor::keytypelist_type;

	std::string name;
	std::string id;
	std::string label;
	size_t offset{0};
	size_t size{0};
	endianess_type endian{endianess_type::BIGENDIAN};
	outformat_type outfmt{std::ios_base::dec};
	std::optional<color_type> alarmcol;
	std::optional<converted_value_type> max;
	std::optional<converted_value_type> min;
	std::optional<double> conversion_factor;
	std::optional<double> conversion_offset;
	std::optional<std::size_t> conversion_bytemask;
	std::optional<std::size_t> conversion_bitshift;
	bool signbit{false};
	std::optional<keyvalue_type> key_value;
	std::optional<keytype_type> key_type{keytype_type::INT};
	std::optional<std::size_t> key_size;
	std::optional<std::size_t> key_offset;
	bool multikey{false};
	bool alarm{false};
	std::optional<std::string> dbentry_measurement;
	std::optional<std::string> dbentry_tagmetric;
	std::optional<std::string> dbentry_taginstance;
private:
	value_type val_{0};
	converted_value_type converted_val_{0.};
	outtype_type outtype_{outtype_type::DOUBLE};
	intype_type intype_{intype_type::INT};

template<typename Val> 	void convert(const Val v){
		// Enable the alternative according to the output type
		enable_alternative(outtype_,converted_val_);
		// Convert it if both mask and bitshift are set. Check is done in mask_and_shift. This is valid only if the input type is integer.
//		auto converted=mask_and_shift(v,conversion_bytemask,conversion_bitshift);
		Val converted=mask_and_shift(v,conversion_bytemask,conversion_bitshift);
		// Create the lambda needed to handle the various alternatives
		fcutils::overload ovl{
			[converted,conv_f=conversion_factor,conv_o=conversion_offset](auto & v) mutable {
				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
				using converted_type = fcutils::type_traits::remove_cvref_t<decltype(converted)>;
				if constexpr (fcutils::type_traits::is_explicitly_convertible_to_v<converted_type,value_type>){
					v = (conv_f)?static_cast<value_type>(converted)*static_cast<value_type>(*conv_f):static_cast<value_type>(converted);
					v = (conv_o)?v+static_cast<value_type>(*conv_o):v;
				} else {
					std::cout << "No conversion is possible " << std::endl;
				}
				}
			,
//				// String and char, no conversion is needed ad the moment
			[converted](char & v) mutable {
				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
				using converted_type = fcutils::type_traits::remove_cvref_t<decltype(converted)>;
				if constexpr (std::is_same<value_type,converted_type>::value){
					v = converted;
				} else {
					std::cout << "No conversion is possible " << std::endl;
				}
			},
			[converted](std::string & v) mutable {
				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
				using converted_type = fcutils::type_traits::remove_cvref_t<decltype(converted)>;
				if constexpr (std::is_same<value_type,converted_type>::value){
					v = converted;
				} else {
					std::cout << "No conversion is possible " << std::endl;
				}
			}
		};
		// Convert if conversion factor and offset are set
		std::visit(ovl,converted_val_);
//		std::visit(
//			[converted,conv_f=conversion_factor,conv_o=conversion_offset](auto & v) mutable {
//				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
//				using converted_type = fcutils::type_traits::remove_cvref<decltype(converted)>;
//				if constexpr ( std::is_same_v<value_type,char> || std::is_same_v<value_type,std::string> ||
//								std::is_same_v<converted_type,char> || std::is_same_v<converted_type,std::string>) {
//					// is char or std::string do not use conversion factors at all
//					if (std::is_same_v<converted_type,value_type>) v = converted;
//				} else {
//					v = (conv_f)?static_cast<value_type>(converted)*static_cast<value_type>(*conv_f):static_cast<value_type>(converted);
//					v = (conv_o)?v+static_cast<value_type>(*conv_o):v;
//				}
//		}, converted_val_);
	}
	void set_alarm(){
		// Check for limits if set
		alarm=(max)?(converted_val_>*max):false;
		alarm=(min)?(alarm||(converted_val_<*min)):alarm;
	}

public:

	value() = default;
	value( const value &) = default;
	value( value &&) = default;
	value & operator=( const value & ) = default;
	value & operator=( value && ) = default;
	template<typename Val> self_type & operator=(const Val v){
		// Search for the type in the list. If present then assign and convert
		if constexpr (fcutils::type_traits::list_search<Val,typelist_type>::index >0 ){
			using value_type = fcutils::type_traits::remove_cvref_t<decltype(val_)>;
			// The type is in the list. Assign it
			std::visit([&v]( auto & val_ ) mutable { val_=static_cast<value_type>(v);}, val_);
			// Convert only if is not a char or a std::string
			convert(v);
			set_alarm();
		}
		return *this;
	}

	auto & get_value() const {
		return val_;
	}

	auto get_value() {
		return val_;
	}

	auto & get_converted_value() const {
		return converted_val_;
	}

	auto  get_converted_value() {
		return converted_val_;
	}

	void set_intype(intype_type it){
		intype_=it;
		// Reset the alternative hold in the variant
		enable_alternative(it,val_);
	}

	void set_outtype(outtype_type ot){
		outtype_=ot;
		// Reset the alternative hold in the variant
		enable_alternative(ot,converted_val_);
		if(max) {
			double temp;
			fcutils::overload ovl{ [&temp](auto & v)mutable{ temp=static_cast<fcutils::type_traits::remove_cvref_t<decltype(temp)>>(v);},
				[]( char & v) mutable { std::cout << " char case do nothing" << std::endl;},
				[]( std::string & v) mutable { std::cout << " std:string case do nothing" << std::endl;}
			};
//			std::visit([&temp](auto & v)mutable{ temp=static_cast<fcutils::type_traits::remove_cvref_t<decltype(temp)>>(v);},*max);
			std::visit(ovl,*max);
			set_alternative(ot,*max,temp);
		}
		if(min) {
			double temp;
			fcutils::overload ovl{ [&temp](auto & v)mutable{ temp=static_cast<fcutils::type_traits::remove_cvref_t<decltype(temp)>>(v);},
				[]( char & v) mutable { std::cout << " char case do nothing" << std::endl;},
				[]( std::string & v) mutable { std::cout << " std:string case do nothing" << std::endl;}
			};
			std::visit(ovl,*min);
//			std::visit([&temp](auto & v)mutable{ temp=static_cast<fcutils::type_traits::remove_cvref_t<decltype(temp)>>(v);},*min);
			set_alternative(ot,*min,temp);
		}
	}

	void set_inouttype(intype_type it, outtype_type ot){
		set_intype(it);
		set_outtype(ot);
	}

	void set_keytype(keytype_type it){
		key_type=it;
		// Reset the alternative hold in the variant, if the optional is set
		if(key_value) enable_alternative(it,*key_value);
	}

	template <typename Data> void set_key_value( Data d){
		if(key_type){
			// Enable the option
			key_value.emplace();
			std::visit([&d](auto &v){
				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
				v= static_cast<value_type>(d);},*key_value);
		}
	}

	template <typename Data> void set_value( Data d){
		operator= (d);
	}

	template <typename Data> void set_value( const Data *d){
		std::visit( [d, s=size, o=offset ](auto & v) mutable {
			// Copy s bytes from the data buffer.
			auto temp=v;
			std::copy_n(reinterpret_cast< const char *>(d)+o,s,reinterpret_cast<Data *>(&temp));
			v=temp;
		}, val_);
		std::visit([this](auto & v) { convert(v);}, val_);
		set_alarm();
	}

	//
	auto get_intype() const { return intype_;}
	auto get_outtype() const { return outtype_;}

	std::ostream & to_stream(std::ostream &os) const {
		auto m=fcutils::terminal::ansi::color::BG_DEFAULT;
		if(alarmcol) m=(alarm)?*alarmcol:m;
		os << m;
		std::visit([&os]( auto & v ) { os << v; },converted_val_);
		os << fcutils::terminal::ansi::format::TXT_RESET;
		return os;
	}

    boost::format & to_format(boost::format & fmt) const {
		std::visit([&fmt]( auto & v ) mutable { fmt % v; },converted_val_);
    	return fmt;
    }

    boost::format & to_format(boost::format & fmt, std::size_t width, std::size_t precision) const {
    		std::stringstream ss;
    		// First set the background color if an alarm occurred
    		auto m=fcutils::terminal::ansi::color::BG_DEFAULT;
            if(alarmcol) m=(alarm)?*alarmcol:m;
            ss << m;
            // Then format according to the output type
    		switch(outtype_){
    		case outtype_type::DOUBLE:
    		case outtype_type::FLOAT:
    			// floating point case, build the format string with width and precision: %w.pf
    			ss<< '%' << width << '.' << precision << 'f';
    			break;
    		case outtype_type::INT:
    			// Integer type, check if the hex has been request
    		{
    			char f=(outfmt==std::ios_base::hex)?'X':'i';
    			ss<< "%#" << width << f;
    			break;
    		}
    		default:
    			// all the others, set just the width
    			ss<< '%' << width << 'd';
    			break;
    		}
    		// then set back the background color
    		ss << fcutils::terminal::ansi::color::BG_DEFAULT;
    		// Now copy the format string into the provided format object
    		fmt=boost::format(ss.str().c_str());
    		// Save the value into the format object and return it
    		std::visit([&fmt]( auto & v ) mutable { fmt % v; },converted_val_);
    		return fmt;
    }

    void print(std::ostream & os=std::cout) const {
		os << " Value name: " << name <<", id: "<< id <<", label: " << label <<'\n';
		os << " Memory offset: " << offset <<", size (in Byte) :" << size << ", intype: " <<
				fcutils::type_traits::enum_to_underlying(intype_) << ", outtype: " << fcutils::type_traits::enum_to_underlying(outtype_) <<
		", signed: " << std::boolalpha<< signbit <<std::noboolalpha  <<'\n';
		if(conversion_bytemask) {
			os << " Bytemask: " << std::hex << std::showbase << *conversion_bytemask << std::noshowbase << std::dec;
		} else {
			os << " No Bytemask ";
		}
		if(conversion_bitshift){
			os << ", bitshift: "<< *conversion_bitshift << '\n';
		} else {
			os << ", no bitshift. \n";
		}
		if(conversion_factor){
			os << " Conversion factor: "<< *conversion_factor;
		} else {
			os << " No conversion factor, ";
		}
		if(conversion_offset){
			os << ", conversion offset: " << *conversion_offset << '\n';
		} else {
			os << " No conversion offset. \n";
		}
		os << " Endianness: " << fcutils::type_traits::enum_to_underlying(endian) << '\n'; //<< ", out format: " << outfmt << '\n';
		std::string temp{" Raw data value: "};
		auto visitor=[&temp,&os](auto &v){ os << temp << v; };
		std::visit(visitor,val_);
		temp= ". Converted data value: ";
		std::visit(visitor,converted_val_);
		if(min){
			temp= "\n Min: ";
			std::visit(visitor,*min);
		} else {
			os << "No Min value set. ";
		}
		if(max){
			temp= ". Max: ";
			std::visit(visitor,*max);
		} else {
			os << " No Max value set. \n";
		}
		os << std::boolalpha << ". Alarm: " << alarm << std::noboolalpha << '\n';
		if(key_value){
			temp= " Key value: ";
			std::visit(visitor,*key_value);
		} else {
			os << " No Key value set. \n";
		}
		if(key_type){
			os << " Key type: " << fcutils::type_traits::enum_to_underlying(*key_type) <<',';
		} else {
			os << " No Key type set. \n";
		}
		if(key_size){
			os << " Key size: " << *key_size <<',';
		} else {
			os << " No Key size set. \n";
		}
		if(key_type){
			os << ", Key offset: " << *key_offset <<',';
		} else {
			os << " No Key offset set. \n";
		}
		if(dbentry_measurement){
			os << "\n DB entry measurement: " << *dbentry_measurement <<',';
		} else {
			os << " No DB entry measurement set. \n";
		}
		if(dbentry_tagmetric){
			os << " DB entry tag metric: " << *dbentry_tagmetric <<',';
		} else {
			os << " No DB entry tag metric set. \n";
		}
		if(dbentry_taginstance){
			os << " DB entry tag instance: " <<*dbentry_taginstance <<'\n';
		} else {
			os << " No DB entry tag instance set. \n";
		}
		os << std::boolalpha << " Multikey: " << multikey << std::noboolalpha <<'\n'<< std::endl;
	}

	template <typename Tree> bool fetch(Tree && pt){
		return fetch_value(std::forward<Tree>(pt),*this);
	}
};

std::ostream & operator <<(std::ostream &os, const value & v){
	// Backup the actual flags
	std::ios_base::fmtflags ff{os.flags()};
	// Set the format based on the value one
	os.setf (v.outfmt , std::ios::basefield);
	// Print the value
	v.to_stream(os);
	os.flags(ff);
	return os;
}


bool fetch_value( value::tree_type & pt, value & v){
		const std::size_t FIELDWIDTH{6};
		bool success=false;
		v.name=pt.get_child(xml::slowcontrol::variable::field::NAMEOPT).get_value("noname");
		v.id=pt.get_child(xml::slowcontrol::variable::field::IDOPT).get_value("noid");
		v.label=pt.get_child(xml::slowcontrol::variable::field::LABELOPT).get_value("nolabel");
		if(v.label.size()<FIELDWIDTH) v.label=std::string(FIELDWIDTH-v.label.size(),' ') + v.label;
		v.offset=pt.get_child(xml::slowcontrol::variable::field::OFFSETOPT).get_value(-1);
		v.size=pt.get_child(xml::slowcontrol::variable::field::SIZEOPT).get_value(0);
		std::string temp=pt.get_child(xml::slowcontrol::variable::field::INTYPEOPT).get_value(xml::slowcontrol::variable::value::SDOUBLE);
		// default in and out type to int
		v.set_inouttype(inouttype::INT, inouttype::INT);
		auto ittemp=string_to_object(temp,STRING_TO_INOUTTYPE);
		v.set_intype(ittemp);
		auto ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::OUTTYPEOPT);
		if(ptvar) {
			temp=ptvar->get_value(xml::slowcontrol::variable::value::SDOUBLE);
			auto ottemp=string_to_object(temp,STRING_TO_INOUTTYPE);
			v.set_outtype(ottemp);
		}
		ptvar.reset();
//		temp=pt.get_child(xml::slowcontrol::variable::field::ENDIANNESSOPT).get_value(xml::slowcontrol::variable::value::SBIGENDIAN);
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::ENDIANNESSOPT);
		if(ptvar) {
			temp=ptvar->get_value(xml::slowcontrol::variable::value::SBIGENDIAN);
		} else {
			temp=xml::slowcontrol::variable::value::SBIGENDIAN;
		}
		v.endian= string_to_object(temp, STRING_TO_ENDIANESS);
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::SIGNEDOPT);
		// Signbit is false by default
		v.signbit=false;
		if(ptvar){
			v.signbit=ptvar->get_value<bool>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::FORMATOPT);
		if(ptvar){
			temp=ptvar->get_value(xml::slowcontrol::variable::value::SDEC);
		} else {
			temp=xml::slowcontrol::variable::value::SDEC;
		}
		v.outfmt= string_to_object(temp, STRING_TO_OUTFORMAT);
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::ALARMCOLOROPT);
		if(ptvar) {
			temp=ptvar->get_value(xml::slowcontrol::variable::value::SRED);
			v.alarmcol=string_to_object(temp, STRING_TO_BG_COLOR);
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::MAXOPT);
		fcutils::overload ovl{
			// The standard case. Is an integer, float or double. Just assume is a double and then convert
			[&ptvar](auto & v){
				using value_type = fcutils::type_traits::remove_cvref_t<decltype(v)>;
				v=static_cast<value_type>(ptvar->get_value<double>());},
				// The boolean case
				[&ptvar](bool & v){
					v=ptvar->get_value(v);
				},
				// The char case
				[&ptvar](char & v){
					v=ptvar->get_value(v);
				},
				// The std::string case
				[&ptvar](std::string & v){
					v=ptvar->get_value(v);
				}
		};
		if(ptvar) {
			v.max=enable_alternative(v.get_outtype());
			// Use the overload pattern to rule them all
			std::visit(ovl,*v.max);
			// If no alarmcolor set then use the red color
			if(!v.alarmcol) v.alarmcol=string_to_object(xml::slowcontrol::variable::value::SRED, STRING_TO_BG_COLOR);
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::MINOPT);
		if(ptvar) {
			v.min=enable_alternative(v.get_outtype());
			// Use the overload pattern to rule them all
			std::visit(ovl,*v.min);
			// If no alarmcolor set then use the red color
			if(!v.alarmcol) v.alarmcol=string_to_object(xml::slowcontrol::variable::value::SRED, STRING_TO_BG_COLOR);
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::CONVERSIONFACTOROPT);
		if(ptvar) {
			v.conversion_factor=ptvar->get_value<double>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::CONVERSIONOFFSETOPT);
		if(ptvar) {
			v.conversion_offset=ptvar->get_value<double>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::CONVERSIONBYTEMASKOPT);
		if(ptvar) {
			std::string temp=ptvar->get_value(xml::slowcontrol::variable::value::DEFBYTEMASK);
			std::stringstream ss;
			ss << std::hex << temp;
			ss >> *v.conversion_bytemask;
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::CONVERSIONBITSHIFTOPT);
		if(ptvar){
			v.conversion_bitshift=ptvar->get_value<std::size_t>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::KEYTYPEOPT);
		if(ptvar) {
			temp=ptvar->get_value(xml::slowcontrol::variable::value::SINT);
			v.key_type=string_to_object(temp,STRING_TO_KEYTYPE);
		}
		ptvar.reset();
		// It must be executed after the key_type fetch
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::KEYVALUEOPT);
		if(ptvar) {
			// Enable the optional
			v.key_value=enable_alternative<value::keyvalue_type>(*v.key_type);
			// Use the overload pattern to rule them all
			std::visit(ovl,*v.key_value);
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::KEYSIZEOPT);
		if(ptvar) {
			v.key_size=ptvar->get_value<std::size_t>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::KEYOFFSETOPT);
		if(ptvar){
			v.key_offset=ptvar->get_value<std::size_t>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::MULTIKEYOPT);
		// Multikey is false by default
		v.multikey=false;
		if(ptvar){
			v.multikey=ptvar->get_value<bool>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::DBENTRYMEASUREMENTOPT);
		if(ptvar){
			v.dbentry_measurement=ptvar->get_value<std::string>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::DBENTRYTAGMETRICOPT);
		if(ptvar){
			v.dbentry_tagmetric=ptvar->get_value<std::string>();
		}
		ptvar.reset();
		ptvar=pt.get_child_optional(xml::slowcontrol::variable::field::DBENTRYTAGINSTANCEOPT);
		if(ptvar){
			v.dbentry_taginstance=ptvar->get_value<std::string>();
		}
		success=true;
		return success;
	}

} // namespace monitor
} // namespace fcutils



#endif // VALUE_HPP_ 
