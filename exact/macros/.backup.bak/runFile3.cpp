R__LOAD_LIBRARY(libExACT.so)

#include "TSystem.h"
#include <TTree.h>
#include <TFile.h>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <ctime>
#include <iomanip>

std::vector<std::vector<std::string>> readCSV(const std::string& filePath);
void GetTime(uint64_t TriggerBoardTime);
size_t findClosestIndex(const std::vector<double>& sortedVec, double value);
std::vector<std::string> split(const std::string& str, char delimiter);
void processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end);
std::time_t convertToUnixTime(const std::string& datetime);
std::string formatDateTime(const std::string& date, const std::string& time);

tm* utcTime;
int microsec;
uint64_t convtime;

int TelescopeInformationMerge3 (std::string extraFile, std::string StateMessageFile,std::string filename, std::string fileOut){

    // Call the function to read the CSV file
    std::vector<std::vector<std::string>> data = readCSV(extraFile);
    cout << filename << endl;
	
    std::size_t start = filename.find("CoBo0_AsAd0_")+12; // Find the position after "COBO_"
    std::string date = filename.substr(start, 10);  // Extract "2024-08-07"
    std::string hyphdate = date;
    // Remove the hyphens to get "20240807"
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
	size_t startPos = filename.find("Trinity/Data/") + 13;
    std::string checkingdate = filename.substr(startPos, 8); //extract the date it was taken from to check for errors
    if (checkingdate != date){
		cout << "Wrong file in directory!!!" << endl;
		return 0;
	}	
	//cout << date << endl;
    std::string weatherfilename = Form("/storage/hive/project/phy-otte/shared/Trinity/MiscData/WeatherData/weather/weather_%s",date.c_str());
	std::string sunmoonfilename = Form("/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data1/%s/celestialPositions%s.csv", date.c_str(),hyphdate.c_str());	
    //cout << weatherfilename << endl;

	//cout << sunmoonfilename << endl;
    std::vector<std::vector<std::string>> WeatherData = readCSV(weatherfilename);
	cout << "read weather" << endl;	
	std::vector<std::vector<std::string>> SunmoonData = readCSV(sunmoonfilename);	
	cout << "read sunmoon" << endl;
	std::vector<double> WeatherTime;
	std::vector<double> SunmoonTime;
    std::vector<std::vector<float>> WeatherTempature;
    std::vector<std::vector<float>> WeatherHumidity;
	std::vector<std::vector<float>> SunAzimuth;
	std::vector<std::vector<float>> SunElevation;
	std::vector<std::vector<float>> MoonAzimuth;
	std::vector<std::vector<float>> MoonElevation;


    bool firstRow = true;
    
    // Process the 2D vector for Weather(for example, print it)
    for (const auto& row : WeatherData) {
          if (firstRow) {
            firstRow = false;  // Skip the first row
            continue;
        }
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                std::string firstValue = row[33];

                //cout << firstValue << endl;
                std::time_t weatherunixTime =convertToUnixTime(firstValue);
                //cout << weatherunixTime << endl;
				double timeInDouble = static_cast<double>(weatherunixTime);
                WeatherTime.push_back(timeInDouble);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: " << row[33] << std::endl;
                WeatherTime.push_back(0);
            } catch (const std::out_of_range& e) {
            	WeatherTime.push_back(0);   
			} 


            std::vector<float> columns;
            processRow(row, columns, 14, 14);
            WeatherTempature.push_back(columns);
            columns.clear();

            processRow(row, columns, 13, 13);
            WeatherHumidity.push_back(columns);
            columns.clear();


            
        }
	
    }	

	//Process the 2D vector for SunMoon
	bool firstRowSun = true;
  
    for (const auto& row : SunmoonData) {
          if (firstRowSun) {
            firstRowSun = false;  // Skip the first row		
            continue;
        }
		
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                std::string firstValue = formatDateTime(row[0], row[1]);
                std::time_t sunmoonunixTime =convertToUnixTime(firstValue);
                
		double timeInDouble = static_cast<double>(sunmoonunixTime);
                SunmoonTime.push_back(timeInDouble);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: "  << std::endl;
                SunmoonTime.push_back(0);
            } catch (const std::out_of_range& e) {
                // Handle out-of-range number cases
                std::cerr << "Number out of range in the first column: " << std::endl;
                SunmoonTime.push_back(0);
            }

            std::vector<float> columns;
            processRow(row, columns,6, 6);
            SunAzimuth.push_back(columns);
            columns.clear();

            processRow(row, columns, 7, 7);
            SunElevation.push_back(columns);
            columns.clear();
	
            processRow(row, columns, 8, 8);
           	MoonAzimuth.push_back(columns);
            columns.clear();

            processRow(row, columns, 9, 9);
            MoonElevation.push_back(columns);
            columns.clear();


        }
    }
	
    //std::cout << "Number of rows in weather humidity vector: " <<WeatherHumidity.size() << std::endl;

    // Call the function to read the CSV file
    std::vector<std::vector<std::string>> StateMessageData = readCSV(StateMessageFile);
   	cout << "read statemessage " << endl; 

    // Vector to store the columsn from the csv
    std::vector<double> StateMessageTime;
    std::vector<std::vector<float>> StateMessageHV;
    std::vector<std::vector<float>> StateMessageHVCurrent;

    std::vector<std::vector<float>> StateMessageSiPMTempatures;
    std::vector<std::vector<float>> StateMessageUCTempatures;
    std::vector<std::vector<float>> StateMessageHVSwitch;
    std::vector<std::vector<float>> StateMessageMUSICPower;

    std::vector<std::vector<float>> StateMessageASADCurrent;
    std::vector<std::vector<float>> StateMessageSIABCurrents;
    std::vector<std::vector<float>> StateMessageTBCurrent;

    //firstRow = true; 
    // Process the 2D vector (for example, print it)
    for (const auto& row : StateMessageData) {
          if (&row == &StateMessageData.back()) {
            //firstRow = false;  // Skip the first row
            continue;
        }
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                double firstValue = std::stod(row[0]) / 1000000000.0;
                StateMessageTime.push_back(firstValue);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: " << row[0] << std::endl;
                StateMessageTime.push_back(0);
            } catch (const std::out_of_range& e) {
                // Handle out-of-range number cases
                std::cerr << "Number out of range in the first column: " << row[0] << std::endl;
                StateMessageTime.push_back(0);
            }


            std::vector<float> columns;
            processRow(row, columns, 2, 5);
            StateMessageHV.push_back(columns);
            columns.clear();

            processRow(row, columns, 6, 9);
            StateMessageHVCurrent.push_back(columns);
            columns.clear();

            processRow(row,columns,10,25);
            StateMessageUCTempatures.push_back(columns);
            columns.clear();


            processRow(row,columns,26,41);
            StateMessageMUSICPower.push_back(columns);
            columns.clear();
            
            processRow(row,columns,42,57);
            StateMessageHVSwitch.push_back(columns);
            columns.clear();

            processRow(row, columns, 58, 73);
            StateMessageSiPMTempatures.push_back(columns);
            columns.clear();

            processRow(row, columns, 74, 74);
            StateMessageASADCurrent.push_back(columns);
            columns.clear();

            processRow(row, columns, 75, 90);
            StateMessageSIABCurrents.push_back(columns);
            columns.clear();

            processRow(row, columns, 91, 91);
            StateMessageTBCurrent.push_back(columns);
            columns.clear();

            
        }


    }
