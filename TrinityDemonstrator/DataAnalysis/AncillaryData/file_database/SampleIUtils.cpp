#include "SampleIUtils.h"

int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
    // Get the Arguments
    std::string folString = argv[1];
    // Load in all the files
    std::string FolderPath = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
    std::vector<std::string>fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
 

    // Load in the Calibration factors from the HLED branch in the file of CalibrationFactor.cpp
    std::vector<double> CalibratedHLEDperPixel = util->readFileToVector(Form("%s%s_Calibration_Factor.csv", CalibrationFactorDir.c_str(), folString.c_str()));

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    //for(int f = 90; f<100; f++){
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }
        LoadEvents(FilePath, "Test");
        LoadEventsHLED(FilePath, "HLED");
        ev = new Event();
        evHLED = new Event();
        SetBranches(ev);
        SetBranchesHLED(evHLED);
        
        int nEntries = tree->GetEntries();
        // int nEntriesHLED = treeHLED->GetEntries();
        if (nEntries==0) {
            continue;
        }    
        
        std::cout << "Total Number of Events: " << nEntries << std::endl;
        for(int EventCounter = 0; EventCounter < nEntries; EventCounter++){
            cout << "File Name: "<<  fileNamesVec[f] << " Event: " << EventCounter << "/" << nEntries << endl;
            tree->GetEntry(EventCounter);
            std::vector<Int_t> TrigMus = ev->GetROIMusicID();
 
            cout << "Triggered Music Test: " << TrigMus[0] << " Music Position ID: " << util->GetMUSICPosition(TrigMus[0]) << endl;
            Pulse *pulse;
            // std::vector<vector<int>> trace = event->GetSignalValue();
            std::vector<float> Amplitudes;
            std::vector<float> Pedestals;
            for (int k = 0; k < MaxNofChannels; k++) {
                pulse = new Pulse(ev->GetSignalValue(k));
                Amplitudes.push_back(pulse->GetAmplitude());
                Pedestals.push_back(pulse->GetPedestal());
                delete pulse;
            }
            cout << "Max Pixel: " << util->GetMaximumPixelID(Amplitudes,TrigMus[0]) << " Max Amplitude: " << util->convertADC2PE(util->GetMaximum(Amplitudes,TrigMus[0])) << " Min Pixel: " << util->GetMinimumPixelID(Amplitudes,TrigMus[0]) << " Min Amplitude: " << util->GetMinimum(Amplitudes,TrigMus[0])<< endl;
            cout << "Ped Avg: " << util->GetPedestalAverage(ev->GetSignalValue(0),100,100,false) << endl;
            cout << "Event Average Avg: " << util->GetEventAverageAmplitude(Amplitudes) << endl;
            
            std::vector<int> SaturatedPixels = util->GetSaturatedPixels(ev->GetSignalValue());
            cout << "Saturated Pixels: " << std::accumulate(SaturatedPixels.begin(), SaturatedPixels.end(), 0) << endl;
        }

        
        // for(int EventCounter = 0; EventCounter < nEntriesHLED; EventCounter++){
        //     treeHLED->GetEntry(EventCounter-nEntries);
        //     std::vector<Int_t> TrigMus = evHLED->GetROIMusicID();
        //     // cout << "Triggered Music HLED: " << TrigMusHLED[0] << endl;
        //     Pulse *pulse;
        //     // std::vector<vector<int>> trace = event->GetSignalValue();
        //     std::vector<float> Amplitudes;
        //     for (int k = 0; k < MaxNofChannels; k++) {
        //         pulse = new Pulse(evHLED->GetSignalValue(k));
        //         Amplitudes.push_back(pulse->GetAmplitude());
        //         delete pulse;
        //     }
        //     cout << "Max Pixel: " << util->GetMaximumPixelID(Amplitudes,TrigMus[0]) << " Max Amplitude: " << util->GetMaximum(Amplitudes,TrigMus[0]) << " Min Pixel: " << util->GetMinimumPixelID(Amplitudes,TrigMus[0]) << " Min Amplitude: " << util->GetMinimum(Amplitudes,TrigMus[0])<< endl;
        //     cout << " Ped Avg: " << util->GetPedestalAverage(evHLED->GetSignalValue(0),100,100,false) << endl;
        //     cout << " Event Average Avg: " << util->GetEventAverageAmplitude(Amplitudes) << endl;
            
        //     delete pulse;

        // }
        delete fO;
        delete tree;
        delete treeHLED;
        delete ev;
        delete evHLED;
    }

}
