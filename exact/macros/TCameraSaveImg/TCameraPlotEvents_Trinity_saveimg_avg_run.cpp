R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>

#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>

int iLastPixHLED = -1;
int EventCounter = 0;
TLatex *text = 0;
TTree *tree = 0;
Event *ev;
long triggerTime;

TCanvas *c_disp = 0;
TH1I *hPixelTraceLED = 0;
BiFocal *bf = new BiFocal();

int MaxNofChannels = 256;
int LengthofPedestal = 200;
int SignalStart = 230;
int SignalWidth = 20;

void LoadEvents(std::string filename, std::string treeString);
void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF);
void PlotAverageAmplitude(std::string filename, std::string orientation);


//Calculates the 2D bin coordinates associated with a 1D vector/number, i.e. pixel number, based on Trinity camera layout
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





void TCameraPlotEvents_Trinity_saveimg_avg_run(std::string filename, std::string orientation,std::string treeString)
{

    
	c_disp = new TCanvas("Display","CameraPlot",950,1000);
	c_disp->Divide(2,2);
	LoadEvents(filename, treeString);

	
	int nEntries = tree->GetEntries();


        
        PlotAverageAmplitude(filename,orientation);
	



}

void LoadEvents(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
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


// Function to format the pixel number with leading zeros
std::string FormatNumber(int num, int maxNum) {
    // Calculate the number of digits needed to represent maxNum
    int maxDigits = (int)log10(maxNum) + 1;

    // Create a string with the formatted pixel number
    std::ostringstream ss;
    ss << std::setw(maxDigits) << std::setfill('0') << num;
    return ss.str();
}

// Function to extract the last directory from the given filepath
TString ExtractLastDirectory(const TString& filepath) {
    Ssiz_t lastSlash = filepath.Last('/');
    if (lastSlash >= 0) {
        return filepath(lastSlash + 1, filepath.Length() - lastSlash - 2);
    }
    return "";
}

TString GetBaseFilename(const TString& filename) {
    // Get the base filename by finding the last path separator
    Ssiz_t lastSeparator = filename.Last('/');
    if (lastSeparator != kNPOS) {
        return filename(lastSeparator + 1, kNPOS);
    }
    return filename; // No path separator found
}

TString RemoveExtension(const TString& filename) {
    // Remove the extension by finding the last '.' character
    Ssiz_t lastDot = filename.Last('.');
    if (lastDot != kNPOS) {
        return filename(0, lastDot);
    }
    return filename; // No extension found
}


void PlotAverageAmplitude(std::string filename, std::string orientation)
{
    gROOT->SetBatch(kTRUE);
    
    TString mainOutputDirectory = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/May2024/PixelMapping";
    
    // Extract the last directory from the input filename
    const char* cFilename = filename.c_str();
    const char* cLastDirectory = gSystem->DirName(cFilename);
    TString lastDirectory = cLastDirectory;
    lastDirectory = gSystem->BaseName(lastDirectory.Data());

    // Manually remove the ".root" extension from the root file name
    const char* cRootFilename = gSystem->BaseName(cFilename);
    TString rootFileName = cRootFilename;
    size_t extensionPos = rootFileName.Index(".root");
    if (extensionPos != kNPOS) {
        rootFileName.Remove(extensionPos);
    }
    

// Extract the second-to-last directory
    TString secondToLastDirectory;

    // Split the path into components
    std::istringstream iss(filename.c_str());
    std::string directory;

    // Store components in a vector
    std::vector<std::string> directories;
    while (std::getline(iss, directory, '/')) {
        directories.push_back(directory);
    }

    // Check if there are at least two directories
    if (directories.size() >= 2) {
        // Extract the second-to-last directory
        secondToLastDirectory = directories[directories.size() - 3].c_str(); // Use -3 instead of -2

        std::cout << "Filename: " << filename.c_str() << std::endl;
        std::cout << "Second to last directory: " << secondToLastDirectory.Data() << std::endl;
    } else {
        std::cout << "Error: Not enough directories in the path." << std::endl;
        //return 1; // Add this to exit the program if there's an error
    }

    TString outputDirectory = Form("%s/%s/%s", mainOutputDirectory.Data(), secondToLastDirectory.Data(), orientation.c_str());
    gSystem->mkdir(outputDirectory, kTRUE);
    
    // Initialize the histogram for average amplitudes
    TH2F *hcam_avg = new TH2F("hcam_avg", "Average Amplitude Distribution", 16, -0.5, 15.5, 16, -0.5, 15.5);
    hcam_avg->SetStats(0);
    
    int nEntries = tree->GetEntries();
    std::cout << "Total Number of Events: " << nEntries << std::endl;

    // Initialize a histogram to count the number of contributions per pixel
    TH2F *hcam_count = new TH2F("hcam_count", "", 16, -0.5, 15.5, 16, -0.5, 15.5);
    
    while (EventCounter < nEntries)
    {

        ev = new Event();
	SetBranches(ev,&triggerTime, bf);

        tree->GetEntry(EventCounter);

        for (int j = 0; j < MaxNofChannels; j++)
        {
            ExtractedData *extractedDataHLED = new ExtractedData();
            Pulse *pulse = new Pulse(ev->GetSignalValue(j));
            extractedDataHLED->SetAmplitude(pulse->GetAmplitude());

            int nx, ny;
            FindBin(j, &nx, &ny);
            
            // Accumulate amplitude and count per pixel
            hcam_avg->AddBinContent(hcam_avg->GetBin(nx + 1, ny + 1), extractedDataHLED->GetAmplitude());
            hcam_count->AddBinContent(hcam_count->GetBin(nx + 1, ny + 1), 1);
            
            delete extractedDataHLED;
            delete pulse;
        }
        EventCounter++;
    }

    // Calculate the average amplitude per pixel
    hcam_avg->Divide(hcam_count);
    
    TCanvas *avgCanvas = new TCanvas("AverageCanvas", "Average Amplitude Plot", 600, 600);
    hcam_avg->Draw("colz");
    DrawMUSICBoundaries();
    avgCanvas->SaveAs(Form("%s/%s_AverageAmplitude.png", outputDirectory.Data(), rootFileName.Data()));
    
    std::cout << "Output directory: " << outputDirectory.Data() << std::endl;

    delete avgCanvas;
    delete hcam_avg;
    delete hcam_count;
}


void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF)
{
    tree->SetBranchAddress("Events", &ev);
}

