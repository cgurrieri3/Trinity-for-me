//R__LOAD_LIBRARY(libExACT.so)
#include "CSVData.h"
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

#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <TPaletteAxis.h>
#include <TEllipse.h>

#include <TH2F.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <TArrow.h>
#include <sys/types.h>
#include <vector>
#include <TLatex.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TFile.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TMath.h>
#include <TPrincipal.h>
#include "Math/Integrator.h"
#include <TPaveText.h>
#include <random>
#include <utility>
#include <TBox.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>
#include <IEvent.h>
#include <set>

std::vector<std::vector<std::string>> readCSV(const std::string& filePath);
void GetTime(uint64_t TriggerBoardTime);
size_t findClosestIndex(const std::vector<double>& sortedVec, double value);
// std::vector<std::string> split(const std::string& str, char delimiter);
// void processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end);
// std::time_t convertToUnixTime(const std::string& datetime);
// std::string formatDateTime(const std::string& date, const std::string& time);
void TelescopeInformationMerge3(CSVData& csvdata,std::string filename, std::string fileOut);
void appendToCSV(const std::string& filename, const std::string& data);
void EmptyDirectory(const char* dirPath, int strt, int end);
std::vector<std::string> read_directory( const std::string& path = std::string());

tm* utcTime;
int microsec;
uint64_t convtime;
std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator/";
std::string dataDir = "/storage/osg-otte1/shared/TrinityDemonstrator/Data/";
std::string badfilescsv = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/scripts/MergedData/BadFiles.csv";

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

// get files sorted
std::vector <std::string> read_directory(const std::string& path){
	std::vector <std::string> result;
	dirent* de;
	DIR* dp;
	errno = 0;
	dp = opendir( path.empty() ? "." : path.c_str() );
	if (dp)
	{
	while (true)
		{
		errno = 0;
		de = readdir( dp );
		if (de == NULL) break;
		
		result.push_back( Form("%s%s",path.c_str(),de->d_name) );
		}
	closedir( dp );
	std::sort( result.begin(), result.end() );
	}
	return result;
}

