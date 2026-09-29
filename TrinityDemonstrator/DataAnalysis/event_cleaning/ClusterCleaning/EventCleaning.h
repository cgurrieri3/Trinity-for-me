//R__LOAD_LIBRARY(libExACT.so)
//#include "libExACT.so"
#include "PCA.h"
#include "PlotHelp.h"
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
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TFile.h>
#include <TBox.h>
#include <TMarker.h>
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
#include <IEvent.h>
#include <IPlotTools.h>
#include <ISiPM.h>
#include <execution>
#include "EventInfo.h"
#include <SEvent.h>


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
IPlotTools *plottools;
CEvent *cev;
// Saturated pixel IDs as measured on the raw trace, before cleaning has run. Held here
// so CompletePanel4 can intersect them with the surviving pixels once those are known.
std::vector<Int_t> SaturatedPixelIDsRaw;
PlotHelp *plothelp;
EventInfo *eventInfo=0;

TTree *tree = 0;
TTree *treeHLED = 0;
IEvent *ev;
IEvent *evHLED;
ISiPM *sipmInfo;


TH2F* hcam_panel1=0;
TH2F* hcam_panel2=0;
TH2F* hcam_panel3=0;
TH2F* hcam_panel4=0;
TH2F* COGgraph=0;
TH2F* COGgraphweighted=0;
TH2F* pixeldist=0;

TFile *fO;
TFile *file;

TCanvas *c_cleaned =0;
TCanvas *c_PCA=0;
int MaxNofChannels = 256;
std::string OutputFileRoot = "";
std::string OutputFilePDF = "";
std::string OutputFilePDFOpen = "";
std::string OutputFilePDFClose = "";
std::string OutputFileEventCleaningDataRoot = "";
std::string whatData = "Data";

std::string mnt = "/cedar/";
std::string dataDir = "/cedar/DataAnalysis/MergedData/Output/";
std::string outDir = "/cedar/DataAnalysis/event_cleaning/Output/";


std::string neighborDir = "/cedar/DataAnalysis/event_cleaning/ClusterCleaning/neighbors/";
std::string CalibrationFactorDir = "/cedar/DataAnalysis/flasher_calibration/Output/";

// FIX THESE 2025-09-23
std::string muonDir = "/cedar/DataAnalysis/DataTxtFiles/Muon/";
std::string bkgDir = "/cedar/DataAnalysis/DataTxtFiles/BackgroundSamples/RandomSampling/";
std::string simDir = "/cedar/DataAnalysis/SimulationAnalysis/data/";

std::string outDirBkg = "/cedar/DataAnalysis/event_cleaning/OutputBkg/";
std::string outDirMuon = "/cedar/DataAnalysis/event_cleaning/OutputSim/";
std::string outDirSim = "/cedar/DataAnalysis/event_cleaning/OutputSim/";


const int TriggeredChannelAmpCutOff = 481; // (200 ADC/8 PE) Cut off for the triggered music channel
const int TimeBinAll = 239; // Difference between triggered pixel time bin and the pixels around it time bin difference more that 1 risk saving cross talk events
const int CorePixelAmpCutOff = 481; // (200ish ADC/8PE) // Cut off for the pixels surrounding the triggered music channel
const int SaturatedPixelCutoff = 256; // 0 removed saturated pixels max channels from being cleaned. 1 allows them to be cleaned (changed from 0 on 2/26/2025)
int FlasherEventsCutOff = 800; // (350 ish ADC/15 PE ) Average amplitude across the camera ~1200 is  Flasher event
int PixelSurviveCutOff = 3; //How many pixels need to survive cleaning to plot
const int WLRatioCutOff = 100; // length width How elliptical you require the events to be after the Principle Compomnent Analysis (helps removed crosstalk events.)
const int rmTopRow = 1; // removed the top row of the camera (sky) 0 = remove, 1 = dont remove
int sizeCutoff = 0; // in PE so 100, or 200 pe removed the top row of the camera (sky) 0 = remove, 1 = dont remove
int NumberOfCoresCutoff = 2; // 1, 2,3 cores removed the top row of the camera (sky) 0 = remove, 1 = dont remove
float CrosstalkAllowanceCutoff = 0.3; // typically 0.3 removed the top row of the camera (sky) 0 = remove, 1 = dont remove
float CorePixelCutOff = 1.5;

