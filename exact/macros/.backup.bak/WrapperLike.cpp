R__LOAD_LIBRARY(libExACT.so)

#include <iostream>
#include <TSystem.h>
#include <string>
#include <TString.h>
#include <cstdio>

int  WrapperLike(){
		std::string date = "20240718";
		//std::string date_str = std::to_string(date);
		//std::string pre_directory = "/storage/hive/project/phy-otte/shared/Trinity/Data/" + date_str  + "/RawDataMerged/";
		std::string pre_directory = "/storage/hive/project/phy-otte/shared/Trinity/Data/" + date + "/RawDataMerged";
		const char* directory = pre_directory.c_str();
		
		void *dirp = gSystem->OpenDirectory(directory);
		const char *entry;
		bool hasFiles = false;
		int filesDel = 0;
			while ((entry = gSystem->GetDirEntry(dirp))) {
				TString prefilePath = TString::Format("%s/%s", directory, entry);
				std::string filePath = std::string(prefilePath.Data());
				std::cout << filePath << std::endl;
//				// Skip "." and ".." entries
/*				if (TString(entry) == "." || TString(entry) == "..") {
					continue;
				}
		
				if (std::remove(filePath) == 0) {
					std::cout << "File deleted succesfully. #" << filesDel <<  std::endl; 
				}
				else{
					std::cout <<"Error in deleting file" << std::endl;
				}
			
			filesDel++;
*/				
			}
			
		
	
	
	
	return 0;
}
 


