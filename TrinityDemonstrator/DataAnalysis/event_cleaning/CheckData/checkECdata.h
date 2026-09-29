#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <iostream>
#include <filesystem>
#include  <TCanvas.h>

#include <IUtilities.h>
#include <IPlotTools.h>
#include <TArrow.h>
#include <TLatex.h>
#include <algorithm>
#include <vector>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TEllipse.h>


using namespace std;
TFile *f1 =0;
TH1F* h1 = 0;
int MaxNofChannels = 256;



IPlotTools *plottools = new IPlotTools();
// PlotHelp *plothelp = new PlotHelp(); // initialize the plot help class in scope for whole program
IUtilities *util = new IUtilities();

std::string FolderPath;
std::string DatesListPath;
std::vector<std::string> fileNamesVec;
std::vector<std::string> MergedfileNamesVec;
std::string MergedFolderPath;
std::vector<std::string> allowedFilesVec;