// The CARE traces are embedded into the 512 bin demonstrator window centred, not at the
// demonstrator's peak bin, so the simulated pulse lands ~59 bins early (offset 156 in
// PlotCamera::create_root_file_care + kCareTimeBin 25 = 181; measured camera peak 178-183).
// Every cut downstream assumes the pulse sits at TimeBinAll, so sim traces are shifted onto
// that bin before any Pulse is built.
const int SimPeakTimeBin = 184; // time bin the CARE pulse actually lands on in the sim traces
const int SimTimeBinOffset = TimeBinAll - SimPeakTimeBin; // 239 - 180 = 59


void SetBranches(IEvent *evD);
void SetBranchesHLED(IEvent *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);
void saveEventInfo(EventInfo* evI, TTree* treeSims);

void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp);
std::vector<double> CreateWLRatio(PCA& pca, TVectorD& eigenVals, TMatrixD& eigenVecs);
std::vector<int> CheckCrossPoints(TArrow* arrow, TH2F* hist, EventInfo* eventInfo);
void DrawSaturationMarkers(const std::vector<Int_t>& pixelIDs);
void CompletePanel4(PCA& pca, TH2F* hcam_panel4, CEvent* cev, std::vector<double> EllipicRatio, TVectorD& eigenVals, TMatrixD& eigenVecs, EventInfo* eventInfo);
bool projectOnMajorAxis(double xcog, double ycog, double anglerad, std::vector<int> sur_pix,
                       std::vector<float> amps, std::vector<double>& d, std::vector<double>& q);
double getM3Long(double xcog, double ycog, double anglerad, std::vector<int> sur_pix, std::vector<float> amps);
double getM3LongPow3(double xcog, double ycog, double anglerad, std::vector<int> sur_pix, std::vector<float> amps);
std::vector<double> generateRandomNumbers();
void CreateFileName(std::string filename, bool bkg);
std::vector<Int_t> ShiftTrace(const std::vector<Int_t>& trace, int offset);

void CreateFileName(std::string filename, std::string dataType) {
    if (dataType == "bkg") {
        outDir = outDirBkg;
    }
    if (dataType == "muon") {
        outDir = outDirMuon;
    }
    if (dataType == "sim") {
        outDir = outDirSim;
    }
    OutputFileRoot=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        WLRatioCutOff,
        rmTopRow
    );

    OutputFileEventCleaningDataRoot = Form("%sDataFiles/Data_EventCleaning%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        WLRatioCutOff,
        rmTopRow
    );
    int check = mkdir(Form("%sDataFiles",outDir.c_str()),0777);

    // check if directory is created or not
    if (!check)
        printf("Directory created\n");
    else {
        printf("Unable to create directory\n");
    }


    // Create the file name with the cariables in the name for sorting of them. 
    // _TC_#_ = TriggeredChannelAmpCutOff
    // TB_#_ = TimeBinAll
    // NP_#_ = CorePixelAmpCutOff
    // s_#_ = SaturatedPixelCutoff
    // FA_#_ = FlasherEventsCutOff
    // mp_# = PixelSurviveCutOff
    OutputFilePDF=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        WLRatioCutOff,
        rmTopRow
    );

    OutputFilePDFOpen=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf[",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        WLRatioCutOff,
        rmTopRow
    );

    OutputFilePDFClose=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf]",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        WLRatioCutOff,
        rmTopRow
    );
}


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


