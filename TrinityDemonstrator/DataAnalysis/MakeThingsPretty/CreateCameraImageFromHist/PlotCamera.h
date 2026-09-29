#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TBox.h>
#include <set>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TLine.h>
#include <fstream>
#include <sstream>
#include <TPaletteAxis.h>
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
#include <CEvent.h>
#include <Event.h>
#include <IEvent.h>
#include <IPlotTools.h>
#include <ISiPM.h>
#include <TColor.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>





IUtilities *util;
IPlotTools *plottools;
TFile *file;
std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator/";
// std::string dataDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/";
// std::string outDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/DeadTime/Output/";

TCanvas *canvas=0;

float PtoPEconversion = 0.4*0.83;
std::vector<std::vector<std::string>> splitCSVLines(const std::vector<std::string>& lines); 


std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Simulations/TDemSims/";
// std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Simulations/";
std::string outDir = "/storage/hive/project/phy-otte/sstepanoff3/MakeThingsPretty/Images/";

// std::string OutputFileRoot=Form("%sSimulations20250529.root",outDir.c_str());



std::vector<std::vector<std::string>> splitCSVLines(const std::vector<std::string>& lines) {
    std::vector<std::vector<std::string>> result;

    for (const auto& line : lines) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string item;

        while (std::getline(ss, item, ',')) {
            row.push_back(item);
        }

        result.push_back(row);
    }

    return result;
}

