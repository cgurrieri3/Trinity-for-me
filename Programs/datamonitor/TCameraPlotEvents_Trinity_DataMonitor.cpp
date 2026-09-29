//R__LOAD_LIBRARY(libExACT.so) 
/**
 * When compiling the libraries are linked at compilation time. R_LOAD_LIBRARY is a ROOT macro directive, it makes no sense to the compiler
 * */
#include<TH1.h>
#include<TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>

/**
 * Since you will be compiling it, you need to specify all
 * classes to the compiler.
 * It won't load them by default, like when you launch ROOT 
 * in the interactive shell.
 * */
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TLatex.h>
#include <TFile.h>
#include <TBox.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>

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
void PlotEvent(std::string filename, std::string outpath, int eventIndex);
void ShowInfoAtCursor(int x, int y);

void FindBin(int Pixel_ID,int *row, int *col)
{
    // Calculate the SIAB number (0 to 15)
    int SIAB_Number = Pixel_ID / 16;

    // Calculate the pixel number within the SIAB (0 to 15)
    int SIAB_Pixel_Number = Pixel_ID % 16;

    // Calculate the row and column within the SIAB (0 to 3 for both)
    int SIAB_Pixel_Row = SIAB_Pixel_Number / 4;
    int SIAB_Pixel_Col = SIAB_Pixel_Number % 4;

    // Calculate the overall row and column
    *row = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
    *col = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
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

int FindPixel(int row, int col)
{
    // Calculate the SIAB number (0 to 15)
    int SIAB_Row = row / 4;
    int SIAB_Col = col / 4;
    int SIAB_Number = SIAB_Row * 4 + SIAB_Col;

    // Calculate the pixel number within the SIAB (0 to 15)
    int SIAB_Pixel_Row = row % 4;
    int SIAB_Pixel_Col = col % 4;
    int SIAB_Pixel_Number = SIAB_Pixel_Row * 4 + SIAB_Pixel_Col;

    // Calculate the pixel number (0 to 255)
    int Pixel_Number = SIAB_Number * 16 + SIAB_Pixel_Number;
    return Pixel_Number;
}


void TCameraPlotEvents_Trinity_DataMonitor(std::string filename, std::string outpath,std::string treeString)
{

    LoadEvents(filename, treeString);
	int nEntries = tree->GetEntries();

    // Generate a vector of indices from 0 to nEntries-1
    std::vector<int> indices(nEntries);
    for (int i = 0; i < nEntries; ++i) {
        indices[i] = i;
    }

    // Shuffle the indices randomly
    std::random_shuffle(indices.begin(), indices.end());

    // Limit to the first 5 indices (or fewer if nEntries < 5)
    int numEventsToPlot = std::min(5, nEntries);

    for (int i = 0; i < numEventsToPlot; ++i)
    {
        EventCounter = indices[i]; // Set EventCounter to the randomly selected index

        ev = new Event();
        SetBranches(ev, &triggerTime, bf);
        PlotEvent(filename, outpath, EventCounter);
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

void PixelClicked()
{
    int event = gPad->GetEvent();
    TObject *o = gPad->GetSelected();
    if (!o) return;
    if (!(o->InheritsFrom("TH2")))
       return;
    TH2 *h = (TH2F*)o;
    int px = gPad->GetEventX();
    int py = gPad->GetEventY();
    Float_t xx = gPad->AbsPixeltoX(px);
    Float_t yy = gPad->AbsPixeltoY(py);
    Float_t x = 0.5+gPad->PadtoX(xx);
    Float_t y = 0.5+gPad->PadtoY(yy);
    int pix = FindPixel((int)y,(int)x);
    if(pix!=iLastPixHLED)
    {
        int iLastPix = -1;
        ShowInfoAtCursor((int) x, (int) y);
    }
   if (event == 11)
    {
        PlotTrace(pix);
    }
}

void ShowInfoAtCursor(int x, int y)
{
    int MUSIC_column = x/4;
    int MUSIC_row = y/2;
    int MUSIC_ID =  MUSIC_column*2+MUSIC_row%2+((MUSIC_row/2)%4)*8;
    int MUSIC_Channel = x%4+4*(y%2);
    int PixID = ((MUSIC_row/2)%4)*8*8 +((MUSIC_row)%2)*8+MUSIC_column*16+MUSIC_Channel;

    TString statusline;
    statusline.Form("Event #: %i  MUSIC_ID: %i    MUSIC_Channel: %i    Pixel ID: %i",EventCounter, MUSIC_ID, MUSIC_Channel, PixID);
    if(text!=0)
        text->Delete();
    TLatex T1;
    text = T1.DrawLatexNDC(0.25,0.95,statusline.Data());
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

void PlotEvent(std::string filename, std::string outpath, int eventIndex)
{
    
    gROOT->SetBatch(kTRUE);
    
    TString mainOutputDirectory = outpath;
    
    
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
    
    
    std::cout << "Filename: " << filename << std::endl;    
    // Extract the last directory (with the date) from the input filename
    std::cout << "Last directory: " << lastDirectory.Data() << std::endl;
    // Get the base name of the .root file without the extension
    std::cout << "Root directory: " << rootFileName.Data() << std::endl;

    

    // Construct the subdirectory path with the root file name
    TString outputDirectory = Form("%s/%s",mainOutputDirectory.Data(), rootFileName.Data());


        
  




    // Create the output directory if it doesn't exist
    gSystem->mkdir(outputDirectory, kTRUE);

    TH2F *hcam = new TH2F("hcam", "", 16, -0.5, 15.5, 16, -0.5, 15.5);
    hcam->SetStats(0);
    hcam->Draw("colz");
    DrawMUSICBoundaries();

    /**
     * A Timer has to be called in order to freeze the plotted object
     * This allows the display to exist without exiting
     * There is a way to keep updating it while it is frozen on the
     * TTimer.
     * 
     * You will have to develop this in order for continuous updating
     * of the plot. 
     * 
     * In this program the above plot is the only one that should work.
     * 
     * The rest of them will flash and disappear.
     * 
     * */
    //HandleInput();
    int nEntries = tree->GetEntries();
    std::cout << "Total Number of Events: " << nEntries << std::endl;

    
    EventCounter = eventIndex;
    
        tree->GetEntry(EventCounter);
        //std::cout << "Event# " << EventCounter << " is displayed." << std::endl;
        hcam->Reset();
        
        TString pixelPlotDir = Form("%s/Event_%s", outputDirectory.Data(),  FormatNumber(EventCounter, nEntries).c_str());
        gSystem->mkdir(pixelPlotDir, kTRUE);

        std::vector<int> selectedChannels;
        for (int i = 0; i < 5; ++i) // Assuming 5 channels to be randomly selected
        {
            int randomChannel = gRandom->Integer(MaxNofChannels);
            selectedChannels.push_back(randomChannel);
        }
        
        TCanvas *pixelCanvas = new TCanvas("PixelCanvas", "Pixel Plot", 600, 600);
        
        for (int j = 0; j < MaxNofChannels; j++) 
	{
            ExtractedData *extractedDataHLED = new ExtractedData();
            Pulse *pulse = new Pulse(ev->GetSignalValue(j));
            extractedDataHLED->SetAmplitude(pulse->GetAmplitude());

            int nx, ny;
            FindBin(j, &nx, &ny);
            hcam->SetBinContent(ny + 1, nx + 1, extractedDataHLED->GetAmplitude());
            


            hcam->Draw("colz");
            gPad->Modified();
            gPad->Update();
            
        
        

      delete extractedDataHLED;
      delete pulse;

    }
    pixelCanvas->SaveAs(Form("%s/%s_Event_%s.png", outputDirectory.Data(), rootFileName.Data() ,  FormatNumber(EventCounter, nEntries).c_str()));
//    pixelCanvas->SaveAs("Test.png");
     for (int j : selectedChannels)        
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
            
            traceCanvas->SaveAs(Form("%s/Event_%s/%s_Event_%s_Pixel_%s.png", outputDirectory.Data(),  FormatNumber(EventCounter, nEntries).c_str(), rootFileName.Data() ,  FormatNumber(EventCounter, nEntries).c_str() , FormatNumber(j, MaxNofChannels).c_str()));

      



    }
    
            //std::cout << "Output directory: " << outputDirectory.Data() << std::endl;
}

void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF)
{
    tree->SetBranchAddress("Events", &ev);
}

/**
 * All C++ programs need a main method.
 * Without it, the compiler does not know where the program starts.
 * The way I will structure this is of very bad programming style, but
 * given the code already works, it will have to be this way.
 * 
 * Basically, main should not just call some other method where everything
 * gets executed.
 * */

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Too few arguments" << endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string treeName = argv[3];
    std::string outPath = argv[2];

    cout << "filename: " << filename << endl;
    cout << "treeName: " << treeName << endl;
    cout << "outPath: " << outPath << endl;

    //TApplication *app = new TApplication("App", &argc, argv);
    //TROOT root("DisplayEvts", "Display Events");

    TCameraPlotEvents_Trinity_DataMonitor(filename, outPath, treeName);

    //app->Run();

    return 0;
}
