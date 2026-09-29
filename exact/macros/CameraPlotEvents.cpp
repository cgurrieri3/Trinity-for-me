/***********************************************************************************************************************************************************************
************************************************************************************************************************************************************************
This macro is an event display for CT data. When running the macro in root one can scan through and display each event in a choosen file.
Top left plot displays the amplitude of light in ADC counts as seen on the camera for each pixel.
Top right plot is a histogram of amplitude distribution over all pixels in the current diplayed event.
Bottom left plot displays traces in the clicked pixel on the top left plot. (traces are the signal detected during 5.12us in a selected pixel for a selected event)
This macro  uses exact classes and must be run within the exact program. 
To run the display:
	0. make sure to have installed and updated exact, and go to the directory for this script.
	1. open root in terminal: root
	2. enter the following command: .x CameraPlotEvents.cpp("data_file.root", "tree_name")
	3. tree_name can be either BiFocal for events that triggered on the bifocal condition, or HLED for scans with the Health LED
	4. when the command is executed a display with 3 plots will open and in the terminal one can scan through events with in the file by clicking enter
	5. to exit display instead of enter type q and then .q to exit root. 
************************************************************************************************************************************************************************
************************************************************************************************************************************************************************/

R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <chrono>

int iLastPixHLED = -1;
int EventCounter = 0;
TLatex *text = 0;
TTree *tree = 0;
IEvent *ev;
BiFocal *bf = new BiFocal();
ISiPM *sipmInfo;
TCanvas *c_disp = 0;
TH1F *hPixelTraceLED = 0;
int NPixs = 256;
long triggerTime;
IPlotTools *plottools;


void LoadEvents(std::string filename, std::string treeString); // access the file with events
void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF);
void PlotEvent();
void ShowInfoAtCursor(int x, int y);

// main function to open display, set up a pointer for data acess,  and run all functions below
void CameraPlotEvents(std::string filename,std::string treeString){
	c_disp = new TCanvas("Display","CameraPlot",950,1000);
	c_disp->Divide(2,2);

	LoadEvents(filename, treeString);
    ev = new IEvent(); // pointer for accessing events from file
    sipmInfo = new ISiPM();
	SetBranches(ev,&triggerTime, bf);
	PlotEvent();

}

