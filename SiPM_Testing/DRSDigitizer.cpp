/* \file DRSDigitizer.cpp
   Talks with the Digitizer. Configures it and acquires data through this class.
*/

#include "DRSDigitizer.h"

#include <stdlib.h> 
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//Constructor
DRSDigitizer::DRSDigitizer()
{

  vector<float> tmp(1024,0);
  for(int i=0;i<4;i++)
	 {
	   time.push_back(tmp);
	   wave.push_back(tmp);
     }


   drs = new DRS();

   if(!InitializeBoard())
     exit(0);


   SetDefaultSettings();
}


//Destructor
DRSDigitizer::~DRSDigitizer()
{
	delete drs;
}


///////////////////////////////////////////////////////////////////////////
//
//  Setting the sampling rate in GS/s
//
/////////////////////////////////////////////////////////////////////////
Bool_t DRSDigitizer::SetSampleRate(double dFreq)
{

   if(dFreq<0.1 || dFreq > 5)
    {
      cout<<"You are asking for a sampling rate that is out of the valid range of 0.1 GS/s and 5 GS/s"<<endl;
      return kFALSE;
    }

   Bool_t locked = b->SetFrequency(dFreq, true);
    
   b->SelectClockSource(0);
   b->SetDominoMode(1);
   b->SetDominoActive(1);
   b->EnableTcal(1,0);
   b->EnableTcal(0,0);
   b->EnableAcal(1,0.5);
   b->EnableAcal(0,0);

   if(!locked)
    {
      cout<<"The PLL did not lock into the new sampling frequency of "<<dFreq<<" GS/s"<<endl;
      return kFALSE;
    }
  else
   {
     cout<<"Changed to a sampling rate of "<<dFreq<<" GS/s. True sampling rate is "<<b->GetTrueFrequency()<<endl;
   }

  return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Switching into transparent mode 
//
///////////////////////////////////////////////////////////////////////////////

void DRSDigitizer::SetTransparentMode(Bool_t flag)
{ 
	if(flag)
	  cout<<"Switching into transparent mode, can use triggers on Ch1 to Ch4"<<endl;
	else
      cout<<"Turning transparent mode off"<<endl;

	b->SetTranspMode(flag); 

} 

////////////////////////////////////////////////////////////////////////////////
//
//   Setting the minimum of the input range somewhere between -0.5 V to 0 V
//
///////////////////////////////////////////////////////////////////////////////

Bool_t DRSDigitizer::SetMinimumVoltage(double dMinimum)
{

   if(dMinimum<-0.5 || dMinimum>0)
      {
        cout<<"You are trying to set a minimum input voltage that is out of the possible range of -0.5 V ...0 V: "<<dMinimum<<endl;
        return kFALSE;
      }

   int r = b->SetInputRange(dMinimum+0.5);

   if(r==0)
     cout<<"Did not succeed in setting the minimum voltage to "<<dMinimum<<" V"<<endl;

  return r;

}

///////////////////////////////////////////////////////////////////////////////////////
//
// Setting the trigger source. Possible options are:
//    OR  0=CH1, 1=CH2, 2=CH3, 3=CH4, 4=EXT
//    AND 8=CH1, 9=CH2, 10=CH3, 11=CH4, 12=EXT
//    you have to add those numbers bit wise to arrive at the requested trigger condition
//    triggering only on Ch1 would be 8; ch1 and ch2 is 17.... 
//
/////////////////////////////////////////////////////////////////////////////////////

Bool_t DRSDigitizer::SetTriggerSource(int source)
{
	  cout<<"Setting trigger source to "<<source<<endl;
      // Set trigger configuration
      // OR  0=CH1, 1=CH2, 2=CH3, 3=CH4, 4=EXT
      // AND 8=CH1, 9=CH2, 10=CH3, 11=CH4, 12=EXT
      b->EnableTrigger(1, 0);           // enable hardware trigger
      b->SetTriggerSource(source);
      
	  return kTRUE;

}

//////////////////////////////////////////////////////////////
//
// Sets the trigger on Channel 1...4 The external trigger is 5
//
/////////////////////////////////////////////////////////////

Bool_t DRSDigitizer::SetTriggerOnChannel(int channel)
{

  if(channel>5 || channel<1)
  {
     cout<<"You are trying to set a channel that is not in the allowed range 1 to 5: "<<channel<<endl;
	 return kFALSE;
  }
  cout<<"Setting trigger to channel "<<channel<<endl;
  SetTriggerSource(1<<(channel-1));
  return kTRUE;

}

///////////////////////////////////////////////////////////////
//
// Setting the trigger threshold in V for all channels
//
///////////////////////////////////////////////////////////////

void DRSDigitizer::SetTriggerThreshold(float level)
{ 
	cout<<"Setting the trigger threshold for all channels to "<<level<<" V"<<endl;
	b->SetTriggerLevel(level); 

} 

/////////////////////////////////////////////////////////////////////////
//
// Setting the trigger threshold for an individual channel 1 to 4
// 
//
////////////////////////////////////////////////////////////////////////

Bool_t DRSDigitizer::SetIndividualTriggerThreshold(int ch, float lvl)
{
    cout<<"Setting the trigger threshold of channel "<<ch<<" to "<<lvl<<" V"<<endl;

	return b->SetIndividualTriggerLevel(ch-1, lvl);  
	
}

//////////////////////////////////////////////////////////////////////////
//
// Set whether we trigger on the positive (false) or negative slope (true)
//
//////////////////////////////////////////////////////////////////////////

void DRSDigitizer::SetTriggerSlope(Bool_t slope)
{ 
	if(slope)
	   cout<<"Trigger on falling edge."<<endl;
	else
	   cout<<"Trigger on rising edge."<<endl;
	
	b->SetTriggerPolarity(slope);  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the trigger delay in ns
//
/////////////////////////////////////////////////////////////////////////////

Bool_t DRSDigitizer::SetTriggerDelay(float dly)
{ 
	
	cout<<"Trigger delay is "<<dly<<" ns."<<endl;
	return b->SetTriggerDelayNs(dly);  
	
}


//--------------------------------------------------------------------------
//
// Function to initialize the digitizer and get some basic information
// With this function we also set the class wide board handle
//
//----------------------------------------------------------------------------
Bool_t DRSDigitizer::InitializeBoard()
{

  cout<<"Initializing digitizer"<<endl<<endl;
  // exit if no board found
   nBoards = drs->GetNumberOfBoards();
   if (nBoards == 0) {
      printf("No DRS4 evaluation board found\n");
      return 0;
   }

   if (nBoards > 1) {
      printf("More than one DRS4 evaluation board found. Can't handle that many\n");
      return 0;
   }

   b = drs->GetBoard(0);
   printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
   b->GetBoardSerialNumber(), b->GetFirmwareVersion());

   /* initialize board */
   b->Init();

   cout<<"Initialization complete"<<endl;
   return 1;
}

//----------------------------------------------------------------------
//
// Switching to default settings.
//
//----------------------------------------------------------------------

void DRSDigitizer::SetDefaultSettings()
{
  cout<<"Setting defaults"<<endl;

   SetSampleRate(5.0);

   /* enable transparent mode needed for analog trigger */
   //SetTransparentMode(kTRUE);

   SetMinimumVoltage(-0.5);
   
   /* use following line to turn on the internal 100 MHz clock connected to all channels  */
   b->EnableTcal(1);

   //Set the trigger source to channel 1
   SetTriggerOnChannel(1);
   SetTriggerThreshold(0.05);            // 0.05 V
   SetTriggerSlope(false);        // positive edge
   SetTriggerDelay(0);  // zero ns trigger delay
}

void DRSDigitizer::SoftTrigger()
{
       cout<<"Switching into soft trigger mode"<<endl;
       b->SoftTrigger();
}


void DRSDigitizer::StartCapture()
{

         //clear chip (necessary for DRS4 to reduce noise)
        b->StartClearCycle();
        b->FinishClearCycle();

        //start board (activate domino wave)
        b->StartDomino();

}

//------------------------------------------------------------------------------------
//
// Get Event
//
//------------------------------------------------------------------------------------
void DRSDigitizer::CaptureEvent()
{
        //wait for trigger
        //printf("Waiting for trigger...");
        while(b->IsBusy());
        //read all waveforms
        b->TransferWaves(0, 8);

        //decode waveform (Y) array second channel in mV
        //Note: On the evaluation board input #1 is connected to channel 0 and 1 of
        //the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
        //get the input #2 we have to read DRS channel #2, not #1

        //Read all waveforms
        int tc = b->GetTriggerCell(0); 
        for(int i=0;i<4;i++)
         {
           b->GetWave(0, i*2,carray,1,tc);
           wave[i].assign(carray,carray+1024);
           b->GetTime(0, i*2, tc,carray);
		   time[i].assign(carray,carray+1024);
         }

        //Start Capturing the next event while we work with this data
        StartCapture();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// Get a copy of the amplitudes of a channel from the last capture ch1 to ch4
//
///////////////////////////////////////////////////////////////////////////////////////
vector<float> DRSDigitizer::GetSamples(int ch)
{
   if(ch<1 || ch>4)
	 {
	   cout<<"DRSDigitizer::GetSamples: Ups you are asking for a channel that is not between 1 and 4: "<<ch<<endl;
	   vector<float> tmp;
	   return tmp;
	 }

   return wave[ch-1];

}


////////////////////////////////////////////////////////////////////////////////////////
//
// Get a copy of the times of a channel from the last capture
//
///////////////////////////////////////////////////////////////////////////////////////
vector<float> DRSDigitizer::GetTimes(int ch)
{
   if(ch<1 || ch>4)
	 {
	   cout<<"DRSDigitizer::GetTimes: Ups you are asking for a channel that is not between 1 and 4: "<<ch<<endl;
	   vector<float> tmp;
	   return tmp;
	 }
   return time[ch-1];
}

