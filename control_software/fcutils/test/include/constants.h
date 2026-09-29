#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define POWER_ON_SEQ	0x01
#define INIT_SEQ	0x02
#define CONFIGURE_SEQ	0x03
#define START_SEQ	0x04
#define STOP_SEQ	0x05
#define POWER_OFF_SEQ   0x06

#define Process_OFF				0x00
#define Process_ON				0x01

#define QID_RC					0x10
#define QID_SIAB				0x20
#define QID_TRGB				0x30
#define QID_COBO				0x40
#define QID_LVPS				0x50
//#define QID_PDU					0x60

#define REBOOT_CPU_ID			0x01
#define SHUTDOWN_CPU_ID			0x02
#define Init_SIAB_ID			0x03
#define Init_TRGB_ID			0x04
#define Init_COBO_ID			0x05
#define Init_LVPS_ID			0x06
//#define Init_PDU_ID				0x07
//#define Flush_FTP_ID			0x08

#define Update_Runtime_ID		0x09
#define RUN_START				0x0A
#define RUN_STOP				0x0B
#define STATE_MSG_EN_ID			0x0C
#define STATE_MSG_DIS_ID		0x0D
//#define GET_LOG_ID				0x0E
#define DO_TRG_SCAN_BF			0x0F

#define Kill_SIAB_ID			0x10
#define Kill_TRGB_ID			0x11
#define Kill_COBO_ID			0x12
#define Kill_LVPS_ID			0x13
//#define Kill_PDU_ID				0x14
#define Kill_dataRouter_ID		0x15
#define Kill_getEccServer_ID	0x16
#define Kill_ALL_ID				0x17

//#define Fix_LVPS_Client_ID		0x18
#define Archive_LOGS_ID			0x19
#define DO_TRG_SCAN_SF			0x1A
//#define Get_Files_Starlink		0x1B
#define Do_HV_Current_Scan		0x1C
//#define RE_DOWNLOAD_BACKUP		0x1D
#define DO_TRG_SCAN_SF_PBP      0x1E

const std::string HOME_DIR="/home/trinity";
const std::string CS_DIR = HOME_DIR+"/Programs/Trinity/control_software/";
const std::string SCRIPT_DIR = CS_DIR+"fcutils/test/scripts/";
const std::string SEQ_DIR = CS_DIR+"sequences/";
const std::string TB_DIR = CS_DIR+"tb/mesaflash/";
const std::string CoBo_DIR = CS_DIR+"CoBo/";
const std::string PDU_DIR = CS_DIR+"pdu/";
const std::string LOG_DIR = CS_DIR+"fcutils/test/LOGS/";
const std::string ARCHIVE_DIR = CS_DIR+"Archive/LOGS/";
const std::string FINAL_TSCAN_DIR = CS_DIR+"/Archive/Tscan/";
const std::string FINAL_COUNTERS_DIR = CS_DIR+"/Archive/TB_Data/Run_Counters/";
const std::string HV_SCAN_DIR = CS_DIR+"/Archive/HV_Scan/";
const std::string FTP_DIR = "/srv/ftp/data/";

// Full Commands for Controlling Trigger Board
// Use TB Configuration Document to modify these!
const std::string TB_Init					= "0200000000000000";
const std::string TB_Prepare				= "0400640000000000";
const std::string TB_Config_BF_Only			= "0501000200000003";
const std::string TB_Config_SF_Only			= "0502000240000003";
const std::string TB_Config_HLED1_Only		= "05040002000F51C3";
const std::string TB_Config_HLED10_Only		= "05040002000F54C3";
const std::string TB_Config_BF_IntHLED1		= "05050002000F51C3";
const std::string TB_Config_BF_IntHLED10	= "05050002000F54C3";
const std::string TB_Tscan_BF_Config		= "0501020200005002";
const std::string TB_Tscan_SF_Config		= "0F060002040058C2";
const std::string TB_Enable_Trigger			= "0800000000000000";
const std::string TB_Disable_Trigger		= "0900000000000000";
const std::string TB_Get_GL_ctrs   			= "0A00000000000000";
const std::string TB_Get_CL_ctrs   			= "0B00000000000000";
const std::string TB_Save_Data				= "0C00000000000000";
const std::string TB_Clear_Counters			= "0D00000000000000";
const std::string TB_Save_Counters			= "0E00000000000000";
const uint32_t rate_counter_period[6]       = {1,2,5,10,20,30};

const std::string CoBo_Start_Run 	 		= "0400000000000000";
const std::string CoBo_Stop_Run 	 		= "0500000000000000";
const std::string CoBo_ReConfigure	 		= "0600000000000000";

const std::string GET_ALL_SIAB_SIPM_TEMP	= "0000FFFF0A010000";
const std::string GET_ALL_SIAB_UC_TEMP		= "0000FFFF0B010000";
const std::string GET_ALL_SIAB_HV_STATE		= "0000FFFF21010000";

const std::string PING_SIAB					= "0000000030000000";
const std::string PING_ALL					= "3000000000000000";

const std::string turn_on_hv[4]     = {"000030301E000000", "0000C0C01E000000", "000003031E000000", "00000C0C1E000000"};
const std::string turn_off_hv[4]    = {"000030301F000000", "0000C0C01F000000", "000003031F000000", "00000C0C1F000000"};

const std::string SIAB_HV_list[4]   = {"Bottom Left", "Top Left", "Bottom Right", "Top Left"};

#endif
