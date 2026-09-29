R__LOAD_LIBRARY(libExACT.so)

#include <iostream>
#include <TSystem.h>
#include <string>
#include <TString.h>
#include <cstdio>

int WrapperLike(int date){
		std::string date_str = std::to_string(date);
		//std::string pre_directory = "/storage/hive/project/phy-otte/shared/Trinity/Data/" + date_str  + "/RawDataMerged/";
		std::string pre_directory = "/storage/hive/project/phy-otte/nlew3/hello/";
		const char* directory = pre_directory.c_str();	
		if (!replace){
			void *dirp = gSystem->OpenDirectory(directory);
			const char *entry;
			//bool hasEntries = false;
			bool hasFiles = false;
				while ((entry = gSystem->GetDirEntry(dirp))) {
				TString filePath = TString::Format("%s/%s", directory, entry);

				// Skip "." and ".." entries
				if (TString(entry) == "." || TString(entry) == "..") {
					continue;
				}

				// Check if the entry is a file
				if (gSystem->AccessPathName(filePath, kFileExists) == 0) {
					std::cout << "File: " << entry << std::endl;
					hasFiles = true;
				}
				}

				if (!hasFiles) {
					std::cout << "Directory contains no files." << std::endl;
				}
				else{
					std::cout << "Directory contains files." << std::endl;
				}	

				// Close directory
				gSystem->FreeDirectory(dirp);
	
	}
	else{
		
		void *dirp = gSystem->OpenDirectory(directory);
		const char *entry;
		//bool hasEntries = false;
		bool hasFiles = false;
			while ((entry = gSystem->GetDirEntry(dirp))) {
				TString filePath = TString::Format("%s/%s", directory, entry);

				// Skip "." and ".." entries
				if (TString(entry) == "." || TString(entry) == "..") {
					continue;
				}
		
				if (std::remove(filePath) == 0) {
					std::cout << "File deleted succesfully." << std::endl; 
				}
				else{
					std::cout <<"Error in deleting file" << std::endl;
				}
				 
			}
	}
	return 0;
}
 


