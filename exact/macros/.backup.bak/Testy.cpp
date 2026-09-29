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
#include <ctime>
#include <iomanip>


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

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

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

void  Testy() {
	 std::vector<std::vector<float>> SunAzimuth;
	std::string filen  = "/storage/hive/project/phy-otte/shared/Trinity/Data/20241009/RawDataMerged/CoBo0_AsAd0_2024-10-09T05:49:26.562_0000.root";
	TFile* file = TFile::Open(filen.c_str());
	if (file && file->TestBit(TFile::kRecovered)) {
        	std::cout << " was not properly closed!" << std::endl;
    	}	
/*	//storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibratimn/AncillaryData/Data1/20241011/celestialPositions2024-10-11.csv
	std::vector<std::vector<std::string>> SunmoonData = readCSV(filen);
	std::string date ="20231004";
	std::string weatherfilename = Form("/storage/hive/project/phy-otte/shared/Trinity/MiscData/WeatherData/weather/weather_%s",date.c_str());
    	std::string sunmoonfilename = Form("/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data1/%s/celestialPositions2023-10-04.csv", date.c_str());
    //cout << weatherfilename << endl;
	size_t startPos = filen.find("Trinity/Data/") + 13;	
	std::string checkingdate = filen.substr(startPos, 8); //extract the date it was taken from to check for errors
    	cout << checkingdate << endl;
	if (checkingdate != date){
        cout << "Wrong file in directory!!!" << endl;
        
    }
   //cout << sunmoonfilename << endl;
    	std::vector<std::vector<std::string>> WeatherData = readCSV(weatherfilename);
	cout << "kill" << endl;
    	std::vector<std::vector<std::string>> SunmoonData1 = readCSV(sunmoonfilename);	cout << "myself" <<endl;
	cout << "myself" <<endl;
*/	

}


/*
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

void Testy() {
	std::string date = "20240907";
    std::string time = "0:00:00";

    std::string iso8601 = formatDateTime(date, time);
	std::string hello = Form("/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibratimn/AncillaryData/Data1/%s/%s", date.c_str()," celestial");
	cout << hello << endl;
    std::cout << "Formatted Date and Time: " << iso8601 << std::endl;
}
*/
