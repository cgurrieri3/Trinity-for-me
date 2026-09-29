#include <TROOT.h>
#include "TSystem.h"
#ifndef IFHBEVENT_H
#define IFHBEVENT_H

#include "Event.h"

using namespace std;
/**
 * \class IEvent
 * \brief Class that contains the raw data plus the auxilliary data
 * 
 * Inherits from class Event and adds new data to each of the events based on the auxilliary data obtained from the State Messages.
 * Data contains information about the telescope conditions at the time of trigger
 * @see Event()
 * 
 * */

class IEvent : public Event{
	public:
		/** 
		 * Public constructor
		 * */
		IEvent(); 
		/**
		 * Public destructor
		 * */
		~IEvent(); 

	protected:
		
		float IcorrectionFactor;/**< Depreacted*/


	private:
		// Parameters from the first file
		float sunAzimuth;/**< float containing the azimuth of the Sun*/
		float sunElevation;/**< float containing the elevation of the Sun*/
		float moonAzimuth;/**< float containing the azimuth of the Moon*/
		float moonElevation;/**< float containing the elevation of the Moon*/
		float moonIllumination;/**< float containing the elevation of the Moon*/
		float CameraRA;/**< float containing the part of the sky in the FOV RA*/
    	float CameraDEC;/**< float containing the part of the sky in the FOV DEC*/


		// Parameters from the second file
		int TrigEvent; /**< int containing the number of triggers in the run*/
		std::vector<float> hv; /**< std::vector of size 8 containing the setting of the 8 different high voltage channels*/
		std::vector<float> hvc;/**< std::vector of size 8 containing the current of the * different high voltage channels*/
		std::vector<float> ucTemp; /**< std::vector of size 32 containing the temperature for the 32 microControllers*/
		std::vector<int> siabMPWR;/**< std::vector<int> of the MUSIC power switch*/
		std::vector<int> hvSW;/**< std::vector<int> of the High voltage power switch*/
		std::vector<float> sipmTemp;/**< std::vector<float> of the temperature of the SiPM matrices*/
		std::vector<float> siabCurr;
		float ASADcurr;
		float TBcurr;

		// Parameters from the weather file
		float outsideTempature;
		float humidity;

		//
		bool isOpen;
		//


	public:
		// void LoadFromFile(const std::string& filename1, const std::string& filename2);
		/**
		 * Public method to fill in most of the member variables. Vectors have size row x cols, depending on the csv file used to obtain them.
		 * Each of the rows corresponds to a different time stamp and each column to different values of the different auxilliary data sources. Column 0 should be the timestamp
		 * 
		 * 
		 * @param timestamp An int storing the time of trigger in seconds after a preset vlaue (May 13th)
		 * @param data A std::vector of strings with size row x cols from the .csv file containing the balloon infromation.
		 * @param data2 A std::vector of strings containing the state message information.
		 * */
		//void SetParametersFromTimestamp(std::vector<float> hv_arg, std::vector<float> hvc_arg,std::vector<float> sipmTemp_arg,std::vector<float> UCtemps_arg,std::vector<float> MUSICpower_arg,std::vector<float> HVswitch_arg,std::vector<float> ASADcurr_arg,std::vector<float> siabCurr_arg,std::vector<float> TBCurr_arg,std::vector<float> Humid_arg,std::vector<float> OutTemp_arg);
		void SetParametersFromTimestamp(std::string path_to_exact,std::vector<float> hv_arg, std::vector<float> hvc_arg,std::vector<float> sipmTemp_arg,std::vector<float> UCtemps_arg,std::vector<float> MUSICpower_arg,std::vector<float> HVswitch_arg,std::vector<float> ASADcurr_arg,std::vector<float> siabCurr_arg,std::vector<float> TBCurr_arg,std::vector<float> Humid_arg,std::vector<float> OutTemp_arg,std::vector<float> sunAzi_arg, std::vector<float> SunEle_arg, std::vector<float> MoonAzi_arg, std::vector<float> MoonEle_arg,std::vector<float> MoonIll_arg, std::vector<float> CamRA_arg, std::vector<float> CamDEC_arg);
		/**
		 * Public method to get the closest timestamp to the event in the auxilliary data file1.
		 * 
		 * @return An int with the closest timestamp
		 * */
		int GetclosestTimestamp1() const;
		/**
		 * Public method to get the closest timestamp to the event in the auxilliary data file2.
		 * 
		 * @return An int with the closest timestamp
		 * */
		int GetclosestTimestamp2() const;
		/**
		 * Public method to get the azimuth of the Sun's location in degrees
		 * 
		 * @return A float with the azimuth in degrees
		 * */
		float GetSunAzimuth() const;
		/**
		 * Public method to get the elevation of the Sun's location in degrees
		 * 
		 * @return A float with the elevation in degrees
		 * */
		float GetSunElevation() const;
		/**
		 * Public method to get the azimuth of the Moon's location in degrees
		 * 
		 * @return A float with the azimuth in degrees
		 * */
		float GetMoonAzimuth() const;
		/**
		 * Public method to get the elevation of the Moon's location in degrees
		 * 
		 * @return A float with the elevation in degrees
		 * */
		float GetMoonElevation() const;
		/**
		 * Public method to get the illumination of the Moon in 0.xx
		 * 
		 * @return A float with the  Moon in 0.xx
		 * */

