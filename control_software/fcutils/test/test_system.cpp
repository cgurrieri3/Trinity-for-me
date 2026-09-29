///
///
/// @brief To test fcutils/system utilities
///
/// @file test_system.cpp
/// @author Francesco S. Cafagna (francesco.cafagna@ba.infn.it)
/// @date 21 feb 2022 
///

#include <iostream>
#include <boost/asio.hpp>
#include "fcutils/system/getip.hpp"
#include "fcutils/networking/udpbeacon.hpp"
#include "fcutils/networking/udpbeaconmanager.hpp"

int main(){
	std::string dev{"eth0"},msg;
	fcutils::networking::udpbeacon beacon{1s};
	fcutils::networking::udpjsonbeacon jbeacon{1500ms};
	fcutils::networking::udpbeaconmanager<fcutils::networking::udpstringbeacon,true> bmanager;
	bmanager.add_beacon(8889,1s,dev,8889); //,std::string{"eth0"},8889);
	bmanager.add_beacon(8887,3s,"eth0",8887); //,std::string{"eth0"},8887);
	bmanager.start();
	beacon.start();
	jbeacon.start();
	std::cout << " Enter the device name: ";
	while (std::cin>>dev){
		msg=dev;
		msg+=" <" + fcutils::system::getip(dev) + '>';
		msg+=", netmask <" + fcutils::system::getip(dev,fcutils::system::netmask) + '>';
		msg+=", broadcast <" + fcutils::system::getip(dev,fcutils::system::broadcast) + '>';
		msg+='\n';
		std::cout << " Ip addresses for device: " << msg << std::endl;
		beacon.set_message(msg);
		bmanager.set_message(8889,msg);
		bmanager.set_message(8887,msg);
		std::cout << " Enter the device name: ";
	}

	bmanager.stop();
	beacon.stop();
	jbeacon.stop();
	return 0;
}


