//tells root where to find ExACT libraries. Only needed if using as a macro for root. To compile, you remove this line and include ExACT libraries directly
R__LOAD_LIBRARY(libExACT.so)
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

//include exact and root libraries; copied from DataMonitor.cpp, some headers may not be used in code
/*#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TLatex.h>
#include <TFile.h>
#include <TBox.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>*/

//set "base" directories
//dataDir is where the daily data directories are saved (i.e. "YYYYMMDD" directories)
//outDir is the directory where output will be saved
std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
std::string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/amplots/";

//initialize or declare global variables, which are allocated static memory and are available in every scope
TTree *tree = 0;
Event *ev;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;
TH1 *aDist = new TH1F("hAmpD","Amplitude Distribution",1000,0,3999);
TH2F *aMap = new TH2F("hAmp","Average Amplitude [ADC Counts]",16,-0.5,15.5,16,-0.5,15.5);
TH2F *aTraces = new TH2F("hTrace","Scatterplot of all Traces",512,-0.5,511.5,4000,-0.5,3999.5);

//declare functions to be defined later
void SetBranches(Event **evD);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();

/*int main(int argc, char **argv){
	if(argc < 2){
		cout << "Too few arguments; please include the date, file and branch names" << endl;
		return 1;
	}
	std::string folStr = argv[1];
	std::string fileStr = argv[2];
	std::string treeStr = argv[3];*/
void amplots_Trinity(std::string folStr, std::string fileStr, std::string treeStr){
	//assembles the directory where merged .root data files are saved; assumes that each date (folString) contains a directory "RawDataMerged" where the merged files are saved
	std::string fileName = Form("%s%s/RawDataMerged/CoBo0_AsAd0_%s_0000.root",dataDir.c_str(),folStr.c_str(),fileStr.c_str());
	cout << "Loading file: " << fileName << endl;
	//opens fileName
	TFile *f0 = TFile::Open(fileName.c_str());
	//sets tree to treeString branch of f0
	tree = (TTree*)f0->Get(treeStr.c_str());
	//create new Event object (ExACT type)
	ev = new Event();
	//SetBranches defined below
	SetBranches(&ev);
	//get number entries in current file, add them to running total
	int nEntries = tree->GetEntries();
	std::cout << "Total Number of Events: " << nEntries << std::endl;
	//vector to store data to plot/avg
	std::vector<Double_t> ampl(MaxNofChannels, 0.0);
    //iterate through each event in file
	for(int EventCounter = 0; EventCounter < nEntries; EventCounter++){
		//Get the tree entry associated with event number EventCounter
		tree->GetEntry(EventCounter);
		//Create pulse object pointed named pulse
		Pulse *pulse;
		for(int i = 0; i < MaxNofChannels; i++){
			//Set pulse object pointer to new Pulse object with event signal data
			std::vector<Int_t> trace = std::vector<Int_t>(512);
			trace = ev->GetSignalValue(i);
			pulse = new Pulse(trace);
			Double_t amplP = pulse->GetAmplitude();
			aDist->Fill(amplP);
			ampl[i] += (amplP/nEntries);
			for(int b = 0; b < 512; b++){
				aTraces->Fill(b,trace[b]);
			}
			//have to delete pulse object here to avoid memory leak
			delete pulse;
		}
	}
	//have to delete ev, tree, f0 objects here to avoid memory leak
	delete ev;
	delete tree;
	delete f0;

	for(int i = 0; i < MaxNofChannels; i++){
		int nx, ny;
		//FindBin defined below
		FindBin(i,&nx,&ny);
		//set values for each pixel/bin in histogram
		aMap->SetBinContent(nx+1,ny+1,ampl[i]);
	}

	c_disp = new TCanvas("Display","CameraPlot",2500,1000); c_disp->Divide(2,1);
	c_disp->cd(1); c_disp->cd(1)->SetRightMargin(0.15); aMap->SetStats(0); aMap->Draw("colz"); DrawMUSICBoundaries();
	c_disp->cd(2); aDist->SetStats(0); aDist->GetXaxis()->SetTitle("Signal Amplitude [ADC Count]"); aDist->Draw("hist");
	std::string outStr = Form("%s%s/%s",outDir.c_str(),folStr.c_str(),fileStr.c_str());
	gSystem->mkdir(outStr.c_str(),kTRUE);
  	c_disp->Print(Form("%s%s/%s/Amplitudes_%s_%s.png",outDir.c_str(),folStr.c_str(),fileStr.c_str(),fileStr.c_str(),treeStr.c_str()));

	delete c_disp;
	c_disp = new TCanvas("Display","CameraPlot",1000,1000);
	aTraces->SetStats(0); aTraces->GetXaxis()->SetTitle("Relative time (ns)"); aTraces->Draw("scat");
	c_disp->Print(Form("%s%s/%s/Traces_%s_%s.png",outDir.c_str(),folStr.c_str(),fileStr.c_str(),fileStr.c_str(),treeStr.c_str()));
//	return 0;
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
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;	
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
