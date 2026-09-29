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
void PlotEvent(std::string filename, std::string treeString);


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



Bool_t HandleInput()
{
	TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
	while (1)
	{
        	timer.TurnOn();
        	TString input = Getline("Type 'q' to exit, <return> to go on:");
        	timer.TurnOff();
        	if (input=="q\n") return kFALSE;
        	if (input=="\n") return kTRUE;
	}
	return kFALSE;
}


void TCameraPlotEvents_Trinity_saveimg(std::string filename,std::string treeString)
{

        int pixelId = 163; // Replace with your desired Pixel ID
        int pixelRow, pixelCol;
        FindBin(pixelId, &pixelRow, &pixelCol);
        cout << "Row: " << pixelRow << ", Col: " << pixelCol << " for Pixel ID: " << pixelId << endl;
    
	c_disp = new TCanvas("Display","CameraPlot",950,1000);
	c_disp->Divide(2,2);
	LoadEvents(filename, treeString);

	
	int nEntries = tree->GetEntries();


        while (EventCounter < nEntries)
        {
            ev = new Event();
	    SetBranches(ev,&triggerTime, bf);
	    PlotEvent(filename, treeString);
           EventCounter++;
        } 
	



}

void LoadEvents(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void PlotTrace(int iPix)
{
    if(hPixelTraceLED == 0)
    {
        hPixelTraceLED = new TH1I("hPixelTraceLED","Pixel Trace",500,-0.5,499.5);
        hPixelTraceLED->SetStats(0);
        hPixelTraceLED->GetXaxis()->SetTitle("ADC sample");
        hPixelTraceLED->GetYaxis()->SetTitle("ADC counts");
    }

    hPixelTraceLED->Reset();
    TString title;
    title.Form("Trace of Pixel %i", iPix);
    hPixelTraceLED->SetTitle(title);
    std::vector<Int_t> trace = std::vector<Int_t>(MaxNofChannels);

    trace  = ev->GetSignalValue(iPix);
    for(int k = 0; k<512; k++){
        hPixelTraceLED->SetBinContent(k+1,trace[k]);
    }

    c_disp->cd(3);
    hPixelTraceLED->Draw();

    gPad->Modified();
    gPad->Update();
}



void DrawMUSICBoundaries()
{
    TBox *b = new TBox(-.5,-0.5,3.5,1.5);
    b->SetFillStyle(0);
    b->SetLineColor(kRed);
    b->Draw();

    for(int i=0; i<MaxNofChannels/8; i++)
    {
        TBox *bn = (TBox*)b->Clone();
        bn->SetY1((i%8)*2-0.5);
        bn->SetY2((i%8)*2+1.5);
        bn->SetX1((i/8)*4-0.5);
        bn->SetX2((i/8)*4+3.5);
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

void PlotEvent(std::string filename, std::string treeString)
{
    gROOT->SetBatch(kTRUE);
    
    TString mainOutputDirectory = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/exact_TCameraPlotEvents_output";
    
    
  // Extract the last directory from the input filename
    /*const char* cFilename = filename.c_str();
    const char* cLastDirectory = gSystem->DirName(cFilename);
    TString lastDirectory = cLastDirectory;
    lastDirectory = gSystem->BaseName(lastDirectory.Data());*/

    // Extract the last directory from the input filename
    const char* cFilename = filename.c_str();
    const char* cLastDirectory = gSystem->DirName(cFilename);
    TString lastDirectory = cLastDirectory;

    // Extract the last directory
    lastDirectory = gSystem->BaseName(lastDirectory.Data());

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

    // Manually remove the ".root" extension from the root file name
    const char* cRootFilename = gSystem->BaseName(cFilename);
    TString rootFileName = cRootFilename;
    size_t extensionPos = rootFileName.Index(".root");
    if (extensionPos != kNPOS) {
        rootFileName.Remove(extensionPos);
    }
    
    
    std::cout << "Filename: " << filename << std::endl;    
    // Extract the last directory (with the date) from the input filename
  //  TString lastDirectory = ExtractLastDirectory(filename);
    std::cout << "Second to last directory: " << secondToLastDirectory.Data() << std::endl;
    // Get the base name of the .root file without the extension
   // TString rootFileName = RemoveExtension(GetBaseFilename(filename));
    std::cout << "Root directory: " << rootFileName.Data() << std::endl;

    

    // Construct the subdirectory path with the root file name
    //TString outputDirectory = Form("%s/%s/%s", mainOutputDirectory.Data(), lastDirectory.Data(), rootFileName.Data());
    TString outputDirectory = Form("%s/%s/%s", mainOutputDirectory.Data(), secondToLastDirectory.Data(), treeString.c_str());

   // Create the subdirectory if it doesn't exist
    gSystem->mkdir(outputDirectory, kTRUE);
        
  




    // Create the output directory if it doesn't exist
    gSystem->mkdir(outputDirectory, kTRUE);

    c_disp->cd(1);
    TH2F *hcam = new TH2F("hcam", "", 16, -0.5, 15.5, 16, -0.5, 15.5);
    hcam->SetStats(0);
    hcam->Draw("colz");
    DrawMUSICBoundaries();
    int nEntries = tree->GetEntries();
    std::cout << "Total Number of Events: " << nEntries << std::endl;

    while (EventCounter < nEntries)
    {
        tree->GetEntry(EventCounter);
        cout << "Event# " << EventCounter << " is displayed." << endl;
        hcam->Reset();
        /*TString pixelPlotDir = Form("%s/Event_%s", outputDirectory.Data(),  FormatNumber(EventCounter, nEntries).c_str());
        gSystem->mkdir(pixelPlotDir, kTRUE);*/
        
        TCanvas *pixelCanvas = new TCanvas("PixelCanvas", "Pixel Plot", 600, 600);
        for (int j = 0; j < MaxNofChannels; j++)
        {
            ExtractedData *extractedDataHLED = new ExtractedData();
            Pulse *pulse = new Pulse(ev->GetSignalValue(j));
            extractedDataHLED->SetAmplitude(pulse->GetAmplitude());

            int nx, ny;
            FindBin(j, &nx, &ny);
            hcam->SetBinContent(nx + 1, ny + 1, extractedDataHLED->GetAmplitude());

            


            hcam->Draw("colz");
            gPad->Modified();
            gPad->Update();
            
            



           /*if(hPixelTraceLED == 0)
            {
                hPixelTraceLED = new TH1I("hPixelTraceLED","Pixel Trace",500,-0.5,499.5);
                hPixelTraceLED->SetStats(0);
                hPixelTraceLED->GetXaxis()->SetTitle("ADC sample");
                hPixelTraceLED->GetYaxis()->SetTitle("ADC counts");
            }

            hPixelTraceLED->Reset();
            TString title;
            title.Form("Trace of Pixel %i", j);
            hPixelTraceLED->SetTitle(title);
            std::vector<Int_t> trace = std::vector<Int_t>(MaxNofChannels);

            trace  = ev->GetSignalValue(j);
            for(int k = 0; k<512; k++){
                hPixelTraceLED->SetBinContent(k+1,trace[k]);
            }

            TCanvas *traceCanvas = new TCanvas("TraceCanvas", "Trace Plot", 600, 600);
            hPixelTraceLED->Draw();

            gPad->Modified();
            gPad->Update();
            
            traceCanvas->SaveAs(Form("%s/Event_%s/%s_Event_%s_Pixel_%s.png", outputDirectory.Data(),  FormatNumber(EventCounter, nEntries).c_str(), rootFileName.Data() ,  FormatNumber(EventCounter, nEntries).c_str() , FormatNumber(j, MaxNofChannels).c_str()));*/

    
    


            delete extractedDataHLED;
            delete pulse;
        }
        pixelCanvas->SaveAs(Form("%s/%s_Event_%s.png", outputDirectory.Data(), rootFileName.Data() ,  FormatNumber(EventCounter, nEntries).c_str()));
        EventCounter++;

        //if (!HandleInput())
          //  break;
    }
    
            std::cout << "Output directory: " << outputDirectory.Data() << std::endl;
}

void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF)
{
    tree->SetBranchAddress("Events", &ev);
}

