#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <iomanip>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <concurrency/guarded_thread.hpp>
#include "constants.h"
#include "posix_message_queue.hpp"

#define Start_ECC_Server	0x01
#define Start_DataRouter	0x02
#define Init_Experiment		0x03
#define Start_Run			0x04
#define Stop_Run			0x05
#define AsAd_Off			0x06

#define CMD_PING_COBO		0x30

using namespace std;
using fcutils::system::posix_message_queue;

std::string response = "4040";
bool new_cmd = false;

void StartECCServer()
{
	std::string command = "getEccServer -h "+CoBo_DIR+ "ECCRepository/ &";
	system(command.c_str());
}

void StartDataRouter()
{
	std::string command = "cd "+CoBo_DIR+"format/ && dataRouter 192.168.41.20:46003 192.168.41.20:46005 TCP FrameStorage 2>&1 &";
	system(command.c_str());
}

void InitializeExperiment()
{
	std::string command = CoBo_DIR+"scripts/ExpectScript.sh";
	system(command.c_str());
}

void StartRun()
{
	std::string command = CoBo_DIR+"scripts/ExpectStartAcq.sh";
	system(command.c_str());
}

void StopRun()
{
	std::string command = CoBo_DIR+"scripts/ExpectStopAcq.sh";
	system(command.c_str());
}

void ASADOff()
{
	std::string command = CoBo_DIR+"scripts/ExpectASADOff.sh";
	system(command.c_str());
}

std::vector<uint32_t> HextoInt(const std::string& hex, int msg_size)
{
	std::vector<uint32_t> vmsg;
	std::cout <<"HEX "<<hex<<std::endl;
	for (int i = 0; i < 16; i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		std::cout<<"BYTE: "<<i<<" "<<byteString<<std::endl;
		vmsg.push_back(stoi(byteString,0,16));
	}
	return vmsg;
}

void Process_CMD(std::vector<uint32_t>& vCMD, std::string &response)
{
	uint32_t CMD_ID  = vCMD.at(0);

	switch (CMD_ID)
	{
		case Start_ECC_Server:
			StartECCServer();
			break;
		case Start_DataRouter:
			StartDataRouter();
			break;
		case Init_Experiment:
			InitializeExperiment();
			break;
		case Start_Run:
			StartRun();
			break;
		case Stop_Run:
			StopRun();
			break;
		case AsAd_Off:
			ASADOff();
			break;
		default:
			break;
	}
	response="4040";
}

void Parse_Run_CMD(const std::string& msg, int msg_size, std::string& response)
{
	std::vector<uint32_t> vCMD;

	if ((msg_size%2) > 0){
		cout << "Length of message mismatch. Please enter even number of hex letters." << endl;
		return;
	}
	
	vCMD = HextoInt(msg, msg_size);

	if (vCMD.size() != 8){
		cout << "Length of message mismatch. Please enter 8 bytes or 16 hex letters." << endl;
		return;
	}

	if(vCMD.at(0) == CMD_PING_COBO){
		std::cout << "CoBo Process is Pinged" << std::endl;
		response = "0x40";
	}else{
		Process_CMD(vCMD, response);
		std::cout << "response: " << response << endl;
	}
}

void read_mq(sigval_t sig)
{
	int numRead{-1};
	posix_message_queue* mqp=static_cast<posix_message_queue*>(sig.sival_ptr);
	auto msg_size=mqp->get_msgsize();
	auto msg=new char[msg_size];
	auto mq_name=mqp->get_name();
	mqp->notify(read_mq);
	while ((numRead = mqp->receive(msg, msg_size)) > 0)
	{
		std::cout << " Queue: " << mq_name <<", message received: " << msg << ", of size: " << numRead << std::endl;
		Parse_Run_CMD(msg, numRead, response);
	}
	new_cmd = true;
	delete [] msg;
}

int main()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << "Staring CoBo at: " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;

	std::string mqname="/writetoCoBo";
	std::cout << "Creating and opening the POSIX message queue: " << mqname << " for listening to commands ..." << std::endl;
	posix_message_queue rq(mqname.c_str());
	rq.open();

	mqname="/writetoRC";
	std::cout << "Creating and opening the POSIX message queue: " << mqname << " for responding to master ..." << std::endl;
	posix_message_queue wq(mqname.c_str());
	wq.open();

	rq.notify(read_mq);

	while(true){
		if(new_cmd){
			new_cmd = false;
			if(response == "0x40"){
				wq.send(response);
			}
			response.clear();
		}
		usleep(100000);
	}

	return 0;
}
