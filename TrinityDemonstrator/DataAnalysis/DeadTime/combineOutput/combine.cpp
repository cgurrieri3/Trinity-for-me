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
#include <IEvent.h> // or whatever your header file is

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <folder_path> <histogram_name> <output_file>" << std::endl;
        return 1;
    }
    
    std::string folderPath = "/mnt/DataAnalysis/DeadTime/Output/";
    std::string histName = "DeltaTDist";
    std::string outputFile = "/mnt/DataAnalysis/DeadTime/combineOutput/output.root";
    
    IUtilities *utils = new IUtilities();
    std::vector<std::string> files = utils->GetFilesInDirectory(folderPath, ".root");
    
    if (files.empty()) {
        std::cerr << "No .root files found in " << folderPath << std::endl;
        return 1;
    }
    
    TH1I* hCombined =0;
    
    for (const auto& filename : files) {
        std::string fullPath = folderPath + "/" + filename;
        TFile* file = TFile::Open(fullPath.c_str(), "READ");
        
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file: " << fullPath << std::endl;
            continue;
        }
        
        TH1I* hist = (TH1I*)file->Get(histName.c_str());
        
        if (!hist) {
            std::cerr << "Histogram " << histName << " not found in " << filename << std::endl;
            file->Close();
            continue;
        }
        
        if (!hCombined) {
            hCombined = (TH1I*)hist->Clone("hCombined");
            hCombined->SetDirectory(0);
        } else {
            hCombined->Add(hist);
        }
        
        file->Close();
    }
    
    if (!hCombined) {
        std::cerr << "No histograms were combined!" << std::endl;
        return 1;
    }
    
    TFile* outFile = TFile::Open(outputFile.c_str(), "RECREATE");
    hCombined->Write();
    outFile->Close();
    
    std::cout << "Combined " << files.size() << " files into " << outputFile << std::endl;
    
    return 0;
}