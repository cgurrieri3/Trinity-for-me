R__LOAD_LIBRARY(libExACT.so)
//#include <Event.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <algorithm>
#include "TSystem.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
TTree *tree = 0;
Event *ev;
/*
std::vector<std::string> GetSortedEntriesByDate(const char* directory);
void  differenceTime(std::string& filename);
void  NumberOfEvents(std::string date);

int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
        return 1;
    }
    std::string folString = argv[1];
	NumberOfEvents(folString);
   return 0; 
}
*/
//Finds number of events for HLED and Test branch
void  NumberOfEvents() {
    //Finds directory with correct date and sorts it based on time
	//std::string filePath = "/storage/hive/project/phy-otte/nlew3/exact/incomplete.root";			
	//std::string char_filePath = "/storage/hive/project/phy-otte/shared/Trinity/Data/20241011/RawDataMerged/CoBo0_AsAd0_2024-10-11T04:08:40.206_0000.root";
	std::string char_filePath = "/storage/hive/project/phy-otte/shared/Trinity/Data/20240718/RawDataMerged/CoBo0_AsAd0_2024-07-18T10:56:05.106_0000.root";
	const char* filePath = char_filePath.c_str();
		if (gSystem->AccessPathName(filePath, kFileExists) == 0) {
		//	std::cout << "File(" << count << "/" << sortedLength << "): " << char_file << std::endl;
		//	differenceTime(sortedFiles[count-1]);
			TFile *fO = new TFile(filePath, "READ");
			std::string first = "HLED";
			tree = (TTree*)fO -> Get(first.c_str());
			tree -> SetBranchAddress("Events", &ev);
			int nEntries = tree -> GetEntries();
			std::cout<<"The number of entries for HLED is: " << nEntries << std::endl;	
			std::string second = "Test";
			tree = (TTree*)fO -> Get(second.c_str());
			tree -> SetBranchAddress("Events", &ev);
			int nEntriesTwo = tree -> GetEntries();
			std::cout<<"The number of entries for Test is: " << nEntriesTwo << std::endl;
			int totalEntries = nEntries + nEntriesTwo;
			std::cout << "Total number of events is: " << totalEntries << std::endl;
		}


}

//Takes in directory and sorts files by time of creation
std::vector<std::string> GetSortedEntriesByDate(const char* directory) {
    void* dirp = gSystem->OpenDirectory(directory);
    if (!dirp) {
        std::cerr << "Cannot open directory: " << directory << std::endl;
        return {};
    }

    std::vector<std::string> entries;
    const char* entry;

    // Loop through directory entries
    while ((entry = gSystem->GetDirEntry(dirp)) != nullptr) {
        // Skip "." and ".."
        if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
            continue;
        }

        // Store each valid entry (file) into the vector
        entries.push_back(entry);
    }

    gSystem->FreeDirectory(dirp);

    // Sort the filenames lexicographically (this works for date-based filenames like "2024-09-07T03:48:14.745_0000")
    std::sort(entries.begin(), entries.end());

    return entries;
}

void  differenceTime(std::string& filename) {
	// Parse the input timestamp
	std::string dateTime = filename.substr(12,19);
	std::tm tm = {};
	std::istringstream ss(dateTime);

	// Parse date-time in format: "YYYY-MM-DDTHH:MM:SS"
	ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

	// Convert std::tm to time_t (epoch time)
	std::time_t timeT = timegm(&tm);

	auto timeTT = std::chrono::system_clock::from_time_t(timeT);

	auto currentTime = std::chrono::system_clock::now();

	auto duration = currentTime - timeTT;
	
	auto totalMinutes = std::chrono::duration_cast<std::chrono::seconds>(duration).count()/60;

	std::cout << "It has been " << totalMinutes << " minutes  since this file has been created" << std::endl;
}

