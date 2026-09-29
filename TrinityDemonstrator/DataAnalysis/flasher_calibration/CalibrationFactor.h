//R__LOAD_LIBRARY(libExACT.so)
//#include "libExACT.so"
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <math.h>
#include <TPaletteAxis.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
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
#include <numeric>
#include <TFile.h>
#include <TBox.h>
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
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TMath.h>
#include "Math/Integrator.h"
#include <TPaveText.h>
#include <sys/stat.h>
#include <IUtilities.h>
#include <IPlotTools.h>
#include <IEvent.h>



TTree *tree = 0;
TTree *treeHLED = 0;
IEvent *ev;
IEvent *evHLED;
TFile *file;
IUtilities *util;
IPlotTools *plottools;

TFile *fO;
TCanvas *c_disp = 0;
TCanvas *c_cleaned =0;
int MaxNofChannels = 256;

// std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator/";
std::string dataDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/";
std::string outDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/flasher_calibration/Output/";

// Initialize functions
void SetBranches(IEvent *evD);
void SetBranchesHLED(IEvent *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);


const int TimeBinAll = 239; // Difference between triggered pixel time bin and the pixels around it time bin difference more that 1 risk saving cross talk events
int FlasherEventsCutOff = 960; //960// Average amplitude across the camera ~1200 is  Flasher event

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

void SetBranches(IEvent *evD)
{
    tree->SetBranchAddress("Events", &ev);
}

void SetBranchesHLED(IEvent *evD)
{
    treeHLED->SetBranchAddress("Events", &evHLED);
}