//    std::cout << "Length of StateMessageTime vector: " << StateMessageTime.size() << std::endl;
//    std::cout << "Number of rows in columns2to5 vector: " << StateMessageHV.size() << std::endl;
//    std::cout << "Number of rows in SiPM Temps vector: " << StateMessageSiPMTempatures.size() << std::endl;


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
	
	
    int nRunHLED = 0;
    int nRunBF = 0;
    int nRunForced = 0;
    int nRunTest = 0;

    unsigned long long rTimeHLED = 0;
    unsigned long long rTimeBiFocal = 0;
    unsigned long long rTimeTest = 0;
    unsigned long long rTimeForced = 0;


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

    for(int i = 0; i < tHLED->GetEntries(); i++){
        tHLED->GetEntry(i);

       //cout << "HLED Event# " << i <<" is being saved." << endl;
        
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
       // std::cout << "Trigger Time (UTC): ";
       // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(StateMessageTime, convtime);
       // std::cout << "Closest value to Event time for statemessage : " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
       // std::cout << "Closest index: " << closestIndex << std::endl;

        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(WeatherTime, convtime);
     //   std::cout << "Closest value to Event time for weather: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherTime[WXclosestIndex] << std::endl;
     //   std::cout << "Closest index: " << WXclosestIndex << std::endl;
      
		// Find the closest sunmoon time 
		int SMXclosestIndex = findClosestIndex(SunmoonTime, convtime);	
      //  std::cout << "Closest value to Event time for sunmoon: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[SMXclosestIndex] << std::endl;
      //  std::cout << "Closest index: " << SMXclosestIndex << std::endl;
		
        // Addes the closetest index of each statemessage to the file using the IEvents class
        iEvHLED->SetParametersFromTimestamp(
            StateMessageHV[closestIndex],
            StateMessageHVCurrent[closestIndex],
            StateMessageSiPMTempatures[closestIndex],
            StateMessageUCTempatures[closestIndex],
            StateMessageMUSICPower[closestIndex],
            StateMessageHVSwitch[closestIndex],
            StateMessageASADCurrent[closestIndex],
            StateMessageSIABCurrents[closestIndex],
            StateMessageTBCurrent[closestIndex],
            WeatherHumidity[WXclosestIndex],
            WeatherTempature[WXclosestIndex],	
			SunAzimuth[SMXclosestIndex],
			SunElevation[SMXclosestIndex],
			MoonAzimuth[SMXclosestIndex],
			MoonElevation[SMXclosestIndex]
			);

        
        
        //iEvHLED->SetRunNumber(nRunHLED);
        //iEvHLED->SetRevTimeTB(rTimeHLED);

        tMHLED->Fill();
    }

    // for(int i = 0; i < tBiFocal->GetEntries(); i++){
    //     if(i%100 == 0){
    //         cout << "Processing BiFocal event: "<<i<<endl;
    //     }
    //     fIn->cd();
    //     tBiFocal->GetEntry(i);
    //     timeAfterLaunch = rTimeBiFocal- normalizedTimeNs;
    //     timeAfterLaunch = timeAfterLaunch/1e8;
    //      if(timeAfterLaunch>0){
    //         timeAfterLaunch = timeAfterLaunch/1e8;
    //     }else{
    //         timeAfterLaunch = 0;
    //     }
    //     iEvBiFocal->SetCoBoTime(evBiFocal->GetCoBoTime());
    //     iEvBiFocal->SetUNIXTime(evBiFocal->GetUNIXTime());
    //     iEvBiFocal->SetTBTime(evBiFocal->GetTBTime());

    //     iEvBiFocal->SetEventType(0);
    //     iEvBiFocal->SetROIPixelID(evBiFocal->GetROIPixelID());
    //     iEvBiFocal->SetROIMusicID(evBiFocal->GetROIMusicID());
    //     iEvBiFocal->SetSignalValue(evBiFocal->GetSignalValue());

    //     iEvBiFocal->SetParametersFromTimestamp(timeAfterLaunch,data,StateMessageData);

    //     iEvBiFocal->SetRunNumber(nRunBF);
    //     iEvBiFocal->SetRevTimeTB(rTimeBiFocal);

    //     fOut->cd();
    //     tMBiFocal->Fill();
    // }
    // for(int i = 0; i < tForced->GetEntries(); i++){
    //     if(i%100 == 0){
    //         cout << "Processing Forced event: "<<i<<endl;
    //     }
    //     fIn->cd();
    //     tForced->GetEntry(i);
    //     timeAfterLaunch = rTimeForced- normalizedTimeNs;
    //     timeAfterLaunch = timeAfterLaunch/1e8;
    //      if(timeAfterLaunch>0){
    //         timeAfterLaunch = timeAfterLaunch/1e8;
    //     }else{
    //         timeAfterLaunch = 0;
    //     }
    //     iEvForced->SetCoBoTime(evForced->GetCoBoTime());
    //     iEvForced->SetUNIXTime(evForced->GetUNIXTime());
    //     iEvForced->SetTBTime(evForced->GetTBTime());

    //     iEvForced->SetEventType(0);
    //     iEvForced->SetROIPixelID(evForced->GetROIPixelID());
    //     iEvForced->SetROIMusicID(evForced->GetROIMusicID());
    //     iEvForced->SetSignalValue(evForced->GetSignalValue());

    //     iEvForced->SetParametersFromTimestamp(timeAfterLaunch,data,StateMessageData);

    //     iEvForced->SetRunNumber(nRunForced);
    //     iEvForced->SetRevTimeTB(rTimeForced);

    //     fOut->cd();
    //     tMForced->Fill();
    // }

    for(int i = 0; i < tTest->GetEntries(); i++){
        fIn->cd();
        tTest->GetEntry(i);
       // cout << "Test Event# " << i <<" is being saved." << endl;
        
        iEvTest->SetCoBoTime(evTest->GetCoBoTime());
        iEvTest->SetUNIXTime(evTest->GetUNIXTime());
        iEvTest->SetTBTime(evTest->GetTBTime());

        iEvTest->SetEventType(0);
        iEvTest->SetROIPixelID(evTest->GetROIPixelID());
        iEvTest->SetROIMusicID(evTest->GetROIMusicID());
        iEvTest->SetSignalValue(evTest->GetSignalValue());


        GetTime((uint64_t)evTest->GetTBTime());
       // std::cout << "Trigger Time (UTC): ";
       // std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(StateMessageTime, convtime);
       // std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
       // std::cout << "Closest index: " << closestIndex << std::endl;
        
        // Find the closest weather time
        int WXclosestIndex = findClosestIndex(WeatherTime, convtime);
       // std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< WeatherTime[closestIndex] << std::endl;
       // std::cout << "Closest index: " << WXclosestIndex << std::endl;
      
		// Find the closest sunmoon time 
		int SMXclosestIndex = findClosestIndex(SunmoonTime, convtime);	
       // std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< SunmoonTime[closestIndex] << std::endl;
       // std::cout << "Closest index: " << SMXclosestIndex << std::endl;
        // add the SiPM temps to the .root file
        //std::vector<float> value = StateMessageSiPMTempatures[closestIndex];
        // std::cout << "First column values (divided by 10,000,000):" << std::endl;
        //for (float v : value) {
        //    std::cout << v << std::endl;
        //}
        iEvTest->SetParametersFromTimestamp(
            StateMessageHV[closestIndex],
            StateMessageHVCurrent[closestIndex],
            StateMessageSiPMTempatures[closestIndex],
            StateMessageUCTempatures[closestIndex],
            StateMessageMUSICPower[closestIndex],
            StateMessageHVSwitch[closestIndex],
            StateMessageASADCurrent[closestIndex],
            StateMessageSIABCurrents[closestIndex],
            StateMessageTBCurrent[closestIndex],	
            WeatherHumidity[WXclosestIndex],
            WeatherTempature[WXclosestIndex],	
			SunAzimuth[SMXclosestIndex],
			SunElevation[SMXclosestIndex],
			MoonAzimuth[SMXclosestIndex],
			MoonElevation[SMXclosestIndex]
			);

            
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

    cout<< "SM and .root files merged Sucessfully"<< endl;
    // Delete all the objects for memory 
    delete iEvHLED;
    delete iEvBiFocal;
    delete iEvTest;
    delete iEvForced;

    //cout << 1 << endl;
    

    delete evHLED;
    delete evBiFocal;
    delete evTest;
    delete evForced;

    //cout << 2 << endl;
    delete fIn;
    delete fOut;

    //cout << 3 << endl;
    cout << "Completed deleting objects" << endl;
    return 0;
}