// Slides a trace by offset bins, padding with the leading baseline sample so the pedestal
// window (the first samples of the trace) is unchanged. A positive offset moves the pulse
// later in the window, which is what the sim needs to line up with the demonstrator.
std::vector<Int_t> ShiftTrace(const std::vector<Int_t>& trace, int offset)
{
    if (offset == 0 || trace.empty()) {
        return trace;
    }
    std::vector<Int_t> shifted(trace.size(), trace.front());
    for (std::size_t i = 0; i < trace.size(); i++) {
        long src = static_cast<long>(i) - offset;
        if (src >= 0 && src < static_cast<long>(trace.size())) {
            shifted[i] = trace[src];
        }
    }
    return shifted;
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

void saveEventInfo(EventInfo* evI, TTree* treeSims) {
    // TFile *fileOutput = new TFile("Test.root", "UPDATE");
    // treeSims->Branch("Cleaned","EventInfo",evI);
    treeSims->Fill();
    // fileOutput->Write();
    // fileOutput->Close();
}



// /***
// Start of Functions

// ***/



void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp){
    


    // (*COG)[0] = 0.0;
    // (*COG)[1] = 0.0;
   

    // Loop over the bins of the histogram and extract the bin content (data points)
    for (int bin = 0; bin <= MaxNofChannels; bin++) {
       
        int nx, ny;
        plottools->FindBin(bin, &nx, &ny);
        // Get the bin center for X and Y
        double weight = hist->GetBinContent(nx+1, ny+1);
         // Check if the value is infinity
        if (std::isinf(weight)) {
            weight = 0.0; // Set to 0
        }
        
        // Add the data point to the PCA object weighted by the bin content
        if (weight > 0) {  // Only add rows for non-zero bins

            std::vector<double> randomNumbers;
            for (double w = 0; w < weight; w++){
                randomNumbers = generateRandomNumbers();
                // cout << "nx: " << nx <<  " ny: " << ny << endl;
                // cout << "rand 1: " << nx + randomNumbers[0] << " rand 2: " << ny + randomNumbers[1] << endl;
                std::vector<double> data = {nx + randomNumbers[0], ny+randomNumbers[1]};
                // cout << "COG " << weight << "total" << totalAmp << endl;
                
                pca.AddRow(data);  
            }
            if (nx == 0){
                nx = 0.1;
            }
            if (ny == 0){
                ny = 0.1;
            }

            // (*COG)[0] += (nx * (weight/totalAmp));
            // (*COG)[1] += (ny * (weight/totalAmp));
        }   
    }
}

std::vector<double> generateRandomNumbers() {
    // Define the random number generator and distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.49, 0.49);

    // Generate two random numbers and return them as a pair
    return {dist(gen), dist(gen)};
}

std::vector<double> CreateWLRatio(PCA& pca, TVectorD& eigenVals, TMatrixD& eigenVecs) {
    eigenVals.ResizeTo(2);
    eigenVecs.ResizeTo(2,2);
    std::vector<double> sigmas;

    pca.ComputeEigenDecomposition();
    eigenVals = pca.GetEigenValues();
    eigenVecs = pca.GetEigenVectors();
    sigmas = pca.GetSigmas();                            
    
    double r1 = sqrt(eigenVals[0]);
    double r2 = sqrt(eigenVals[1]);
    std::vector<double> EllipicRatio = {r1,r2,r2/r1};
    cout << "WL " << EllipicRatio[0] << "," << EllipicRatio[1] << ", ratio:" <<  EllipicRatio[2] << "is: " << WLRatioCutOff/100.0 << endl;
    
    return EllipicRatio;
}

