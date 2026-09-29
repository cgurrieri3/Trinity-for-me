//includes libraries used in script
#include <TH1.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <TLatex.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TLatex.h>
#include <TFile.h>
#include <TBox.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLegend.h>

//set "base" directories
//dataDir is where the daily data directories are saved (i.e. "YYYYMMDD" directories)
//outDir is the directory where output will be saved
std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
std::string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/Output/";

//initialize or declare global variables, which are allocated static memory and are available in every scope
TTree *tree = 0;
Event *ev;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;

//declare functions to be defined later
void SetBranches(Event **evD);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();
void addAverageLines();
std::vector<std::string> read_directory( const std::string& path = std::string() );
std::vector<double> getAvgeragesFromText();
std::time_t convertToUnixTime(const std::string& timeString, int t_v);
Double_t Median(vector<Double_t> v);

//"main" function; the first function that is called when the script is started. to run in root, this can be any type of function but must be named the same as the .cpp file name. to compile, this would need to be reorganized into a file i.e. int main(int argc char* argv[])
//folString is the name of the directory under dataDir associated with the date that you want to analyze
//treeString is the name of the tree that has the data you want to analyze, i.e. "Test","HLED"
int main(int argc, char **argv){
	if(argc < 1){
		cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
	std::string folString = argv[1];
	std::string testString = "Test";
	std::string ledString = "HLED";
	//checks RC Log file for the night for instances where trigger threshold is set
  	std::string logDir = Form("%s%s/LOGS/rc.log",dataDir.c_str(),folString.c_str());
  	std::ifstream logFile(logDir.c_str()); // Open the file
  	if (!logFile.is_open()) {
		std::cerr << "Error opening file\n";
    	//return 1;
	}
  	std::string trStr = "200000ffff250300"; //first 16 bits of 18 bit hex command for setting trigger threshold, last 2 bits are the threshold value
  	std::string line, prevLine;
  	std::vector<std::string> trigger_threshold; // Vector of strings
  	std::vector<double_t> trig_thres_time;

	// The creatation of a variable that will count how many values are outside the set view for the plots the users see 
	std::vector<int> ViewOverFlow(7,0);
	// View limits for the plots the users sees in order of apperance 
	std::vector<double> ylimits = {3770,3800,15,40,75,200,175,375,235,260,0,600,-0.5,0.7};

  	bool found = false;
  	while (std::getline(logFile, line)) { // Read file line by line
    	if (found) { // If the previous line contained the phrase
    		std::string Trigger_set_time = line.substr(13, 13);
        	//Replace '_' with 'T'
			for (char& c : Trigger_set_time) {
		    	if (c == '_') {
		            c = 'T';
		        }
			}
			std::time_t unixThresTime=convertToUnixTime("20"+Trigger_set_time,0);
			std::cout << "Unix Thres Timestamp: " << unixThresTime << std::endl;
        	trig_thres_time.push_back(unixThresTime);
        	found = false; // Reset found flag
      	}
      	size_t pos = line.find(trStr); // Search for the phrase in the line
      	if (pos != std::string::npos) { // If the phrase is found
          	std::string Trigger_value_hex = line.substr(51, 2);
          	int Trigger_value = std::stoi(Trigger_value_hex, nullptr, 16);
          	trigger_threshold.push_back(std::to_string(Trigger_value));
          	found = true; // Set flag to indicate that the next line should be processed
          	prevLine = line; // Store the current line
      	}
  	}
	logFile.close(); // Close the file


	//assembles the directory where merged .root data files are saved; assumes that each date (folString) contains a directory "RawDataMerged" where the merged files are saved
	std::string dirName = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
	cout << dirName << endl;
	//declare a counter for total number of entries to average over as well as vectors to store the averages of each parameter
	int tEntries = 0;
	int tEntriesLED = 0;
	TH1::AddDirectory(false); // removes warning for memory leak https://root-forum.cern.ch/t/troot-append-replacing-existing-th1-h-potential-memory-leak/42387/8
	TH1 *ledDist = new TH1F("hledDist","Amplitudes normalized to camera median",100,0,2);
	//vector of vectors to store pixel averages
	//Means[0] is mead pedestal
	//Means[1] is mean pedestal RMS
	//Means[2] is mean amplitude during triggered events
	//Means[3] is the mean number of charges recorded during events
	//Means[4] is the mean time of the signal peak of each event
	std::vector<std::vector<Double_t>> Means(7,std::vector<Double_t>(MaxNofChannels, 0.0));

	std::vector<Double_t> triggerRate_vec; // create a vector to store trigger rates as double_t
	std::vector<Double_t> unixTimeFile_vec;

	//Initialize/declare vectors to store event times (TB times) and camera averages of parameters. eventMeans indexing follows same order as Means
	std::vector<uint64_t> eventTimes;
	std::vector<uint64_t> eventTimesLED;
	std::vector<std::vector<Double_t>> eventMeans(7,std::vector<Double_t>());


	// Declate the vectors for the running average
	std::vector<std::vector<Double_t>> RunningMean(7,std::vector<Double_t>());
	std::vector<Double_t> runningSum(7, 0.0);

	std::vector <std::string> fileNamesVec;
	fileNamesVec = read_directory(dirName.c_str());

	string day;
	//Initialize dir as the directory dirname
	for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
      	//ADD GET TIME of file
		//checks if file extention is "root" (typically, all files in RawDataMerged directory will be anyway)
		if(fileNamesVec[f].substr(fileNamesVec[f].find_last_of(".")+1) == "root"){
			cout << "Loading file: " << fileNamesVec[f] << endl;
			std::string dateTtime = fileNamesVec[f].substr(86, 23); // string variable for date and time
			day = dateTtime.substr(0,10); //string variable with just the date
			std::time_t unixTime = convertToUnixTime(dateTtime,1);
			std::cout << "Unix Timestamp: " << unixTime << std::endl;
			unixTimeFile_vec.push_back(unixTime);
			//opens fileName
			TFile *f0 = TFile::Open(fileNamesVec[f].c_str());

			tree = (TTree*)f0->Get(ledString.c_str());
			//create new Event object (ExACT type)
			ev = new Event();
			//SetBranches defined below
			SetBranches(&ev);
			//get number entries in current file, add them to running total
			int nEntriesLED = tree->GetEntries();
			tEntriesLED += nEntriesLED;
			std::cout << "Total Number of Flasher Events: " << nEntriesLED << std::endl;
			//iterate through each event in file
			for(int EventCounter = 0; EventCounter < nEntriesLED; EventCounter++){
				//Get the tree entry associated with event number EventCounter
				tree->GetEntry(EventCounter);
				Pulse *pulse;
				bool noHV = false;
				for(int i = 0; i < MaxNofChannels; i++){
					pulse = new Pulse(ev->GetSignalValue(i));
					int ampVal = pulse->GetAmplitude();
					delete pulse;
					if(ampVal <= 100){
						noHV = true;
						break;
					}
				}
				if(noHV){
					tEntriesLED -= 1;
					continue;
				}
				uint64_t tempT = ev->GetTBTime();
				uint64_t tempTs = tempT*1e-8;
				eventTimesLED.push_back(tempTs);
				//initialize eventMeans value by adding a new value of 0.0
				for(int i = 5; i < 7; i++){
					eventMeans[i].push_back(0.0);
					RunningMean[i].push_back(0.0);
				}
				//Create pulse object pointed named pulse
				ledDist->Reset();
				std::vector<Double_t> ampMed(MaxNofChannels);
				for(int i = 0; i < MaxNofChannels; i++){
					//Set pulse object pointer to new Pulse object with event signal data
					pulse = new Pulse(ev->GetSignalValue(i));
					Means[5][i] += pulse->GetAmplitude();
					ampMed[i] = pulse->GetAmplitude();
					eventMeans[5][tEntriesLED-nEntriesLED+EventCounter] += pulse->GetAmplitude();
					//have to delete pulse object here to avoid memory leak
					delete pulse;
				}
				Double_t medianLED = Median(ampMed);
				for(int i = 0; i < MaxNofChannels; i++){
					pulse = new Pulse(ev->GetSignalValue(i));
					ledDist->Fill(pulse->GetAmplitude()/medianLED);
					delete pulse;
				}
				eventMeans[6][tEntriesLED-nEntriesLED+EventCounter] = ledDist->GetStdDev();
				eventMeans[5][tEntriesLED-nEntriesLED+EventCounter] /= MaxNofChannels;
				//Average eventMeans values; add points to vector
				for(int i = 5; i < 7; i++){
					
					if (eventMeans[i][tEntriesLED-nEntriesLED+EventCounter] < ylimits[i*2] ||  eventMeans[i][tEntriesLED-nEntriesLED+EventCounter] > ylimits[i*2+1]){
						ViewOverFlow[i] += 1;
					}
					runningSum[i] += eventMeans[i][tEntriesLED-nEntriesLED+EventCounter];
					RunningMean[i][tEntriesLED-nEntriesLED+EventCounter] = runningSum[i]/(tEntriesLED-nEntriesLED+EventCounter+1);
				}
			}
			cout << "Flasher events kept: " << tEntriesLED << endl;
			//have to delete ev, tree objects here to avoid memory leak
			delete ev;
			delete tree;

			//sets tree to treeString branch of f0
			tree = (TTree*)f0->Get(testString.c_str());
			//create new Event object (ExACT type)
			ev = new Event();
			//SetBranches defined below
			SetBranches(&ev);
			//get number entries in current file, add them to running total
			int nEntries = tree->GetEntries();
			tEntries += nEntries;
			std::cout << "Total Number of Events: " << nEntries << std::endl;
			Double_t triggerRate = nEntries/102.0; // get triggerRate = entries/second
			triggerRate_vec.push_back(triggerRate);
			//iterate through each event in file
			for(int EventCounter = 0; EventCounter < nEntries; EventCounter++){
				//Get the tree entry associated with event number EventCounter
				tree->GetEntry(EventCounter);
				//adds TB time of current event to eventTimes
				uint64_t tempT = ev->GetTBTime();
				uint64_t tempTs = tempT*1e-8;
				eventTimes.push_back(tempTs);
				//initialize eventMeans value by adding a new value of 0.0
				for(int i = 0; i < 5; i++){
					eventMeans[i].push_back(0.0);
					RunningMean[i].push_back(0.0);
				}
				//Create pulse object pointed named pulse
				Pulse *pulse;
				for(int i = 0; i < MaxNofChannels; i++){
					//Set pulse object pointer to new Pulse object with event signal data
					pulse = new Pulse(ev->GetSignalValue(i));
					//Add values from this event to vectors holding total values to calculate mean
					Means[0][i] += pulse->GetPedestal();
					Means[1][i] += pulse->GetPedestalRMS();
					Means[2][i] += pulse->GetAmplitude();
					Means[3][i] += pulse->GetCharge();
					Means[4][i] += pulse->GetTimePeak();

					eventMeans[0][tEntries-nEntries+EventCounter] += pulse->GetPedestal();
					eventMeans[1][tEntries-nEntries+EventCounter] += pulse->GetPedestalRMS();
					eventMeans[2][tEntries-nEntries+EventCounter] += pulse->GetAmplitude();
					eventMeans[3][tEntries-nEntries+EventCounter] += pulse->GetCharge();
					eventMeans[4][tEntries-nEntries+EventCounter] += pulse->GetTimePeak();
					//have to delete pulse object here to avoid memory leak
					delete pulse;
				}
				//Average eventMeans to vector; 
				for(int i = 0; i < 5; i++){
					eventMeans[i][tEntries-nEntries+EventCounter] /= MaxNofChannels;
					if (eventMeans[i][tEntries-nEntries+EventCounter] < ylimits[i*2] ||  eventMeans[i][tEntries-nEntries+EventCounter] > ylimits[i*2+1]){
						ViewOverFlow[i] += 1;
					}
					runningSum[i] += eventMeans[i][tEntries-nEntries+EventCounter];
					RunningMean[i][tEntries-nEntries+EventCounter] = runningSum[i]/(tEntries-nEntries+EventCounter+1);
				}
			}
			//have to delete ev, tree, f0 objects here to avoid memory leak
			delete ev;
			delete tree;
			delete f0;
		}
	}
	//Initialize canvas and histograms
	c_disp = new TCanvas("Display","CameraPlot",1250,1000);
	//Initialize vector of titles for plots
	std::vector<std::string> htitles = {"hPed","Average Pedestal [ADC Counts]","hPedRMS","Average Pedestal RMS [ADC Counts]","hAmp","Average Amplitude [ADC Counts]","hCharge","Average Charge [ADC Counts]","hPeakT","Average Peak Time [Time bins]","hAmpLED","Average Amplitude of HLED Events [ADC Counts]","hAnmpLEDNorm","Standard Deviation of Amplitude Distribution"};
	//Initialize vector of histogram objects
	std::vector<TH2F *> hDraw;
	//Create histogram objects
	for(int i = 0; i < 6; i++){
		TH2F *hi = new TH2F(htitles[i*2].c_str(),htitles[i*2 + 1].c_str(),16,-0.5,15.5,16,-0.5,15.5);
		hDraw.push_back(hi);
	}

	//Average summed values by dividing by total entries
	for(int i = 0; i < MaxNofChannels; i++){
		//average each pixel/bin
		for(int j = 0; j < 5; j++){
			Means[j][i] /= tEntries;
		}
		Means[5][i] /= tEntriesLED;
		int nx, ny;
		//FindBin defined below
		FindBin(i,&nx,&ny);
		//set values for each pixel/bin in histogram
		for(int j = 0; j < 6; j++){
			hDraw[j]->SetBinContent(nx+1,ny+1,Means[j][i]);
		}
	}

	//Create historgram for Flasher Events
	auto min_timeF = *std::min_element(eventTimesLED.begin(), eventTimesLED.end());
	auto max_timeF = *std::max_element(eventTimesLED.begin(), eventTimesLED.end());
	// Create histograms for scatter plots
    std::vector<TH1F *> hDrawF;
	for(int i = 0; i < 2; i++){
	// Ensure valid indices for htitles
	
	// Corrected constructor call
	TH1F *hiF = new TH1F("hiF", // Name
		htitles[i*2 + 11].c_str(),      // Title
		static_cast<Int_t>(eventTimesLED.size()), // Number of bins in X-axis
		min_timeF,                      // X-axis lower bound
		max_timeF                    // X-axis upper bound
	);
	hDrawF.push_back(hiF);
	}

	// Create histograms for scatter plots
    std::vector<TH1F *> runAF;
	for(int i = 0; i < 2; i++){
	// Ensure valid indices for htitles
	
	// Corrected constructor call
	TH1F *runF = new TH1F("runF", // Name
		htitles[i*2 + 11].c_str(),      // Title
		static_cast<Int_t>(eventTimesLED.size()), // Number of bins in X-axis
		min_timeF,                      // X-axis lower bound
		max_timeF                    // X-axis upper bound
	);
	runAF.push_back(runF);
	}

	
	auto min_time = *std::min_element(eventTimes.begin(), eventTimes.end());
	auto max_time = *std::max_element(eventTimes.begin(), eventTimes.end());
	// Create histograms for regular events
    std::vector<TH1F *> hDraw2;
	for(int i = 0; i < 5; i++){
	// Ensure valid indices for htitles
	
	// Corrected constructor call
	TH1F *hi2 = new TH1F("hi2", // Name
		htitles[i*2 + 1].c_str(),      // Title
		static_cast<Int_t>(eventTimes.size()), // Number of bins in X-axis
		min_time,                      // X-axis lower bound
		max_time                    // X-axis upper bound
	);
	hDraw2.push_back(hi2);
	}

	// Create histograms for scatter plots
    std::vector<TH1F *> runA;
	for(int i = 0; i < 5; i++){
	// Ensure valid indices for htitles
	
	// Corrected constructor call
	TH1F *runAvg = new TH1F("runAvg", // Name
		htitles[i*2 + 1].c_str(),      // Title
		static_cast<Int_t>(eventTimes.size()), // Number of bins in X-axis
		min_time,                      // X-axis lower bound
		max_time                    // X-axis upper bound
	);
	runA.push_back(runAvg);
	}


	Double_t medianNightLED = Median(Means[5]);
	TH2F *hAmpLEDNorm = (TH2F*)hDraw[5]->Clone(htitles[12].c_str());
	hDraw.push_back(hAmpLEDNorm);
	hDraw[6]->Scale(1/medianNightLED);
	hDraw[6]->SetTitle("Average Amplitude of HLED Events normalized to median");

	//Begin writing c_disp TCanvas as a pdf; bracket "[" indicates the first page, and subsequent prints to the same file will append as pages
	c_disp->Print(Form("%s%s.pdf[",outDir.c_str(),folString.c_str()));
	
	auto min_timeTr = *std::min_element(unixTimeFile_vec.begin(), unixTimeFile_vec.end());
	auto max_timeTr = *std::max_element(unixTimeFile_vec.begin(), unixTimeFile_vec.end());
	std::string title = day + " Trigger Rate" ;
	TH1F *trigR = new TH1F("trigR", // Name
		title.c_str(),      // Title
		static_cast<Int_t>(unixTimeFile_vec.size()), // Number of bins in X-axis
		min_timeTr,                      // X-axis lower bound
		max_timeTr                    // X-axis upper bound
	);

	for (size_t i = 0; i < unixTimeFile_vec.size(); ++i) {
    	//g->SetPoint(i, unixTimeFile_vec[i], triggerRate_vec[i]);
		trigR->AddBinContent(i,triggerRate_vec[i]);
  	}
  	
	trigR->GetXaxis()->SetTimeDisplay(1);
	trigR->GetXaxis()->SetNdivisions(505);
	trigR->GetXaxis()->SetTimeFormat("%H:%M");
	trigR->GetXaxis()->SetTimeOffset(0,"gmt");
	trigR->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
  	trigR->GetYaxis()->SetTitle("Trigger Rate [Events/s]");

	//Don't draw stats box
	trigR->SetStats(0);
	trigR->SetMarkerStyle(7);
	trigR->SetMarkerSize(10);
	trigR->SetMarkerColor(1);
	trigR->Draw("P");

	// Add label
	std::vector<Double_t>::iterator trigIt = std::max_element(triggerRate_vec.begin(),triggerRate_vec.end());
  	Double_t ymax = *trigIt;
  	//ymax *= 2.1;
	for (size_t i = 0; i < trig_thres_time.size(); ++i){
    	// Dynamically calculate the y-position of the label
    	Double_t labelY = ymax - (i + 1) * (ymax / (trig_thres_time.size() + 1));
		TLatex* label = new TLatex(trig_thres_time[i], labelY , trigger_threshold[i].c_str());
		label->SetTextSize(0.04);
		label->SetTextAlign(22);
    	label->SetTextColor(kBlue);
		label->Draw();
	}



  	//Add current canvas as page to output pdf
  	c_disp->Print(Form("%s%s.pdf",outDir.c_str(),folString.c_str()));
	delete c_disp;
	c_disp = new TCanvas("Display","CameraPlot",2500,1000);
  
	std::vector<double> AverageValuesVector;
    AverageValuesVector = getAvgeragesFromText();
	
	std::vector<double> hRanges = {3500,4000,0,50,0,500,0,1100,235,255,0,700,0.75,1.25};
	//Loop through each histogram
	for(int i = 0; i < 2; i++){
		//Clear canvas so we can keep reusing the same canvas object
		c_disp->Clear();
		c_disp->Divide(2,1);
		c_disp->cd(1);
		//Set min and max range for bin value gradient
		hDraw[i+5]->SetMinimum(hRanges[i*2+10]);
		hDraw[i+5]->SetMaximum(hRanges[i*2+11]);
		//Don't draw stats box
		hDraw[i+5]->SetStats(0);
		//Set margin size so palette values aren't clipped
		c_disp->cd(1)->SetRightMargin(0.15);
		//Draw histogram to canvas; colz is defined in THistPainter ROOT documentation
		hDraw[i+5]->Draw("colz");
		//DrawMUSICBoundaries is defined below
		DrawMUSICBoundaries();
		//Add current canvas as page to output pdf
		c_disp->cd(2);
		//Fill the histogram with values from the arrays
    	for (size_t p = 0; p < eventTimesLED.size(); ++p) {
        	hDrawF[i]->AddBinContent(p,eventMeans[i+5][p]);
			//cout << p <<"*** " <<eventMeans[i][p] << " ";
        	runAF[i]->AddBinContent(p,RunningMean[i+5][p]);
			//cout << p <<"*** " <<eventMeans[i][p] << " ";
    	}
	
		hDrawF[i]->GetXaxis()->SetTimeDisplay(1);
		hDrawF[i]->GetXaxis()->SetNdivisions(505);
		hDrawF[i]->GetXaxis()->SetTimeFormat("%H:%M");
		hDrawF[i]->GetXaxis()->SetTimeOffset(0,"gmt");
		hDrawF[i]->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
		hDrawF[i]->SetMaximum(ylimits[i*2+11]);
		hDrawF[i]->SetMinimum(ylimits[i*2+10]);

		//DonFt draw stats box
		hDrawF[i]->SetStats(0);
		hDrawF[i]->SetMarkerStyle(6);
		hDrawF[i]->SetMarkerSize(6);
		hDrawF[i]->SetMarkerColor(1);
		hDrawF[i]->Draw("P");

		runAF[i]->SetMarkerStyle(6);
		runAF[i]->SetMarkerSize(6);
		runAF[i]->SetMarkerColor(6);
		runAF[i]->Draw("P,SAME");

		// // // Add green line 
		// TLine *l=new TLine(eventTimes[0],AverageValuesVector[i],eventTimes[eventTimes.size()-1],AverageValuesVector[i]);
		// l->SetLineColor(kGreen);
		// l->SetLineColorAlpha(kGreen,0.6);
		// l->SetLineWidth(4);
		// l->Draw("SAME");


		// // Add Legend
		auto legend = new TLegend(0.1,0.86,0.9,0.94);
		legend->SetNColumns(4);
		std::string overFlowMess = "Overflow points : " + std::to_string(ViewOverFlow[i+5]);
		legend->SetHeader(overFlowMess.c_str(),"C"); // option "C" allows to center the header
		legend->AddEntry(hDrawF[i],"Data Points","p");
		legend->AddEntry(runAF[i],"Running Avg","p");
		// legend->AddEntry(l,"Expected Avg","l");
		legend->Draw("SAME");

		//Add current canvas as page to output pdf
		c_disp->Print(Form("%s%s.pdf",outDir.c_str(),folString.c_str()));
	}

	//Loop through each histogram
	for(int i = 0; i < 5; i++){
		//Clear canvas so we can keep reusing the same canvas object
		c_disp->Clear();
		c_disp->Divide(2,1);
		c_disp->cd(1);
		//Set min and max range for bin value gradient
		hDraw[i]->SetMinimum(hRanges[i*2]);
		hDraw[i]->SetMaximum(hRanges[i*2+1]);
		//Don't draw stats box
		hDraw[i]->SetStats(0);
		//Set margin size so palette values aren't clipped
		c_disp->cd(1)->SetRightMargin(0.15);
		//Draw histogram to canvas; colz is defined in THistPainter ROOT documentation
		hDraw[i]->Draw("colz");
		//DrawMUSICBoundaries is defined below
		DrawMUSICBoundaries();
		//Add current canvas as page to output pdf
		c_disp->cd(2);
		//Fill the histogram with values from the arrays
    	for (size_t p = 0; p < eventTimes.size(); ++p) {
        	hDraw2[i]->AddBinContent(p,eventMeans[i][p]);
			//cout << p <<"*** " <<eventMeans[i][p] << " ";
    	}
		for (size_t p = 0; p < eventTimes.size(); ++p) {
        	runA[i]->AddBinContent(p,RunningMean[i][p]);
			//cout << p <<"*** " <<eventMeans[i][p] << " ";
    	}
	
		hDraw2[i]->GetXaxis()->SetTimeDisplay(1);
		hDraw2[i]->GetXaxis()->SetNdivisions(505);
		hDraw2[i]->GetXaxis()->SetTimeFormat("%H:%M");
		hDraw2[i]->GetXaxis()->SetTimeOffset(0,"gmt");
		hDraw2[i]->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
		hDraw2[i]->SetMaximum(ylimits[i*2+1]);
		hDraw2[i]->SetMinimum(ylimits[i*2]);

		//Don't draw stats box
		hDraw2[i]->SetStats(0);

		hDraw2[i]->SetMarkerStyle(6);
		hDraw2[i]->SetMarkerSize(6);
		hDraw2[i]->SetMarkerColor(1);
		hDraw2[i]->Draw("P");

		runA[i]->SetMarkerStyle(6);
		runA[i]->SetMarkerSize(6);
		runA[i]->SetMarkerColor(6);
		runA[i]->Draw("P,SAME");

		// // Add green line 
		TLine *l=new TLine(eventTimes[0],AverageValuesVector[i],eventTimes[eventTimes.size()-1],AverageValuesVector[i]);
		l->SetLineColor(kGreen);
		l->SetLineColorAlpha(kGreen,0.6);
		l->SetLineWidth(4);
		l->Draw("SAME");

		// // Add Legend
		auto legend = new TLegend(0.1,0.86,0.9,0.94);
		legend->SetNColumns(4);
		std::string overFlowMess = "Overflow points : " + std::to_string(ViewOverFlow[i]);
		legend->SetHeader(overFlowMess.c_str(),"C"); // option "C" allows to center the header
		legend->AddEntry(hDraw2[i],"Data Points","p");
		legend->AddEntry(runA[i],"Running Avg","p");
		legend->AddEntry(l,"Expected Avg","l");
		legend->Draw("SAME");
		//Add current canvas as page to output pdf
		c_disp->Print(Form("%s%s.pdf",outDir.c_str(),folString.c_str()));
		
	}
	//Closes the pdf (doesn't add another page)
	c_disp->Print(Form("%s%s.pdf]",outDir.c_str(),folString.c_str()));
	system(Form("chmod 660 %s%s.pdf",outDir.c_str(),folString.c_str()));
	
	return 0;
}