// Function to read a CSV file and return the data as a 2D vector
std::vector<std::vector<std::string>> readCSV(const std::string& filePath) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> fields = split(line, ',');
        data.push_back(fields);
    }

    file.close();
    return data;
}

void GetTime(uint64_t TriggerBoardTime){
    convtime = (TriggerBoardTime / 100000000ULL);
    
    //Assuming timeTrig is in the format "seconds since epoch with 10^-7 second resolution"
    time_t epochTime = static_cast<time_t>(convtime); // Convert to seconds
    microsec = convtime % 1000000;
    
    // Convert to struct tm in UTC
    utcTime = gmtime(&epochTime);

}

// Function to find the closest index to a given value
size_t findClosestIndex(const std::vector<double>& sortedVec, double value) {
    // Use lower_bound to find the position where 'value' would be inserted
    auto it = std::lower_bound(sortedVec.begin(), sortedVec.end(), value);

    // If 'it' is the beginning of the vector, return the first index
    if (it == sortedVec.begin()) {
        return 0;
    }

    // If 'it' is the end of the vector, return the last index
    if (it == sortedVec.end()) {
        return sortedVec.size() - 1;
    }

    // Calculate the distances to the nearest elements
    size_t index = it - sortedVec.begin();
    double distanceToLeft = value - sortedVec[index - 1];
    double distanceToRight = sortedVec[index] - value;

    // Return the closest index
    return (distanceToLeft <= distanceToRight) ? index - 1 : index;
}

