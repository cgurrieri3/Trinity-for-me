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

int main(int argc, char* argv[])
{
	bool angleScan = false;									//If true, perform a scan to determine the rotational angle in mounting quadrant. 
	bool fineScan = false;									//Scan the whole surface of a pixel. 
	int addGap1 = 0, addGap2 = 0, addGap3 = 0;		//Additional gaps between SiPM tiles and quadrants.
	string outStringName = "output.root";
	string histStringName = "Module Unknown";
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
   //digitizer->SetTriggerOnChannel(1);
   digitizer->SetTriggerSource(1);						//1=channel1, 2=channel2, 4=channel3, 8=channel4, 16=ext.
   digitizer->SetTriggerSlope(0);						//0=rising edge; 1=falling edge.
   digitizer->SetTriggerThreshold(0.03);				//Set trigger threshold in volt.
   digitizer->SetTriggerDelay(0);						//Set delay in nanosec.
   digitizer->StartCapture();								

   SwitchControl sw(6);
   StageControl sc(7);
   SignalAnalysis sa;

   //Initialize linear stages
   //sc.Restore_Setting();
   //sc.Set_Hold_Current();									//Turn off hold current
   //sc.Renumber();												//Give each stage a number ID
   //sc.init();													//Go to home position
   sw.switch_channel(16);									//Reset switch device
   sw.switch_channel(3);									//Open all channels except 2,5,9,13	

   //Move to initial position
   sc.moveAbs(1, 762);										//Center at 
   sc.moveAbs(2, 690);										
   sc.moveAbs(3, 875);										//Center at 
   //sc.moveAbs(1, 733);																				
   //sc.moveAbs(3, 865);											
   //Getline("Stop");
   //Setup drawing
   TCanvas *c1 = new TCanvas("c1"," ", 200, 200, 700, 600);
   TCanvas *c2 = new TCanvas("c2"," ", 900, 200, 700, 600);
   TCanvas *c3 = new TCanvas("c3"," ", 600, 400, 700, 600);

   int row = 8, column = 8;
   int nevents = 100; 										//number of traces recorded for each measurement.
   int movingAvgSample = 10;								//number of samples used to calculate moving average.
   int maxSample = 1024;									//max number of samples from recorded trace that are used for analysis.
   sa.setValues(movingAvgSample, maxSample, 8*sampleRate, nevents, nevents*maxSample/sampleRate, false);		

   c1->cd();
   TH2D *h1 = new TH2D("h1","Signal Amplitude vs Position", column, 0, column, row, 0, row);
   h1->GetXaxis()->SetTitle("X");
   h1->GetYaxis()->SetTitle("Y");
   h1->SetStats(0);
   h1->Draw("colz"); 
   
   TGraph *gr = new TGraph();
   gr->SetTitle("Averaged Signal");
   gr->GetXaxis()->SetTitle("Time [ns]");
   gr->GetYaxis()->SetTitle("Amplitude [mV]");

   double avgAmplitude; 
   int i, j, k, l, m, n;									//Dummy counters
   int stepSize = 10, fineStepSize = 3, move = 12;
   double x1, x2, z1, z2; 									//Coordinates of pixel centers at two ends of quadrants; used to account for rotational shift in mounting quadrant.
   double angle = 0;
   int gap = (addGap1 + addGap2 + addGap3)/3;
   int centersDistance = 486 + gap;						//Distance between two far end pixel's centers along one quadrant's side.

   for(i=0; i<row; i++)
   {
      sc.moveRel(3, -stepSize);
      for(j=0; j<column; j++)
      {	
         sc.moveRel(1, -stepSize);
         avgAmplitude = 0;
         vTraceAvg.assign(1024, 0);
         for(k=0; k<nevents; k++)
	 		{
            digitizer->CaptureEvent();					//Record trace
            vTraceTime = digitizer->GetTimes(2);			
            vTraceCh2 = digitizer->GetSamples(2);			
	    		tEvents.Fill();    

	    		avgAmplitude += sa.getAmplitude(vTraceCh2);
	    		for(l=0; l<maxSample; l++)
	    			vTraceAvg[l] += vTraceCh2[l] / nevents;
         }

         avgAmplitude /= nevents;
	 		c1->cd();
	 		h1->Fill(j, i, avgAmplitude);
         c1->Update();
         for(k=0; k<maxSample; k++)
         	gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
         c2->cd();
         gr->Draw();
         c2->Update();

	 		//Search for pixel center
	 		if(avgAmplitude > 50)							//Trigger threshold.
	 		{
	 			cout<<"Signal amplitude reached the threshold for pixel's center search. We are getting close!"<<endl;
	    		for(int ID=1; ID<4; ID+=2)					//Search for center position on the XZ plane; 1=X, 2=Y, 3=Z.
	    		{
	       		for(j=0; j<10; j++)
	       		{
	     	      	double amplitude = 0;
		  				vTraceAvg.assign(1024, 0);
	          		sc.moveRel(ID, fineStepSize);
	          		for(k=0; k<nevents; k++)
	          		{
	             		digitizer->CaptureEvent();
                     vTraceCh2 = digitizer->GetSamples(2);
	             		tEvents.Fill(); 

	    					amplitude += sa.getAmplitude(vTraceCh2);
	    					for(l=0; l<maxSample; l++)
	    						vTraceAvg[l] += vTraceCh2[l] / nevents;
	          		}

         			for(k=0; k<maxSample; k++)
         				gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
         			gr->Draw();
         			c2->Update();

		  				//If measured amplitude falls below half the trigger amplitude, then move to pixel's center.
	          		if((amplitude/nevents) < (avgAmplitude/2))		
	          		{
		     				sc.moveRel(ID, -move);
		     				if(ID == 1)
		     					x1 = sc.getX();
		     				else
		     					z1 = sc.getZ();
		     				j = 10;								//End loop scan
	          		}
	       		}

	       		if(ID == 1)
	       		{
	       			//Measure signal amplitude at new location.
	       			avgAmplitude = 0;
	       			for (k=0; k<nevents; k++)
   	       		{
      		  			digitizer->CaptureEvent();
	          			vTraceCh2 = digitizer->GetSamples(2);
	          			tEvents.Fill();    
							avgAmplitude += sa.getAmplitude(vTraceCh2) / nevents;
	       			}
	    			}
	    			else
	    				printf("First pixel's center position is: (%.2f, %.2f) \n", x1, z1); 
	 			}
	 			i = row;											//End outer loop scan
      	}	 
   	}
   	if((i+1) < row)
   		sc.moveRel(1, stepSize*column);
   	else if((i+1) == row)
   	{
   		cout<<"Pixel center was not found. Modify your search and rescan."<<endl;
   		return 0;
   	}
   }

   if(fineScan)
   {
   	sc.moveRel(1, 40);
   	sc.moveRel(3, 40);
   	int xStep = 50, yStep = 50, xStepSize = 1;
   	for(i=0; i<yStep; i++)
   	{
   		for(j=0; j<xStep; j++)
   		{


   		}
   	}

   }

   if(angleScan)
   {
   	cout<<"Begin the scan to calculate the rotational angle shift.	Scan for the center of the pixel at the other corner!"<<endl;
   	sc.moveRel(1, -centersDistance);					//Move to the far end pixel.
      sw.switch_channel(14);								//Switch to the correct channel for different quadrant.
   	for(int ID=1; ID<4; ID+=2)		
   	{
      	avgAmplitude = 0;
      	vTraceAvg.assign(1024, 0);
      	for(k=0; k<nevents; k++)
      	{
         	digitizer->CaptureEvent();
         	vTraceCh2 = digitizer->GetSamples(2);
         	tEvents.Fill();  

	    		avgAmplitude += sa.getAmplitude(vTraceCh2);
	    		for(l=0; l<maxSample; l++)
	    			vTraceAvg[l] += vTraceCh2[l] / nevents;
      	}

      	avgAmplitude /= nevents;
         for(k=0; k<maxSample; k++)
         	gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
         gr->Draw();
         c2->Update();
	      for(j=0; j<10; j++)
	      {
	     	  	double amplitude = 0;
		  		vTraceAvg.assign(1024, 0);
		  		if(ID == 1)
            	sc.moveRel(1, -fineStepSize);
 	 			else
	    			sc.moveRel(ID, fineStepSize);

	      	for(k=0; k<nevents; k++)
	      	{
	        		digitizer->CaptureEvent();
               vTraceCh2 = digitizer->GetSamples(2);
	        		tEvents.Fill(); 

	    			amplitude += sa.getAmplitude(vTraceCh2);
	    			for(l=0; l<maxSample; l++)
	    				vTraceAvg[l] += vTraceCh2[l] / nevents;
	      	}

         	for(k=0; k<maxSample; k++)
         		gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
         	gr->Draw();
         	c2->Update();
	      	if((amplitude/nevents) < (avgAmplitude/2))		
	      	{
		     		if(ID == 1)
		     		{
		     			sc.moveRel(1, move);
		     			x2 = sc.getX();
		     		}
		     		else
		     		{
		     			sc.moveRel(ID, -move);
		     			z2 = sc.getZ();
		     		}
		     		j = 10;										//End loop scan
	         }
	      }
   	}
   	angle = atan((z2-z1)/(x2-x1));		
   	printf("Rotation angle = %f radians.\n", angle);
   	//Move back to first pixel to begin module scan
   	sc.moveAbs(1, x1);
   	sc.moveAbs(3, z1);
   	//sw.switch_channel(16);
	}

   cout<<"Begin scanning the module!"<<endl;
	c1->cd();
   TH2D *h2 = new TH2D("h2", histName, 16, 0, 16, 16, 0, 16);
   h2->GetXaxis()->SetTitle("Pixel Element");
   h2->GetYaxis()->SetTitle("Pixel Element");
   h2->SetStats(0);
   h2->Draw("colz");
   int quadrantID;
   //int channel[4][4] = {{1, 5, 9, 13}, {0, 4, 8, 12}, {3, 7, 11, 15}, {2, 6, 10, 14}};
   //int channel[4][4] = {{1, 5, 9, 13}, {0, 4, 8, 12}, {3, 7, 11, 15}, {2, 6, 10, 14}};
   int channel[4][4] = {{3, 7, 10, 14}, {2, 6, 11, 15}, {0, 4, 8, 12}, {1, 5, 9, 13}};
   int xCoordinate = 0, yCoordinate = 15;
   row = 4; column = 4;
   vector<double> amplitudeArray;
   for(quadrantID=0; quadrantID<4; quadrantID++)
   {
   	for(j=0; j<column; j++)
   	{
   		for(n=0; n<2; n++)
   		{
   			for(i=0; i<row; i++)
   			{
   				sw.switch_channel(channel[i][j]);
   				for(m=0; m<2; m++)
   				{
						avgAmplitude = 0;
      				vTraceAvg.assign(1024, 0);
      				for(k=0; k<nevents; k++)
      				{
         				digitizer->CaptureEvent();
         				vTraceCh2 = digitizer->GetSamples(2);
         				tEvents.Fill();  

	    					avgAmplitude += sa.getAmplitude(vTraceCh2);
	    					for(l=0; l<maxSample; l++)
	    						vTraceAvg[l] += vTraceCh2[l] / nevents;
      				}
      				avgAmplitude /= nevents;
      				amplitudeArray.push_back(avgAmplitude);
						c1->cd();
	 					h2->Fill(xCoordinate, yCoordinate, avgAmplitude);
	 					c1->Modified();
         			c1->Update();
         			c2->cd();
         			for(k=0; k<maxSample; k++)
         				gr->SetPoint(k, vTraceTime[k], vTraceAvg[k]);
         			gr->Draw();
         			c2->Update();
						yCoordinate--;
         			//Move down to next pixel
         			if(((i%2) == 1) && ((m%2) == 1))
         			{
         				sc.moveRel(1, (34 + gap)*sin(angle));
	    					sc.moveRel(3, -(34 + gap)*cos(angle));
         			}
         			else
         			{
         				sc.moveRel(1, 32*sin(angle));
	    					sc.moveRel(3, -32*cos(angle));
         			}
   				}	
   			}
   			//Move over to next column
   			if(((j%2) == 1) && ((n%2) == 1))
         	{
         		sc.moveRel(1, -(260 + 2*gap)*sin(angle) - (34 + gap)*cos(angle));
	    			sc.moveRel(3, (260 + 2*gap)*cos(angle) - (34 + gap)*sin(angle));
         	}
         	else
         	{
         		sc.moveRel(1, -(260 + 2*gap)*sin(angle) - 32*cos(angle));
	    			sc.moveRel(3, (260 + 2*gap)*cos(angle) - 32*sin(angle));
         	}

         	xCoordinate++;
   			if(quadrantID < 2)
   				yCoordinate = 15;
   			else
   				yCoordinate = 7;
   		}
   	}
   	if(quadrantID < 3)
   		Getline("Set up the next quadrant for scanning. Then Type <return> to go on: ");

   	if(quadrantID == 0)
   	{
   		swap(channel[2][0], channel[3][0]);
   		swap(channel[2][1], channel[3][1]);
   	}
   	else if(quadrantID == 1)
   	{
   		sc.moveRel(1, (520 + 4*gap)*cos(angle) + (260 + 2*gap)*sin(angle));
   		sc.moveRel(3, (520 + 4*gap)*sin(angle) - (260 + 2*gap)*cos(angle));
   		xCoordinate = 0;
   		for(int r=0; r<2; r++)
   		{
      		for(int c=0; c<4; c++)
	 				swap(channel[r][c], channel[3-r][3-c]);
   		}
   	}
   	else if(quadrantID == 2)
   	{
   		swap(channel[0][2], channel[1][2]);
   		swap(channel[0][3], channel[1][3]);
   	}
   }
   c3->cd();
	TH1D *h3 = new TH1D("h3", distName, 50, 100, 200);
	h3->GetXaxis()->SetTitle("Signal Amplitude [mV]");
	h3->GetYaxis()->SetTitle("Count");
	for(int pixel=0; pixel<64; pixel++)
	{
		if(amplitudeArray[pixel] > 30)							//Eliminate low signal outliers
			h3->Fill(amplitudeArray[pixel]);
	}
	h3->Draw();
	gStyle->SetOptStat(1110);

  	TFile *fOut = new TFile(outName.Data(),"UPDATE");
  	if(!fOut->IsOpen())
   {
      cout<<"error opening root output file: "<<outName<<endl;
      cout<<"...exiting"<<endl;
      exit(-1);
   }
  	cout<<"Have opened the root outputfile: "<<outName<<endl;
	h2->Write(histName);
	h3->Write(distName);
   TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
   timer.TurnOn();
   Getline("Type <return> to go on: ");
   timer.TurnOff();
   fOut->Close();

   return 0;
}

//Possible problem: sometimes the stages are not renumbered correctly; sometimes they don't move. So watch the reply data.
