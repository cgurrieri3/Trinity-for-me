#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <functional>
#include <sys/reboot.h>
#include <iomanip>
#include "posix_message_queue.hpp"
#include "fcutils/utility/utility.hpp"
#include "fcutils/string/string.hpp"
#include "state_message_cmds.h"
#include <unistd.h>

using namespace std;
using fcutils::system::posix_message_queue;

int TrigRate = 0;
int HVScanNo = 0;
int Run_Number = 0;
int BF_TScanNo = 0;
int SF_TScanNo = 0;
int run_interval = 120;		// In seconds
int state_msg_interval = 30;	// In seconds
bool NewCMDFlag = false;
bool Temp_Flag = false;
bool RUN_ENABLED = false;
bool IsProcNewCMD = false;
bool EN_STATE_MSG = false;
bool IsInsideRunTimer = false;
bool IsTakingStateMSG = false;
bool IsInsideStopStartRun = false;
std::string state_response = "1010";
std::string ftdi_device_name = "/dev/ttyUSB0";

std::string LogFileID[9] = {"11111", "22222", "33333", "44444", "55555", "66666", "77777", "88888", "99999"};
std::string LogFileList[9] = {"startup", "Client_SIAB", "Client_TB", "Client_CoBo", "Client_LVPS", "Client_PDU", "cs", "rc", "dp"};

std::string mqname[8] = {"/CStoRC","/RCtoCS","/writetoRC","/writetoSIAB","/writetoTB","/writetoCoBo","/writetoPS","/writetoPDU"};
posix_message_queue rqcs(mqname[0].c_str());	// from Control Software to Master Control
posix_message_queue wqcs(mqname[1].c_str());	// from Master Control to Control Software
posix_message_queue rqrc(mqname[2].c_str());	// from clients to Master Control
posix_message_queue wqsiab(mqname[3].c_str());	// from Master Control to SIABs
posix_message_queue wqtrgb(mqname[4].c_str());	// from Master Control to TB
posix_message_queue wqcobo(mqname[5].c_str());	// from Master Control to CoBo
posix_message_queue wqlvps(mqname[6].c_str());	// from Master Control to LVPS
posix_message_queue wqpdu(mqname[7].c_str());	// from Master Control to PDU

// Equivalent Hex of 50 (DEC) is 32 (Hex) OR 2 (Char) showing in state messages. This means process is turned on.
// Equivalent Hex of 64 (DEC) is 40 (Hex) OR @ (Char) showing in state messages. This means process is turned off.
int SIAB_STATE = Process_OFF;
int TRGB_STATE = Process_OFF;
int COBO_STATE = Process_OFF;
int LVPS_STATE = Process_OFF;
int PDU_STATE  = Process_OFF;

/////////////////////////////////////////////////////////
///////////////// Time and Dtae Functions ///////////////
/////////////////////////////////////////////////////////

std::string Get_Time_Str()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[20];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%H%M%S",timeinfo);
  std::string str(buffer);
  return str;
}

std::string Get_Date_Str()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[20];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%y%m%d",timeinfo);
  std::string str(buffer);
  return str;
}

std::string Get_DateTime_Str()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[20];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%y%m%d_%H%M%S",timeinfo);
  std::string str(buffer);
  return str;
}

/////////////////////////////////////////////////////////
///////////////// Power-On Seq Commands /////////////////
/////////////////////////////////////////////////////////

void Reset_CT_CPU()
{
	system("sudo reboot &");
}

void Shutdown_CT_CPU()
{
	system("sudo shutdown -h now &");
}

void Init_SIAB()
{
	std::string command = "sudo mkdir -p "+LOG_DIR+" && "+"sudo ./client_siab >> "+LOG_DIR+"Client_SIAB.log &";
	system(command.c_str());
	SIAB_STATE = Process_ON;
}

void Kill_SIAB()
{
	std::cout << "Killing the SIAB Process" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/client_siab/ && !/awk/ {print $2}' | xargs kill)");
	SIAB_STATE = Process_OFF;
}

void Init_TRGB()
{
	std::string command = "sudo "+CS_DIR+"tb/mesaflash/client_tb >> "+LOG_DIR+"Client_TB.log &";
	system(command.c_str());
	TRGB_STATE = Process_ON;
}

void Kill_TRGB()
{
	std::cout << "Killing the Trigger Board Process" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/client_tb/ && !/awk/ {print $2}' | xargs kill)");
	TRGB_STATE = Process_OFF;
}

void Init_COBO()
{
	std::string command = "sudo ./client_cobo >> "+LOG_DIR+"Client_CoBo.log &";
	system(command.c_str());
	COBO_STATE = Process_ON;
}

void Kill_COBO()
{
	std::cout << "Killing the CoBo Process" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/client_cobo/ && !/awk/ {print $2}' | xargs kill)");
	COBO_STATE =Process_OFF;
}

void Init_LVPS()
{
	std::string command = "sudo ./client_lvps -p "+ftdi_device_name+" >> "+LOG_DIR+"Client_LVPS.log &";
	system(command.c_str());
	LVPS_STATE = Process_ON;
}

void Kill_LVPS()
{
	std::cout << "Killing the LVPS Process" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/client_lvps/ && !/awk/ {print $2}' | xargs kill)");
	LVPS_STATE = Process_OFF;
}

void Kill_getEccServer()
{
	std::cout << "Killing the getEccServer" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/getEccServer/ && !/awk/ {print $2}' | xargs kill)");
}

void Kill_dataRouter()
{
	std::cout << "Killing the dataRouter" << std::endl;
	system("sudo echo $(/bin/ps -fu $USER | awk '/dataRouter/ && !/awk/ {print $2}' | xargs kill)");
}

void Kill_All()
{
	Kill_COBO();
	Kill_SIAB();
	Kill_TRGB();
	Kill_LVPS();
	Kill_getEccServer();
	Kill_dataRouter();
}

std::string HVInt_to_HVcmdStr(int HV_Value)
{
	int firstdigit  = (HV_Value/1000);
	int seconddigit = ((HV_Value/100) % 10);
	int thirddigit  = ((HV_Value % 10) % 10);
	int fourthdigit = (HV_Value % 10);

	auto tmp1 = fcutils::string::int_to_hex(firstdigit);
	auto tmp2 = fcutils::string::int_to_hex(seconddigit);
	auto tmp3 = fcutils::string::int_to_hex(thirddigit);
	auto tmp4 = fcutils::string::int_to_hex(fourthdigit);
	std::string HV_str = tmp1.substr(8, 2) + tmp2.substr(8,2) + tmp3.substr(8,2) + tmp4.substr(8,2);
	std::string SetHVcmd = "0A"+HV_str+"000000";

	return SetHVcmd;
}

