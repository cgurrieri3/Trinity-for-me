#include "strlcpy.h"
#include "DRSDigitizer.h"
#include <Getline.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TAxis.h>
#include <TTimer.h>
#include "SignalAnalysis.h"
#include "StageControl.h"
#include "TempControl.h"

bool bDisplay = true;

int main(int argc, char* argv[])
{
   TempControl ch(5);
	ch.setPoint(20);

	StageControl sc(7);
   SignalAnalysis sa;
   //Initializing
   //sc.Restore_Setting();
   //sc.Renumber();
   //sc.Set_Hold_Current();	//Turn off hold current
	//sc.MoveAbs(0, 0);

   //sc.moveAbs(1, 279);		//center at 279, testing at 231, for single tile, center at 277
   //sc.moveAbs(2, 894);	   //895, for single tile, 900 = 8.1 mm away from fiber. For quadrant, at this distance, it is 5.4mm away from the fiber end.
   //sc.moveAbs(3, 772);		//center at 772, testing at 786, for single tile, center at 771

	sc.getX();
	sc.getY();
	sc.getZ();

   //Initiate root
   TROOT root("DisplayEvts","Display Results");		//The TROOT object is the entry point to the ROOT system.
   TApplication *theApp;	// Create an application environment. The application environment provides an interface to the graphics system and eventloop.
   theApp = new TApplication("App",&argc,argv);

   //Configure DRS
   TTree tEvents("tEvents","Tree with all the events");
   vector< Float_t > vTraceCh1;
   vTraceCh1.assign(1024,0);
   vector< Float_t > vTraceCh2;
   vTraceCh2.assign(1024,0);
   vector< Float_t > vTraceTime;
   vTraceTime.assign(1024,0);
   vector< Float_t > vTraceAvg;
   vTraceAvg.assign(1024,0);
   tEvents.Branch("vTraceCh2",&vTraceCh2);
   tEvents.Branch("vTraceTime",&vTraceTime);

   DRSDigitizer *digitizer = new DRSDigitizer();
   double sampleRate = 1;								//Sampling rate at 5 GHz.
   digitizer->SetSampleRate(sampleRate);
   //digitizer->SetTriggerOnChannel(1);
   digitizer->SetTriggerSource(1);						//1=channel1, 2=channel2, 4=channel3, 8=channel4, 16=ext.
   digitizer->SetTriggerSlope(0);						//0=rising edge; 1=falling edge.
   digitizer->SetTriggerThreshold(0.2);				//Set trigger threshold in volt.
   digitizer->SetTriggerDelay(0);						//Set delay in nanosec.
   digitizer->StartCapture();		

   //Setup drawing
   TCanvas *c1 = new TCanvas("c1"," ", 200, 200, 700, 600);
   TH1D *h1 = new TH1D("h1","Signal Amplitude Distribution", 20, 0, 20);
   h1->GetXaxis()->SetTitle("Amplitude [mV]");
   h1->SetStats(0);
   h1->Draw();

   TCanvas *c2 = new TCanvas("c2"," ", 900, 200, 700, 600);
   TGraph *gr = new TGraph();
   gr->SetTitle("Averaged Signal");
   gr->GetXaxis()->SetTitle("Time [ns]");
   gr->GetYaxis()->SetTitle("Amplitude [mV]");

   TCanvas *c3 = new TCanvas("c3"," ", 600, 400, 700, 600);
   TGraph *gr2 = new TGraph();
   gr2->SetTitle("Signal Trace");
   gr2->GetXaxis()->SetTitle("Time [ns]");
   gr2->GetYaxis()->SetTitle("Amplitude [mV]");

	int i, j;
   int nevents = 1000; 										//number of traces recorded for each measurement.
   int movingAvgSample = 10;								//number of samples used to calculate moving average.
   //int maxSample = 1024;									//max number of samples from recorded trace that are used for analysis.
   int maxSample = 800;
   sa.setValues(movingAvgSample, maxSample, 8*sampleRate, nevents, nevents*maxSample/sampleRate, false);	
   double avgAmplitude = 0, amplitude = 0;
   
   for(i=0; i<nevents; i++)
	{
      digitizer->CaptureEvent();							//Record trace
      vTraceTime = digitizer->GetTimes(2);			
      vTraceCh2 = digitizer->GetSamples(2);			
		tEvents.Fill();    

		amplitude = sa.getAmplitude(vTraceCh2);
		h1->Fill(amplitude);
		avgAmplitude += amplitude;
		for(j=0; j<maxSample; j++)
		{
			vTraceAvg[j] += vTraceCh2[j] / nevents;
			gr2->SetPoint(j ,vTraceTime[j], vTraceCh2[j]);
		}

		if(bDisplay)
      {
         c3->cd();
         gr2->Draw("APL");
         c3->Update();
         //hold the code;
         TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
         timer.TurnOn();
         TString input = Getline("Type <return> to go on: ");
         timer.TurnOff();
      }
   }

	avgAmplitude /= nevents;
	cout<<"Average amplitude: "<<avgAmplitude<<endl;

	c1->cd();
	//c1->Modified();
	c1->Update();

	for(j=0; j<maxSample; j++)
     	gr->SetPoint(j, vTraceTime[j], vTraceAvg[j]);
   c2->cd();
   gr->Draw("APL");
   c2->Update();

   TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
   timer.TurnOn();
   TString input = Getline("Type <return> to go on: ");
   timer.TurnOff();

   return 0;
}