		float GetMoonIlluminaiton() const;
		/**
		 * Public method to get the number of triggered events in the run
		 * 
		 * @return An int with the number of triggered events
		 * */
		int GetTrigEvent() const;
		/**
		 * Public method to get the std::vector of size 8 (typically) that contains the high voltage setting for each of the 8 channels.
		 * 
		 * @return A std::vector<float> with the high voltage setting in V
		 * */
		const std::vector<float>& Gethv() const;
		/**
		 * Public method to get the std::vector of size 8 (typically) that contains the high voltage current for each of the 8 channels.
		 * 
		 * @return A std::vector<float> with the high voltage current in mA
		 * */
		const std::vector<float>& Gethvc() const;
		/**
		 * Public method to get the std::vector of size 32 (typically) that contains the temperature of the 32 microControllers
		 * 
		 * @return A std::vector<float> with the microController temperature in C
		 * */
		const std::vector<float>& GetUCTemp() const;
		/**
		 * Public method to get the temperature of the CPU
		 * 
		 * @return An int with the CPU temperature in C
		 * */
		
		const std::vector<int>& GetsiabMPWR() const;
		/**
		 * Public method to get the HV switch setting for each of the 32 SiPM matrices
		 * 
		 * @return A std::vector<int> with the HV power switch
		 * */
		const std::vector<int>& GethvSW() const;
		/**
		 * Public method to get the temperature of the SiPMs
		 * 
		 * @return An std::vector<float> with the temperature of the 32 SiPMs in C
		 * */
		const std::vector<float>& GetSiPMTemp() const;
		/**
		 * Public method to get the SIAB current values
		 * 
		 * @return An std::vector<float> with the SIAB of the 16 SiPMs in mA
		 * */
		const std::vector<float>& GetSIABCurrent() const;
		/**
		 * Public method to obtain the ASAD current
		 * 
		 * @return float with the ASAD current value in  mA
		 * */
		float GetASADCurrent() const;
		/**
       	 	* Public method to obtain the TriggerBoard current
		 * 
		 * @return float with the TriggerBoard current value in  mA
		 * */
		float GetTBCurrent() const;
		/**
		 * Public method to find the closest time stamp position in the time vector
		 * 
		 * @param timeArray A std::vector containing all the time stamps from the file
		 * @param eventTime An unsigned long long containing the time when the event was recorded in 10s of nano seconds.
		 * @return An int with the position of the closest time stamp
		 * */
		float GetOutsideTempature() const;
		float GetHumidity() const;
		static int FindClosestTimestampIndex(std::vector<unsigned long long> timeArray, unsigned long long eventTime);

		////
		bool GetDoorPosition(const std::vector<float>& hv, const std::vector<float>& hvc);
		////
			
	private:
		/**
		 * Public method to find the closest time stamp in the data vector 
		 * @see SetParametersFromTimestamp
		 * 
		 * @param data A std::vector containing all the information from the auxilliary data file
		 * @param timestamp An int with the time the event was recorded in seconds after some reference (May 13th)
		 * @return An int with the closest time stamp
		 * */
    	int findClosestTimestamp(const std::vector<std::vector<std::string>>& data, int timestamp) const;

};
#endif
