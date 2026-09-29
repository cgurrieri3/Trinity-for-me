///
///
/// @brief To format an influxdb line protocol from a value
///
/// @file make_influxdb_line.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 11 ago 2022 
///

#ifndef MAKE_INFLUXDB_LINE_HPP_
#define MAKE_INFLUXDB_LINE_HPP_
#include <string>
#include <variant>
#include "fcutils/monitor/value.hpp"
#include "fcutils/utility/overload.hpp"

namespace fcutils{
namespace monitor{

std::string make_influxdb_line(const value &v){
		std::string line;
		if(!v.dbentry_measurement) return "no measurement";
		line=*v.dbentry_measurement;
		if(!v.dbentry_tagmetric) return "no tag metric";
		if(!v.dbentry_taginstance) return "no tag instance";
		line+=',';
		line+=*v.dbentry_tagmetric;
		line+='=';
		line+=*v.dbentry_taginstance;
		line+=" value=";
		fcutils::overload ovl{
			[&line](const auto & v){ line+=std::to_string(v);},
			[&line](const char & c){ line+=c; },
			[&line](const std::string & s){ line+=s;}
		};
		std::visit(ovl,v.get_converted_value());
		return line;
}

} // namespace monitor
} // namespace fcutils



#endif // MAKE_INFLUXDB_LINE_HPP_ 