void HV_Current_Scan(std::string& msg)
{
	int HVStartPoint;
	int HVNofSteps;
	int HVStepSize;
	int HVCurrentSafeValue;

	HVStartPoint = stoi(msg.substr(0, 4), 0, 16);
	HVNofSteps = stoi(msg.substr(4, 2), 0, 16);
	HVStepSize = stoi(msg.substr(6, 4), 0, 16);
	HVCurrentSafeValue = stoi(msg.substr(10, 2), 0, 16);

	std::cout << "\n--------------------------------------------" << std::endl;
	std::cout << "Starting to Scan HV Current ..." << std::endl;
	std::cout << "HV Start Point: " << (float(HVStartPoint)/100) << " V" << std::endl;
	std::cout << "Num of HV Steps: " << HVNofSteps << std::endl;
	std::cout << "HV Step Size: " << (HVStepSize*10) << " mV" << std::endl;
	std::cout << "HV Current Safe Value: " << HVCurrentSafeValue << " mA" << std::endl << std::endl;

	char tmp[100];
	char filename[100];
	std::ofstream HVScan_File;

	strcpy(filename, HV_SCAN_DIR.c_str());
	strcat(filename, "HV_Current_Scan_");
	strcat(filename, Get_DateTime_Str().c_str());
	sprintf(tmp, "_%02d.txt", HVScanNo);
	strcat(filename, tmp);
	HVScan_File.open(filename, ios::app|ios::ate);

	bool FoundUnsafeHV = false;
	int HVchUnsafe[HVNofSteps][8] = {0};

	// To speed things up, we set HK data rate to 5s temporary
	std::string lvps_log_5s = "0713880000000000";
	wqlvps.send(lvps_log_5s);
	sleep(10);

	std::string turn_on_all_music = "0000FFFF08000000";
	wqsiab.send(turn_on_all_music);
	sleep(5);

	for (int i=0; i<HVNofSteps; i++)
	{
		if(!FoundUnsafeHV)
		{
			int HV_val = HVStartPoint + i*HVStepSize;
			std::string hvcmd = HVInt_to_HVcmdStr(HV_val);
			wqlvps.send(hvcmd);
			sleep(10);
			std::cout << "Setting HV to: " << (float(HV_val)/100) << " V" << std::endl;
			HVScan_File << "HV Value: " << ((float)HV_val/100) << " V" << std::endl;

			float HV_current[4] = {0};
			for(int j=0; j<4; j++)
			{
				wqsiab.send(turn_on_hv[j]);
				sleep(1);

				sleep(15);
				uint32_t* hk_data;
				hk_data = Get_LVPS_data();

				for (int kk=0; kk<4; kk++)
				{
					std::cout << ((*(hk_data+kk+32))*2.441406E-06) << "\t";
				}
				std::cout << std::endl;

				HV_current[j] = ((*(hk_data+j+32))*2.441406E-06);
				if(HV_current[j] > HVCurrentSafeValue)
				{
					std::cout << "Found an Unsafe HV Section on ch: " << j << std::endl;
					FoundUnsafeHV = true;
					HVchUnsafe[i][j] = 1;
				}
				HVScan_File << "ch#" << j << ":\t" << HV_current[j] << " mA" << std::endl;

				wqsiab.send(turn_off_hv[j]);
				sleep(1);
			}
		}else{
			std::cout << "Stopping the HV Current Scan due to 1 or more Unsafe section" << std::endl;
		}
	}

	// Now, we revert back hk data rate to 10s again
	std::string lvps_log_10s = "0727100000000000";
	wqlvps.send(lvps_log_10s);

	HVScan_File << "-----------------------------------------" << std::endl;
	for (int k=0; k<HVNofSteps; k++)
	{
		for (int m=0; m<4; m++)
		{
			HVScan_File << HVchUnsafe[k][m] << "\t";
		}
		HVScan_File << std::endl;
	}
	HVScan_File << "-----------------------------------------" << std::endl;
	HVScan_File.close();
	HVScanNo++;
	std::cout<<"Finished HV Scan"<<endl;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
///////////////// Run Control Commands /////////////////
////////////////////////////////////////////////////////

void Archive_Counters()
{
	// Create a directory for today's counters files inside TB_DIR
	std::string Today_DIR = FINAL_COUNTERS_DIR+Get_Date_Str()+"/"+Get_Time_Str()+"/";
	std::string Create_CMD = "sudo mkdir -p "+Today_DIR+" &";
	system(Create_CMD.c_str());
	usleep(250*1000);
	std::string GCTRs_MoveCMD = "mv " +TB_DIR+"G_Ctrs* "+Today_DIR+" &";
	std::string CCTRs_MoveCMD = "mv " +TB_DIR+"C_Ctrs* "+Today_DIR+" &";
	system(GCTRs_MoveCMD.c_str());
	usleep(250*1000);
	system(CCTRs_MoveCMD.c_str());
	usleep(250*1000);
}

void Update_Runtime(std::string& msg)
{
	std::string runtime_str = msg.substr(0, 4);
	int runtime = stoi(runtime_str,0,16);

	fstream runtime_file;
	std::string filename = CS_DIR+"fcutils/test/include/Run_Duration.txt";
	runtime_file.open(filename.c_str(), ios::out);
	if(runtime_file.is_open())
	{
		runtime_file << runtime;
		std::cout << "Run time is updated to " << runtime << " Seconds." << std::endl;
		std::cout << "Please restart startup service ..." << std::endl;
		runtime_file.close();
	}
}

std::string Update_TB_GC_CMD(int Run_Num)
{
	std::string temp = fcutils::string::int_to_hex(Run_Num);
	std::string run_num_str = temp.substr(6, 4);
	std::string GC_CMD = "0A00" + run_num_str + "00000000";
	return GC_CMD;
}

std::string Update_TB_CC_CMD(int Run_Num)
{
	std::string temp = fcutils::string::int_to_hex(Run_Num);
	std::string run_num_str = temp.substr(6, 4);
	std::string CC_CMD = "0B00" + run_num_str + "00000000";
	return CC_CMD;
}

std::string Update_TB_Event_CMD(int Run_Num)
{
	std::string temp = fcutils::string::int_to_hex(Run_Num);
	std::string run_num_str = temp.substr(6, 4);
	std::string Event_CMD = "0C" + run_num_str + "0000000000";
	return Event_CMD;
}

void Start_Run()
{
	// This flag is to make sure pinging TB does not coincide with reading TB memory.
	IsInsideStopStartRun = true;

	std::cout << "\n---------------------" << std::endl;
	std::cout << "Starting the run#" << Run_Number << std::endl;
	std::cout << "Please wait 10 seconds before stopping the run" << std::endl;
	wqcobo.send(CoBo_Start_Run);
	sleep(5);
	wqtrgb.send(TB_Enable_Trigger);
	sleep(5);
	RUN_ENABLED = true;
	std::cout << "You can stop the run manually now, if you want!" << std::endl;
	IsInsideStopStartRun = false;
}

void Stop_Triggering()
{
	// This flag is to make sure pinging TB does not coincide with reading TB memory.
	IsInsideStopStartRun = true;

	std::cout << "Stopping the run#" << Run_Number << std::endl;

	std::cout << "Disabling the Global Trigger" << std::endl;
	wqtrgb.send(TB_Disable_Trigger);
	usleep(10000);

	std::cout << "Stopping the CoBo ..." << std::endl;
	wqcobo.send(CoBo_Stop_Run);
	sleep(1);

	std::cout << "Saving All Counters on TB Memory ..." << std::endl;
	wqtrgb.send(TB_Save_Counters);
	sleep(2);

	std::cout << "Recording the Global Counters ..." << std::endl;
	wqtrgb.send(Update_TB_GC_CMD(Run_Number));
	sleep(2);

	if(state_response.length() == 16)
	{
		TrigRate = stoi(state_response.substr(8, 8), 0 ,16);
		state_response.clear();
		std::cout << "Number of Triggered Events: " << TrigRate << std::endl;
	}else{
		TrigRate = 0xFFFF;
	}

	std::cout << "Recording the Channel Counters ..." << std::endl;
	wqtrgb.send(Update_TB_CC_CMD(Run_Number));
	sleep(2);

	std::cout << "Saving the Events data ..." << std::endl;
	wqtrgb.send(Update_TB_Event_CMD(Run_Number));
	sleep(4);

	std::cout << "Clearing the Counters ..." << std::endl;
	wqtrgb.send(TB_Clear_Counters);

	Archive_Counters();
	Run_Number++;
	
	std::string Move_Files = CoBo_DIR+"scripts/MoveFilesDone.sh &";
	system(Move_Files.c_str());
	std::cout << "---------------------" << std::endl;

	IsInsideStopStartRun = false;
}

void Stop_Run()
{
	if(RUN_ENABLED)
	{
		while(IsInsideRunTimer){
			std::cout << "Waiting 2 Seconds for Run Timer to finish ..." << std::endl;
			sleep(2);
		}
		RUN_ENABLED = false;
		std::cout << "Disabling Run_Enabled Flag ..." << std::endl;
		std::cout << "Stopping the Run Manually ..." << std::endl;
		sleep(1);
		Stop_Triggering();
		sleep(1);
		std::cout << "Run is stopped Manually." << std::endl;
		std::cout << "---------------------" << std::endl;
	}
}

void Check_Run_Status()
{
	if(RUN_ENABLED)
	{
		IsInsideRunTimer = true;
		std::cout << "Stopping the Run automatically ..." << std::endl;
		Stop_Triggering();
		sleep(1);
		std::cout << "Starting the Run automatically ..." << std::endl;
		Start_Run();
		IsInsideRunTimer = false;
	}
}

void Archive_All_Log_Files()
{
	// Create a folder inside Archive LOG directory for today's log files
	std::string Today_DIR = ARCHIVE_DIR+Get_Date_Str()+"/";
	std::string Create_CMD = "sudo mkdir -p "+Today_DIR+" &";
	system(Create_CMD.c_str());
	sleep(1);
	std::string LOGS_CopyCMD = "sudo cp " +LOG_DIR+"*.log "+Today_DIR+" &";
	system(LOGS_CopyCMD.c_str());
	sleep(10);
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
///////////////// State MSG Commands ///////////////////
////////////////////////////////////////////////////////

void Enable_State_MSG()
{
	EN_STATE_MSG = true;
}

void Disable_State_MSG()
{
	EN_STATE_MSG = false;
}

std::string Get_Camera_HV_State()
{
	//std::cout << "Getting HV State from SIABs" << std::endl;
	wqsiab.send(GET_ALL_SIAB_HV_STATE);
	sleep(2);

	FILE *hFile = NULL;
	uint32_t *hBuffer = NULL;
	uint32_t HV_Switch_State = 0;
  uint32_t HV_State[16] = {0};
	std::string HV_State_str = "0";
	std::string hv_filename = "hv_state_tmp.txt";
  hFile = fopen(hv_filename.c_str(),"rb");
  if(hFile == NULL){
  	std::cout << "Could not open HV State file. Does it exist?" << std::endl;
    std::cout << "Error code: " << errno << std::endl;
    HV_State_str = Short_to_Str(HV_Switch_State);
  }else{
  	fseek(hFile, 0, SEEK_END);
  	int hSize = ftell(hFile);
  	if(hSize != 64){
  		std::cout << "HV State File Size Mismatch. Size: " << hSize << "\tSkipping this sample"<< std::endl;
  	}else{
  		rewind(hFile);
  		hBuffer = (uint32_t*) malloc (sizeof(uint32_t)*16);
  		size_t hResult = fread(hBuffer, 1, hSize, hFile);
  		for(int i=0; i<16; i++){
  			HV_State[i] = *(hBuffer + i);
				if(HV_State[i] == 1){
					//Doing nothing. Leaving the bit to be zero.
				}
				else if(HV_State[i] == 2){
					HV_Switch_State = (HV_Switch_State | (1 << i));
				}else{
					std::cout << "Unknown HV State for SIAB#" << i << ": " << HV_State[i] << std::endl;
				}
			}
		}
		HV_State_str = Short_to_Str(HV_Switch_State);
		//std::cout << "HV switch: " << HV_Switch_State << std::endl;

		fclose(hFile);
		free(hBuffer);
	}

	system("sudo rm hv_state_tmp.txt &");
	usleep(1000);
	return HV_State_str;
}

std::string Get_Camera_Temp()
{
	std::string Temp_str;
	//std::cout << "Getting Camera SiPM Temperature" << std::endl;
  wqsiab.send(GET_ALL_SIAB_SIPM_TEMP);
	sleep(2);
  FILE *sFile = NULL;
  uint32_t *sBuffer = NULL;
  uint32_t SiPM_temp[16] = {0};
  std::string sipm_filename = "sipm_tmp.txt";
  sFile = fopen(sipm_filename.c_str(),"rb");
  if(sFile == NULL){
  	std::cout << "Could not open SiPM Temp file. Does it exist?" << std::endl;
  	std::cout << "Error code: " << errno << std::endl;
  }else{
  	fseek(sFile, 0, SEEK_END);
  	int sSize = ftell(sFile);
  	if(sSize != 64){
  		std::cout << "SiPM Temp File Size Mismatch. Size: " << sSize << "\tSkipping this sample"<< std::endl;
  	}else{
  		rewind(sFile);
  		sBuffer = (uint32_t*) malloc (sizeof(uint32_t)*16);
  		size_t sResult = fread(sBuffer, 1, sSize, sFile);
  		for(int i=0; i<16; i++){
  			SiPM_temp[i] = *(sBuffer+i);
  		}
  	}
  	fclose(sFile);
  	free(sBuffer);
  }

  for(int i=0; i<16; i++){
  	std::string stemp_str = Short_to_Str(SiPM_temp[i]);
  	Temp_str += stemp_str;
  }

  system("sudo rm sipm_tmp.txt &");
  usleep(10000);

  //std::cout << "Getting Camera UC Temperature" << std::endl;
  wqsiab.send(GET_ALL_SIAB_UC_TEMP);
  sleep(2);
  FILE *uFile = NULL;
  uint32_t *uBuffer = NULL;
  uint32_t UC_temp[16] = {0};
  std::string UC_temp_str = "0";
  std::string uc_filename = "uc_tmp.txt";
  uFile = fopen(uc_filename.c_str(),"rb");
  if(uFile == NULL){
  	std::cout << "Could not open UC Temp file. Does it exist?" << std::endl;
  	std::cout << "Error code: " << errno << std::endl;
  }else{
  	fseek(uFile, 0, SEEK_END);
  	int uSize = ftell(uFile);
    if(uSize != 64){
    	std::cout << "UC Temp File Size Mismatch. Size: " << uSize << "\tSkipping this sample"<< std::endl;
    }else{
    	rewind(uFile);
    	uBuffer = (uint32_t*) malloc (sizeof(uint32_t)*16);
    	size_t uResult = fread(uBuffer, 1, uSize, uFile);
    	for(int i=0; i<16; i++){
    		UC_temp[i] = *(uBuffer+i);
    	}
    }
    fclose(uFile);
    free(uBuffer);
  }

  for(int i=0; i<16; i++){
  	std::string utemp_str = Short_to_Str(UC_temp[i]);
  	Temp_str += utemp_str;
  }

  system("sudo rm uc_tmp.txt &");
  usleep(10000);
  return Temp_str;
}

bool Get_State_MSG()
{
	if(EN_STATE_MSG && (!IsProcNewCMD))
	{
		// This is being checked in Process_Incoming_CMD function every time a new command arrives.
		// If it is true, that function sleeps until this flag turns false.
		IsTakingStateMSG = true;

		// This step adds the status of the processes and trigger rate to state messages
		int PR_State = (((SIAB_STATE) & 0x01) | ((TRGB_STATE<<1) & 0x02) | ((COBO_STATE<<2) & 0x04) | ((LVPS_STATE<<3) & 0x08));
		if(NewCMDFlag) {
			PR_State = (PR_State | 0x10);
		}
		std::string state_msg = "C" + Byte_to_Str(PR_State);

		state_msg += Short_to_Str(TrigRate);

		// This step adds the UNIX timestamp to the state message
		unsigned long int sec = time(NULL);
		state_msg += Word_to_Str((uint32_t)sec);

		// This step adds the HK data to the state message
		uint32_t* hk_data;
		hk_data = Get_LVPS_data();
		uint32_t Music_PWR_State = 0;
		uint32_t SIAB_current[16] = {0};
		uint32_t HV_current[4] = {0};
		uint32_t HV_value[4] = {0};
		uint32_t DAQ_current[4] = {0};
		std::string HV_Value_str;
		std::string HV_Current_str;
		std::string SIAB_Current_str;
		for(int i=0; i<16; i++){
			SIAB_current[i] = *(hk_data+i);
			SIAB_Current_str += Word_to_Str(SIAB_current[i]);
			if(SIAB_current[i] > 4096000){
				Music_PWR_State = (Music_PWR_State | (1 << i));
			}
		}
		std::string Music_PWR_str = Short_to_Str(Music_PWR_State);

		for(int i=0; i<4; i++){
			HV_value[i] = *(hk_data+i+48);
			HV_Value_str = HV_Value_str + Word_to_Str(HV_value[i]);
			HV_current[i]  = *(hk_data+i+32);
			HV_Current_str += Word_to_Str(HV_current[i]);
			DAQ_current[i] = *(hk_data+i+40);
		}

		// This part is checking the HV current consumption and if it is above 20mA, it shuts down SIABs HV switches connected to that channel.
		for(int i=0; i<4; i++){
			if(((double)HV_current[i]*2.441406E-06) > 20.0){
				std::cout << "---------------------------------------------------------------------" << std::endl;
				std::cout << "---------------------------------------------------------------------" << std::endl;
				std::cout << "UNIX Time is: " << Word_to_Str((uint32_t)sec) << std::endl;
				std::cout << "Measured HV on channel#" << i << ": " << ((double)HV_current[i]*2.441406E-06) << " mA" << std::endl;
				std::cout << "Turning off HV channel#" << i << std::endl;
				wqsiab.send(turn_off_hv[i]);
				std::cout << "This is HV for 4 SIABs on the " << SIAB_HV_list[i] << " of the camera looking at the focal plane" << std::endl;
				std::cout << "---------------------------------------------------------------------" << std::endl;
				std::cout << "---------------------------------------------------------------------" << std::endl;
				sleep(1);
			}
		}

		std::string AsAd_Current_str = Word_to_Str(DAQ_current[0]);
		std::string TB_Current_str = Word_to_Str(DAQ_current[2]);

		std::string Camera_Temp_str = Get_Camera_Temp();
		usleep(1000*100);
		std::string Camera_HV_str = Get_Camera_HV_State();

		state_msg = state_msg + Music_PWR_str + Camera_HV_str + AsAd_Current_str + TB_Current_str + HV_Value_str + HV_Current_str + Camera_Temp_str + SIAB_Current_str;

		// This is a temporary file that is always being overwritten with most recent message
		// This file is being copied by grafana for the monitoring purposes.
		std::string smtmp_filename = ARCHIVE_DIR+"SM_LOG.bin";
		std::ofstream SM_TMP_File(smtmp_filename);
		SM_TMP_File << state_msg;
		SM_TMP_File.close();

		// This is the file that archives all state messages for current date.
		std::string smlog_filename = ARCHIVE_DIR+"State_MSG_LOG_"+Get_Date_Str()+".bin";
		std::ofstream StateMSG_File(smlog_filename, ios::app);
		StateMSG_File << state_msg;
		StateMSG_File.close();
		IsTakingStateMSG = false;
	}
	return true;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
///////////////// Trigger Scan Commands ////////////////
////////////////////////////////////////////////////////

// In this function, we update the TB configuration command with the new StepDuration code.
// This code is a number between 0 and 5 and would be translated to a rate counter period as following
// 0x00 = 1 Second, 0x01 = 2 Second, 0x02 = 5 Second, 0x03 = 10 Seconds, 0x04 = 20 Seconds, 0x05 = 30 Seconds
std::string Update_TScan_Config(int StepDuration_Code, const std::string Tscan_Config)
{
	std::string dtemp_hex = fcutils::string::int_to_hex(StepDuration_Code);
	std::string stp_duration_code_str = dtemp_hex.substr(8, 2);
	std::string result = Tscan_Config.substr(0, 4) + stp_duration_code_str + Tscan_Config.substr(6, 10);
	std::cout << "TScan Configuration Command: " << result << std::endl;
	return result;
}

std::string Update_TScan_Gname(int Run_Num, int StartPoint, int NofSteps, int StepSize, int StepDuration_Code)
{
	std::string RunNum_hex = fcutils::string::int_to_hex(Run_Num);
	std::string RunNum_str = RunNum_hex.substr(8, 2);

	std::string StartPoint_hex = fcutils::string::int_to_hex(StartPoint);
	std::string StartPoint_str = StartPoint_hex.substr(6, 4);

	std::string NofSteps_hex = fcutils::string::int_to_hex(NofSteps);
	std::string NofSteps_str = NofSteps_hex.substr(8, 2);

	std::string StepSize_hex = fcutils::string::int_to_hex(StepSize);
	std::string StepSize_str = StepSize_hex.substr(8, 2);

	std::string StepDuration_hex = fcutils::string::int_to_hex(rate_counter_period[StepDuration_Code]);
	std::string StepDuration_str = StepDuration_hex.substr(8, 2);

	std::string GC_CMD = "0A01" + RunNum_str + StartPoint_str + NofSteps_str + StepSize_str + StepDuration_str;
	return GC_CMD;
}

std::string Update_TScan_Cname(int Run_Num, int StartPoint, int NofSteps, int StepSize, int StepDuration_Code)
{
	std::string RunNum_hex = fcutils::string::int_to_hex(Run_Num);
	std::string RunNum_str = RunNum_hex.substr(8, 2);

	std::string StartPoint_hex = fcutils::string::int_to_hex(StartPoint);
	std::string StartPoint_str = StartPoint_hex.substr(6, 4);

	std::string NofSteps_hex = fcutils::string::int_to_hex(NofSteps);
	std::string NofSteps_str = NofSteps_hex.substr(8, 2);

	std::string StepSize_hex = fcutils::string::int_to_hex(StepSize);
	std::string StepSize_str = StepSize_hex.substr(8, 2);

	std::string StepDuration_hex = fcutils::string::int_to_hex(rate_counter_period[StepDuration_Code]);
	std::string StepDuration_str = StepDuration_hex.substr(8, 2);

	std::string CC_CMD = "0B01" + RunNum_str + StartPoint_str + NofSteps_str + StepSize_str + StepDuration_str;
	return CC_CMD;
}

void Archive_TrigScan(int Tscan_type_index)
{
	std::string type[2] = {"BF", "SF"};

	// Create a directory for today's counters files
	std::string Today_DIR = FINAL_TSCAN_DIR+Get_Date_Str()+"/"+type[Tscan_type_index]+"/"+Get_Time_Str()+"/";
	std::string Create_CMD = "mkdir -p "+Today_DIR+" &";
	system(Create_CMD.c_str());
	usleep(500*1000);
	std::string TScan_MoveCMD   = "mv " +TB_DIR+"TScan_Output_"+type[Tscan_type_index]+"* "+Today_DIR+" &";
	std::string TScan_GMoveCMD  = "mv " +TB_DIR+"TScan_G_* "+Today_DIR+" &";
	std::string TScan_CMoveCMD  = "mv " +TB_DIR+"TScan_C_* "+Today_DIR+" &";
	system(TScan_MoveCMD.c_str());
	usleep(500*1000);
	system(TScan_GMoveCMD.c_str());
	usleep(500*1000);
	system(TScan_CMoveCMD.c_str());
	usleep(500*1000);
	std::cout << "Moving Trigger Scan Counters to: " << Today_DIR << std::endl;
}

void Do_Trigger_Scan_BF(std::string& msg)
{
	printf("\n--------------- Starting BF Trigger Scan ---------------\n");
	// msg contains 7-bytes.
	// Byte 0 and 1 are Start point in DAC units, Byte 2 and 3 are Number of Steps,
	// Byte 4 and 5 are Step Size and Byte 6 is Step Duration.

	int StartPoint   = stoi(msg.substr(0, 4),0,16);
	int NofSteps     = stoi(msg.substr(4, 4),0,16);
	int StepSize     = stoi(msg.substr(8, 4),0,16);
	int StepDuration = stoi(msg.substr(12,2),0,16);
	std::cout << "Start Point: " << StartPoint << std::endl;
	std::cout << "Number of Steps: " << NofSteps << std::endl;
	std::cout << "Step Size: " << StepSize << std::endl;
	std::cout << "Step Duration: " << rate_counter_period[StepDuration] << " Seconds" << std::endl;

	std::string TScan_Config = Update_TScan_Config(StepDuration, TB_Tscan_BF_Config);						// This updates the TB config based on step duration code.
	std::string TScan_Get_GC = Update_TScan_Gname(BF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.
	std::string TScan_Get_CC = Update_TScan_Cname(BF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.

	wqtrgb.send(TB_Init);
	sleep(1);

	char tmp[100];
	char filename[200];
	std::ofstream TScan_File;
	strcpy(filename, TB_DIR.c_str());
	strcat(filename, "TScan_Output_BF_");
	strcat(filename, Get_DateTime_Str().c_str());
	sprintf(tmp, "_%02d.txt", BF_TScanNo);
	strcat(filename, tmp);
	TScan_File.open(filename, ios::app|ios::ate);
	TScan_File << "Step" << "\t" << "Threshold" << "\t" << "Rate" << std::endl;

	for(int i=0; i<NofSteps; i++)
	{
		int trg_threshold = StartPoint + i*StepSize;
		std::stringstream trg_stream;
		trg_stream << std::setfill('0') << std::setw(4) << std::hex << trg_threshold;
		std::string siab_cmd = "0000FFFF2503" + trg_stream.str();
		printf("-------------------------------------------------\n");
		std::cout << "Setting the threshold on Music chips to: " << trg_threshold << " DAC units." << std::endl;
		wqsiab.send(siab_cmd);
		sleep(10);
		std::cout << "Done." << std::endl;

		std::cout << "Preparing the Trigger Board" << std::endl;
		wqtrgb.send(TB_Prepare);
		sleep(1);
		std::cout << "Configuring the Trigger Board" << std::endl;
		wqtrgb.send(TScan_Config);
		sleep(2);
		std::cout << "Starting the Global Trigger\n" << std::endl;
		wqtrgb.send(TB_Enable_Trigger);

		std::cout << "Waiting for " << rate_counter_period[StepDuration] << " Seconds\n" << std::endl;
		sleep(rate_counter_period[StepDuration]);

		// Waiting another 10ms to make sure rate period is over.
		usleep(1000*10);

		std::cout << "Stopping the Global Trigger" << std::endl;
		wqtrgb.send(TB_Disable_Trigger);
		sleep(2);

		std::cout << "Saving All the Counters" << std::endl;
		wqtrgb.send(TB_Save_Counters);
		sleep(1);

		std::cout << "Recording Channel Counters" << std::endl;
		wqtrgb.send(TScan_Get_CC);
		sleep(2);

		std::cout << "Recording Global Counters" << std::endl;
		wqtrgb.send(TScan_Get_GC);
		sleep(2);

		int Trigger_Rate = stoi(state_response.substr(8, 8), 0 ,16);
		state_response.clear();
		TScan_File << i << "\t\t" << trg_threshold << "\t\t" << Trigger_Rate << std::endl;
	}

	TScan_File.close();
	std::cout << "Finished BF Trigger Scan Number:" << BF_TScanNo << std::endl;
	printf("-------------------------------------------------\n");

	// Passing 0 will archive BF Tscan files
	Archive_TrigScan(0);
	BF_TScanNo++;

	printf("-------------------------------------------------\n");
	printf("-------------------------------------------------\n\n");
}

void Do_Trigger_Scan_SF(std::string& msg)
{
	printf("\n--------------- Starting Single Focus Trigger Scan ---------------\n");
	// msg contains 7-bytes.
	// Byte 0 and 1 are Start point in DAC units, Byte 2 and 3 are Number of Steps,
	// Byte 4 and 5 are Step Size and Byte 6 is Step Duration.

	int StartPoint   = stoi(msg.substr(0, 4),0,16);
	int NofSteps     = stoi(msg.substr(4, 4),0,16);
	int StepSize     = stoi(msg.substr(8, 4),0,16);
	int StepDuration = stoi(msg.substr(12,2),0,16);
	std::cout << "Start Point: " << StartPoint << std::endl;
	std::cout << "Number of Steps: " << NofSteps << std::endl;
	std::cout << "Step Size: " << StepSize << std::endl;
	std::cout << "Step Duration: " << rate_counter_period[StepDuration] << " Seconds" << std::endl;

	std::string TScan_Config = Update_TScan_Config(StepDuration, TB_Tscan_SF_Config);							// This updates the TB config based on step duration code.
	std::string TScan_Get_GC = Update_TScan_Gname(SF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.
	std::string TScan_Get_CC = Update_TScan_Cname(SF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.

	wqtrgb.send(TB_Init);
	sleep(1);

	char tmp[100];
	char filename[100];
	std::ofstream TScan_File;
	strcpy(filename, TB_DIR.c_str());
	strcat(filename, "TScan_Output_SF_");
	strcat(filename, Get_DateTime_Str().c_str());
	sprintf(tmp, "_%02d.txt", SF_TScanNo);
	strcat(filename, tmp);
	TScan_File.open(filename, ios::app|ios::ate);
	TScan_File << "Step" << "\t" << "Threshold" << "\t" << "Rate" << std::endl;

	for(int i=0; i<NofSteps; i++)
	{
		int trg_threshold = StartPoint + i*StepSize;
		std::stringstream trg_stream;
		trg_stream << std::setfill('0') << std::setw(4) << std::hex << trg_threshold;
		std::string siab_cmd = "FFFFFFFF2503" + trg_stream.str();
		printf("-------------------------------------------------\n");
		std::cout << "Setting the threshold on Music chips to: " << trg_threshold << " DAC units." << std::endl;
		wqsiab.send(siab_cmd);
		sleep(10);
		std::cout << "Done." << std::endl;

		std::cout << "Preparing the Trigger Board" << std::endl;
		wqtrgb.send(TB_Prepare);
		sleep(1);
		std::cout << "Configuring the Trigger Board" << std::endl;
		wqtrgb.send(TScan_Config);
		sleep(2);
		std::cout << "Starting the Global Trigger\n" << std::endl;
		wqtrgb.send(TB_Enable_Trigger);

		std::cout << "Waiting for " << rate_counter_period[StepDuration] << " Seconds\n" << std::endl;
		sleep(rate_counter_period[StepDuration]);

		// Waiting another 10ms to make sure rate period is over.
		usleep(1000*10);

		std::cout << "Stopping the Global Trigger" << std::endl;
		wqtrgb.send(TB_Disable_Trigger);
		sleep(2);

		std::cout << "Saving All the Counters" << std::endl;
		wqtrgb.send(TB_Save_Counters);
		sleep(1);

		std::cout << "Recording Channel Counters" << std::endl;
		wqtrgb.send(TScan_Get_CC);
		sleep(2);

		std::cout << "Recording Global Counters" << std::endl;
		wqtrgb.send(TScan_Get_GC);
		sleep(2);

		int Trigger_Rate = stoi(state_response.substr(8, 8), 0 ,16);
		state_response.clear();
		TScan_File << i << "\t\t" << trg_threshold << "\t\t" << Trigger_Rate << std::endl;
	}

	TScan_File.close();
	std::cout << "Finished SF Trigger Scan Number:" << SF_TScanNo << std::endl;
	printf("-------------------------------------------------\n");

	// Passing 1 will archive Single Focus Tscan files
	Archive_TrigScan(1);
	SF_TScanNo++;

	printf("-------------------------------------------------\n");
	printf("-------------------------------------------------\n\n");
}

// Performs a SF trigger scan, pixel-wise.
// Raise threshold on all  pixels not scanned, record the threshold and 
// rate. Once completed, switch to different pixel.

void Do_Trigger_Scan_SF_PBP(std::string& msg)
{
	printf("\n--------------- Starting Single Focus Trigger Scan ---------------\n");
	// msg contains 7-bytes.
	// Byte 0 and 1 are Start point in DAC units, Byte 2 and 3 are Number of Steps,
	// Byte 4 and 5 are Step Size and Byte 6 is Step Duration.
	//int NoOfPixels = 16*16;
	//int NoOfPixels = 16;
	int NoOfPixels = 8;
	int StartPoint   = stoi(msg.substr(0, 4),0,16);
	int NofSteps     = stoi(msg.substr(4, 4),0,16);
	int StepSize     = stoi(msg.substr(8, 4),0,16);
	int StepDuration = stoi(msg.substr(12,2),0,16);
	std::cout << "Start Point: " << StartPoint << std::endl;
	std::cout << "Number of Steps: " << NofSteps << std::endl;
	std::cout << "Step Size: " << StepSize << std::endl;
	std::cout << "Step Duration: " << rate_counter_period[StepDuration] << " Seconds" << std::endl;

	std::string TScan_Config = Update_TScan_Config(StepDuration, TB_Tscan_SF_Config);							// This updates the TB config based on step duration code.
	std::string TScan_Get_GC = Update_TScan_Gname(SF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.
	std::string TScan_Get_CC = Update_TScan_Cname(SF_TScanNo, StartPoint, NofSteps, StepSize, StepDuration);	// This just updates the TB command with run number.

	wqtrgb.send(TB_Init);
	sleep(1);

	char tmp[100];
	char filename[100];
	std::ofstream TScan_File;
	strcpy(filename, TB_DIR.c_str());
	strcat(filename, "TScan_Output_SF_PBP");
	strcat(filename, Get_DateTime_Str().c_str());
	sprintf(tmp, "_%02d.txt", SF_TScanNo);
	strcat(filename, tmp);
	TScan_File.open(filename, ios::app|ios::ate);
	TScan_File << "Th[DAC]:" << "\t";
	for(int i = 0; i < NofSteps; i++){
		TScan_File << StartPoint + i*StepSize<<"\t";
	}
	TScan_File<<std::endl;
	TScan_File << "Pixel #" << "\t\t" << "Rate" << std::endl;

	printf("-------------------------------------------------\n");
	printf("Setting all discriminators to Max Threshold\n");
	printf("-------------------------------------------------\n");
	std::stringstream trg_stream;
	trg_stream << std::setfill('0') << std::setw(4) << std::hex << 0;
	std::string siab_cmd = "0000FFFF2503" + trg_stream.str();
	wqsiab.send(siab_cmd);
	sleep(10);
	std::cout << "Done." << std::endl;

	std::cout << "Preparing the Trigger Board" << std::endl;
	wqtrgb.send(TB_Prepare);
	sleep(1);
	std::cout << "Configuring the Trigger Board" << std::endl;
	wqtrgb.send(TScan_Config);
	sleep(2);


	for(int i=0; i<NoOfPixels; i++){
		printf("-------------------------------------------------\n");
		printf("Running Trigger Scan on pixel no.: %d\n",i);

		std::stringstream siabID;
		//int siabNum = i/16;
		//int siabNum = i;
		//siabID << std::setfill('0') << std::setw(8) << std::hex << (1 << siabNum);
		siabID << std::setfill('0') << std::setw(8) << std::hex << 0x0000FFFF;
		TScan_File << i << "\t\t";

		/*int musicCh = (((i%8) + 4) & 0x7); //when i is pixel id (range 0-255)
		int musicLoc = ((((i/8)%2) + 1) << 4 | musicCh); //when i is pixel id (range 0-255)

		int musicCh = (((i%8) + 4) & 0x7); //when i is siab channel id (range 0-15)
		int musicLoc = (((i%8) + 1) << 4 | musicCh); //when i is siab channel id (range 0-15)*/

		int musicLoc = (3 << 4 | i); //when i is music channel id (range 0-7)

		std::stringstream music_pos;
		music_pos << std::setfill('0') << std::setw(2) << std::hex << musicLoc;
		//music_pos << std::setfill('0') << std::setw(2) << std::hex << 0x30;

		for(int j=0; j<NofSteps; j++){
			int trg_threshold = StartPoint + j*StepSize;
			trg_stream.str("");
			trg_stream << std::setfill('0') << std::setw(4) << std::hex << trg_threshold;
			//trg_stream << std::setfill('0') << std::setw(1) << std::hex << i%16;
			siab_cmd = siabID.str() + "26" + music_pos.str() + trg_stream.str();
			printf("-------------------------------------------------\n");
			std::cout << "Setting the threshold on Music chips to: " << trg_threshold << " DAC units." << std::endl;
			wqsiab.send(siab_cmd);
			sleep(10);
			std::cout << "Done." << std::endl;

			std::cout << "Starting the Global Trigger\n" << std::endl;
			wqtrgb.send(TB_Enable_Trigger);

			std::cout << "Waiting for " << rate_counter_period[StepDuration] << " Seconds\n" << std::endl;
			sleep(rate_counter_period[StepDuration]);

			// Waiting another 10ms to make sure rate period is over.
			usleep(1000*10);

			std::cout << "Stopping the Global Trigger" << std::endl;
			wqtrgb.send(TB_Disable_Trigger);
			sleep(2);

			std::cout << "Saving All the Counters" << std::endl;
			wqtrgb.send(TB_Save_Counters);
			sleep(1);

			std::cout << "Recording Channel Counters" << std::endl;
			wqtrgb.send(TScan_Get_CC);
			sleep(2);

			std::cout << "Recording Global Counters" << std::endl;
			wqtrgb.send(TScan_Get_GC);
			sleep(2);

			int Trigger_Rate = stoi(state_response.substr(8, 8), 0 ,16);
			state_response.clear();
			TScan_File << Trigger_Rate << "\t";
		}
		TScan_File << std::endl;
		printf("-------------------------------------------------\n");
		printf("Setting threshold of pixel no.: %d to Max Threshold\n",i);
		printf("-------------------------------------------------\n");
		trg_stream.str("");
		trg_stream << std::setfill('0') << std::setw(4) << std::hex << 0;
		siab_cmd = siabID.str() + "26" + music_pos.str() + trg_stream.str();
		wqsiab.send(siab_cmd);
		sleep(10);
		std::cout << "Done." << std::endl;

	}
	

	TScan_File.close();
	std::cout << "Finished SF Trigger Scan Number:" << SF_TScanNo << std::endl;
	printf("-------------------------------------------------\n");

	// Passing 1 will archive Single Focus Tscan files
	Archive_TrigScan(1);
	SF_TScanNo++;

	printf("-------------------------------------------------\n");
	printf("-------------------------------------------------\n\n");
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
///////////////// Control SW Commands //////////////////
////////////////////////////////////////////////////////

std::string find_seq_name(const std::string& seq_msg)
{
        std::string seq_str = seq_msg.substr(0, 2);
        int seq_id = stoi(seq_str,0,16);
        std::string subseq_str = seq_msg.substr(2, 2);
        std::cout << "Sequence ID: " << seq_id << " was received." << std::endl;
        std::string filename = "";
        switch(seq_id)
        {
                case POWER_ON_SEQ:
                        filename = SEQ_DIR + "power_on_seq.txt";
                        break;
                case INIT_SEQ:
                        filename = SEQ_DIR + "init_seq.txt";
                        break;
                case CONFIGURE_SEQ:
                        filename = SEQ_DIR + "config_seq_"+subseq_str+".txt";
                        break;
                case START_SEQ:
                        filename = SEQ_DIR+ "start_daq_seq.txt";
                        break;
                case STOP_SEQ:
                        filename = SEQ_DIR + "stop_daq_seq.txt";
                        break;
                case POWER_OFF_SEQ:
                        filename = SEQ_DIR + "power_off_seq.txt";
                        break;
                default:
                        break;
        }
        return filename;
}

void Process_RC_Msg(const std::string& msg)
{
	std::string CMD_str = msg.substr(0, 2);
	std::string msg_payload = msg.substr(2, 14);
	int CMD_ID = stoi(CMD_str,0,16);

	switch(CMD_ID)
	{
		case REBOOT_CPU_ID:
			Reset_CT_CPU();
			break;
		case SHUTDOWN_CPU_ID:
			Shutdown_CT_CPU();
			break;
		case Init_SIAB_ID:
			Init_SIAB();
			break;
		case Init_TRGB_ID:
			Init_TRGB();
			break;
		case Init_COBO_ID:
			Init_COBO();
			break;
		case Init_LVPS_ID:
			Init_LVPS();
			break;
		case Update_Runtime_ID:
			Update_Runtime(msg_payload);
			break;
		case RUN_START:
			Start_Run();
			break;
		case RUN_STOP:
			Stop_Run();
			break;
		case STATE_MSG_EN_ID:
			Enable_State_MSG();
			break;
		case STATE_MSG_DIS_ID:
			Disable_State_MSG();
			break;
		case DO_TRG_SCAN_BF:
			Do_Trigger_Scan_BF(msg_payload);
			break;
		case DO_TRG_SCAN_SF:
			Do_Trigger_Scan_SF(msg_payload);
			break;
		case DO_TRG_SCAN_SF_PBP:
			Do_Trigger_Scan_SF_PBP(msg_payload);
			break;
		case Kill_SIAB_ID:
			Kill_SIAB();
			break;
		case Kill_TRGB_ID:
			Kill_TRGB();
			break;
		case Kill_COBO_ID:
			Kill_COBO();
			break;
		case Kill_LVPS_ID:
			Kill_LVPS();
			break;
		case Kill_dataRouter_ID:
			Kill_dataRouter();
			break;
		case Kill_getEccServer_ID:
			Kill_getEccServer();
			break;
		case Kill_ALL_ID:
			Kill_All();
			break;
		case Archive_LOGS_ID:
			Archive_All_Log_Files();
			break;
		case Do_HV_Current_Scan:
			HV_Current_Scan(msg_payload);
			break;
		default:
			break;
	}
}

void Process_Incoming_CMD(const std::string& msg)
{
	while(IsTakingStateMSG){
		sleep(1);
	}

	// This is to make sure new commands and state messages
	// are not using the queue at the same time.
	IsProcNewCMD = true;

	std::string qid_str = msg.substr(0, 2);
	std::string msg_payload = msg.substr(2, 16);
	int qid = stoi(qid_str,0,16);
	std::cout << "CPU Time is: " << Get_DateTime_Str() << std::endl;

	switch(qid)
	{
		case QID_RC:
			Process_RC_Msg(msg_payload);
			break;
		case QID_SIAB:
			wqsiab.send(msg_payload);
			break;
		case QID_TRGB:
			wqtrgb.send(msg_payload);
			break;
		case QID_COBO:
			wqcobo.send(msg_payload);
			break;
		case QID_LVPS:
			wqlvps.send(msg_payload);
			break;
		default:
			break;
	}
	NewCMDFlag = !NewCMDFlag;
	IsProcNewCMD = false;
}

void read_mq_cs(sigval_t sig){
	int numRead{-1};
	// Retrive the posix message queue object from the sigval_t infos.
	posix_message_queue* mqp=static_cast<posix_message_queue*>(sig.sival_ptr);
	auto msg_size=mqp->get_msgsize();
	auto msg=new char[msg_size];
	auto mq_name=mqp->get_name();
	mqp->notify(read_mq_cs);
	while ((numRead = mqp->receive(msg, msg_size)) > 0){
		std::cout << " Queue: " << mq_name <<", message received: " << msg << std::endl;
	}

	std::string umsg(msg);
	std::string umsg_id_str = umsg.substr(0, 2);
	int umsg_id = stoi(umsg_id_str,0,16);
        if(umsg_id == 0xAA)
        {
		std::string seq_payload = umsg.substr(2, 16);
		std::cout << "A sequence was received. Starting to execute the sequence. Here is the sequence payload: " << seq_payload << '\n';
		std::string filename = find_seq_name(seq_payload);

		std::string line;
		std::cout << "Loading the filename: " << filename << std::endl;
		std::ifstream infile;
		infile.open(filename.c_str());
		while(std::getline(infile, line))
		{
		        std::cout << line << std::endl;
		        std::getline(infile, line);
		        std::istringstream iss(line);
		        std::string command = iss.str();
		        std::cout << "sequence message: " << command << std::endl;
		        if(command.substr(0,1) != "*"){
				Process_Incoming_CMD(command);
		        }
		        else{
		                std::cout << "skipping this command ..." << std::endl;
		        }
		        int delay = 0;
		        std::getline(infile, line);
		        std::istringstream iss_delay(line);
		        iss_delay >> delay;
		        std::cout << "delaying for " << delay << " seconds."<< std::endl;
		        sleep(delay);
		}
                std::cout << "Finished loading this sequence: " << filename << std::endl;
                infile.close();
		msg[0]='\0';
		delete [] msg;
	}else{
		Process_Incoming_CMD(msg);
		msg[0]='\0';
		delete [] msg;
	}
}

void read_mq_rc(sigval_t sig){
	int numRead{-1};
	// Retrive the posix message queue object from the sigval_t infos.
	posix_message_queue* mqp=static_cast<posix_message_queue*>(sig.sival_ptr);
	auto msg_size=mqp->get_msgsize();
	auto msg=new char[msg_size];
	auto mq_name=mqp->get_name();
	mqp->notify(read_mq_rc);
	while ((numRead = mqp->receive(msg, msg_size)) > 0){
		std::cout << " Queue: " << mq_name <<", message received: " << msg << std::endl;
	}
	state_response = msg;
	delete [] msg;
}

void master_timer(std::function<void(void)> func, uint32_t interval)
{
	std::thread([func, interval]()
	{
		while(true)
		{
			auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
			func();
			std::this_thread::sleep_until(x);
		}
	}).detach();
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	if(argc > 1) {
		ftdi_device_name = argv[1];
		std::cout << "Using given USB port: " << ftdi_device_name << " for LVPS microcontroller board" << std::endl;
	}else{
		std::cout << "Using default USB port: " << ftdi_device_name << " for LVPS microcontroller board" << std::endl;
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << "Staring Master Control at: " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;

	fstream runtime_file;
	std::string filename = CS_DIR+"fcutils/test/include/Run_Duration.txt";
	runtime_file.open(filename.c_str(),ios::in);
	if(runtime_file.is_open())
	{
		std::string run_time_str = "120";
		if(getline(runtime_file, run_time_str))
		{
			run_interval = stoi(run_time_str);
		}
		runtime_file.close();
	}

	rqcs.open();
	wqcs.open();
	rqrc.open();
	wqsiab.open();
	wqtrgb.open();
	wqcobo.open();
	wqlvps.open();
	wqpdu.open();

	rqcs.notify(read_mq_cs);
	rqrc.notify(read_mq_rc);

	master_timer(Check_Run_Status, run_interval*1000);
	master_timer(Get_State_MSG, state_msg_interval*1000);

	while(true)
	{
		usleep(100000);
	}

	return 0;
}
