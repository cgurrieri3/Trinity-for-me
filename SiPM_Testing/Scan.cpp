#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>

#include "strlcpy.h"
#include "DRSDigitizer.h"

#include <TString.h>
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
#include "SwitchControl.h"

/*------------------------------------------------------------------*/

double average(std::vector<double> const& v){
  int sum = 0;
  if(v.empty()){
    return 0;
  }
  
  auto const count = static_cast<double>(v.size());
  for (int i : v) {
    sum += i;
  }
  return sum / count;
}

int main(int argc, char* argv[])
{
	bool angleScan = false;									//If true, perform a scan to determine the rotational angle in mounting quadrant. 
	bool fineScan = false;									//Scan the whole surface of a pixel. 
	int addGap1 = 0, addGap2 = 0, addGap3 = 0;		//Additional gaps between SiPM tiles and quadrants.
	int SiPM_ID;
	cout << "SiPM ID number: ";
	cin >> SiPM_ID;
	string outStringName = "SiPM_ID" + to_string(SiPM_ID)+ ".root";
	string histStringName = "SiPM" + to_string(SiPM_ID);
	TString outName = outStringName.c_str(); 
	TString histName = histStringName.c_str();
	TString distName = histName;
	for(int i=1; i<argc; i++)
  	{
   	string arg = argv[i];
   	if((arg == "-f") || (arg == "-of"))
   	{
   		if((i+1) < argc)
   		{
   			arg = argv[++i];
   			outName = (arg + ".root").c_str();
   			histName = arg.c_str();
   			distName = (arg + "_Amplitude_Distribution").c_str();
   		}
   		else
   		{
   			cerr<<"Output file name requires a string argument."<<endl;
   			return 0;
   		}
   	}
   	else if(arg == "-angleScan")
   		angleScan = true;
   	else if(arg == "-fineScan")
   		fineScan = true;
   	else if((arg == "-gap") || (arg == "-gaps"))
   	{
   		if((i+3) < argc)
   		{
   			addGap1 = atoi(argv[++i]);
   			addGap2 = atoi(argv[++i]);
   			addGap3 = atoi(argv[++i]);
   		}
   		else
   		{
   			cerr<<"Angle scan requires three integer arguments for added gaps between SiPM tiles and quadrants."<<endl;
   			return 0;
   		}
   	}
   }

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
   double sampleRate = 5.0;								//Sampling rate at 5 GHz.
   digitizer->SetSampleRate(sampleRate);
   digitizer->SetTriggerSource(1);						//1=channel1, 2=channel2, 4=channel3, 8=channel4, 16=ext.
   digitizer->SetTriggerSlope(0);						//0=rising edge; 1=falling edge.
   digitizer->SetTriggerThreshold(0.2);				//Set trigger threshold in volt.
   digitizer->SetTriggerDelay(205);						//Set delay in nanosec.
   digitizer->StartCapture();								

   cout<<"Initializing switch"<<endl;
   SwitchControl sw(5);
   cout<<"Initializing linear stage"<<endl;
   StageControl sc(7);
   SignalAnalysis sa;

   //Initialize linear stages
   //sc.Restore_Setting();
   //sc.Set_Hold_Current();								//Turn off hold current
   //sc.Renumber();											//Give each stage a number ID
   //sc.moveAbs(1, 0);										//Center at 720
   //sc.moveAbs(2, 0);										//Beam size is 1.8mm at 690
   //sc.moveAbs(3, 0);										//Center at 829
   
	sw.switch_channel(16);									//Reset switch device,i.e open first channel in each group. 
   //sw.switch_channel(0);									//
   //sw.switch_channel(7);									//Manually set some initial channels to open as well. 
   //sw.switch_channel(10);									//
   //sw.switch_channel(13);							   //
	
   // Move to 0,0,0 (Uncomment if stage was powered off or power cycled)
   //sc.moveAbs(1, 0);									        
   //sc.moveAbs(2, 0);									        
   //sc.moveAbs(3, 0);									        
   //sleep(10);
   
   //Move to initial position
   cout<<"Moving to initial position..."<<endl;
   sc.moveAbs(1, 490); // SPB2									        
   sc.moveAbs(2, 600); // SPB2									        
   sc.moveAbs(3, 775); // SPB2
   //sc.moveAbs(1, 525);									        
   //sc.moveAbs(2, 600);									        
   //sc.moveAbs(3, 800);									        
   
   Getline("Stop. Check that everything lines up");


   //Setup drawing
   TCanvas *c1 = new TCanvas("c1","Trinity Test SiPM", 200, 200, 1000, 1000);
   //c1->Divide(2,2);
   TCanvas *c2 = new TCanvas("c2"," ", 900, 200, 700, 600);
   TCanvas *c3 = new TCanvas("c3"," ", 600, 400, 700, 600);

   int row = 4, column = 4;                        //Quadrants are arrays of 4x4 pixels.
   int nevents = 100; 										//number of traces recorded for each measurement.
   int movingAvgSample = 10;								//number of samples used to calculate moving average.
   //int maxSample = 1024;
   int maxSample = 200;
//max number of samples from recorded trace that are used for analysis.
   sa.setValues(movingAvgSample, maxSample, 8*sampleRate, nevents, nevents*maxSample/sampleRate, false);		

   c1->cd(1);
   TH2D *h1 = new TH2D("h1","Signal Amplitude vs Position", column, 0, column, row, 0, row);
   h1->GetXaxis()->SetTitle("X");
   h1->GetYaxis()->SetTitle("Y");
   h1->SetStats(0);
   h1->SetMinimum(0);
   h1->Draw("colz"); 
   
   TGraph *gr = new TGraph();
   gr->SetTitle("Averaged Signal");
   gr->GetXaxis()->SetTitle("Time [ns]");
   gr->GetYaxis()->SetTitle("Amplitude [mV]");

   double avgAmplitude = 0, amplitude = 0; 
   int i, j, k, l, m, n;									//Dummy counters
   int dir, up;
   int stepSize = 63, fineStepSize = 3, move = 18;
   double x1, x2, z1, z2; 									//Coordinates of pixel centers at two ends of quadrants; used to account for rotational shift in mounting quadrant.
   double angle = 0;
   int gap = (addGap1 + addGap2 + addGap3)/3;
   int centersDistance = 486 + gap;						//Distance between two far end pixel's centers along one quadrant's side.
   //int channel[4][4] = {{12,8,4,0},{13,9,5,1},{14,10,6,2},{15,11,7,3}}; // default from old code
   int channel[4][4] = {{5,4,1,0}, {13,12,9,8}, {10,11,14,15}, {2,3,6,7}}; // Trinity SiPM channel layout 
   //int channel[4][4] = {{15,2,0,13}, {14,3,1,12}, {11,6,4,9}, {10,7,5,8}}; // SPB2 channel layout (SiPM 18)
   vector<double> amplitudeArray;

   c1->cd();
   gPad->SetRightMargin(0.2); 
   TString sTitle;
   sTitle.Form("SiPM_ID %i",SiPM_ID); 
   TH2D* h = (TH2D*)h1->Clone(sTitle.Data());
   h->SetTitle(sTitle.Data());
   h->Draw("colz");
   h->Reset();

   dir = -1;	
   up = -1;
   
   for(i=0; i<row; i++)
     {
       if(i>0)
	 sc.moveRel(3, (stepSize*up));
       for(j=0; j<column; j++)
         {	
	   if(j>0)
	     sc.moveRel(1, (stepSize*dir));
           cout<<"doing pixel "<<channel[i][j]<<endl;
           sw.switch_channel(channel[i][j]);								
           avgAmplitude = 0;
           vTraceAvg.assign(1024, 0);
	   
	   for(k=0; k<nevents; k++)
	     {
	       digitizer->CaptureEvent();					//Record trace
	       vTraceTime = digitizer->GetTimes(2);			
	       vTraceCh2 = digitizer->GetSamples(2);			
	       //tEvents.Fill();    
	       
	       avgAmplitude += sa.getAmplitude(vTraceCh2);
	       //cout << avgAmplitude << endl;
	       for(l=0; l<maxSample; l++)
		 vTraceAvg[l] += vTraceCh2[l] / nevents;
	     }

	   avgAmplitude /= nevents;
	   //cout << "Out of loop: "<<avgAmplitude << endl;
	   amplitudeArray.push_back(abs(avgAmplitude));
	   h->Fill(j, 3-i, abs(avgAmplitude));
	   
	   
	   c1->cd();
	   c1->Modified();
	   c1->Update();
	   for(k=0; k<maxSample; k++)
	     gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
	   c2->cd();
	   gr->Draw("APL");
	   c2->Update();
	   
	 }            //end loop over columns
       
       
       if((i+1) < row)
	 sc.moveRel(1, stepSize*(column-1)*dir*-1);
       
     }					//end loop over rows

   
   // Normalize 
   double norm = average(amplitudeArray);
   h->Scale(1./norm);
   c1->Update();
   

   // Move to 0,0,0 (uncomment if need to)
   //sc.moveAbs(1, 0);									        
   //sc.moveAbs(2, 0);									        
   //sc.moveAbs(3, 0);									        
   //sleep(10);


   

   c3->cd();
   TH1D *h3 = new TH1D("h3", distName, 200, 0, 400);
   h3->GetXaxis()->SetTitle("Signal Amplitude [mV]");
   h3->GetYaxis()->SetTitle("Count");
   for(int pixel=0; pixel<64; pixel++)
     {
       //if(amplitudeArray[pixel] > 30)							//Eliminate low signal outliers
	 h3->Fill(amplitudeArray[pixel]);
     }
   h3->Draw();
   gStyle->SetOptStat(1110);
   c3->Update();
   
   Getline("Stop. Data collection is complete!");

   TFile *fOut = new TFile(outName.Data(),"UPDATE");
   if(!fOut->IsOpen())
     {
       cout<<"error opening root output file: "<<outName<<endl;
       cout<<"...exiting"<<endl;
       exit(-1);
     }
   cout<<"Have opened the root outputfile: "<<outName<<endl;
   h->Write(histName);
   h3->Write(distName);

   TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
   timer.TurnOn();
   //Getline("Type <return> to go on: ");
   timer.TurnOff();

}

//Possible problem: sometimes the stages are not renumbered correctly; sometimes they don't move. So watch the reply data.