void SetBranches(Event **evD)
{
	//this really probably does not need to be a defined function since it's just one line. sets branch of tree to "Events"
	tree->SetBranchAddress("Events", evD);
}

//Calculates the 2D bin coordinates associated with a 1D vector/number, i.e. pixel number, based on Trinity camera layout
void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number / 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number % 4;
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}

void addAverageLines(){
    //creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TLine *l=new TLine(-2.5,3780,4,3780);
	l->SetLineColor(kBlue);
	l->SetLineWidth(6);
	l->Draw();
}

std::vector<double> getAvgeragesFromText() {
    std::ifstream inputFile("/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/macro/AvgValuesFromGoodNights.txt");

    // Check if the file is successfully opened 
    if (!inputFile.is_open()) { 
        cerr << "Error opening the file!" << endl;  
    } 
  
    string line; // Declare a string variable to store each 
                 // line of the file 
  
    // Read each line of the file and print it to the 
    // standard output stream 
    std::vector<double> avgValues(5,0.0);
     
    int indexAvgValues = 0;
    while (getline(inputFile, line)) { 
        std::string number;
        int index_colon = 100;
        for(int i = 0; i<line;++i )
        {
            if (line[i] == ':') {      
                index_colon = i;
            }
            if (i > index_colon) {
                number += line[i];
            }
        }   
        double convertNum = std::stod(number);
        avgValues[indexAvgValues] = convertNum;
        indexAvgValues += 1;
    }

    
    // Close the file 
    inputFile.close(); 

    return avgValues;
}

