#include <vector>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TAxis.h>
#include <TTimer.h>
#include <TStyle.h>
#include <fstream>
#include <algorithm>
#include <TGraph.h>

using namespace std;

class SignalAnalysis
{
   private:

	double m = 1; 					//Factor used in crosstalk measurement. Voltageline increment.
   int avgSample, maxSample, baselineSample, nevent;
	double totalTime;
	bool cDisplay;
	vector<float> smoothedTrace;
	vector<float> sumOfTraces;


   public:

   SignalAnalysis();
	int setValues(int, int, int, int, double, bool);					
	vector<float> smoothTrace(vector<float>);								//Smooth out the input trace.
	double getMax(vector<float>);												//Return max value of a trace
	double getBaseline(vector<float>);										//Calculate the baseline	
	double getAmplitude(vector<float>);										//Return max value of a trace after subtracting the baseline.
	void readTrace(vector<float>);											//Add the input trace to existing traces.	
	void resetTrace();				
};
	
