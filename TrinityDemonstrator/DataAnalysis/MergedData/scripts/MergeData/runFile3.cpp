#include "runFile3.h"
int main(int argc, char **argv){

    if(argc < 1){
		cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
    std::string folString = argv[1];
    
    
    std::string mount = "";
    mount = argv[2];
    std::string filename_argument = "";
    filename_argument = argv[3];
    
    if (mount != ""){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt=mount.c_str();
        dataDir = Form("%sData/",mnt.c_str());
        badfilescsv = Form("%sDataAnalysis/MergedData/scripts/MergedData/BadFiles.csv",mnt.c_str());

    }

    std::string dirName = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
    cout << "Directory: " <<dirName << endl;
    std::vector <std::string> fileNamesVec;

    if (filename_argument != "n"){ // if the file name not specified then do all files in the directory
        std::cout << "using specific file name" << std::endl;
        std::string specificfile = Form("%s%s/RawDataMerged/%s",dataDir.c_str(),folString.c_str(),filename_argument.c_str());
        fileNamesVec.push_back(specificfile);
    } else {
        fileNamesVec = read_directory(dirName.c_str());
        fileNamesVec.erase(fileNamesVec.begin(), fileNamesVec.begin() + 2);
    }
    //Set the file in and file directories
    std::string fileDirIn = Form("%sData/%s/RawDataMerged",mnt.c_str(),folString.c_str());
    std::string fileDirOut = Form("%sDataAnalysis/MergedData/Output/%s",mnt.c_str(),folString.c_str());

	// load in all the csv data into the class to be accessed later easily
    std::string weatherfilename = Form("%sMiscData/WeatherData/weather/weather_%s",mnt.c_str(),folString.c_str());
    std::string sunmoonfilename = Form("%sDataAnalysis/AncillaryData/Data1/celestialPositions%s.csv", mnt.c_str(), folString.c_str());
    std::string StateMessageFile = Form("%sDataAnalysis/AncillaryData/Data2/statemessages%s.csv", mnt.c_str(), folString.c_str());
                    
    CSVData csvdata(StateMessageFile,weatherfilename,sunmoonfilename);
    csvdata.SetCSV();

    //std::vector<std::vector<float>> testhere = csvdata.GetWeatherHumidity();
    // for (int i = 0; i < testhere.size(); i++)
    // {
    //     for (int j = 0; j < testhere[i].size(); j++)
    //     {
    //         cout << testhere[i][j] << ", ";
    //     }
    //     cout << endl;
    // }
     
    
    // Create the folder 
    // const char* cfileDirIn = fileDirIn.c_str();
    const char* cfileDirOut = fileDirOut.c_str();
    if (gSystem->MakeDirectory(cfileDirOut) != 0) {
                std::cerr << "Failed to create directory: " << cfileDirOut << std::endl;
        }else {
                std::cout << "Directory created: " << cfileDirOut << std::endl;
        }	
    
    //Go through every file in that night of data
    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        
        std::string errorString = fileNamesVec[f];
        // if (fileNamesVec[f] == "/storage/osg-otte1/shared/TrinityDemonstrator/Data/20231023/RawDataMerged/CoBo0_AsAd0_2023-10-23T05:33:23.309_0000.root"){
        //     errorString = errorString + " error because evil file";
        //     appendToCSV(badfilescsv, errorString);			
        //     continue;
        // }

        // Try to open the file
        TFile* testfile = TFile::Open(fileNamesVec[f].c_str());
        if (testfile){
            if (testfile->TestBit(TFile::kRecovered)) {
                std::cout << "Warning: The file was not properly closed!" << std::endl;
                errorString = errorString + "error because file not properly closed";
                appendToCSV(badfilescsv, errorString);			
                continue;
            }
            else{
                std::string entry = fileNamesVec[f];
                // cout << "Processing file: " << entry << endl;
                size_t coboPos = entry.find("CoBo0");
                if (coboPos != std::string::npos) {
                    entry = entry.substr(coboPos, entry.length() - coboPos);
                }
                cout << "Processing file: " << entry << endl;
                std::cout << "The file was properly closed" << endl;
                std::string fileOut = Form("%s/Merged_%s", cfileDirOut, entry.c_str());
                std::cout <<"OUTfile name: " << fileOut << std::endl;
                TelescopeInformationMerge3(csvdata,fileNamesVec[f], fileOut);
            }

        }else {
            cout << " cannot open file " << endl;
            errorString = errorString + "error because file could not be opened";
            appendToCSV(badfilescsv, errorString);			
            continue;	
        }
        testfile->Close();
        delete testfile;
    }
    return 0;
}


