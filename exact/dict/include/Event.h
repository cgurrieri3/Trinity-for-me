#ifndef EVENT_H 
#define EVENT_H

#include <TROOT.h>
#include "ReadConfiguration.h"
#include <vector>
using namespace std;

/**
 * \class Event
 * \brief Class used for combining Trigger Board data and CoBo data (Trigger information and raw data) 
 * 
 * Class stores the raw data from the CoBo as form of traces for each event and sorted according to pixel number. Each object also contains trigger information like time of trigger and trigger cell that caused the readout.
 * */

class Event {
public:
	/**
	 * Constructor taking no arguments
	 * */
	Event();
	/**
	 * Destructor
	 * */
	~Event();

	/**
	 * Public member used to set the value for the cobo time at which the event was recorded
	 * @param rcvTime An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	void SetCoBoTime(ULong64_t rcvTime);
	/**
	 * Public member used to set the value for the UNIX time at which the event was recorded
	 * @param rcvTime An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	void SetUNIXTime(ULong64_t rcvTime);
	/**
	 * Public member used to set the value for the Trigger Board time at which the event was recorded in TB clock ticks
	 * @param rcvTime An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	void SetTBTime(ULong64_t rcvTime);
	/**
	 * Public member used to set the type of event (based on which trigger type was issued)
	 * @param rcvEventType An int that categorizes the different trigger types ("hled","bifocal","forced","test")
	 * */
	void SetEventType(Int_t rcvEventType);
	/**
	 * Public member used to set the value for the 2 cells that triggered the readout.
	 * Each of these cells is connected to 8 pixels.
	 * @param musicIDs A std::vector of size 2 (typically) conatining the 2 cell IDs that issued the trigger
	 * */
	void SetROIMusicID(vector<Int_t> musicIDs);
	/**
	 * Public member used to set the value for the region of interest around the triggered cells. The ROI is defined as the 16 pixels related to the triggered cell plus a ring of 1 pixel around them.
	 * @param pixIDs A std::vector of size 36 (typically) containing the region of interest including the pixels in the triggered cell plus a ring around them
	 * */
	void SetROIPixelID(vector<Int_t> pixIDs);
	/**
	 * Public member used to set the value for the traces of each pixel captured at the time of readout
	 * @param signalTrace A std::vector of size 512x512 (typically) with the trace for each of the pixels at the time of trigger
	 * */
	void SetSignalValue(vector<vector<Int_t>> signalTrace);

	/**
	 * Public member used to get the value for the cobo time at which the event was recorded
	 * @return An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	ULong64_t GetCoBoTime();
	/**
	 * Public member used to get the value for the UNIX time at which the event was recorded
	 * @return An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	ULong64_t GetUNIXTime();
	/**
	 * Public member used to get the value for the Trigger Board time at which the event was recorded in TB clock ticks
	 * @return An unsigned 64 bit number used to store the time in 10s of nanoseconds
	 * */
	ULong64_t GetTBTime();
	/**
	 * Public member used to get the type of event (based on which trigger type was issued)
	 * @return An int that categorizes the different trigger types ("hled","bifocal","forced","test")
	 * */
	Int_t GetEventType();
	/**
	 * Public member used to get the value for the 2 cells that triggered the readout.
	 * Each of these cells is connected to 8 pixels.
	 * @return A std::vector of size 2 (typically) conatining the 2 cell IDs that issued the trigger
	 * */
	vector<Int_t> GetROIMusicID();
	/**
	 * Public member used to get the value for the region of interest around the triggered cells. The ROI is defined as the 16 pixels related to the triggered cell plus a ring of 1 pixel around them.
	 * @return A std::vector of size 36 (typically) containing the region of interest including the pixels in the triggered cell plus a ring around them
	 * */
	vector<Int_t> GetROIPixelID();
	/**
	 * Public member used to get the value for the traces of each pixel captured at the time of readout
	 * @return A std::vector of size 512x512 (typically) with the trace for each of the pixels at the time of trigger
	 * */
	vector<vector<Int_t>> GetSignalValue();
	/**
	 * Public member used to get the value of the trace for a specific pixel
	 * @return A std::vector of size 512 (typically) with the trace for the specified pixel at the time of trigger
	 * */
	vector<Int_t> GetSignalValue(int pixID);
	

protected:
	ULong64_t tb_time; /**< unsigned 64 bit number containing time*/
	ULong64_t cobo_time;/**< unsigned 64 bit number containing time*/
	ULong64_t unix_time;/**< unsigned 64 bit number containing time*/
	Int_t eventType;/**< int number containing event type*/

	vector<vector<Int_t>> signalValue;/**< std::vector of size 512x512 conatining the 512 samples in a trace for each of the 512 pixels*/
	vector<Int_t> roiMusicID;/**< std::vector of size 2 containing the IDs for the triggered cells*/
	vector<Int_t> roiPixelID;/**< std::vector of size 36 containing the pixel IDs for the region of interest*/

//ClassDef(Event,1);
};
#endif