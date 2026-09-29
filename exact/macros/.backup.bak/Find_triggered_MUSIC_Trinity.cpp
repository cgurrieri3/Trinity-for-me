R__LOAD_LIBRARY(libExACT.so)

#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>

#include <typeinfo>

#include <iostream>
#include <TH2.h>
#include <TPaveStats.h>

#include <cmath>
#include <bitset>


int EventCounter = 0;
TLatex *text = 0;
TFile *f0 = 0;
TTree *tree = 0;
Event *ev;
long triggerTime;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;



void LoadEvents(std::string filename, std::string treeString);
void SetBranches(Event *evD);
void FindMusic(std::string filename);


void Find_triggered_MUSIC_Trinity(std::string filename,std::string treeString){
	LoadEvents(filename, treeString);
	// open file
	//file = new IFile(filename);
        ev = new Event();
	SetBranches(ev);
	FindMusic(filename);
	delete ev;
}

void LoadEvents(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    f0 = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)f0->Get(treeString.c_str());
}

void FindMusic(std::string filename){

	TString mainOutputDirectory = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/May2024/MUSIC_mapping/";

        int nEntries = tree->GetEntries();
        std::cout << "Total Number of Events: " << nEntries << std::endl;

    // Extract the last directory from the input filename
    const char* cFilename = filename.c_str();
    // Manually remove the ".root" extension from the root file name
    const char* cRootFilename = gSystem->BaseName(cFilename);
    TString rootFileName = cRootFilename;
    size_t extensionPos = rootFileName.Index(".root");
    if (extensionPos != kNPOS) {
        rootFileName.Remove(extensionPos);
    }


	
        
	//std::string MUSIC_filename = mainOutputDirectory + rootFileName +"_TrigMusic.csv";
        std::string MUSIC_filename = Form("%s%s_TrigMusic.csv", mainOutputDirectory.Data(), rootFileName.Data());
	std::cout << MUSIC_filename << std::endl;
        std::ofstream MUSIC_file(MUSIC_filename, std::ios::trunc);
        MUSIC_file << "EventNumber" <<"  "<< "TrigMusic" << std::endl;
        // Close the file
        MUSIC_file.close();
     

        while (EventCounter < nEntries) {
    
        
	tree->GetEntry(EventCounter);
	std::cout << "Event num: " << EventCounter <<" from "<<nEntries<< std::endl;


	std::vector<Int_t> TrigMus;


  
        TrigMus = ev->GetROIMusicID();
        
        std::ofstream MUSIC_file;
        MUSIC_file.open(MUSIC_filename, std::ios::app);
    
        // Check if the file is opened successfully
        if (!MUSIC_file) {
            std::cerr << "Failed to open the file." << std::endl;
        }
        // Write the value to the file
        MUSIC_file << EventCounter <<"  "<< TrigMus[0] << std::endl;
    
        // Close the file
        MUSIC_file.close();
        
        

        std::cout << "Event: " << EventCounter<< ", TrigMus: " << TrigMus[0]<< std::endl;

        EventCounter++;  

        }
             
    }

 
// this is for choosing branches in the root file
void SetBranches(Event *evD)
{
    tree->SetBranchAddress("Events", &ev);
}
