#include <Deadtime.h>


int main(int argc, char **argv){
    if(argc < 1){
		cout << "Too few arguments; please include the date YYYYMMDD data directory to summarize" << endl;
		return 1;
	}

    std::string mount = argv[2];
    
    // if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
    //     std::cout << "using mounted directory path" << std::endl;
    //     mnt="/mnt/";
    //     dataDir = "/mnt/DataAnalysis/MergedData/Output/";
    //     outDir = "/mnt/DataAnalysis/flasher_calibration/Output/";
    // }

    if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt="/mnt/";
    } else if (mount != "n"){
        std::cout << "using specific directory path" << std::endl;
        mnt=mount.c_str();
    }

    dataDir = Form("%sDataAnalysis/MergedData/Output/",mnt.c_str());
    // dataDir = Form("%sDataAnalysis/MergedData/Output/lsheram6_MergedFiles/",mnt.c_str());
    outDir = Form("%sDataAnalysis/DeadTime/Output/",mnt.c_str());

    std::string folString = argv[1];

    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());

    

    std::vector<std::string>fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    cout << fileNamesVec[0] << endl;
    
    // Remove elements matching the pattern
    fileNamesVec.erase(
        std::remove_if(
            fileNamesVec.begin(),
            fileNamesVec.end(),
            [](const std::string& name) {
                return name.find("_0001.root") != std::string::npos;
            }
        ),
        fileNamesVec.end()
    );

    std::vector<double> TotalAmplitudeValues(MaxNofChannels, 0.0);
    std::vector<double> AvgAmplitudeValuesPixels(MaxNofChannels, 0.0);
    int hled_event_counter = 0;

    TH1F* h1 = new TH1F("h1", "Time Difference between two events", 1000, -0.5, 999.5);
    h1->SetXTitle("delta T");
    h1->SetYTitle("Counts");
    h1->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    h1->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    // TH1F* h4 = (TH1F*)h1->Clone("h4");
    h1->SetStats(0); 

    
    

    

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    // for(int f = 50; f<60; f++){
        try {
            std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
            if (!util->isBranchPresentInFile(FilePath, "Test")) {
                continue; // Skip to the next branch if not present
            }

            LoadEvents(FilePath, "Test");
            LoadEventsHLED(FilePath, "HLED");
            ev = new IEvent();
            evHLED = new IEvent();
            int nEntries = 0;
            int nEntriesHLED = 0;
            
            SetBranches(ev);
            SetBranchesHLED(evHLED);
            nEntries = tree->GetEntries();
            nEntriesHLED = treeHLED->GetEntries();
            if (nEntries==0) {
                continue;
            }   


            ULong64_t priorTimeStamp = 0;
            ULong64_t CurrentTimeStamp = 0;

            

            for(int EventCounter = 0; EventCounter < (nEntries+nEntriesHLED); EventCounter++){
                // TH2F* hEvent = new TH2F("hEvent", "Event",16, -0.5, 15.5, 16, -0.5, 15.5);
                std::vector<float> Amplitudes;
                
                
                // this allow you to have all the info you need from the event in one for loop so you dont have to have one for HLED and one for Test since the events can sometimes cross over.
                Pulse *pulse; 
                if (EventCounter < nEntries) {
                    tree->GetEntry(EventCounter);      
                    for (int k = 0; k < MaxNofChannels; k++) {
                        pulse = new Pulse(ev->GetSignalValue(k));
                        // AmplitudesTest.push_back(pulse->GetAmplitude());
                        Amplitudes.push_back(pulse->GetAmplitude());        
                        // PedestalRMSTest.push_back(pulse->GetPedestalRMS());      
                        // if (PedestalRMSTest[k] == 0){
                        //     cout << "Event Test" << EventCounter-nEntries << " has a Pedestal RMS of " << PedestalRMSTest[k] << endl;
                        //     cout << "The amplitude is " << pulse->GetAmplitude() << endl;
                        // }             
                        delete pulse;
                    }
                    
                } else {
                    treeHLED->GetEntry(EventCounter-nEntries);
                    for (int k = 0; k < MaxNofChannels; k++) {
                        pulse = new Pulse(evHLED->GetSignalValue(k));
                        Amplitudes.push_back(pulse->GetAmplitude());
                        // PedestalRMSHLED.push_back(pulse->GetPedestalRMS()); 
                        
                        // if (PedestalRMSHLED[k] == 0){
                        //     cout << "Event HLED " << EventCounter-nEntries << " has a Pedestal RMS of " << PedestalRMSHLED[k] << endl;
                        //     cout << "The amplitude is " << pulse->GetAmplitude() << endl;
                        // }
                        delete pulse;
                    }
                }

                CurrentTimeStamp = ev->GetTBTime();
                if (priorTimeStamp == 0){
                    cout << "setting the first time step" <<endl;
                    priorTimeStamp = CurrentTimeStamp;
                    continue;
                }
                // cout << "Prior Trigger time: " << priorTimeStamp << endl;
                
                // cout << "Current Trigger time: " << CurrentTimeStamp << endl;
                cout << "Different in Trigger Time: " << CurrentTimeStamp - priorTimeStamp << "ns" << endl;
                
                if(priorTimeStamp !=CurrentTimeStamp){
                    h1->Fill(static_cast<int>(CurrentTimeStamp - priorTimeStamp));
                    
                }
                priorTimeStamp = CurrentTimeStamp;
                
            }
            delete evHLED;
            delete ev;
            delete fO;
            delete tree;
            delete treeHLED;
        } catch (const std::exception& e){
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }
    }

    file = new TFile(Form("%s%s_Deadtime.root", outDir.c_str(), folString.c_str()), "RECREATE");  // "RECREATE" to overwrite if it exists

    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    c_cleaned->cd(0);
    h1->Draw("hist");
    c_cleaned->Write("canvas");
    cout << h1->GetMinimum() << endl;
    cout << h1->GetMaximum() << endl;

    h1->Write("DeltaTDist");


    


    file->Close();
    util->setFilePermissions(Form("%s%s_Deadtime.root", outDir.c_str(), folString.c_str()));

    return 0;
}




