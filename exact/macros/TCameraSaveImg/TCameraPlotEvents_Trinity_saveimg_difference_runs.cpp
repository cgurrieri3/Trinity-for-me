R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>

#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <vector>

int iLastPixHLED = -1;
int EventCounter = 0;
TLatex *text = 0;
TTree *tree = 0;
Event *ev;
long triggerTime;

TCanvas *c_disp = 0;
BiFocal *bf = new BiFocal();

int MaxNofChannels = 256;

void LoadEvents(std::string filename, std::string treeString);
void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF);
TH2F* CalculateAverageAmplitude(std::string filename, std::string treeString);
void PlotDifference(std::string filename1, std::string filename2, std::string treeString);

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

void TCameraPlotEvents_Trinity_saveimg_difference_runs(std::string filename1, std::string filename2, std::string treeString) {
    c_disp = new TCanvas("Display", "CameraPlot", 950, 1000);
    c_disp->Divide(2, 2);

    PlotDifference(filename1, filename2, treeString);
}

void LoadEvents(std::string NameofFile, std::string treeString) {
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF) {
    tree->SetBranchAddress("Events", &ev);
}

void DrawMUSICBoundaries() {
    TBox *b = new TBox(-0.5, -0.5, 1.5, 3.5);
    b->SetFillStyle(0);
    b->SetLineColor(kRed);
    b->Draw();
    for (int i = 1; i < MaxNofChannels / 8; i++) {
        TBox *bn = (TBox*)b->Clone();
        bn->SetX1((i % 8) * 2 - 0.5);
        bn->SetX2((i % 8) * 2 + 1.5);
        bn->SetY1((i / 8) * 4 - 0.5);
        bn->SetY2((i / 8) * 4 + 3.5);
        bn->Draw();
    }
}


void SetBlueToRedPalette() {
    const Int_t NRGBs = 3;
    const Int_t NCont = 255;

    // Define the positions of the colors in the gradient
    Double_t stops[NRGBs] = { 0.00, 0.50, 1.00 };
    // Define the red, green, and blue components for each position
    Double_t red[NRGBs]   = { 0.00, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 1.00, 0.00 };
    Double_t blue[NRGBs]  = { 1.00, 1.00, 0.00 };

    // Create the gradient color table
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    // Set the number of contours to use
    gStyle->SetNumberContours(NCont);
}


TH2F* CalculateAverageAmplitude(std::string filename, std::string treeString) {
    LoadEvents(filename, treeString);
    
    int nEntries = tree->GetEntries();
    TH2F *hcam_avg = new TH2F("hcam_avg", "Average Amplitude Distribution", 16, -0.5, 15.5, 16, -0.5, 15.5);
    TH2F *hcam_count = new TH2F("hcam_count", "", 16, -0.5, 15.5, 16, -0.5, 15.5);

    for (int i = 0; i < nEntries; ++i) {
        ev = new Event();
        SetBranches(ev, &triggerTime, bf);
        tree->GetEntry(i);

        for (int j = 0; j < MaxNofChannels; ++j) {
            ExtractedData *extractedDataHLED = new ExtractedData();
            Pulse *pulse = new Pulse(ev->GetSignalValue(j));
            extractedDataHLED->SetAmplitude(pulse->GetAmplitude());

            int nx, ny;
            FindBin(j, &nx, &ny);

            hcam_avg->AddBinContent(hcam_avg->GetBin(nx + 1, ny + 1), extractedDataHLED->GetAmplitude());
            hcam_count->AddBinContent(hcam_count->GetBin(nx + 1, ny + 1), 1);

            delete extractedDataHLED;
            delete pulse;
        }
        delete ev;
    }

    hcam_avg->Divide(hcam_count);
    delete hcam_count;
    return hcam_avg;
}

void PlotDifference(std::string filename1, std::string filename2, std::string treeString) {
    TH2F* hcam_avg1 = CalculateAverageAmplitude(filename1, treeString);
    TH2F* hcam_avg2 = CalculateAverageAmplitude(filename2, treeString);

    TH2F* hcam_diff = (TH2F*)hcam_avg1->Clone("hcam_diff");
    hcam_diff->Add(hcam_avg2, -1);
    hcam_diff->SetTitle("Difference in Average Amplitude Distribution");

    // Suppress the statistics box
    hcam_diff->SetStats(0);

    // Set the custom color palette
    SetBlueToRedPalette();

    TString mainOutputDirectory = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/May2024/FlasherComparison";
    TString outputDirectory = Form("%s/%s", mainOutputDirectory.Data(), "Difference");
    gSystem->mkdir(outputDirectory, kTRUE);
    
    TString baseFilename1 = gSystem->BaseName(filename1.c_str());
    TString baseFilename2 = gSystem->BaseName(filename2.c_str());
    baseFilename1.ReplaceAll(".root", "");
    baseFilename2.ReplaceAll(".root", "");

    TCanvas *diffCanvas = new TCanvas("DifferenceCanvas", "Difference in Average Amplitude Plot", 600, 600);
    hcam_diff->Draw("colz");
    DrawMUSICBoundaries();
    diffCanvas->SaveAs(Form("%s/Difference_%s_vs_%s.png", outputDirectory.Data(), baseFilename1.Data(), baseFilename2.Data()));
    
    delete diffCanvas;
    delete hcam_avg1;
    delete hcam_avg2;
    delete hcam_diff;
}