std::vector<int> CheckCrossPoints(TArrow* arrow, TH2F* hist,EventInfo* eventInfo) {
    // get the arrow slope
    cout << "Checking cross points" << endl;
    double x1 = arrow->GetX1();
    double y1 = arrow->GetY1();
    double x2 = arrow->GetX2();
    double y2 = arrow->GetY2();

    // Line vector components
    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = std::sqrt(dx*dx + dy*dy);

    // Degenerate line guard
    if (len == 0.0) return {0, 0};

    int cLineIntersectPixels    = 0;
    int cLineNonIntersectPixels = 0;

    int nBinsX = hist->GetNbinsX();
    int nBinsY = hist->GetNbinsY();

    double RMSMajorAxis = 0.0;
    double WeightedRMSMajorAxis = 0.0;
    double totalWeight = 0.0;

    for (int ix = 1; ix <= nBinsX; ++ix) {
        for (int iy = 1; iy <= nBinsY; ++iy) {

            if (hist->GetBinContent(ix, iy) <= 0) continue;

            // Bin center in axis coordinates
            double cx = hist->GetXaxis()->GetBinCenter(ix);
            double cy = hist->GetYaxis()->GetBinCenter(iy);

            // Perpendicular distance from bin center to the infinite line
            // d = |(P - P1) x d_hat|  (2D cross product gives scalar)
            double dist = std::abs((cy - y1)*dx - (cx - x1)*dy) / len;
            plothelp->AddtoDistance2MajorAxis(dist);
            eventInfo->SetDistance2MajorAxis(dist);
            RMSMajorAxis += dist*dist;
            WeightedRMSMajorAxis += dist*dist*hist->GetBinContent(ix, iy);
            totalWeight += hist->GetBinContent(ix, iy);
            // Half-diagonal of the bin as intersection threshold
            double hw = 0.5 * hist->GetXaxis()->GetBinWidth(ix);
            double hh = 0.5 * hist->GetYaxis()->GetBinWidth(iy);
            double threshold = 0.905*std::sqrt(hw*hw + hh*hh);
            cout << threshold << endl;
            if (dist <= threshold) {
                ++cLineIntersectPixels;
                cout << "Intersecting pixel at (" << cx << ", " << cy << ") with content " << hist->GetBinContent(ix, iy) << " distance " << dist << endl;

            }
            else {
                ++cLineNonIntersectPixels;
                cout << "Non-intersecting pixel at (" << cx << ", " << cy << ") with content " << hist->GetBinContent(ix, iy) << " distance " << dist << endl;
            }
        }
    }
    RMSMajorAxis = std::sqrt(RMSMajorAxis/(cLineIntersectPixels+cLineNonIntersectPixels));
    WeightedRMSMajorAxis = std::sqrt(WeightedRMSMajorAxis/(totalWeight));
    cout << "RMS Major Axis: " << RMSMajorAxis << endl;
    cout << "Weighted RMS Major Axis: " << WeightedRMSMajorAxis << endl;

    plothelp->AddtoPixelsonMajorAxis(cLineIntersectPixels);
    plothelp->AddtoPixelsoffMajorAxis(cLineNonIntersectPixels);
    plothelp->AddtoRatioPixelsMajorAxis((double)cLineIntersectPixels/(cLineIntersectPixels+cLineNonIntersectPixels));
    plothelp->AddtoRatioPixelsMajorAxisPerPixel((double)cLineIntersectPixels/(cLineIntersectPixels+cLineNonIntersectPixels),
                                                cLineIntersectPixels+cLineNonIntersectPixels);
    plothelp->AddtoRMSMajorAxis(RMSMajorAxis);
    plothelp->AddtoWeightedRMSMajorAxis(WeightedRMSMajorAxis);

    eventInfo->SetRMSMajorAxis(RMSMajorAxis);
    eventInfo->SetWeightedRMSMajorAxis(WeightedRMSMajorAxis);
    eventInfo->SetPixelsonMajorAxis(cLineIntersectPixels);
    eventInfo->SetPixelsoffMajorAxis(cLineNonIntersectPixels);
    eventInfo->SetRatioPixelsMajorAxis((double)cLineIntersectPixels/(cLineIntersectPixels+cLineNonIntersectPixels));


    return {cLineIntersectPixels, cLineNonIntersectPixels};

}

// Dots the given pixels on whatever pad is current, so saturation is visible on the camera
// image itself and not only in the header text. Call it AFTER the panel histogram has been drawn:
// TH1::Draw() without "same" clears the pad, so the next event's panel takes these markers with it
// rather than letting them pile up, which is the same lifetime the arrows and ellipse below rely on.
void DrawSaturationMarkers(const std::vector<Int_t>& pixelIDs)
{
    for (std::vector<Int_t>::size_type s = 0; s < pixelIDs.size(); s++) {
        int nx, ny;
        plottools->FindBin((int)pixelIDs[s], &nx, &ny);
        // camera bins are one unit wide and centred on the integer bin index, so the bin index
        // is the pixel centre the dot goes on
        TMarker* satDot = new TMarker(nx, ny, kFullCircle);
        satDot->SetMarkerColor(kBlack);
        satDot->SetMarkerSize(0.7); // small enough to leave the amplitude colour around it readable
        satDot->Draw();
    }
}