Bool_t HandleInput()
{// allows for looping through events by clicking enter on terminal
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

void LoadEvents(string NameofFile, std::string treeString)
{ // loads data/events from file
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}
void PlotTrace(int iPix)
{// function for ploting individual traces
    if(hPixelTraceLED == 0) // histogram set-up
    {
        hPixelTraceLED = new TH1F("hPixelTraceLED","Pixel Trace",500,-0.5,499.5);
        hPixelTraceLED->SetStats(0);
        hPixelTraceLED->GetXaxis()->SetTitle("ADC sample");
        hPixelTraceLED->GetYaxis()->SetTitle("PEs");
    }

    hPixelTraceLED->Reset();
    TString title;
    title.Form("Trace of Pixel %i", iPix);
    hPixelTraceLED->SetTitle(title);
    
    std::vector<Int_t> trace = std::vector<Int_t>(NPixs);
    
    // converstion variables for ADC to PEs
    std::vector<double> amplToPE= sipmInfo->GetAmplToPE();
    std::vector<double> tempCorrection= sipmInfo->GetTCorrection();
    std::vector<double> absGain= sipmInfo->GetGain();
    float amplToPE_forPix = amplToPE[iPix];
    float absGain_forPix = absGain[iPix];
    //float tempCorrection_forPix = tempCorrection[iPix];
    
    

    trace  = ev->GetSignalValue(iPix); // getting data for traces
    for(int k = 0; k<512; k++){
    	// converting to PEs

        float peAmplitude = (float(trace[k]))*(1.0/absGain_forPix);

       hPixelTraceLED->SetBinContent(k+1,peAmplitude);
        
 
        //hPixelTraceLED->SetBinContent(k+1,trace[k]);
    }
    
    // displaying the histogram    
    c_disp->cd(3);
    hPixelTraceLED->Draw();
    gPad->Modified();
    gPad->Update();
}

void PixelClicked()
{// allows clicking within the display to show traces of clicked pixels
    int event = gPad->GetEvent();
    TObject *o = gPad->GetSelected();
    if (!o) return;
    if (!(o->InheritsFrom("TH2")))
       return;
    TH2F *h = (TH2F*)o;
    int px = gPad->GetEventX();
    int py = gPad->GetEventY();
    Float_t xx = gPad->AbsPixeltoX(px);
    Float_t yy = gPad->AbsPixeltoY(py);
    Float_t x = 0.5+gPad->PadtoX(xx);
    Float_t y = 0.5+gPad->PadtoY(yy);
    int pix = plottools->FindPixel((int)x,(int)y);
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

    TString statusline;
    statusline.Form("Pixel: %i, MUSIC: %i, SIAB: %i", plottools->FindPixel(x,y), plottools->FindMUSIC(x,y), plottools->FindSIAB(x,y));
    if(text!=0)
        text->Delete();
    TLatex T1;
    text = T1.DrawLatexNDC(0.25,0.95,statusline.Data());
    gPad->Modified();
    gPad->Update();
}

void PlotEvent()
{// finction that makes the top plots for amplitude display

	// initializing histograms
    c_disp->cd(1);
    gPad->AddExec("ev","PixelClicked()");
    TH2F *hcam = new TH2F("hcam","",16,-0.5,15.5,16,-0.5,15.5);
    hcam->SetStats(0);
    hcam->Draw("colz");
    plottools->DrawMUSICBoundaries();
    
    TH1F *hChg = new TH1F("hChg","Charge Distribution",4096, 0, 4096);
    hChg->SetStats(0);
    hChg->GetXaxis()->SetTitle("Charge [ADC]");
    hChg->GetYaxis()->SetTitle("No. of Pixels");

	  // retrieving entries from data file to see the number of events in the file
    int nEntries = tree->GetEntries();
    std::cout << "Total Number of Events: " << nEntries << std::endl;

    tree->SetBranchAddress("SiPM", &sipmInfo); // access tree to get callibration values

	// loop for all events
     while(1)
    {   
        tree->GetEntry(EventCounter); // getting data from file for current event
        cout << "Event# " << EventCounter <<" is displayed." << endl;

		// reseting histograms
        hcam->Reset();
        hChg->Reset();
        c_disp->cd(4);
        gPad->Clear();

		// converstion variables for ADC to PEs
        std::vector<double> amplToPE= sipmInfo->GetAmplToPE();
        std::vector<double> absGain= sipmInfo->GetGain();
    	  std::vector<double> tempCorrection= sipmInfo->GetTCorrection();
        std::vector<float> sipmTemp = ev->GetSiPMTemp();

        // setting up for data extraction for plotting
        Int_t nEventsHLED = 0;
        ExtractedData *extractedData[NPixs];
        for (int i = 0; i<NPixs; i++){
            extractedData[i] = 0;
            extractedData[i] = new ExtractedData();
         }
         
		// looping through each pixel to get data
        Pulse *pulse;
        double maxCharge = 0.0; // Variable to track the maximum charge
        for(int j = 0; j<NPixs; j++){
                pulse = new Pulse(ev->GetSignalValue(j)); //retrieving amplitude per pixel

                extractedData[j]->SetAmplitude(pulse->GetAmplitude()); // setting that amplitude to variable setup above
				// convert and callibrate to PEs
                //float peAmplitude = (extractedData[j]->GetAmplitude())/((1.0/tempCorrection[j])*amplToPE[j]);
                float peAmplitude = (extractedData[j]->GetAmplitude())*(1.0/absGain[j]);
                
                //cout<<j<<"    "<<extractedData[j]->GetAmplitude()<<"    "<<sipmTemp[j/16]<<"    "<<tempCorrection[j]<<"    "<<amplToPE[j]<<"    "<<absGain[j]<<"    "<<peAmplitude<<endl;

				// functions to figure out location for plotting
                int nx, ny;
                plottools->FindBin(j,&nx,&ny);
                // filling histograms
                //hcam->SetBinContent(nx+1,ny+1,extractedData[j]->GetAmplitude());
                //hChg->Fill(extractedData[j]->GetAmplitude());
				        hcam->SetBinContent(nx+1,ny+1,peAmplitude);
                hChg->Fill(peAmplitude);

                // Update maxCharge if the current peAmplitude is larger
                    if (peAmplitude > maxCharge) {
                        maxCharge = peAmplitude;
                    }
             }

    // Set the x-axis range for hChg to the maximum charge value
    hChg->GetXaxis()->SetRangeUser(0, maxCharge * 1.1);
		// retrieving trigger time to display
        std::uint64_t unixTimestampInNanoseconds =  static_cast<std::uint64_t>(ev->GetTBTime());

		std::string utcTimeInNanoseconds = IUtilities::ConvertUnixToUTCfromFile(unixTimestampInNanoseconds);
		    std::cout << "Trigger Time (UTC): " << utcTimeInNanoseconds << endl;

		// displaying histograms
        c_disp->cd(1);
        hcam->SetMinimum(0);
        c_disp->cd(1)->Modified();
        c_disp->cd(1)->Update();

        c_disp->cd(2);
        hChg->Draw();
        c_disp->cd(2)->Modified();
        c_disp->cd(2)->Update();

        // Display Event information
        c_disp->cd(4); // Go to the lower right subplot
        TLatex infoText;
        infoText.DrawLatexNDC(0.12, 0.85, Form("Event#: %d", EventCounter));
        infoText.DrawLatexNDC(0.12, 0.75, Form("Trigger Time (UTC): %s", utcTimeInNanoseconds.c_str()));
        c_disp->cd(4)->Modified();
        c_disp->cd(4)->Update();

		// looping through events
        EventCounter++;
        if(EventCounter>=nEntries){
        	EventCounter = 0;
        }
        if(!HandleInput())
            break;
    }
}

void SetBranches(Event *evD, long *timeTrig, BiFocal *bfF)
{ 
// setting branches to loaction where data should be accessed
    tree->SetBranchAddress("Events", &ev);
}