void TelescopeInformationMerge3(CSVData& csvdata,std::string filename, std::string fileOut){
    // Call the function to read the CSV file
    std::cout << "Starting file: " <<filename << std::endl;
    
    std::size_t start = filename.find("CoBo0_AsAd0_")+12; // Find the position after "COBO_"
    std::string date = filename.substr(start, 10);  // Extract "2024-08-07"
    std::string hyphdate = date;
    // Remove the hyphens to get "20240807"
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    std::cout << "Date extracted: " << date << std::endl;

    
    // create all the classes in IEvent for all the branchs
    IEvent *iEvHLED = new IEvent();
    IEvent *iEvBiFocal = new IEvent();
    IEvent *iEvTest = new IEvent();
    IEvent *iEvForced = new IEvent();

    Event *evHLED = 0;
    Event *evBiFocal = 0;
    Event *evTest = 0;
    Event *evForced = 0;

    TFile *fIn = new TFile(filename.c_str(), "READ");
    TTree *tHLED = (TTree*) fIn->Get("HLED");    
    TTree *tBiFocal = (TTree*) fIn->Get("BiFocal");    
    TTree *tForced = (TTree*) fIn->Get("Forced");    
    TTree *tTest = (TTree*) fIn->Get("Test");
	
	
    // int nRunHLED = 0;
    // int nRunBF = 0;
    // int nRunForced = 0;
    // int nRunTest = 0;
    
    // unsigned long long rTimeHLED = 0;
    // unsigned long long rTimeBiFocal = 0;
    // unsigned long long rTimeTest = 0;
    // unsigned long long rTimeForced = 0;
    
    
    tHLED->SetBranchAddress("Events",&evHLED);
    tBiFocal->SetBranchAddress("Events",&evBiFocal);
    tForced->SetBranchAddress("Events",&evForced);
    tTest->SetBranchAddress("Events",&evTest);
    
    //tHLED->SetBranchAddress("RunNumber",&nRunHLED);
    //tBiFocal->SetBranchAddress("RunNumber",&nRunBF);
    //tForced->SetBranchAddress("RunNumber",&nRunForced);
    //tTest->SetBranchAddress("RunNumber",&nRunTest);

    // tHLED->SetBranchAddress("RTimeTB",&rTimeHLED);
    // tBiFocal->SetBranchAddress("RTimeTB",&rTimeBiFocal);
    // tForced->SetBranchAddress("RTimeTB",&rTimeForced);
    // tTest->SetBranchAddress("RTimeTB",&rTimeTest);
    
	TFile *fOut = new TFile(fileOut.c_str(),"RECREATE");
    
    TTree *tMHLED = new TTree("HLED","HLED Events");
    TTree *tMBiFocal = new TTree("BiFocal","BiFocal Events");
    TTree *tMForced = new TTree("Forced","Forced Events");
    TTree *tMTest = new TTree("Test","Test Events");
	
    tMHLED->Branch("Events","IEvent",&iEvHLED,64000,99);
    tMBiFocal->Branch("Events","IEvent",&iEvBiFocal,64000,99);
    tMForced->Branch("Events","IEvent",&iEvForced,64000,99);
    tMTest->Branch("Events","IEvent",&iEvTest,64000,99);
    
    std::string pathtoexact = Form("%s/DataAnalysis",mnt.c_str());
    for(int i = 0; i < tHLED->GetEntries(); i++){
        tHLED->GetEntry(i);
        
        // cout<<evHLED->GetTBTime()<<endl;
        // cout<<normalizedTimeNs<<endl;
        // cout<<timeAfterLaunch<<endl;;
        iEvHLED->SetCoBoTime(evHLED->GetCoBoTime());
        iEvHLED->SetUNIXTime(evHLED->GetUNIXTime());
        iEvHLED->SetTBTime(evHLED->GetTBTime());
        
        iEvHLED->SetEventType(0);
        iEvHLED->SetROIPixelID(evHLED->GetROIPixelID());
        iEvHLED->SetROIMusicID(evHLED->GetROIMusicID());
        iEvHLED->SetSignalValue(evHLED->GetSignalValue());
        
        GetTime((uint64_t)evHLED->GetTBTime());
        //  std::cout << "Trigger Time (UTC): ";
        // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;
        
        // Find the closest statemessage time
        int closestIndex = findClosestIndex(csvdata.GetStateMessageTime(), convtime);
        //std::cout << "Closest value to Event time for statemessage : " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        // std::cout << "Closest index: " << closestIndex << std::endl;
        
        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(csvdata.GetWeatherTime(), convtime);
        // std::cout << "Closest value to Event time for weather: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherTime[WXclosestIndex] << std::endl;
        // std::cout << WeatherHumidity[WXclosestIndex] << std::endl;
        
		// Find the closest sunmoon time 
        // for (int i=0; i < SunmoonTime.size();i++){
            //     cout << SunmoonTime[i] << endl;
            // }
            int SMXclosestIndex = findClosestIndex(csvdata.GetSunmoonTime(), convtime);	
            // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << ", "<< SMXclosestIndex<< std::endl;
            // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << std::endl;
            
            
            
        // Addes the closetest index of each statemessage to the file using the IEvents class
        
        iEvHLED->SetParametersFromTimestamp(pathtoexact.c_str(),
            csvdata.GetStateMessageHV().at(closestIndex),
            csvdata.GetStateMessageHVCurrent().at(closestIndex),
            csvdata.GetStateMessageSiPMTempatures().at(closestIndex),
            csvdata.GetStateMessageUCTempatures().at(closestIndex),
            csvdata.GetStateMessageMUSICPower().at(closestIndex),
            csvdata.GetStateMessageHVSwitch().at(closestIndex),
            csvdata.GetStateMessageASADCurrent().at(closestIndex),
            csvdata.GetStateMessageSIABCurrents().at(closestIndex),
            csvdata.GetStateMessageTBCurrent().at(closestIndex),
            csvdata.GetWeatherHumidity().at(WXclosestIndex),
            csvdata.GetWeatherTempature().at(WXclosestIndex),	
			csvdata.GetSunAzimuth().at(SMXclosestIndex),
			csvdata.GetSunElevation().at(SMXclosestIndex),
			csvdata.GetMoonAzimuth().at(SMXclosestIndex),
			csvdata.GetMoonElevation().at(SMXclosestIndex),
			csvdata.GetMoonIllumination().at(SMXclosestIndex),
            csvdata.GetCameraRA().at(SMXclosestIndex),
			csvdata.GetCameraDEC().at(SMXclosestIndex)
			);
        fOut->cd();
        tMHLED->Fill();
        // cout << "HLED Event " << i << " processed" << endl;
    }

    for(int i = 0; i < tBiFocal->GetEntries(); i++){
        tBiFocal->GetEntry(i);

        // cout<<evBiFocal->GetTBTime()<<endl;
        // cout<<normalizedTimeNs<<endl;
        // cout<<timeAfterLaunch<<endl;;
        iEvBiFocal->SetCoBoTime(evBiFocal->GetCoBoTime());
        iEvBiFocal->SetUNIXTime(evBiFocal->GetUNIXTime());
        iEvBiFocal->SetTBTime(evBiFocal->GetTBTime());

        iEvBiFocal->SetEventType(0);
        iEvBiFocal->SetROIPixelID(evBiFocal->GetROIPixelID());
        iEvBiFocal->SetROIMusicID(evBiFocal->GetROIMusicID());
        iEvBiFocal->SetSignalValue(evBiFocal->GetSignalValue());

        GetTime((uint64_t)evBiFocal->GetTBTime());
       //  std::cout << "Trigger Time (UTC): ";
       // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(csvdata.GetStateMessageTime(), convtime);
        // std::cout << "Closest value to Event time for statemessage : " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        // std::cout << "Closest index: " << closestIndex << std::endl;

        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(csvdata.GetWeatherTime(), convtime);
        // std::cout << "Closest value to Event time for weather: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherTime[WXclosestIndex] << std::endl;
        // std::cout << WeatherHumidity[WXclosestIndex] << std::endl;

		// Find the closest sunmoon time 
        // for (int i=0; i < SunmoonTime.size();i++){
        //     cout << SunmoonTime[i] << endl;
        // }
		int SMXclosestIndex = findClosestIndex(csvdata.GetSunmoonTime(), convtime);	
        // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << ", "<< SMXclosestIndex<< std::endl;
        // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << std::endl;

        // Addes the closetest index of each statemessage to the file using the IEvents class
        iEvBiFocal->SetParametersFromTimestamp(pathtoexact.c_str(),
            csvdata.GetStateMessageHV().at(closestIndex),
            csvdata.GetStateMessageHVCurrent().at(closestIndex),
            csvdata.GetStateMessageSiPMTempatures().at(closestIndex),
            csvdata.GetStateMessageUCTempatures().at(closestIndex),
            csvdata.GetStateMessageMUSICPower().at(closestIndex),
            csvdata.GetStateMessageHVSwitch().at(closestIndex),
            csvdata.GetStateMessageASADCurrent().at(closestIndex),
            csvdata.GetStateMessageSIABCurrents().at(closestIndex),
            csvdata.GetStateMessageTBCurrent().at(closestIndex),
            csvdata.GetWeatherHumidity().at(WXclosestIndex),
            csvdata.GetWeatherTempature().at(WXclosestIndex),	
			csvdata.GetSunAzimuth().at(SMXclosestIndex),
			csvdata.GetSunElevation().at(SMXclosestIndex),
			csvdata.GetMoonAzimuth().at(SMXclosestIndex),
			csvdata.GetMoonElevation().at(SMXclosestIndex),
			csvdata.GetMoonIllumination().at(SMXclosestIndex),
            csvdata.GetCameraRA().at(SMXclosestIndex),
			csvdata.GetCameraDEC().at(SMXclosestIndex)
			);

        fOut->cd();
        tMBiFocal->Fill();
    }

    for(int i = 0; i < tForced->GetEntries(); i++){
        tForced->GetEntry(i);

        // cout<<evForced->GetTBTime()<<endl;
        // cout<<normalizedTimeNs<<endl;
        // cout<<timeAfterLaunch<<endl;;
        iEvForced->SetCoBoTime(evForced->GetCoBoTime());
        iEvForced->SetUNIXTime(evForced->GetUNIXTime());
        iEvForced->SetTBTime(evForced->GetTBTime());

        iEvForced->SetEventType(0);
        iEvForced->SetROIPixelID(evForced->GetROIPixelID());
        iEvForced->SetROIMusicID(evForced->GetROIMusicID());
        iEvForced->SetSignalValue(evForced->GetSignalValue());

        GetTime((uint64_t)evForced->GetTBTime());
       // std::cout << "Trigger Time (UTC): ";
       // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(csvdata.GetStateMessageTime(), convtime);
        // std::cout << "Closest value to Event time for statemessage : " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        // std::cout << "Closest index: " << closestIndex << std::endl;

        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(csvdata.GetWeatherTime(), convtime);
        // std::cout << "Closest value to Event time for weather: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherHumidity[WXclosestIndex] << std::endl;
        // std::cout << "Closest value for tempature is" << WeatherHumidity[WXclosestIndex] << std::endl;


		int SMXclosestIndex = findClosestIndex(csvdata.GetSunmoonTime(), convtime);	
        // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << ", "<< SMXclosestIndex<< std::endl;
        // std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << std::endl;
		
        // Addes the closetest index of each statemessage to the file using the IEvents class
        iEvForced->SetParametersFromTimestamp(pathtoexact.c_str(),
            csvdata.GetStateMessageHV().at(closestIndex),
            csvdata.GetStateMessageHVCurrent().at(closestIndex),
            csvdata.GetStateMessageSiPMTempatures().at(closestIndex),
            csvdata.GetStateMessageUCTempatures().at(closestIndex),
            csvdata.GetStateMessageMUSICPower().at(closestIndex),
            csvdata.GetStateMessageHVSwitch().at(closestIndex),
            csvdata.GetStateMessageASADCurrent().at(closestIndex),
            csvdata.GetStateMessageSIABCurrents().at(closestIndex),
            csvdata.GetStateMessageTBCurrent().at(closestIndex),
            csvdata.GetWeatherHumidity().at(WXclosestIndex),
            csvdata.GetWeatherTempature().at(WXclosestIndex),	
			csvdata.GetSunAzimuth().at(SMXclosestIndex),
			csvdata.GetSunElevation().at(SMXclosestIndex),
			csvdata.GetMoonAzimuth().at(SMXclosestIndex),
			csvdata.GetMoonElevation().at(SMXclosestIndex),
			csvdata.GetMoonIllumination().at(SMXclosestIndex),
            csvdata.GetCameraRA().at(SMXclosestIndex),
			csvdata.GetCameraDEC().at(SMXclosestIndex)
			);
        fOut->cd();
        tMForced->Fill();
    }
    
    for(int i = 0; i < tTest->GetEntries(); i++){
        
        //fIn->cd();
        tTest->GetEntry(i);

        iEvTest->SetCoBoTime(evTest->GetCoBoTime());
        iEvTest->SetUNIXTime(evTest->GetUNIXTime());
        iEvTest->SetTBTime(evTest->GetTBTime());
        iEvTest->SetEventType(0);
        iEvTest->SetROIPixelID(evTest->GetROIPixelID());
        iEvTest->SetROIMusicID(evTest->GetROIMusicID());
        iEvTest->SetSignalValue(evTest->GetSignalValue());

        GetTime((uint64_t)evTest->GetTBTime());
        //std::cout << "Trigger Time (UTC): ";
        // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(csvdata.GetStateMessageTime(), convtime);
        //std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        // std::cout << "Closest index: " << closestIndex << std::endl;
        
        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(csvdata.GetWeatherTime(), convtime);
        //std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherTime[closestIndex] << std::endl;
       // std::cout << "Closest index: " << WXclosestIndex << std::endl;
      
		// Find the closest sunmoon time 
		int SMXclosestIndex = findClosestIndex(csvdata.GetSunmoonTime(), convtime);	
        // std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[closestIndex] << std::endl;
        // std::cout << "Closest index: " << SMXclosestIndex << std::endl;
        // add the SiPM temps to the .root file
        
        iEvTest->SetParametersFromTimestamp(pathtoexact.c_str(),
            csvdata.GetStateMessageHV().at(closestIndex),
            csvdata.GetStateMessageHVCurrent().at(closestIndex),
            csvdata.GetStateMessageSiPMTempatures().at(closestIndex),
            csvdata.GetStateMessageUCTempatures().at(closestIndex),
            csvdata.GetStateMessageMUSICPower().at(closestIndex),
            csvdata.GetStateMessageHVSwitch().at(closestIndex),
            csvdata.GetStateMessageASADCurrent().at(closestIndex),
            csvdata.GetStateMessageSIABCurrents().at(closestIndex),
            csvdata.GetStateMessageTBCurrent().at(closestIndex),
            csvdata.GetWeatherHumidity().at(WXclosestIndex),
            csvdata.GetWeatherTempature().at(WXclosestIndex),	
			csvdata.GetSunAzimuth().at(SMXclosestIndex),
			csvdata.GetSunElevation().at(SMXclosestIndex),
			csvdata.GetMoonAzimuth().at(SMXclosestIndex),
			csvdata.GetMoonElevation().at(SMXclosestIndex),
			csvdata.GetMoonIllumination().at(SMXclosestIndex),
            csvdata.GetCameraRA().at(SMXclosestIndex),
			csvdata.GetCameraDEC().at(SMXclosestIndex)
			);

        //cout <<  StateMessageHV[closestIndex][0] << endl;
        //iEvTest->SetRunNumber(nRunTest);
        //iEvTest->SetRevTimeTB(rTimeTest);
        fOut->cd();
        tMTest->Fill();
    }
    fOut->Write();

    delete tMHLED;
    delete tMBiFocal;
    delete tMForced;
    delete tMTest;
    fOut->Close();
    fIn->Close();

    cout<< "SM and .root files merged Sucessfully! "<< fileOut.c_str() << endl;
    // Delete all the objects for memory 
    delete iEvHLED;
    delete iEvBiFocal;
    delete iEvTest;
    delete iEvForced;

    delete evHLED;
    delete evBiFocal;
    delete evTest;
    delete evForced;

    delete fIn;
    delete fOut;
    cout << "Completed deleting objects" << endl;
}