void CompletePanel4(PCA& pca, TH2F* hcam_panel4, CEvent* cev, std::vector<double> EllipicRatio, TVectorD& eigenVals, TMatrixD& eigenVecs,  EventInfo* eventInfo) {
    
    
    
    // The parameter text under the title needs two lines now, so pull the frame down to make
    // room. Without this the second line lands inside the frame, on top of the y axis label.
    gPad->SetTopMargin(0.20);
    hcam_panel4->Draw("colz");
    plottools->DrawMUSICBoundaries();
    
    double meanx = hcam_panel4->GetMean(1);
    double meany = hcam_panel4->GetMean(2);
    // cout << "MEAN " << meanx << "," << meany << endl;
    
    double angledeg = atan(eigenVecs[1][0]/eigenVecs[0][0])*(180.0/3.141592653589793238463);
    double anglerad = atan(eigenVecs[1][0]/eigenVecs[0][0]);
    
    
    double r1 = sqrt(eigenVals[0]);
    double r2 = sqrt(eigenVals[1]);
    TArrow* arrow = new TArrow(meanx - 2*(r1*cos(anglerad)), meany - 2*(r1*sin(anglerad)), meanx + 2*(r1*cos(anglerad)), meany + 2*(r1*sin(anglerad)), 0.01, "|"); // "|>" option gives an arrowhead
    TArrow* arrow1 = new TArrow(meanx - 2*(r2*cos(anglerad+1.5708)), meany - 2*(r2*sin(anglerad+1.5708)), meanx + 2*(r2*cos(anglerad+1.5708)), meany + 2*(r2*sin(anglerad+1.5708)), 0.01, "|"); // "|>" option gives an arrowhead
    
    std::vector<int> CrossPoints = CheckCrossPoints(arrow, hcam_panel4,eventInfo);
    cout << "# Intersecting pixels with major axis: " << CrossPoints[0] << endl;
    cout << "# Non-intersecting pixels with major axis: " << CrossPoints[1] << endl;
    arrow->SetLineColor(kRed); // Optional: Set the color of the arrow
    arrow->Draw("SAME");             // Draw the arrow on the same canvas
    arrow1->SetLineColor(kBlue); // Optional: Set the color of the arrow
    arrow1->Draw("SAME");             // Draw the arrow on the same canvas
    
    TEllipse* ell = new TEllipse(meanx, meany,r1,r2,0,360,angledeg);
    ell->SetFillColorAlpha(kGreen,0.00);
    ell->Draw("SAME");
    // Keep only the saturated pixels that also survived cleaning, so the stored IDs describe
    // the cleaned image. Events that never reach here keep the empty list set by Clear().
    std::vector<int> survivingPixels = cev->GetSurvivingPixelPanel3();
    std::vector<Int_t> SaturatedSurvivingIDs;
    for (std::vector<Int_t>::size_type s = 0; s < SaturatedPixelIDsRaw.size(); s++) {
        if (std::find(survivingPixels.begin(), survivingPixels.end(),
                      (int)SaturatedPixelIDsRaw[s]) != survivingPixels.end()) {
            SaturatedSurvivingIDs.push_back(SaturatedPixelIDsRaw[s]);
        }
    }

    // Show where the saturation actually is, not just how much of it there is.
    DrawSaturationMarkers(SaturatedSurvivingIDs);

    TLatex* title = new TLatex();
    title->SetNDC(); // Set to Normalized Device Coordinates (NDC)
    title->SetTextSize(0.03);
    
    float areaEllipse = (3.8/16) * (3.8/16) * (cev->GetSurvivingPixelPanel3().size()); // area of an ellipse
    double conc = (cev->GetTotalCoreAmp())/cev->GetSurvivingPixelTotalAmpPanel3();
    
    
    // Two longitudinal 3rd moments, both projected on the major axis. They do NOT agree in sign
    // and are not interchangeable -- see the header comments on each function.
    double M3Long     = getM3Long(meanx, meany, anglerad, cev->GetSurvivingPixelPanel3(),cev->GetAmplitudeValuesTimeBin());
    double M3LongPow3 = getM3LongPow3(meanx, meany, anglerad, cev->GetSurvivingPixelPanel3(),cev->GetAmplitudeValuesTimeBin());
    // Up-down symmetry: the major axis dotted with the vertical, oriented head-to-tail so the
    // value says which way the image actually points. The eigenvector alone cannot: atan()
    // forces cos(angle) > 0, so it always points toward +x and carries no up/down information.
    // M3Long supplies the missing direction. M3Long > 0 means the faint tail runs toward +x, so
    // the bright head sits toward -x and the head direction is -sign(M3Long)*(cos,sin). Dotting
    // that with the vertical gives +1 for a head pointing straight up, -1 straight down, and 0
    // for an image lying along the camera x axis.
    //
    // getM3Long returns exactly 0.0 only from its guard paths (no usable charge, or an axis too
    // close to vertical to project onto), never from a real image, so that case has no head-tail
    // direction to orient by. It is recorded as NaN rather than flattened onto 0, which would
    // otherwise read as a perfectly horizontal image. PlothUpDownSym skips those entries.
    double upDownSym = std::numeric_limits<double>::quiet_NaN();
    if (M3Long != 0.0) {
        upDownSym = (M3Long > 0.0 ? -1.0 : 1.0) * sin(anglerad);
    }
    // cev->GetSurvivingPixelTotalAmpPanel3()/util->GetADCtoPEratio(),
    // Split over two lines: one line no longer fits the pad width now that it carries both
    // moments. Sat is the saturated pixels that SURVIVED cleaning, matching Pixels and the
    // SaturatedPixelIDs stored on EventInfo, not the raw count seen on the trace.
    title->DrawLatex(0.1, 0.915, Form("WL:%.2f Pixels:%i Sat:%i Area:%.2f Size:%.2f Conc:%.2f",
        EllipicRatio[2],
        static_cast<int>((cev->GetSurvivingPixelPanel3()).size()),
        static_cast<int>(SaturatedSurvivingIDs.size()),
        areaEllipse,
        cev->GetSurvivingPixelTotalAmpPanel3(),
        conc));
    title->DrawLatex(0.1, 0.870, Form("M3Long:%.2f M3Pow3:%.3g UpDownSym:%.2f",
        M3Long,
        M3LongPow3,
        upDownSym));
    delete title;

    // add values  for creating the plots
    plothelp->AddtoWL(EllipicRatio[2]);
    plothelp->AddtoL(EllipicRatio[0]);
    plothelp->AddtoW(EllipicRatio[1]);
    plothelp->AddtoSize(cev->GetSurvivingPixelTotalAmpPanel3());
    plothelp->AddtoSurvivingPixelCount(cev->GetSurvivingPixelPanel3().size());
    plothelp->AddtoConcentation(conc);
    plothelp->AddtoCOGx(meanx);
    plothelp->AddtoCOGy(meany);
    plothelp->AddtoTriggeredPixelsID(cev->GetMaxAmplitudePixelID());
    plothelp->AddtoCoreRatio(cev->GetCoreRatio());
    plothelp->AddtoNumberOfCores(cev->GetNumberofCorePixels());
    plothelp->AddtoAngle(anglerad);
    plothelp->AddtoUpDownSym(upDownSym);
    eventInfo->SetSaturatedPixels(SaturatedSurvivingIDs);
    plothelp->AddtoSaturatedPixels((int)SaturatedSurvivingIDs.size());
    // camera position of every saturated pixel that survived, one entry per pixel
    for (std::vector<Int_t>::size_type s = 0; s < SaturatedSurvivingIDs.size(); s++) {
        int satx, saty;
        plottools->FindBin((int)SaturatedSurvivingIDs[s], &satx, &saty);
        plothelp->AddtoSaturatedPixelPosition(satx, saty);
    }
    // camera position of every pixel that survived cleaning, one entry per pixel
    for (int survivingPixelID : cev->GetSurvivingPixelPanel3()) {
        int spx, spy;
        plottools->FindBin(survivingPixelID, &spx, &spy);
        plothelp->AddtoSurvivingPixelPosition(spx, spy);
    }
    
    // add info for all the events
    eventInfo->SetHPanel4(hcam_panel4);
    eventInfo->SetL(EllipicRatio[0]);
    eventInfo->SetW(EllipicRatio[1]);
    eventInfo->SetWLRatio(EllipicRatio[2]);
    eventInfo->SetAngle(anglerad);
    eventInfo->Setr1(r1);
    eventInfo->Setr2(r2);
    eventInfo->SetSize(cev->GetSurvivingPixelTotalAmpPanel3());
    eventInfo->SetSurvivngPixels(cev->GetSurvivingPixelPanel3().size());
    eventInfo->SetConc(conc);
    eventInfo->SetCOGx(meanx);
    eventInfo->SetCOGy(meany);
    eventInfo->SetTriggeredPixelID(cev->GetMaxAmplitudePixelID());
    eventInfo->SetCoreRatio(cev->GetCoreRatio());
    eventInfo->SetNumberOfCores(cev->GetNumberofCorePixels());
    eventInfo->SetArea(areaEllipse);
    eventInfo->SetM3Long(M3Long);
    eventInfo->SetM3LongPow3(M3LongPow3);
    eventInfo->SetUpDownSym(upDownSym);



            
}

