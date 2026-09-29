#ifndef DRS_Digitizer_1
#define DRS_Digitizer_1


#include "DRS.h"

#include <TROOT.h>

using namespace std;

class DRSDigitizer {

  public:
 
   DRSDigitizer();
  ~DRSDigitizer();

  void SetDefaultSettings();

  //Setting up the digitizer
  Bool_t SetSampleRate(double dFreq);
  Bool_t SetMinimumVoltage(double dMinimum);
  void SetTransparentMode(Bool_t flag); 
  Bool_t SetTriggerSource(int source);
  Bool_t SetTriggerOnChannel(int channel);
  void SetTriggerThreshold(float level);
  void SetTriggerSlope(Bool_t slope); 
  Bool_t SetIndividualTriggerThreshold(int ch, float lvl);
  Bool_t SetTriggerDelay(float dly);
  void SoftTrigger();
  void StartCapture();
  void CaptureEvent();
  vector<float> GetSamples(int ch);
  vector<float> GetTimes(int ch);



  protected:

  int i, j, nBoards;
  DRS *drs;
  DRSBoard *b;

  //Higher order functions
  Bool_t InitializeBoard();
  float carray[1024];
  vector< vector<float> > time;
  vector< vector<float> > wave;


};
#endif
