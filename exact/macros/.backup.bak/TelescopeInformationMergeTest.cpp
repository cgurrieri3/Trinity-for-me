R__LOAD_LIBRARY(libExACT.so)

#include <TTree.h>
#include <TFile.h>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>


std::vector<std::vector<std::string>> readCSV(const std::string& filePath);
void GetTime(uint64_t TriggerBoardTime);
size_t findClosestIndex(const std::vector<double>& sortedVec, double value);
std::vector<std::string> split(const std::string& str, char delimiter);
void processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end);
std::time_t convertToUnixTime(const std::string& datetime);

tm* utcTime;
int microsec;
uint64_t convtime;


int TelescopeInformationMergeTest (std::string extraFile, std::string StateMesssageFile,std::string filename, std::string fileOut){

    // Call the function to read the CSV file
    //std::vector<std::vector<std::string>> data = readCSV(extraFile);
    std::size_t start = filename.find("CoBo0_AsAd0_")+12; // Find the position after "COBO_"
    std::string date = filename.substr(start, 10);  // Extract "2024-08-07"
    
    // Remove the hyphens to get "20240807"
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    cout << date << endl;
    std::string weatherfilename = Form("/storage/hive/project/phy-otte/shared/Trinity/MiscData/WeatherData/weather/weather_%s",date.c_str());
    cout << weatherfilename << endl;
    std::vector<std::vector<std::string>> WeatherData = readCSV(weatherfilename);
    std::vector<std::string> WeatherTime;
    std::vector<std::vector<float>> WeatherTempature;
    std::vector<std::vector<float>> WeatherHumidity;

    bool firstRow = true;
    
    // Process the 2D vector (for example, print it)
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

                cout << firstValue << endl;
                std::time_t weatherunixTime =convertToUnixTime(firstValue);
                cout << weatherunixTime << endl;
                WeatherTime.push_back(firstValue);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: " << row[33] << std::endl;
                WeatherTime.push_back(0);
            } catch (const std::out_of_range& e) {
                // Handle out-of-range number cases
                std::cerr << "Number out of range in the first column: " << row[33] << std::endl;
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

    std::cout << "Length of Weather Time vector: " << WeatherTime.size() << std::endl;
    std::cout << "Number of rows in weather temp vector: " << WeatherTempature.size() << std::endl;
    std::cout << "Number of rows in weather humidity vector: " <<WeatherHumidity.size() << std::endl;

    // Call the function to read the CSV file
    std::vector<std::vector<std::string>> StateMessageData = readCSV(StateMesssageFile);
    

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

    firstRow = true;
    
    // Process the 2D vector (for example, print it)
    for (const auto& row : StateMessageData) {
          if (firstRow) {
            firstRow = false;  // Skip the first row
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
    std::cout << "Length of StateMessageTime vector: " << StateMessageTime.size() << std::endl;
    std::cout << "Number of rows in columns2to5 vector: " << StateMessageHV.size() << std::endl;
    std::cout << "Number of rows in SiPM Temps vector: " << StateMessageSiPMTempatures.size() << std::endl;


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

    tHLED->SetBranchAddress("RTimeTB",&rTimeHLED);
    tBiFocal->SetBranchAddress("RTimeTB",&rTimeBiFocal);
    tForced->SetBranchAddress("RTimeTB",&rTimeForced);
    tTest->SetBranchAddress("RTimeTB",&rTimeTest);

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

        cout << "HLED Event# " << i <<" is being saved." << endl;
        
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
        std::cout << "Trigger Time (UTC): ";
        std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(StateMessageTime, convtime);
        std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        std::cout << "Closest index: " << closestIndex << std::endl;

        // Find the closest weather time
        WXclosestIndex = findClosestIndex(WeatherTime, convtime);
        std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        std::cout << "Closest index: " << WXclosestIndex << std::endl;
        
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
            WeatherTempature[WXclosestIndex]);

        
        
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
        cout << "Test Event# " << i <<" is being saved." << endl;
        
        iEvTest->SetCoBoTime(evTest->GetCoBoTime());
        iEvTest->SetUNIXTime(evTest->GetUNIXTime());
        iEvTest->SetTBTime(evTest->GetTBTime());

        iEvTest->SetEventType(0);
        iEvTest->SetROIPixelID(evTest->GetROIPixelID());
        iEvTest->SetROIMusicID(evTest->GetROIMusicID());
        iEvTest->SetSignalValue(evTest->GetSignalValue());


        GetTime((uint64_t)evTest->GetTBTime());
        std::cout << "Trigger Time (UTC): ";
        std::cout << std::put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(6) << microsec << std::endl;

        // Find the closest statemessage time
        int closestIndex = findClosestIndex(StateMessageTime, convtime);
        std::cout << "Closest value to Event time: " << convtime << " is " << std::fixed << std::setprecision(0)<< StateMessageTime[closestIndex] << std::endl;
        std::cout << "Closest index: " << closestIndex << std::endl;
        
        // add the SiPM temps to the .root file
        std::vector<float> value = StateMessageSiPMTempatures[closestIndex];
        // std::cout << "First column values (divided by 10,000,000):" << std::endl;
        for (float v : value) {
            std::cout << v << std::endl;
        }
        iEvTest->SetParametersFromTimestamp(
            StateMessageHV[closestIndex],
            StateMessageHVCurrent[closestIndex],
            StateMessageSiPMTempatures[closestIndex],
            StateMessageUCTempatures[closestIndex],
            StateMessageMUSICPower[closestIndex],
            StateMessageHVSwitch[closestIndex],
            StateMessageASADCurrent[closestIndex],
            StateMessageSIABCurrents[closestIndex],
            StateMessageTBCurrent[closestIndex]);

            
        //iEvTest->SetRunNumber(nRunTest);
        //iEvTest->SetRevTimeTB(rTimeTest);

        fOut->cd();
        tMTest->Fill();
    }

    fOut->Write();
    fOut->Close();
    fIn->Close();

    cout<< "SM and .root files merged Sucessfully"<< endl;
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
            std::cerr << "Invalid number in column " << (i + 1) << ": " << row[i] << std::endl;
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
