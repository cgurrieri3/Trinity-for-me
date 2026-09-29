//R__LOAD_LIBRARY(libExACT.so)
//#include "libExACT.so"
#include <set>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <TPaletteAxis.h>
#include <TEllipse.h>
#include <IUtilities.h>

#include <TH2F.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <TArrow.h>
#include <sys/types.h>
#include <vector>
#include <TLatex.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TFile.h>
#include <TBox.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TFile.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TMath.h>
#include <TPrincipal.h>
#include "Math/Integrator.h"
#include <TPaveText.h>
#include <random>
#include <utility>

// Define variables (global)
std::vector<double> MaxAmplitudePixel;
std::vector<int> MaxPixelID;
std::vector<int> MaxPeakTimePixel;
std::vector<int> MaxMUSICID;
std::vector<double> MaxAmplitudePixelTimeBin;
std::vector<int> MaxPixelIDTimeBin;
std::vector<double> AvgAmplitudePerEvent;
std::vector<double> CameraAmplitudeAtTimeBin;

IUtilities *util;

TTree *tree = 0;
TTree *treeHLED = 0;
Event *ev;
Event *evHLED;

TH2F* COGgraph=0;
TH2F* COGgraphweighted=0;
TH2F* pixeldist=0;

TFile *fO;
TFile *file;

TCanvas *c_cleaned =0;
TCanvas *c_PCA=0;
int MaxNofChannels = 256;

std::string neighborDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/neighbors/";
std::string CalibrationFactorDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/FlatFieldCalibration/Output/";
std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
// std::string dataDir = "/storage/hive/project/phy-otte/sstepanoff3/Data/";
std::string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/Output/";
// std::string outDir = "/storage/hive/project/phy-otte/sstepanoff3/EventCleaning/ClusterCleaning/Output/";


// Initialize functions

void SetBranches(Event *evD);
void SetBranchesHLED(Event *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries(); 

void LoadEvents(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void LoadEventsHLED(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    //cout << "Loading file for HLED: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    treeHLED = (TTree*)fO->Get(treeString.c_str());
}


void SetBranches(Event *evD)
{
    tree->SetBranchAddress("Events", &ev);
}

void SetBranchesHLED(Event *evD)
{
    treeHLED->SetBranchAddress("Events", &evHLED);
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
    delete b;
}

void FindBin(int pixelID, int *nx, int *ny)
{
	// Calculate the SIAB number (0 to 15)
	int SIAB_Number = pixelID / 16;

        // Calculate the pixel number within the SIAB (0 to 15)
	int SIAB_Pixel_Number = pixelID % 16;

        // Calculate the row and column within the SIAB (0 to 3 for both)
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;

        // Calculate the overall row and column	
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

