///
///
/// @brief To add RFC 5424 format to the message. Is suitable to be sent to a syslog server
///
/// @file add_syslog.hpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 14 giu 2022 
///

#ifndef SYSLOG_FORMATTER_HPP_
#define SYSLOG_FORMATTER_HPP_

#include <string>
#include <chrono>
#include <utility>
#include <unistd.h>
#include <boost/asio/ip/host_name.hpp>

#include "fcutils/string/add_timestamp.hpp"
#include "fcutils/string/trim.hpp"

namespace fcutils{
namespace string{

namespace syslog{
	using u_type=int;
/// Syslog record levels
	enum class severity: u_type
	{
		EMERGENCY 	= 0,                // Equivalent to LOG_EMERG in syslog API
		ALERT 		= 1,                    // Equivalent to LOG_ALERT in syslog API
		CRITICAL 	= 2,                 // Equivalent to LOG_CRIT in syslog API
		ERROR 		= 3,                    // Equivalent to LOG_ERROR in syslog API
		WARNING 	= 4,                  // Equivalent to LOG_WARNING in syslog API
		NOTICE 		= 5,                   // Equivalent to LOG_NOTICE in syslog API
		INFO 		= 6,                     // Equivalent to LOG_INFO in syslog API
		DEBUG 		= 7                     // Equivalent to LOG_DEBUG in syslog API
	};


/// Syslog facility codes
	enum class facility: u_type
	{
		KERNEL 		= 0,               //!< Kernel messages
		USER 		= 1,                 //!< User-level messages. Equivalent to LOG_USER in syslog API.
		MAIL 		= 2,                 //!< Mail system messages. Equivalent to LOG_MAIL in syslog API.
		DAEMON 		= 3,               //!< System daemons. Equivalent to LOG_DAEMON in syslog API.
		SECURITY0 	= 4,            //!< Security/authorization messages
		SYSLOGD 	= 5,              //!< Messages from the syslogd daemon. Equivalent to LOG_SYSLOG in syslog API.
		PRINTER 	= 6,              //!< Line printer subsystem. Equivalent to LOG_LPR in syslog API.
		NEWS 		= 7,                 //!< Network news subsystem. Equivalent to LOG_NEWS in syslog API.
		UUCP 		= 8,                 //!< Messages from UUCP subsystem. Equivalent to LOG_UUCP in syslog API.
		CLOCK0 		= 9,               //!< Messages from the clock daemon
		SECURITY1 	= 10,           //!< Security/authorization messages
		FTP 		= 11,                 //!< Messages from FTP daemon
		NTP 		= 12,                 //!< Messages from NTP daemon
		LOG_AUDIT 	= 13,           //!< Security/authorization messages
		LOG_ALERT 	= 14,           //!< Security/authorization messages
		CLOCK1 		= 15,              //!< Messages from the clock daemon
		LOCAL0 		= 16,              //!< For local use. Equivalent to LOG_LOCAL0 in syslog API
		LOCAL1 		= 17,              //!< For local use. Equivalent to LOG_LOCAL1 in syslog API
		LOCAL2 		= 18,              //!< For local use. Equivalent to LOG_LOCAL2 in syslog API
		LOCAL3 		= 19,              //!< For local use. Equivalent to LOG_LOCAL3 in syslog API
		LOCAL4 		= 20,              //!< For local use. Equivalent to LOG_LOCAL4 in syslog API
		LOCAL5 		= 21,              //!< For local use. Equivalent to LOG_LOCAL5 in syslog API
		LOCAL6 		= 22,              //!< For local use. Equivalent to LOG_LOCAL6 in syslog API
		LOCAL7 		= 23               //!< For local use. Equivalent to LOG_LOCAL7 in syslog API
	};

// The priority is obtained multiplying by 8 the facility value plus the severity
	u_type priority(const severity s=severity::INFO, const facility f=facility::USER){ return static_cast<u_type>(s)+static_cast<u_type>(f)*u_type{8};}
}

class syslog_formatter{
public:

	// trim the program and host names
	explicit syslog_formatter(const std::string & c=program_invocation_short_name, const std::string & host=boost::asio::ip::host_name(), int v=1,
			syslog::severity s=syslog::severity::INFO, syslog::facility f= syslog::facility::USER): progname_{trim(c)}, host_{trim(host)}, version_{v}, severity_{s}, facility_{f}
			{
				proc_id_=std::to_string(getpid());
				pref_post_format();
			};

	syslog_formatter(const syslog_formatter & ) = default;
	syslog_formatter& operator=(const syslog_formatter & ) = default;
	syslog_formatter(syslog_formatter && ) noexcept = default;
	syslog_formatter& operator=(syslog_formatter && ) noexcept = default;

	const char * c_str() const {
		stemp_ = fcutils::string::add_timestamp<fcutils::string::pos::FRONT, fcutils::string::conv::SYSLOGFMT> (pref_) + post_;
		return stemp_.c_str();
	}

	void set_name(const std::string & c){ progname_=trim(c); pref_post_format();}
	void set_postfix(const std::string & c){ host_=trim(c); pref_post_format();}

	const char * get_name() const { return progname_.c_str(); }
	const char * get_host() const { return host_.c_str(); }
	syslog::severity get_severity() const { return severity_;}
	syslog::facility get_facility() const { return facility_;}
	auto get_priority() const { return syslog::priority(severity_, facility_); }

	operator std::string() const {
		c_str();
		// We do not need stemp_ so we move it ... in spite of the guidelines ...
		return std::move(stemp_);
	}

private:
	std::string progname_;
	mutable std::string stemp_,pref_,post_;
	std::string host_;
	std::string proc_id_;
	std::string msgid_{"SPB2"};
	std::string structured_data_{'-'};
	std::string encoding_{};
	syslog::severity severity_;
	syslog::facility facility_;
	int version_;

	void pref_post_format(){
		pref_="<"+std::to_string(get_priority())+">"+std::to_string(version_)+" ";
//		post_=" "+host_+" "+progname_+" "+proc_id_+" "+msgid_+" "+structured_data_+" "+encoding_+" ";
		post_=" "+host_+" "+progname_+" "+proc_id_+" "+msgid_+" "+structured_data_+" ";
	}

};

std::ostream & operator << (std::ostream & o, const syslog_formatter & p){
	return o<< p.c_str();
}

} // namespace string
} // namespace fcutils





#endif // SYSLOG_FORMATTER_HPP_ 
