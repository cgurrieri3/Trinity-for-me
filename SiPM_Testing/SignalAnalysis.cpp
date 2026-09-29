#include "SignalAnalysis.h"
#include <iostream>


SignalAnalysis::SignalAnalysis()
{
	avgSample = 10; 
	maxSample = 1024; 
	baselineSample = 25; 
	nevent = 100; 
	totalTime = 0;
	cDisplay = false;
	sumOfTraces.assign(1024, 0);
	smoothedTrace.assign(1024, 0);
}

/*This sets a few parameters: Number of samples used for calculating the moving average (more samples means smoother line but may bias yourself, fewer samples may be affected by statistical fluctuation), maximum samples used from recorded trace, number of samples used to calculate the baseline, number of recorded traces with each measurement, total time (in nanosec, used for dark rate measurement), and option to display each trace as it's being recorded. Return maximum samples used from recorded trace.*/
int SignalAnalysis::setValues(int avgSamples, int maxSamples, int baselineSamples, int nevents, double totalTimes, bool display = false) 
{
   avgSample = avgSamples;						
   maxSample = maxSamples;						
   baselineSample = baselineSamples;		
   nevent = nevents;
   totalTime = totalTimes;
   cDisplay = display;					//Display trace plot if set to true.					
   if(maxSample > 1024)
      maxSample = 1024;
   if(baselineSample > 1024)
   	baselineSample = 1024;

   return maxSamples;
}

vector<float> SignalAnalysis::smoothTrace(vector<float> vector)
{
   for(int i=0; i<maxSample; i++)
   {
      int count = 0;
      double sum = 0;
      for(int j=(i - avgSample/2); j<=(i + avgSample/2); j++)
      {
	 		if(j<0)   
            j = 0;
	 		sum += vector[j];
         count++;
	 		if((j+1) == maxSample)    
	    		j = (i + avgSample/2);
      }
      smoothedTrace[i] = sum/count;
   }

   return smoothedTrace;
}

double SignalAnalysis::getMax(vector<float> vector)
{
	double maxValue = 0;
	for(int i=0; i<maxSample; i++)
	{
	  if(vector[i] < maxValue){
	    maxValue = vector[i];
	  }
	}
	//cout << "Max Value: " <<maxValue << endl;
	return maxValue;
}

double SignalAnalysis::getBaseline(vector<float> vector)
{
   double baseline = 0;
   for(int i=0; i<baselineSample; i++)
      baseline += vector[i];

   if(baselineSample != 0)
      baseline /= baselineSample;
   //cout << "Baseline: " << baseline <<endl;

   return baseline;
}

double SignalAnalysis::getAmplitude(vector<float> vector) 
{
  double amplitude = getMax(smoothTrace(vector)) - getBaseline(vector);
  //cout <<"Amplitude: " << amplitude <<endl;
  return amplitude;
}

void SignalAnalysis::readTrace(vector<float> vector)
{
	for(int i=0; i<maxSample; i++)
		sumOfTraces[i] += vector[i];
}

void SignalAnalysis::resetTrace()
{
	sumOfTraces.assign(1024, 0);	
}
