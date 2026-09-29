// R__LOAD_LIBRARY(libExACT.so)
// #include "libExACT.so"
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
#include <algorithm>
#include <TEllipse.h>
#include <IUtilities.h>
#include <TObjString.h>
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
#include <Event.h>
#include <IEvent.h>
#include <IPlotTools.h>
#include <ISiPM.h>
#include <execution>



IUtilities *util;
TTree *tree = 0;
TTree *treeHLED = 0;
IEvent *ev;
IEvent *evHLED;
ISiPM *sipmInfo;

TFile *fO;
TFile *file;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string dataDir = "/data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/MergedData/Output/";
std::string outDir = "/data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/Output";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string listDir = "/data/TrinityLabComputer/TrinityDemonstrator/Data/";
std::string listOutDir = "/data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/data_lists/";
// std::string dataDir = "/mnt/DataAnalysis/DataCalibration/MergedData/Output/";



void SetBranches(IEvent *evD);
void SetBranchesHLED(IEvent *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);



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
    // tree->SetBranchAddress("Event", &ev);
    // tree->Scan("Events.Event");
}

void SetBranchesHLED(IEvent *evD)
{
    treeHLED->SetBranchAddress("Events", &evHLED);
}

