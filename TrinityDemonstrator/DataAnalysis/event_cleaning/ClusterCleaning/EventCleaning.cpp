#include "EventCleaning.h"

// Event cleaning code see Latex for more information 
//https://www.overleaf.com/project/67bf40074acfe901c4446a39
//and the pdf in this folder


int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date (YYYYMMDD) data directory to summarize" << endl;
		return 1;
	}
    
    std::string mount = argv[2];
    std::string filename_argument = "";
    filename_argument = argv[3];
    
    if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt="/mnt/";
        dataDir = "/mnt/DataAnalysis/MergedData/Output/";
        neighborDir = "/mnt/DataAnalysis/event_cleaning/ClusterCleaning/neighbors/";
        CalibrationFactorDir = "/mnt/DataAnalysis/flasher_calibration/Output/";
        outDir = "/mnt/DataAnalysis/event_cleaning/Output/";
    } else if (mount != "n"){
        std::cout << "using specific directory path" << std::endl;
        mnt=mount.c_str();
        dataDir = Form("%sDataAnalysis/MergedData/Output/",mnt.c_str());
        neighborDir = Form("%sDataAnalysis/event_cleaning/ClusterCleaning/neighbors/",mnt.c_str());
        CalibrationFactorDir = Form("%sDataAnalysis/flasher_calibration/Output/",mnt.c_str());
        outDir = Form("%sDataAnalysis/event_cleaning/Output/",mnt.c_str());
    }

    

    // Get the Arguments 
    std::string folString = argv[1];

    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
    cout << "FolderPath: " <<FolderPath << endl;
    // cout << "folString: " << folString.c_str() << endl;
    // Createthe fileNames Vector based on the type of data found in the passes argument
    // Sim - Simulation data (Only Test Branch events)
    // muon - muon data (Forced Branch events)
    // bkg - Background data (Test Branch and HLED Branch events in seperate folder)
    // none - normal data
    std::vector<std::string> fileNamesVec;
    if (folString.find("bkg")==0) {
        whatData = "bkg";
        fileNamesVec=util->readFileToVectorString(Form("%s%s.txt",bkgDir.c_str(),folString.c_str()));
        
    } else if (folString.find("muon")==0) {
        whatData = "muon";
        fileNamesVec=util->readFileToVectorString(Form("%s%s.txt",muonDir.c_str(),folString.c_str()));

    } else if (folString.find("sim")==0) {
        cout << "Simulation data detected" << endl;
        whatData = "sim";
        folString = folString.substr(3, 19); // Extract date from filename
        // cout << "Simulation Information: " << folString << endl;
        FolderPath = Form("%s%s/CARE/",simDir.c_str(),folString.c_str());
        cout << "Simulation Folder Path: " << FolderPath << endl;
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
        // cout << "Simulation Files Found: " << fileNamesVec.size() << endl;
        // remove files in the vec that have plots_
        fileNamesVec.erase(
            std::remove_if(
                fileNamesVec.begin(),
                fileNamesVec.end(),
                [](const std::string& name) {
                    return name.find("plots_") != std::string::npos;
                }
            ),
            fileNamesVec.end()
        );
        fileNamesVec.erase(
            std::remove_if(
                fileNamesVec.begin(),
                fileNamesVec.end(),
                [](const std::string& name) {
                    return name.find("traces_") != std::string::npos;
                }
            ),
            fileNamesVec.end()
        );


    } else {
        if (filename_argument != "n"){ // if the file name not specified then do all files in the directory
        // std::cout << "using specific file name" << std::endl;
        // std::string specificfile = Form("%s%s/%s",dataDir.c_str(),folString.c_str(),filename_argument.c_str());
        // // cout << "Specific File: " << specificfile << endl;
        fileNamesVec.push_back(filename_argument);
    } else {
        // fileNamesVec = read_directory(dirName.c_str());
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
        fileNamesVec.erase(fileNamesVec.begin(), fileNamesVec.begin() + 2);
    }
    }
    
    CreateFileName(folString,whatData);
    
    // Creates a root file that stores all the data    
    file = new TFile(OutputFileRoot.c_str(), "RECREATE");  // "RECREATE" to overwrite if it exists
    file->Close();
    
    plothelp = new PlotHelp(); // initialize the plot help class in scope for whole program
    
    // Create a new canvas with 2x2 sub-panels
    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    c_cleaned->Divide(2, 2);
    c_cleaned->Print(OutputFilePDFOpen.c_str());


    // new output file -- keep it OPEN so the trees created below are file-resident.
    // Creating a TTree while no file is open makes it memory-resident; large branches
    // like SEvent::posX/posY then fail to flush their baskets mid-Fill ("basket's
    // WriteBuffer failed").
    TFile *fileOutput = new TFile(OutputFileEventCleaningDataRoot.c_str(), "RECREATE");
    TTree *treeSims = new TTree("EventCleaning", "Events tree");
    eventInfo = new EventInfo(1);
    treeSims->Branch("Cleaned", eventInfo); // Store only serializable data members, not TH2F pointers

    // For sim data: copy the input "Sim" (GrOptics / SEvent) tree into the output
    // Data_EventCleaning file exactly as-is, for ALL events (triggered or not, cleaned or not).
    TTree *treeSimOut = nullptr;
    SEvent *simRunOut = new SEvent();
    if (whatData == "sim"){
        treeSimOut = new TTree("Sim", "Events tree");
        treeSimOut->Branch("GrOptics", &simRunOut);
    }

    
    // fileNamesVec.assign(fileNamesVec.begin() + 134, fileNamesVec.begin() + 135);
    
    // for(int f = 0; f<10; f++){
    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        cout << "Processing file: " << fileNamesVec[f] << endl;
        if (whatData == "bkg" ||  whatData == "muon") {
            std::string date = fileNamesVec[f].substr(7, 10); // Extract date from filename
            // remove - from the date string
            date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
            folString = date; // Update folString with the extracted date
            FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
        } 

        // checks to make sure the  data  file is  readable
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        // std::string FilePath = Form("%s",fileNamesVec[f].c_str());
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }

        int TotalEvents;
        int nEntries;
        int simDate = 0; // date pulled from the SEvent class for sim events
        std::string simRun = ""; // sim run number pulled from the SEvent class for sim events
        int simEvent = 0; // sim event number pulled from the SEvent class for sim events
        TFile *fSim = nullptr;            // sim file kept open so SEvent can be read per event
        TTree *simTree = nullptr;         // "Sim" tree holding one SEvent entry per THROWN event
        std::vector<Long64_t> simTrigIdx;  // "Sim" entry numbers of the triggered events, in order
        SEvent *simRunData = nullptr; // reused across the event loop
        TotalEvents = 0;
        nEntries = 0;
        // loads the events based on the type of data for each file
        if (whatData !="muon" && whatData !="sim"){
            LoadEvents(FilePath, "Test");
            LoadEventsHLED(FilePath, "HLED");
            ev = new IEvent();
            evHLED = new IEvent();
            sipmInfo = new ISiPM();
            SetBranches(ev);
            SetBranchesHLED(evHLED);
            tree->SetBranchAddress("SiPM", &sipmInfo);
            treeHLED->SetBranchAddress("SiPM", &sipmInfo);
            
            
            nEntries = tree->GetEntries();
            int nEntriesHLED = treeHLED->GetEntries();
            if (nEntries==0) {
                continue; // acts like continue in parrellization
            }    
            TotalEvents = nEntries + nEntriesHLED;
            std::cout << "Total Number of Events: " << nEntries << std::endl;
            std::cout << "Total Number of HLED Events: " << nEntriesHLED << std::endl;
            
        } else if (whatData== "muon"){
            
            LoadEvents(FilePath, "Forced");
            ev = new IEvent();
            sipmInfo = new ISiPM();
            SetBranches(ev);
            tree->SetBranchAddress("SiPM", &sipmInfo);
            
            nEntries = tree->GetEntries();
            if (nEntries==0) {
                continue;
            }    
            TotalEvents = nEntries;
            std::cout << "Total Number of Forced Events: " << nEntries << std::endl;
        } else if (whatData == "sim"){
            cout << "Loading Sim File: " << FilePath << endl;
            LoadEvents(FilePath, "Test");
            ev = new IEvent();
            // sipmInfo = new ISiPM();
            SetBranches(ev);
            // tree->SetBranchAddress("SiPM", &sipmInfo);

            // Open the "Sim" tree (GrOptics branch), which holds one SEvent per event.
            // Kept open for the whole event loop so the event number can be read per entry.
            fSim = new TFile(FilePath.c_str(), "READ");
            simTree = (TTree*)fSim->Get("Sim");
            simRunData = new SEvent();
            simTree->SetBranchAddress("GrOptics", &simRunData);
            simTree->GetEntry(0);
            simDate = simRunData->GetDate();
            cout << "Sim Run Date from SEvent: " << simDate << endl;

            // The "Sim" tree holds one entry per THROWN event, while "Test" holds only the events
            // that actually triggered, so indexing "Sim" with the "Test" event counter reads truth
            // from the wrong shower. Build the list of triggered "Sim" entries once per file so
            // event k of "Test" maps onto the k-th triggered "Sim" entry.
            simTrigIdx.clear();
            for (Long64_t s = 0; s < simTree->GetEntries(); s++){
                simTree->GetEntry(s);
                if (simRunData->GetTriggered()) simTrigIdx.push_back(s);
            }
            cout << "Sim entries: " << simTree->GetEntries()
                 << "  triggered: " << simTrigIdx.size()
                 << "  Test entries: " << tree->GetEntries() << endl;
            if (static_cast<Long64_t>(simTrigIdx.size()) != tree->GetEntries()){
                cout << "WARNING: triggered Sim entries != Test entries; the truth info is matched "
                        "to each event purely by order, so the two counts must agree." << endl;
            }

            // Copy every entry of the input "Sim" tree into the output Sim tree exactly as-is,
            // so the truth info is saved for ALL events regardless of triggered/cleaning class.
            if (treeSimOut){
                Long64_t nSim = simTree->GetEntries();
                for (Long64_t s = 0; s < nSim; s++){
                    simTree->GetEntry(s);
                    *simRunOut = *simRunData; // copy the SEvent object exactly
                    treeSimOut->Fill();
                }
                cout << "Copied " << nSim << " Sim entries to output" << endl;
            }

            nEntries = tree->GetEntries();
            if (nEntries==0) {
                continue;
            }    
            TotalEvents = nEntries;
            std::cout << "Total Number of Sim Test Events: " << nEntries << std::endl;
        }
        
        
        // loops through each event in a file
        // for(int EventCounter = 0; EventCounter < 1000; EventCounter++){
        for(int EventCounter = 0; EventCounter < TotalEvents; EventCounter++){
            cev = new CEvent(); // creates a new Clean Event object
            eventInfo->Clear();
            cev->SetEventDate(stoi(folString));
            if (whatData == "sim"){
                // read the SEvent entry for THIS event so the event number changes per event
                float simEnergy = 0.0;
                if (simTree && EventCounter < static_cast<int>(simTrigIdx.size())){
                    // this "Test" event is the EventCounter-th TRIGGERED entry of the "Sim" tree
                    simTree->GetEntry(simTrigIdx[EventCounter]);
                    simEvent = simRunData->GetSimEventNumber();
                    simEnergy = simRunData->GetNeutrinoEnergy(); // switch to GetTauEnergy() for tau energy
                }
                simDate = simRunData->GetDate();
                // The run name changes from sub-run to sub-run within one file, so it has to be
                // re-read for every event rather than cached from the first entry.
                simRun = simRunData->GetSumRun();
                cev->SetEventDate(simDate); // use the date from the SEvent class for sim events
                cev->SetFilename(simRun); // use the run name from the SEvent class for sim events
                cev->SetEventNumber(simEvent);
                plothelp->SetSimEnergy(simEnergy, true); // track this energy for the saved-vs-removed plot
            } else {
                cev->SetFilename(fileNamesVec[f]);
                plothelp->SetSimEnergy(0.0, false); // non-sim: do not track energy

            }

            std::vector<Int_t> TrigMus;
            std::vector<float> AmplitudesTimeBin;
            std::vector<int> PeakTimeBin;
            std::vector<float> Pedestals;
            Pulse *pulse;
            
            if(whatData != "muon") {
                if (EventCounter < nEntries) {
                    tree->GetEntry(EventCounter);
                    TrigMus = ev->GetROIMusicID();
                } else {
                    treeHLED->GetEntry(EventCounter-nEntries);
                    TrigMus = evHLED->GetROIMusicID();
                    // EventCounter = EventCounter -nEntries;
                }
                // tree->GetEntry(EventCounter);
        
                // Set up all the information for the Cleaning Event class
                Pulse *pulse1;
                // std::vector<vector<int>> trace = event->GetSignalValue();
                
                
                for (int k = 0; k < MaxNofChannels; k++) {
                    if (EventCounter < nEntries) {
                        // the CARE pulse sits SimTimeBinOffset bins early in the sim traces, so
                        // slide it onto TimeBinAll before any peak time or amplitude is read
                        std::vector<Int_t> trace = (whatData == "sim")
                            ? ShiftTrace(ev->GetSignalValue(k), SimTimeBinOffset)
                            : ev->GetSignalValue(k);
                        pulse = new Pulse(trace);
                        pulse1 = new Pulse(trace,TimeBinAll,TimeBinAll,512,true);
                        eventInfo->SetEventBranch(1);
                    } else {
                        pulse = new Pulse(evHLED->GetSignalValue(k));
                        pulse1 = new Pulse(evHLED->GetSignalValue(k),TimeBinAll,TimeBinAll,512,true);
                        eventInfo->SetEventBranch(9);
                    }
                    // pulse = new Pulse(ev->GetSignalValue(k));
                    Pedestals.push_back(pulse->GetPedestal());
                    PeakTimeBin.push_back(pulse->GetTimePeak());
                    if (whatData == "sim"){
                        if (pulse->GetAmplitude() == -0) {
                            AmplitudesTimeBin.push_back(0);
                        } else {
                            AmplitudesTimeBin.push_back(-1*pulse->GetAmplitude());
                        }
                        // cout << "Amplitude: " << pulse->GetAmplitude() << endl;
                    } else {
                        AmplitudesTimeBin.push_back(pulse1->GetAmplitude());

                    }
                    delete pulse;
                    delete pulse1;
                }

            } else if  (whatData == "muon"){
                tree->GetEntry(EventCounter);
                TrigMus = ev->GetROIMusicID();
                
                for (int k = 0; k < MaxNofChannels; k++) {
                    // pulse = new Pulse(ev->GetSignalValue(k));
                    pulse = new Pulse(ev->GetSignalValue(k),230,250,512,true);

                    Pedestals.push_back(pulse->GetPedestal());
                    PeakTimeBin.push_back(pulse->GetTimePeak());
                    AmplitudesTimeBin.push_back(pulse->GetAmplitude());
                    
                    delete pulse;

                }
            }
            
            
            
            cev->SetTriggeredMUSICID(TrigMus[0]);
            if (whatData == "sim"){
                // cout << "we should be entering here" << endl;
                // PixelSurviveCutOff=1;
                // sizeCutoff = 0; 
                // NumberOfCoresCutoff = 0; 
                // PixelSurviveCutOff=5;
                // sizeCutoff = 200; 
                // NumberOfCoresCutoff = 3;

                // CorePixelCutOff = 100;
                std::vector<double> fakeGain(256, 24.1); // for simulations that assuming is that the Photon detection eff. was done in a prior step so only the conversion from ADC to PE is needed in this step here
                cev->SetAmplitudeValuesTimeBin(AmplitudesTimeBin,CalibrationFactorDir, "all1new",fakeGain,44.0);
            } else {
                cev->SetAmplitudeValuesTimeBin(AmplitudesTimeBin,CalibrationFactorDir, std::to_string(cev->GetEventDate()),sipmInfo->GetGain(),44.0);

            }
            cev->SetPeakTimeBin(PeakTimeBin);
            AmplitudesTimeBin = cev->GetAmplitudeValuesTimeBin(); // reset the  amplitudesTimeBin to be aboslute gain calibrated
            for (int i = 0; i < AmplitudesTimeBin.size(); i++) {
                cout << AmplitudesTimeBin[i] << " ";
            }
            cout << endl;
            cev->SetAverageAmplitude(util->GetEventAverageAmplitude(cev->GetAmplitudeValuesTimeBin()));
            cev->SetRMS(cev->GetAmplitudeValuesTimeBin());
            plothelp->AddtoRMSratioAVGamp(cev->GetRMSoverAvgAmp());
            // cout <<"Avg amplitude Before: "  << util->GetEventAverageAmplitude(AmplitudesTimeBin) << endl;
            // cout <<"Avg amplitude: "  << cev->GetAverageAmplitude() << endl;
            
            // the amplitude is different when its prior to the october 2024 site visit
            // if (cev->GetEventDate() < 20241001 && whatData != "muon") {
            //     FlasherEventsCutOff = 350; // 350 ADC/8 PE
            // }
            if (whatData != "sim") {
                cev->SetEventNumber(EventCounter);
            }
            if (EventCounter > nEntries && whatData != "muon") {
                cev->SetEventNumber(EventCounter-nEntries);
            } 
            
            // update limit for flasher events cutoff deperciated for now
            // if (cev->GetAverageAmplitude() > (FlasherEventsCutOff/util->GetADCtoPEratio()) && whatData != "muon" && whatData != "sim") {
            //     plothelp->AddHLEDEvent(1);
            //     continue;
            // }
            

            eventInfo->SetDate(cev->GetEventDate());
            eventInfo->SetFile(cev->GetFilename());
            eventInfo->SetEvent(cev->GetEventNumber());
            eventInfo->SetTriggeredMUSICID(cev->GetTriggeredMUSICID());
            eventInfo->SetTriggeredPixelID(cev->GetMaxAmplitudePixelID());
            eventInfo->SetRMS_AvgAmp(cev->GetRMSoverAvgAmp());
            eventInfo->SetRMS(cev->GetRMS());
            eventInfo->SetAvgAmp(cev->GetAverageAmplitude());
            
            
            if (cev->GetRMSoverAvgAmp() < 0.15 && whatData != "muon" && whatData != "sim") {
                plothelp->AddEventFlags(9);
                eventInfo->SetEventFlag(9); // HLED  event
                treeSims->Fill();
                delete cev;
                continue;
            }
            
            
            
            // Remove events where the door is closed. This can be done by checking the HV currents of the SiPMs and 
            // if the sky seeing Channels are below 3.8 mA you know that the door is closed.
            // if (ev->Gethvc()[0] < 3.8 && ev->Gethvc()[2] < 3.8){
                //     plothelp->AddCleanedEvent(1);
                //     continue;
                // }
                
                
            // GetSaturatedPixels returns a 0/1 mask indexed by pixel ID, so the index of every 1
            // is the ID of a saturated pixel. This runs before cleaning, so it is the raw list;
            // CompletePanel4 narrows it to the pixels that actually survived. The cut below has
            // to stay on the raw count because there are no surviving pixels yet.
            std::vector<int> SaturatedPixels;
            if (EventCounter < nEntries) {
                // This reads the RAW trace, unlike the Pulse loop above which slides sim traces
                // onto TimeBinAll. Move the scan window back onto the CARE pulse by the same
                // offset rather than shifting all 256 traces a second time just for this check.
                const int satOffset = (whatData == "sim") ? SimTimeBinOffset : 0;
                SaturatedPixels = util->GetSaturatedPixels(ev->GetSignalValue(),
                                                           IUtilities::kSatWindowStart - satOffset,
                                                           IUtilities::kSatWindowEnd   - satOffset);
            } else {
                // HLED events have to come from their own branch: tree->GetEntry is never called
                // for them, so ev still holds the last Test entry and would report its saturation.
                SaturatedPixels = util->GetSaturatedPixels(evHLED->GetSignalValue());
            }
            SaturatedPixelIDsRaw.clear();
            for (std::size_t p = 0; p < SaturatedPixels.size(); p++) {
                if (SaturatedPixels[p] == 1) SaturatedPixelIDsRaw.push_back((Int_t)p);
            }
            cev->SetSaturatedPixels((int)SaturatedPixelIDsRaw.size());
            if (cev->GetSaturatedPixels() > SaturatedPixelCutoff && whatData != "muon" ) {
                plothelp->AddEventFlags(1);
                eventInfo->SetEventFlag(1);
                treeSims->Fill();
                delete cev;
                continue;
            }
            cout << "Event Number: " << cev->GetEventNumber() << " triggered  Music: " <<cev->GetTriggeredMUSICID() <<endl;
            // cout << "triggered Music pair: " << util->GetMUSICMate(cev->GetTriggeredMUSICID(), true) << endl;
            
            // //check which music has the highest amplitude 
            // cout << "triggered Music amp: " << util->GetMaximum(AmplitudesTimeBin,cev->GetTriggeredMUSICID()) << endl;
            // cout << "triggered Music pair amp: " << util->GetMaximum(AmplitudesTimeBin,util->GetMUSICMate(cev->GetTriggeredMUSICID(), true)) << endl;
            if(util->GetMaximum(cev->GetAmplitudeValuesTimeBin(),cev->GetTriggeredMUSICID()) < util->GetMaximum(cev->GetAmplitudeValuesTimeBin(),util->GetMUSICMate(cev->GetTriggeredMUSICID(), true))){
                cev->SetTriggeredMUSICID(util->GetMUSICMate(cev->GetTriggeredMUSICID(), true));
                eventInfo->SetTriggeredMUSICID(cev->GetTriggeredMUSICID());
                eventInfo->SetTriggeredPixelID(cev->GetMaxAmplitudePixelID());
                // cout << "Replaced. " << endl;
                // cout << "triggered Music pair: " << util->GetMUSICMate(cev->GetTriggeredMUSICID(), true) << endl;
            }
            
            cout << "triggered Music: " <<cev->GetTriggeredMUSICID() << endl;
            cev->SetMaxAmplitude(util->GetMaximum(cev->GetAmplitudeValuesTimeBin(),cev->GetTriggeredMUSICID()));
            // cout << "GetAmplitude: " << cev->GetAmplitudeValuesTimeBin()[239] << endl;
            cout << "Max Amplitude: " << cev->GetMaxAmplitude() << endl;
            cout << "Amplitude Cutoff: " << TriggeredChannelAmpCutOff/util->GetADCtoPEratio() << endl;
            if (cev->GetMaxAmplitude() < TriggeredChannelAmpCutOff/util->GetADCtoPEratio()) {
                plothelp->AddEventFlags(1);
                eventInfo->SetEventFlag(1);
                treeSims->Fill();
                delete cev;
                continue;
            }

            cev->SetPedestalValues(Pedestals);
            cev->SetMaxAmplitudePixelID(util->GetMaximumPixelID(cev->GetAmplitudeValuesTimeBin(),cev->GetTriggeredMUSICID()));
            cout << "Triggered Pixel ID: " << cev->GetMaxAmplitudePixelID() << endl;

            // removed events that have pixel 76 
            // if (cev->GetMaxAmplitudePixelID() == 78 || cev->GetMaxAmplitudePixelID() == 79 || cev->GetMaxAmplitudePixelID() == 77|| cev->GetMaxAmplitudePixelID() == 76 || cev->GetMaxAmplitudePixelID() == 75) {
            //     cout << "Triggered Pixel ID 78 or 79 skipping event." << endl;
            //     plothelp->AddPreCleaned(1);
            //     continue;
            // }

            // removed the hotspot on the worst night 
            if ((cev->GetTriggeredMUSICID() == 18 || cev->GetTriggeredMUSICID() == 19) && (folString == "20250305" || folString == "20250331" || folString == "20250328")){
                cout << "Triggered Music 18,19 skipping event." << endl;
                eventInfo->SetEventFlag(1);
                plothelp->AddEventFlags(1);
                treeSims->Fill();
                delete cev;
                continue;
            }
            
       
            cev->SetSumAmplitude(std::accumulate(AmplitudesTimeBin.begin(), AmplitudesTimeBin.end(), 0.0));
            
            
            /** 
            --------Create Panel 1 
            **/
            c_cleaned->cd(1);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            std::string filenameTitle = cev->GetFilename();
            if (whatData != "sim"){
                int pos = filenameTitle.find("T");
                filenameTitle = filenameTitle.substr(pos+1, 5);
            } 
            // create TH2D for Camera
            hcam_panel1 = new TH2F("hcam_panel1", Form("Calibrated Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()), 16, -0.5, 15.5, 16, -0.5, 15.5);
            hcam_panel1->SetDirectory(0);
            cev->SetPanel1(hcam_panel1);
            // eventInfo->SetHPanel1(hcam_panel1);
            // cev->SetAverageAmplitudeRMS(cev->GetAmplitudeValuesTimeBin());
            // Add a subtitle using TLatex
            TLatex *subtitle = new TLatex();
            subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
            subtitle->SetTextSize(0.03);
            
            subtitle->DrawLatex(0.1, 0.92, Form("Avg Amp Whole Camera : %.2f - Avg RMS: %.2f", cev->GetAverageAmplitude(), cev->GetRMS()));
            subtitle->DrawLatex(0.25, 0.12, Form("Triggered MUSIC: %i  Triggered Pixel: %i",cev->GetTriggeredMUSICID(),util->GetMaximumPixelID(cev->GetAmplitudeValuesTimeBin(),TrigMus[0])));
            // Saturation on the raw image, before cleaning: the boxes say where, the count says
            // how much. Panel 4 repeats this for the saturated pixels that survived cleaning.
            DrawSaturationMarkers(SaturatedPixelIDsRaw);
            subtitle->DrawLatex(0.25, 0.07, Form("Saturated Pixels: %i", (int)SaturatedPixelIDsRaw.size()));
            delete subtitle;
            
            eventInfo->SetHPanel1(hcam_panel1);
            
            /** 
            --------Create Panel 2
            **/
            // Plot hcam using the data from the CSV file
            c_cleaned->cd(2);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            
            
            hcam_panel2 = (TH2F*)hcam_panel1->Clone("hcam_panel2");
            hcam_panel2->SetTitle(Form("Amplitude cut Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel2->SetDirectory(0);
            cev->SetPanel2(hcam_panel2,CorePixelAmpCutOff/util->GetADCtoPEratio());
            std::vector<int> survivingPanel2Pixels = cev->GetSurvivingPixelPanel2();
            // cout<< PixelSurviveCutOff << endl;
            if (survivingPanel2Pixels.size() < static_cast<std::vector<int>::size_type>(PixelSurviveCutOff)) {
                eventInfo->SetEventFlag(2);
                plothelp->AddEventFlags(2);
                treeSims->Fill();
                
                delete hcam_panel1;
                delete hcam_panel2;
                delete cev;
                
                
                cout << "Removed Panel 2" << endl;
                continue;
            }
            
            
            /** 
            --------Create Panel 3
            **/
            c_cleaned->cd(3);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            hcam_panel3 = (TH2F*)hcam_panel2->Clone("hcam_panel3");
            hcam_panel3->SetTitle(Form("Connected Pixel Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel3->SetDirectory(0);
            
            
            cev->SetPanel3(hcam_panel3, neighborDir,NumberOfCoresCutoff,CorePixelCutOff, CrosstalkAllowanceCutoff);
            
            
            //find configure events with amp high and above 13
            // cout << "Configure Event?: " << cev->isConfigureEvent() << endl;
            if (cev->isConfigureEvent() == 1){
                cout << "Removed Panel 3 Configure" << endl;
                eventInfo->SetEventFlag(3);
                plothelp->AddEventFlags(3);
                treeSims->Fill();
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                continue;
            }
            cout << "YOU "<<cev->GetSurvivingPixelPanel3().size() << endl;
            if (cev->GetSurvivingPixelPanel3().size() < static_cast<std::vector<int>::size_type>(PixelSurviveCutOff)) {
                eventInfo->SetEventFlag(3);
                plothelp->AddEventFlags(3);
                treeSims->Fill();
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                
                cout<< "Removed Panel 3  NUmber of Pixels" << endl;
                continue;
            }
            
            if (cev->GetSurvivingPixelTotalAmpPanel3() <= sizeCutoff ) {
                eventInfo->SetEventFlag(3);
                plothelp->AddEventFlags(3);
                treeSims->Fill();
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                
                cout<< "Removed Panel 3 Size" << endl;
                continue;
            }
            cout << cev->GetMaxAmplitude()  << " , " << cev->GetSecondCoreAmp() << endl;
            if (cev->GetMaxAmplitude() == 0.0 || cev->GetSecondCoreAmp() == 0.0) {
                eventInfo->SetEventFlag(3);
                plothelp->AddEventFlags(3);
                treeSims->Fill();
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                
                cout<< "Removed Panel 3 Pixel amount" << endl;
                continue;
            }
            
            /** 
            --------Create Panel 4
            **/
            c_cleaned->cd(4);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            // At some point get panel four into the CEvent class
            // std::vector<double> CenterOfGravity(2, 0.0);
            //Create the PCA
            PCA pca(2);
            
            // do the Principle componet analysis on the cleaned data
            LoadDataPCA(pca,hcam_panel3,cev->GetSurvivingPixelTotalAmpPanel3());
            // //pca.ComputeCovarianceMatrix();
            
            TVectorD eigenVals; // remove the use of the pointer because this is already in the PCA class and calulated in LoadDataPCA
            TMatrixD eigenVecs; 
            std::vector<double> EllipicRatio=CreateWLRatio(pca,eigenVals,eigenVecs); // to do make EllipicRatio a member of cev
            //ex. cev->SetEllipticRatio(EllipicRatio);
            if (EllipicRatio[2] > (WLRatioCutOff/100.0)) {
                eventInfo->SetEventFlag(4);
                plothelp->AddEventFlags(4);
                treeSims->Fill();
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                continue;
            }
            
            
            eventInfo->SetEventFlag(5);
            plothelp->AddEventFlags(5);
            
            plothelp->AddtoEventDate(cev->GetEventDate());

            hcam_panel4 = (TH2F*)hcam_panel3->Clone("hcam_panel4");
            hcam_panel4->SetDirectory(0);
            hcam_panel4->SetTitle(Form("Cleaned Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            CompletePanel4(pca,hcam_panel4,cev,EllipicRatio,eigenVals,eigenVecs,eventInfo);            
            treeSims->Fill();
            
            // For sims, draw the truth info (neutrino energy, distance to and
            // location of the emergence point, distance to the tau decay point)
            // in the middle of the 2x2 display.
            // Drawn BEFORE the canvas is written so it is saved in the .root canvas too.
            TPaveText *simInfo = nullptr;
            if (whatData == "sim"){
                // Distance to emergence point (magnitude of the telescope position vector)
                double emergenceDistance = util->GetEmergenceDistance(simRunData->GetTelescope_Xpos(),
                                                                      simRunData->GetTelescope_Ypos(),
                                                                      simRunData->GetTelescope_Zpos());

                // Emergence angle (azimuth wrapped to [-180,180] from the telescope axis)
                double emergenceAngle = util->GetEmergenceAngle(simRunData->GetAzimuthAngle());

                // Distance to the tau decay point. The shower start (= decay point) and the
                // telescope position are both stored in the same emergence-point-centred frame,
                // so the telescope-to-decay-point distance is the magnitude of their difference.
                // (|showerStart| on its own is the decay length measured from the emergence point.)
                double decayDx = simRunData->GetTelescope_Xpos() - simRunData->GetShowerStartX();
                double decayDy = simRunData->GetTelescope_Ypos() - simRunData->GetShowerStartY();
                double decayDz = simRunData->GetTelescope_Zpos() - simRunData->GetShowerStartZ();
                double decayDistance = std::sqrt(decayDx*decayDx + decayDy*decayDy + decayDz*decayDz);

                c_cleaned->cd(0);
                simInfo = new TPaveText(0.28, 0.463, 0.72, 0.57, "NDC");
                simInfo->SetFillColorAlpha(0, 0.0);
                simInfo->SetFillStyle(0);
                simInfo->SetBorderSize(0);
                simInfo->SetTextSize(0.018);
                simInfo->AddText(Form("Neutrino Energy: %.3g GeV", simRunData->GetNeutrinoEnergy()));
                simInfo->AddText(Form("Emergence Angle: %.3g deg", emergenceAngle));
                simInfo->AddText(Form("Distance to Emergence Point: %.3g m", emergenceDistance));
                simInfo->AddText(Form("Distance to Decay Point: %.3g m", decayDistance));
                simInfo->Draw();
                c_cleaned->Update();
            }

            // create TH2D for Gain
            file = new TFile(OutputFileRoot.c_str(), "UPDATE");
            c_cleaned->Write(Form("N%i-F%s-E%i", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel1->Write(Form("PreN%i-F%s-E%iTH2F", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel4->Write(Form("PostN%i-F%s-E%iTH2F", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            file->Close();

            c_cleaned->Print(OutputFilePDF.c_str());
            if (simInfo){
                c_cleaned->GetListOfPrimitives()->Remove(simInfo); // keep it off later pages
                delete simInfo;
            }


            delete hcam_panel1;
            delete hcam_panel2;
            delete hcam_panel3;
            delete hcam_panel4;
            delete cev;
            
        }
     
        if (whatData != "muon"){
            delete treeHLED;
            delete evHLED;
        }
        if (whatData == "sim"){
            delete simRunData; // close the Sim tree opened for this file
            delete fSim;       // also deletes simTree (owned by the file)
            simRunData = nullptr;
            simTree = nullptr;
            fSim = nullptr;
        }
        delete tree;
        delete ev;
    }
    cout << "we here" << endl;
    fileOutput->cd();
    treeSims->Write();
    if (treeSimOut) treeSimOut->Write(); // sims: input Sim tree copied as-is for all events
    fileOutput->Close();
    file = new TFile(OutputFileRoot.c_str(), "UPDATE");
    plothelp->PlotEventFlags(c_cleaned, OutputFilePDF);
    plothelp->PlothSimEnergySavedVsNotSaved(c_cleaned, OutputFilePDF); // sims only: saved vs removed energy
    plothelp->PlothWL(c_cleaned,OutputFilePDF,outDir,to_string(cev->GetEventDate()));
    plothelp->PlothdistLandW(c_cleaned,OutputFilePDF);
    plothelp->PlothSize(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeSPC(c_cleaned,OutputFilePDF);    
    plothelp->PlothCoreRatio(c_cleaned,OutputFilePDF);    
    plothelp->PlothNumberofCores(c_cleaned,OutputFilePDF);    
    plothelp->PlothCoreRatioandNumberofCores(c_cleaned,OutputFilePDF);    
    plothelp->PlothWLConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothWLSPC(c_cleaned,OutputFilePDF); 
    plothelp->PlothSIZEWL(c_cleaned,OutputFilePDF);   
    plothelp->PlothLvW(c_cleaned,OutputFilePDF);
    plothelp->PlothSPCConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothCOG(c_cleaned,OutputFilePDF);    
    plothelp->PlothTPID(c_cleaned,OutputFilePDF);
    plothelp->PlothRMSratioAvg(c_cleaned, OutputFilePDF);
    plothelp->PlothEventDateTriggeredPixel(c_cleaned, OutputFilePDF);
    plothelp->PlothncoreSIZE(c_cleaned, OutputFilePDF);
    plothelp->PlothnCoreSPC(c_cleaned, OutputFilePDF);
    plothelp->PlothCRSPC(c_cleaned, OutputFilePDF);
    plothelp->PlothSIZEWandL(c_cleaned, OutputFilePDF);
    plothelp->PlotPixelsDistanceToMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlotPixelsRatioDistanceToMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlotdistRMSandWeightedRMS(c_cleaned, OutputFilePDF);
    plothelp->PlotPixelsOnMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlotPixelsOffMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlothRMSvsRatioDistance(c_cleaned, OutputFilePDF);
    plothelp->PlothWRMSvsRatioDistance(c_cleaned, OutputFilePDF);
    plothelp->PlothWLvsRatioPixelsMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlothDistancevsRatioPixelsMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlothCoreOverSPCvsRatioPixelsMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlothSPCvsRatioPixelsMajorAxis(c_cleaned, OutputFilePDF);
    plothelp->PlothSurvivingPixelsX(c_cleaned, OutputFilePDF);
    plothelp->PlothSurvivingPixelsY(c_cleaned, OutputFilePDF);
    plothelp->PlothCOGx(c_cleaned, OutputFilePDF);
    plothelp->PlothCOGy(c_cleaned, OutputFilePDF);
    plothelp->PlothAngle(c_cleaned, OutputFilePDF);
    plothelp->PlothUpDownSym(c_cleaned, OutputFilePDF);
    plothelp->PlothSaturatedPixels(c_cleaned, OutputFilePDF);
    plothelp->PlothSaturatedOverSurviving(c_cleaned, OutputFilePDF);
    plothelp->PlothSaturatedPixelsX(c_cleaned, OutputFilePDF);
    plothelp->PlothSaturatedPixelsY(c_cleaned, OutputFilePDF);
    //plothelp->PlothOnOffMajorAxisvsratio(c_cleaned, OutputFilePDF);

    file->Close();
    c_cleaned->Print(OutputFilePDFClose.c_str());
    util->setFilePermissions(OutputFileRoot.c_str());
    util->setFilePermissions(OutputFileEventCleaningDataRoot.c_str());
    cout << "Completed!" << endl;   
    // cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    delete plothelp;


    
}