// Function to split a string into a vector based on a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end) {
    for (size_t i = start; i <= end; ++i) {
        try {
            destination.push_back(std::stod(row[i]));
        } catch (const std::invalid_argument& e) {
           // std::cerr << "Invalid number in column " << (i + 1) << ": " << row[i] << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Number out of range in column " << (i + 1) << ": " << row[i] << std::endl;
        }
    }
}

std::time_t convertToUnixTime(const std::string& datetime) {
    struct tm tm{};
    
    // Parse the main date-time part "2024-07-19T00:49:26"
    strptime(datetime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
    std::time_t time = mktime(&tm);

    // Handling fractional seconds if present
    std::size_t dotPos = datetime.find('.');
    if (dotPos != std::string::npos) {
        std::string fractionalPart = datetime.substr(dotPos + 1);
        int milliseconds = std::stoi(fractionalPart);
        time += milliseconds / 1000.0;
    }

    return time;
}

std::string formatDateTime(const std::string& date, const std::string& time) {
    // Extract year, month, and day from the date
    std::string year = date.substr(0, 4);
    std::string month = date.substr(4, 2);
    std::string day = date.substr(6, 2);

    // Remove spaces from the time and ensure it has the correct format
    std::stringstream ss(time);
    int hour, minute, second;
    char colon1, colon2;
    ss >> hour >> colon1 >> minute >> colon2 >> second;

    // Format the date and time as ISO 8601
    std::ostringstream formatted;
    formatted << year << "-" << month << "-" << day << "T"
              << std::setw(2) << std::setfill('0') << hour << ":"
              << std::setw(2) << std::setfill('0') << minute << ":"
              << std::setw(2) << std::setfill('0') << second;

    return formatted.str();
}
/*
// Recursive function to empty and delete a directory
void EmptyAndDeleteDirectory(const char* dirPath) {
    void* dirp = gSystem->OpenDirectory(dirPath);
    if (!dirp) {
        std::cerr << "Failed to open directory: " << dirPath << std::endl;
        return;
    }

    const char* entry;
    while ((entry = gSystem->GetDirEntry(dirp))) {
        // Skip special entries "." and ".."
        if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
            continue;
        }

        // Construct the full path of the entry
        TString fullPath = TString::Format("%s/%s", dirPath, entry);
		Long_t id, size, flags, modtime;
        if (gSystem->GetPathInfo(fullPath,&id, &size, &flags, &modtime) == 0) {
		if (flags & 2) {	
            // If entry is a directory, recursively delete its contents
            	EmptyAndDeleteDirectory(fullPath.Data());
				cerr << "flags 2" << endl;
			}
        } else {
            // If entry is a file, delete it
            if (gSystem->Unlink(fullPath) != 0) {
                std::cerr << "Failed to delete file: " << fullPath << std::endl;
            } else {
                std::cout << "File deleted: " << fullPath << std::endl;
            }
        }
    }

    gSystem->FreeDirectory(dirp);

}
*/

void EmptyDirectory(const char* dirPath, int strt, int end) {
    // Open the directory
    void* dirp = gSystem->OpenDirectory(dirPath);
    if (!dirp) {
        std::cerr << "Failed to open directory: " << dirPath << std::endl;
        return;
    }

    const char* entry;
    while ((entry = gSystem->GetDirEntry(dirp))) {
        // Skip special entries "." and ".."
        
        if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
            continue;
        }
		if (isdigit(entry[0])){
			int numentry = std::stoi(entry);
				if (numentry < strt || numentry > end){
					continue;
			}
		}
		else {
		//	cout << "files" << endl;
		}
        // Construct the full path of the entry
        TString fullPath = TString::Format("%s/%s", dirPath, entry);

        // Retrieve path information to check if it's a file or directory
        Long_t id, size, flags, modtime;
        if (gSystem->GetPathInfo(fullPath, &id, &size, &flags, &modtime) == 0) {
            if (flags & 2) { // Check if it's a directory
                // Recursively delete the contents of the directory
                EmptyDirectory(fullPath.Data(), strt, end);
                // Delete the now-empty subdirectory
                if (gSystem->Unlink(fullPath) != 0) {
                    std::cerr << "Failed to delete directory: " << fullPath << std::endl;
                } else {
                    std::cout << "Deleted directory: " << fullPath << std::endl;
                }
            } else {
                // If it's a file, delete it
                if (gSystem->Unlink(fullPath) != 0) {
                    std::cerr << "Failed to delete file: " << fullPath << std::endl;
                } else {
                    std::cout << "Deleted file: " << fullPath << std::endl;
                }
            }
        } else {
            std::cerr << "Failed to get path info for: " << fullPath << std::endl;
        }
    }

    gSystem->FreeDirectory(dirp);
}