// Projects every surviving pixel onto the major axis and returns the signed distance d_i from the
// charge-weighted centroid together with the charge q_i. Pixels with q <= 0 are dropped: in the
// q^1 sum a negative charge flips the weighting, and in the q^3 sum it survives the cube with the
// wrong sign. Returns false when there is nothing usable to form a moment from.
//
// The major axis angle comes from atan(), so it lies in (-90,+90) and cos(theta) > 0: the unit
// vector always points toward +x, which is what makes the sign comparable across events. A
// perfectly vertical axis leaves the direction riding on the arbitrary sign of the eigenvector,
// so it is refused rather than reported as a coin flip.
bool projectOnMajorAxis(double xcog, double ycog, double anglerad, std::vector<int> sur_pix,
                        std::vector<float> amps, std::vector<double>& d, std::vector<double>& q){
    d.clear();
    q.clear();
    double ct = cos(anglerad);
    double st = sin(anglerad);
    if (std::fabs(ct) < 1e-9) {
        return false;
    }
    for (std::vector<int>::size_type p = 0; p < sur_pix.size(); p++) {
        int nx, ny;
        plottools->FindBin(sur_pix[p], &nx, &ny);
        double charge = amps[sur_pix[p]];
        if (charge <= 0) continue;
        d.push_back((nx*1.0 - xcog)*ct + (ny*1.0 - ycog)*st);
        q.push_back(charge);
    }
    return !d.empty();
}

