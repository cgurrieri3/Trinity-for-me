#include "checkECdata.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("First%s <mount>%s <date_list>%s <EC location>%s \n", argv[0], argv[1], argv[2], argv[3]);
        return 1;
    }
    
    std::string mount = argv[1];
    std::string date_list = argv[2];// used to sort door open and door closed files and such 
    // read in the file list and then only allow events that are within the file list to be saved to
    // the output pdf/ root file. 
    // if no date_list is given then all files will go to the pdf/root file
    std::string path_to_EC = argv[3];

    // ensure the file is mounted correctly in the container
    std::cout << "mount: " << mount << " date_list: " << date_list << " folder: " << path_to_EC << std::endl;
    // std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator";
    std::string mnt = "/mnt";
    std::cout << "using standard directory path: " << mnt << std::endl;
    if (mount != "y"){
        std::cout << "using specific directory path: " << mnt << std::endl;
        mnt=mount.c_str();
    }
    // add the mount path to the any paths 
    DatesListPath = Form("%s/DataAnalysis/%s", mnt.c_str(), date_list.c_str());
    std::cout << "data list path " << DatesListPath << std::endl;

    // Check the merged data first by going into each folder and printing out the number of files
    // at the end display all the nights that have 1 or 0 files  or check to make sure they have the same number as the og rawdatamerged
    
    
    if (date_list != "n"){
        std::vector<std::string> tempreadin=util->readFileToVectorString(DatesListPath.c_str());
        for (auto a: tempreadin) {
            if (a.size() == 9){
                a.pop_back();
                allowedFilesVec.push_back(a);
                // cout << ("good value") << a << endl;
            }
        }
        
        cout << "Number of allowed files: " << allowedFilesVec.size() << endl;
        cout << "First Date: " << allowedFilesVec[0] << "\nLast Date: " << allowedFilesVec[allowedFilesVec.size()-1] << endl;
    }
    
    std::vector<std::string> BadMerged; 
    std::vector<std::string> BadFlasher; 
    std::vector<std::string> BadEC; 
    
    
    // MERGED DATA
    //check that the number of files matches with the merged files and then print the overall size of the directory 
    for(auto a:allowedFilesVec){
        cout << "Reviewing folder: " << a;
        MergedFolderPath = Form("%s/DataAnalysis/MergedData/Output/%s/", mnt.c_str(), a.c_str());
        
        std::vector<std::string> MergedfileNamesVec=util->GetFilesInDirectory(MergedFolderPath,".root");
        // get folder size
        float MDdirsize = 0;
        for (auto f: MergedfileNamesVec){
            // cout << f << endl;
            MDdirsize=filesystem::file_size(MergedFolderPath+f) + MDdirsize;
            // cout << MDdirsize << endl;
        }
        MDdirsize=MDdirsize/1000000000;
        // better to check the ratio of files to size of directory 
        float ratioFilestosize = MergedfileNamesVec.size()/MDdirsize;
        // cout << ratioFilestosize << endl;
        if(MergedfileNamesVec.size() < 1 || MDdirsize < 0.25 || ratioFilestosize > 60 ){
            BadMerged.push_back(a);
            cout << " \n-> Num Files: " << MergedfileNamesVec.size() << " Dir size: " << MDdirsize << " GBs - ratio: " << ratioFilestosize << endl;
        } else {
            cout << " Passed Merged File checks." << endl;
        }
    }
    
    
    
    // FLASHER DATA
    // check that a flasher image for that night exist and that the file size seems okay
    
    for(auto a:allowedFilesVec){
        cout << "Reviewing file: " << a;
        std::string FlasherFilePath = Form("%s/DataAnalysis/flasher_calibration/Output/%s_FlasherCalibration_Factor.root", mnt.c_str(),a.c_str());
        
        float MDdirsize = 0;
        
        try{
            MDdirsize=filesystem::file_size(FlasherFilePath) + MDdirsize;
        } catch (std::filesystem::filesystem_error const& ex){
            cout << " File doesnt exist";
            
        }
        
        MDdirsize= round(MDdirsize/1000);
        // cout << " Size " << MDdirsize << endl;
        
        
        int badpixelscount = 0;
        if (MDdirsize != 0){
            f1 = TFile::Open(FlasherFilePath.c_str());
            if (!f1 || f1->IsZombie())
            {
                cout << "file can not be opened\n" << endl;
                
            }
            h1 = static_cast<TH1F*>(f1->Get("CamFlasher"));
            if (!h1)
            {
                cout << "CamFlasher can not be fetched from f1\n" << endl;
                
            }
            for(int j = 0; j<MaxNofChannels; j++){
                int nx, ny;
                plottools->FindBin(j, &nx, &ny);
                auto value =h1->GetBinContent(nx + 1, ny + 1);
                if (value < 0){
                    badpixelscount = badpixelscount +1;  
                }
            }
            
        }
        
        // cout << "Pixel cout that is " << badpixelscount << endl;
        if((MDdirsize < 100 && MDdirsize != 15 && MDdirsize != 16 && MDdirsize != 9 && MDdirsize != 8 && MDdirsize != 10) || badpixelscount > 4){
            BadFlasher.push_back(a);
            cout << " \n-> File size: " << MDdirsize << "KBs - Bad Pixel #: " << badpixelscount << endl;
        } else {
            // check the actual file
            cout << " Passed FlasherCalibrations File checks." << endl;
        }
    }
    
    
    for(auto a:allowedFilesVec){
        cout << "Reviewing file: " << a;
        std::string ECFilePath = Form("%s/DataAnalysis/event_cleaning/Output/%s/Data_EventCleaning%s_TC_481_TB_239_NP_481_s_256_FA_800_mp_3_er_100_tr_1.root", mnt.c_str(), path_to_EC.c_str(),a.c_str());
        // cout << "folder path: " << ECFilePath <<endl;
        // fileNamesVec=util->GetFilesInDirectory(ECFilePath,".root");
        // cout << "This is the first file: " << fileNamesVec[0] << endl;
        // std::string FlasherFilePath = Form("%s/DataAnalysis/flasher_calibration/Output/%s_FlasherCalibration_Factor.root", mnt.c_str(),a.c_str());
        
        float MDdirsize = 0;

        try{
            MDdirsize=filesystem::file_size(ECFilePath) + MDdirsize;
        } catch (std::filesystem::filesystem_error const& ex){
            cout << " File doesnt exist";
            
        }

        MDdirsize= round(MDdirsize/1000);
        // cout << " Size " << MDdirsize << endl;
        if(MDdirsize < 500 || MDdirsize == 0){
            BadEC.push_back(a);
            cout << " \n-> File size: " << MDdirsize << "MBs " << endl;
        } else {
            // check the actual file
            cout << " Passed EC File checks." << endl;
        }
    }



    // LIST OF BAD FILES

    cout << "------- Bad Merged Folder(s): --------" << endl;
    for(auto r: BadMerged){
        cout << r << endl; 
    }
    cout << "------- Bad Flasher Files(s): --------" << endl;
    cout << " Please note this looks for cookie plots by looking for more than 4 pixels less than 0" << endl;
    for(auto r: BadFlasher){
        cout << r << endl; 
    }
    cout << "------- Bad EC Folder(s): --------" << endl;
    for(auto r: BadEC){
        cout << r << endl; 
    }

    // check the night for the event cleaning exists and that the file size is good 

    // TCanvas* c_cleaned = new TCanvas("c_cleaned", "c", 10, 10, 800, 600);
    // c_cleaned->Print("EventSelectionPlots.pdf[");
    // std::string OutputFileRoot = "EventSelectionPlots.root";
    // TFile *file = new TFile(OutputFileRoot.c_str(), "RECREATE");
    // file->Close();  

    // for (auto f: fileNamesVec) {
    // }
    return 0;
}