// get files sorted
std::vector <std::string> read_directory(const std::string& path){
	std::vector <std::string> result;
	dirent* de;
	DIR* dp;
	errno = 0;
	dp = opendir( path.empty() ? "." : path.c_str() );
	if (dp)
	{
	while (true)
		{
		errno = 0;
		de = readdir( dp );
		if (de == NULL) break;
		
		result.push_back( Form("%s%s",path.c_str(),de->d_name) );
		}
	closedir( dp );
	std::sort( result.begin(), result.end() );
	}
	return result;
}

// Function to convert a time string to Unix timestamp
std::time_t convertToUnixTime(const std::string& timeString, int t_v) {
    std::tm tm = {};
    std::istringstream ss(timeString);
    if (t_v == 1) { 
    	ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    } else {
    	ss >> std::get_time(&tm, "%Y%m%dT%H%M%S");
    }
    
    //auto tp = std::chrono::system_clock::from_time_t(std::timegm(&tm));
    //auto duration = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    //return duration.count();
    std::time_t time_stamp = timegm(&tm);
    return time_stamp;
    //return std::chrono::system_clock::to_time_t(tp);
    //return utc_time;
}

//Function for calculating median 
Double_t Median(vector<Double_t> v){
	//Size of vector
	int n = v.size();
	//Make temp copy of the vector to leave original in the same order
	std::vector<Double_t> tempV(v);
    //Sort the vector 
    sort(tempV.begin(), tempV.end()); 
    //Check if the number of elements is odd 
    if(n%2!=0){
        return(Double_t)tempV[n/2];
	}
    //If the number of elements is even, return the average of the two middle elements 
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0; 
}