// Standard Hillas longitudinal third moment, weighted by q^1 and normalised by the total charge:
//     M3Long = cbrt( sum(q_i * d_i^3) / sum(q_i) )
//
// The sign gives the direction of the SKEW, not of the bright end. (xcog,ycog) is the
// charge-weighted centroid, so piling charge at one end drags the centroid there and leaves a long
// faint tail at the other; the cubed distance makes that tail dominate the sum. So M3Long > 0
// means the tail runs toward +x and the brighter, denser end sits toward -x.
// Verified against a hand calculation and synthetic blobs in Claude/test_m3long.cpp.
//
// Note on normalisation: the thesis this analysis follows normalises by the pixel count N rather
// than by sum(q). Both denominators are positive and cbrt preserves sign, so that choice cannot
// change the sign and therefore cannot change any head-tail decision. It only rescales the value.
//
// cbrt is used rather than pow(x,1/3.), which returns NaN for a negative base. The sign is the
// entire point of the quantity.
double getM3Long(double xcog, double ycog, double anglerad, std::vector<int> sur_pix, std::vector<float> amps){
    std::vector<double> d, q;
    if (!projectOnMajorAxis(xcog, ycog, anglerad, sur_pix, amps, d, q)) {
        return 0.0;
    }
    double num = 0.0, qsum = 0.0;
    for (std::vector<double>::size_type i = 0; i < d.size(); i++) {
        num  += q[i] * d[i] * d[i] * d[i];
        qsum += q[i];
    }
    if (qsum <= 0) {
        return 0.0; // no charge, no moment
    }
    return std::cbrt(num / qsum);
}

// M3LongPow3: the thesis variant that weights the third moment by the THIRD POWER of the charge
// and normalises by the pixel count N:
//     M3LongPow3 = cbrt( (1/N) * sum(q_i^3 * d_i^3) )
//
// Its purpose is low-energy images, which go roundish and lose their head-tail asymmetry in the
// noise; giving bright pixels far more weight is meant to recover it.
//
// IMPORTANT: this is NOT a rescaled M3Long and its sign is not the same. Cubing the charge lets
// the bright compact head dominate the sum instead of the faint extended tail, so in practice it
// tends to come out with the OPPOSITE sign to M3Long on the same image. Decide which variable a
// head-tail cut is written against and stay with it; do not mix the two.
double getM3LongPow3(double xcog, double ycog, double anglerad, std::vector<int> sur_pix, std::vector<float> amps){
    std::vector<double> d, q;
    if (!projectOnMajorAxis(xcog, ycog, anglerad, sur_pix, amps, d, q)) {
        return 0.0;
    }
    double num = 0.0;
    for (std::vector<double>::size_type i = 0; i < d.size(); i++) {
        num += q[i] * q[i] * q[i] * d[i] * d[i] * d[i];
    }
    // N is the number of pixels actually summed, i.e. surviving pixels carrying charge.
    return std::cbrt(num / (double)d.size());
}