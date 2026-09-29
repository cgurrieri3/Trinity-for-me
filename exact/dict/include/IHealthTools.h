#ifndef IHEALTHTOOLS_H
#define IHEALTHTOOLS_H

#include "Pulse.h"
#include "Event.h"
#include "IEvent.h"
#include "IFile.h"
#include <TString.h>
#include <TROOT.h>

class IHealthTools{
	/**
	 * \class IHealthTools
	 * \brief Class with methods used in analysis of the traces obtained for each of the events
	 * 
	 * All methods require the same arguments to be used. Some of the arguments are optional.
	 * Some methods have been overloadad in order for them to be used under different conditions and applied to different data sets.
	 * 
	 * */
	public:
		/**
		 * Public constructor with no parameters
		 * Only needed for ROOT library creation
		 * */
		IHealthTools();
		/**
		 * Public destructor
		 * */
		~IHealthTools();

		/*
		Obtains the full camera values for the specified measurement
		Measurement options are Pedestal, Pedestal RMS, Amplitude, 
		Charge and Peaking Time.

		All of the methods below require the same arguments.

		IFile *file: Object defined in ExACT. Contains the merged file and the 4 data Trees (HLED, BiFocal, Test, Forced)

		int entry: Entry to be extracted

		unsigned long long lTimeStart: start of time range

		unsigned long long lTimeEnd: end of time range

		std::string tree: tree name to be accessed. It is not case sensitive
		Can be (hled,bifocal,test,forced)

		float voltage: Checks the bias voltage from Auxiliary data. Tolerance
		is 0.1V.

		unsigned long long eventTime: Trigger time for the entry accessed.

		
		returns

		std:vector<float> with the value from the traces. 

		If the event is outside of time range or the camera was biased at a different voltage all elements are set to -1 and eventTime is set to 0

		*/

		/**
		 * Public method to calculate the pedestal of the trace for each of the traces in an event
		 * 
		 * @param file IFile Object defined in ExACT. Contains the merged file and the 4 data Trees (HLED, BiFocal, Test, Forced)
		 * @param entry An int with the entry (event number) to be extracted
		 * @param lTimeStart An unsigned long long with the unix time start of the time window over which to accept events (in 10s of nanoseconds)
		 * @param lTimeEnd An unsigned long long with the unix time end of the time window over which to accept events (in 10s of nanoseconds)
		 * @param tree A std::string with the name of the tree to access.\ Can be (hled,bifocal,test,forced)
		 * @param voltage A float with the voltage setting which to accept events (tolerance is 0.1 V)
		 * @param eventTime An unsigned long long* where to store the time of the event if accepted. (Set to 0 if event does not satisfy one of the time or voltage conditions)
		 * 
		 * Optional parameters
		 * @param iTWStart An int with the start of the time window for the trace analysis (10s of nanoseconds).\ Default is 200
		 * @param iTWEnd An int with the end of the time window for the trace analysis (10s of nanoseconds).\ Default is 300
		 * 
		 * @return std::vector<float> With the calculated pedestal for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */

		static std::vector<float> CameraPedestal(IFile *file, int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the pedestal RMS for each of the traces in the event
		 * @see CameraPedestal
		 * 
		 * @return std::vector<float> With the calculated pedestal for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */
		static std::vector<float> CameraPedestalRMS(IFile *file, int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the amplitude within the specified time window for each of the traces in the event
		 * @see CameraPedestal(IFile *file, int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300)
		 * 
		 * @return std::vector<float> With the calculated amplitude for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */
		static std::vector<float> CameraAmplitude(IFile *file,int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the amplitude within the specified time window for each of the traces in the event
		 * @see CameraPedestal(IFile *file, int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300,float)
		 * 
		 * * @param file IFile Object defined in ExACT. Contains the merged file and the 4 data Trees (HLED, BiFocal, Test, Forced)
		 * @param entry An int with the entry (event number) to be extracted
		 * @param tree A std::string with the name of the tree to access.\ Can be (hled,bifocal,test,forced)
		 * @param iTWStart An int with the start of the time window for the trace analysis (10s of nanoseconds).\ Default is 200
		 * @param iTWEnd An int with the end of the time window for the trace analysis (10s of nanoseconds).\ Default is 300
		 * 
		 * @return std::vector<float> With the calculated amplitude for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */
		static std::vector<float> CameraAmplitude(IFile *file,int entry, std::string tree, int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the amplitude within the specified time window for each of the traces in a std::vector of size 512x512 (typically). Where there are 512 pixels each with 512 digitized samples
		 * 
		 * @param traces A std::vector of ints with size m x n. Where m is the number of pixels and n the number of samples.
		 * @param iTWStart An int with the start of the time window for the trace analysis (10s of nanoseconds).\ Default is 200
		 * @param iTWEnd An int with the end of the time window for the trace analysis (10s of nanoseconds).\ Default is 300
		 * */
		static std::vector<float> CameraAmplitude(std::vector<std::vector<int>> traces, int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the charge within the specified time window for each of the traces in the event
		 * @see CameraPedestal
		 * 
		 * @return std::vector<float> With the calculated charge for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */
		static std::vector<float> CameraCharge(IFile *file,int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to calculate the peaking time within the specified time window for each of the traces in the event
		 * @see CameraPedestal
		 * 
		 * @return std::vector<float> With the peaking time amplitude for each of the traces in the event if accepted. If the event is not accepted, all entries in the vector are set to -1.
		 * */
		static std::vector<float> CameraPeakingTime(IFile *file,int entry,unsigned long long lTimeStart, unsigned long long lTimeEnd, std::string tree, float voltage, unsigned long long *eventTime,int iTWStart=200, int iTWEnd=300);
		/**
		 * Public method to identify if the digitized data from the camera constitutes a bad event. When the system overtriggers (the trigger rate is higher than the electronics can handle), the beginning of the traces overflows the digitizer. This is shown as a flat line pedestal. The method checks all the traces in an event to identify if it constitutes one of these overflowed events.
		 * 
		 * @param traces A std:vector of size m x n, where m is the number of pixels and n the number of samples. Contains the traces for all the pixels in the camera
		 * @param iTWStart An int with the start of the time window for the trace analysis (10s of nanoseconds).\ Default is 200
		 * @param iTWEnd An int with the end of the time window for the trace analysis (10s of nanoseconds).\ Default is 300
		 * @param nPixelsTolerance An int defining how many pixels must be overflowed to consider it a bad event
		 * @param nSamplesTolerance An int defining how many samples must be overflowed consecutively to consider it a bad event
		 * 
		 * @return bool with the evaluation of whether the event is bad or not.
		 * */
		static bool IsBadEvent(std::vector<std::vector<int>> traces, int iTWStart, int iTWEnd, int nPixelsTolerance = 20, int nSamplesTolerance = 10);
		/**
		 * Public method to identify if the digitized data from the camera constitutes a bad event.
		 * 
		 * @see IsBadEvent
		 * 
		 * @param file IFile Object defined in ExACT. Contains the merged file and the 4 data Trees (HLED, BiFocal, Test, Forced)
		 * @param entry An int with the entry (event number) to be extracted
		 * @param treeName A std::string with the name of the tree to access.\ Can be (hled,bifocal,test,forced)
		 * @param iTWStart An int with the start of the time window for the trace analysis (10s of nanoseconds).\ Default is 200
		 * @param iTWEnd An int with the end of the time window for the trace analysis (10s of nanoseconds).\ Default is 300
		 * @param nPixelsTolerance An int defining how many pixels must be overflowed to consider it a bad event
		 * @param nSamplesTolerance An int defining how many samples must be overflowed consecutively to consider it a bad event
		 * 
		 * @return bool with the evaluation of whether the event is bad or not.
		 * */
		static bool IsBadEvent(IFile *file,int entry, std::string treeName,int iTWStart, int iTWEnd, int nPixelsTolerance = 20, int nSamplesTolerance = 10);



	private:
		/**
		 * Private member to check if the time stamp of the event is within the range specified
		 * 
		 * @param lTime An unsigned long long containing the trigger time in 10s of nanoseconds
		 * @param lTimeStart An unsigned long long with the unix time start of the time window over which to accept events (in 10s of nanoseconds)
		 * @param lTimeEnd An unsigned long long with the unix time end of the time window over which to accept events (in 10s of nanoseconds)
		 * 
		 * @return bool With information if it is within the time range
		 * */
		static bool IsTimeInRange(unsigned long long lTime,unsigned long long lTimeStart, unsigned long long lTimeEnd);


};

#endif