void appendToCSV(const std::string& filename, const std::string& data) {
    std::ofstream file;

    // Open the file in append mode
    file.open(filename, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Determine the next index (counting existing rows)
    int nextIndex = 1;
    std::ifstream readFile(filename);
    std::string line;
    while (std::getline(readFile, line)) {
        nextIndex++;  // Count rows (including header)
    }
    readFile.close();

    // Append the new string to the file
    file << nextIndex << "," << data << "\n";

    file.close();
    std::cout << "error file added to " << filename << " successfully!" << std::endl;
}


int runFile3(int option, int start_time, int end_time) {
	std::string extraFile = "filler";
	std::string StateMessageFile = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/Data2.csv";
	std::string fileDirOut2 = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/MergedData";
	std::string MergedFile = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/MergedData"; 
	if(option == 0){
		const char* cfileDirOut = MergedFile.c_str();
		EmptyDirectory(cfileDirOut, start_time,end_time);				

		
	}
	else if(option == 1){
		std::string bigFileDirIn = "/storage/hive/project/phy-otte/shared/Trinity/Data";
		const char* cbigFileDirIn = bigFileDirIn.c_str();
		void* dirp2 = gSystem->OpenDirectory(cbigFileDirIn);
		if (!dirp2) {
			std::cerr << "Failed to open directory: " << bigFileDirIn << std::endl;
			return 0;
		}	
		const char* entry2;
        while ((entry2 = gSystem->GetDirEntry(dirp2))) {
			std::string date = std::string(entry2);
            // Skip special entries "." and ".."
            if (strcmp(entry2, ".") == 0 || strcmp(entry2, "..") == 0 || !(isdigit(entry2[0]))) {
                continue;
            }
			int intDate = std::stoi(date);
			if ( intDate < start_time || intDate > end_time) {
				continue;
			}
			if (intDate == 20240116 || intDate == 20240119 || intDate == 20240120 || intDate == 20240123 || intDate == 20240127 || intDate == 20240224)	{
				continue;
			}
			// int date = 5
			// start_time = 4
			// end_time = 6
            
			std::string fileDirIn = "/storage/hive/project/phy-otte/shared/Trinity/Data/" + date + "/RawDataMerged";
			std::string fileDirOut = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/MergedData/" + date;
			std::string badfilescsv = "/storage/hive/project/phy-otte/nlew3/BadFiles.csv";
			const char* cfileDirIn = fileDirIn.c_str();
			const char* cfileDirOut = fileDirOut.c_str();
			void *dirp = gSystem->OpenDirectory(cfileDirIn);
			const char *entry;
			if (gSystem->MakeDirectory(cfileDirOut) != 0) {
						std::cerr << "Failed to create directory: " << cfileDirOut << std::endl;
				}else {
						std::cout << "Directory created: " << cfileDirOut << std::endl;
				}	
			while ((entry = gSystem->GetDirEntry(dirp))) {
				if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
						continue;
					}	
				TString filePath = TString::Format("%s/%s", cfileDirIn, entry);
				std::string strFilePath = filePath.Data();
				std::string errorString = strFilePath;
				if (strFilePath == "/storage/hive/project/phy-otte/shared/Trinity/Data/20231023/RawDataMerged/CoBo0_AsAd0_2023-10-23T05:33:23.309_0000.root"){
					errorString = errorString + " error because evil file";
					appendToCSV(badfilescsv, errorString);			
					continue;
				}
				TFile* testfile = TFile::Open(strFilePath.c_str());
				if (testfile){
					if (testfile->TestBit(TFile::kRecovered)) {
						std::cout << "Warning: The file was not properly closed!" << std::endl;
						errorString = errorString + "error because file not properly closed";
						appendToCSV(badfilescsv, errorString);			
						continue;
					}
					else{
						std::cout << "The file was properly closed" << endl;
					}
					testfile->Close();
				
				}else {
					cout << " cannot open file " << endl;
					errorString = errorString + "error because file could not be opened";
					appendToCSV(badfilescsv, errorString);			
					continue;	
				}
				TString cfileOut  = TString::Format("%s/Merged%s", cfileDirOut, entry);
				std::string fileOut = cfileOut.Data();
				TelescopeInformationMerge3(extraFile, StateMessageFile, strFilePath, fileOut);
		
			}
		}	
	}
	return 0;	
}
