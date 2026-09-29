#ifndef PULSE_H
#define PULSE_H
#include <TROOT.h>
#include <ReadConfiguration.h>
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;

class Pulse {
	/**
	 * \class Pulse
	 * \brief The main class for analyzing a trace obtained from data acquisition
	 * 
	 * Pulse is the backbone of the anlaysis software in ExACT. It analyzes the traces provided and obtains the information thaty characterize the particular signal.
	 * */
private:
	Int_t nSamplesPulse;/**< An int with the number of samples in a trace*/


	Double_t pedestal;/**< A double with the pedestal value of the trace*/
	int amplitude;/**< An int with the maximum of the trace when comapared to the pedestal*/
	int charge;/**< An in with the charge contained within the peak of the trace*/
	int timePeak;/**< An int with the time bin where the trace achieves its maximum*/
	int timeFWHM;/**< An int with the Full width at half max of the pulse*/
	int timeFW;/**< An int wit the full width of the pulse*/
	float pedestalRMS;/**< A float with the RMS of the pedestal values*/
	Double_t timeIntegration;/**< A double with how many samples to use for integration*/
	vector<Int_t> trace;/**< A std::vector with the trace to be analyzed*/

	/**
	 * A private member used to calculate the pedestal.
	 * This is the legacy method for calculating the pedestal and should be substituted for the method described in IUtilities::GetPedestalAverage()
	 * 
	 * @see IUtilities::GetPedestalAverage(std::vector<int> trace, int tWindow=20,int pad=10, bool isUnCorrelated=true)
	 * 
	 * This method sums the first nSamplesPedstal and finds their average
	 * 
	 * @param nSamplesPedestal An int describing how many samples to use
	 * 
	 * */
	void CalcPedestal(Int_t nSamplesPedestal);
	/**
	 * A private member used to calculate the amplitude. The peaking time is used and the pedestal obtained from CalcPedestal(Int_t nSamplesPedestal) is subtracted.
	 * 
	 * */
	void CalcAmplitude();
	/**
	 * Sums the specified number of samples within the window to obtain the charge contained in the pulse
	 * 
	 * @param tWinSt An int specifying where the window for integration starts
	 * @param tWinEnd An int specifying where the window for integration ends
	 * @param tWinExt An int how many samples to use in the integration
	 * @param isRevolving A bool to define if the time window should be revolving. If set to true, tWinExt is centered around the peak of the pulse. If not, the first tWinExt samples starting from tWinSt are summed
	 * 
	 * */
	void CalcCharge(Int_t tWinSt, Int_t tWinEnd, Int_t tWinExt, Bool_t isRevolving);
	/**
	 * Private member not implemented
	 * */
	void CalcPeakTime();
	/**
	 * Private member not implemented
	 * */
	void CalcFWHM();
	/**
	 * Private member not implemented
	 * */
	void CalcFW();
	/**
	 * Private member to find on which time bin, within a range the pulse achieves a maximum
	 * 
	 * @param tWinSt An int specifying where the window for the search starts
	 * @param tWinEnd An int specifying where the window for the search ends
	 * 
	 * */
	void FindPeakTime(Int_t tWinSt, Int_t tWinEnd);
	/**
	 * Private member to find what the peak is at the obtained peak time
	 * 
	 * */
	void FindPeak();

public:
	/**
	 * Public contructor (legacy)
	 * 
	 * Used when ExACT was set to online mode and analysis happened during flight
	 * 
	 * @param config A ReadConfiguration* which contains the configuration of the analysis software
	 * @param samples A std::vector with the trace for a specific pixel
	 * 
	 * */
	Pulse(ReadConfiguration *config, std::vector<Int_t> samples);
	/**
	 * Public constructor using only the trace
	 * 
	 * @param samples A std::vector with the trace for a specific pixel
	 * 
	 * */
	Pulse(vector<Int_t> samples);
	/**
	 * Public constructor
	 * 
	 * @param samples A std::vector<int> with the trace for a specific pixel
	 * @param tStart An int specifying where the window for the search starts
	 * @param tEnd An int specifying where the window for the search ends
	 * 
	 * Optional parameters:
	 * @param nSamples An int with the number of samples in the std::vector.\ Default is 512
	 * @param isPedestalSub A bool specifying whether the pedestal should be subtracted or not.\ Default is true
	 * */
	Pulse(vector<int> samples, Int_t tStart, Int_t tEnd, int nSamples = 512, bool isPedestalSub = true);
	/**
	 * Public constructor
	 * 
	 * @param samples A std::vector<float> with the trace for a specific pixel
	 * @param tStart An int specifying where the window for the search starts
	 * @param tEnd An int specifying where the window for the search ends
	 * 
	 * Optional parameters:
	 * @param nSamples An int with the number of samples in the std::vector.\ Default is 512
	 * @param isPedestalSub A bool specifying whether the pedestal should be subtracted or not.\ Default is true
	 * */
	Pulse(vector<float> samples, Int_t tStart, Int_t tEnd, int nSamples = 512, bool isPedestalSub = true);
	/**
	 * Public destructor
	 * */
	~Pulse();


	/**
	 * Public method to get the value stored in pedestal member
	 * 
	 * @return a double with the value of the pedestal
	 * */
	Double_t GetPedestal();
	/**
	 * Public method to get the value stored in pedestalRMS member
	 * 
	 * @return a float with the value of the pedestalRMS
	 * */
	float GetPedestalRMS();
	/**
	 * Public method to get the value stored in amplitude member
	 * 
	 * @return An int with the value of the amplitude
	 * */
	int GetAmplitude();
	/**
	 * Public method to get the value stored in charge member
	 * 
	 * @return An int with the value of the charge
	 * */
	int GetCharge();
	/**
	 * Public method to get the value stored in timePeak member
	 * 
	 * @return An int with the value of the timePeak
	 * */
	int GetTimePeak();
	/**
	 * Public method to get the value stored in timeFWHM member
	 * 
	 * @return An int with the value of the timeFWHM
	 * */
	int GetFWHM();
	/**
	 * Public method to get the value stored in timeFW member
	 * 
	 * @return An int with the value of the timeFW
	 * */
	int GetFW();		
};
#endif