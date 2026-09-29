//R__LOAD_LIBRARY(libExACT.so)

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
std::vector<std::string> GetSortedEntriesByDate(const char* directory);
void  differenceTime(std::string& filename);
void  NumberOfEvents(int date);

int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
        return 1;
    }
    std::string folString = argv[1];
	NumberOfEvents(folString);
	return 0;
    
}

//Finds number of events for HLED and Test branch
void  NumberOfEvents(std::string  date) {
    //Finds directory with correct date and sorts it based on time
	std::string pre_directory = "/home/trinity/Documents/Data/" + date  + "/RawDataMerged/";
	const char* directory = pre_directory.c_str();
	std::vector<std::string> sortedFiles = GetSortedEntriesByDate(directory);
	int count = 1;
	int sortedLength = sortedFiles.size();
	
	//std::cout << now << std::endl;
	///Looks through newly sorted root files and returns the number of events for HLED and Test as well as the file name which is above the corresponding HLED and Test events#s
	for (size_t i = 0; i < sortedFiles.size(); ++i) {
		const char* char_file = sortedFiles[i].c_str();
		TString filePath = TString::Format("%s/%s", directory, char_file);
				
		if (gSystem->AccessPathName(filePath, kFileExists) == 0 && count != 0) {
			std::cout << "File(" << count << "/" << sortedLength << "): " << char_file << std::endl;
			differenceTime(sortedFiles[count]);
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
			std::cout<<"The number of total events is: " << totalEntries << std::endl;
		}
		else{
			std::cout << "skipped " << count << std::endl;
		}
		count++;
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
	
	auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

	std::cout << "It has been " << totalSeconds << " seconds since this file has been created" << std::endl;
}

