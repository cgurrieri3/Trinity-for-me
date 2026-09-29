// R__LOAD_LIBRARY(libExACT.so)
#include "reduceDataSaved.h"



int main(int argc, char **argv) {
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    

    std::string mount = argv[2];
    std::string filename_argument = "";
    filename_argument = argv[3];
    
    if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt="/mnt/";
    } else if (mount != "n"){
        std::cout << "using specific directory path" << std::endl;
        mnt=mount.c_str();
    }
    // Get the Arguments
    std::string folString = argv[1];
    
    std:: string dataDir = Form("%sDataAnalysis/MergedData/Output/",mnt.c_str());
    std:: string outDir = Form("%sDataAnalysis/MergedData/ReducedInfoOutput/%s",mnt.c_str(),folString.c_str());



    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
    std::vector<std::string>fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    cout << "Number of files: " << fileNamesVec.size() << endl;

    if (gSystem->MakeDirectory(outDir.c_str()) != 0) {
        std::cerr << "Failed to create directory: " << outDir.c_str() << std::endl;
    } else {
        std::cout << "Directory created: " << outDir.c_str() << std::endl;
    }	



    
    
    
    // for(int f = 0; f<30; f++){
    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        cout << "Processing file: " << FilePath << endl;
        std::string outputFilePath = Form("%s/Reduced_%s", outDir.c_str(), fileNamesVec[f].c_str());
        
        TFile* file = new TFile(FilePath.c_str(), "READ");
        TFile* outputFile = new TFile(outputFilePath.c_str(), "RECREATE");
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }

        std::vector<std::string> treeNames = {"HLED", "BiFocal", "Forced", "Test"};

        for (const auto& treeName : treeNames) {
            TTree* tree = (TTree*)file->Get(treeName.c_str());
            if (!tree) {
                std::cout << "Tree " << treeName << " not found. Skipping.\n";
                continue;
            }

            
            outputFile->cd();
            // Clone the tree structure only
            TTree* outputTree = tree->CloneTree(0);
            if (!outputTree) {
                std::cerr << "Failed to clone tree: " << treeName << std::endl;
                continue;
            }
            // tree->Close();
            std::cout << "Processing tree: " << treeName << std::endl;

            // Set up input branch
            IEvent* ev = new IEvent();
            // ISiPM* sipmInfo = new ISiPM(256);
            tree->SetBranchAddress("Events", &ev);
            // Create output file for writing
            if (!outputFile->IsOpen()) {
                std::cerr << "Failed to create output file: " << outputFilePath << std::endl;
                continue;
            }
            
            
           
            
            // ULong64_t hv;
            // ULong64_t sipmTemp;
            vector<vector<Int_t>> SignalValue;
            vector<vector<Int_t>> SignalValueUpdated;
            vector<vector<Int_t>> PedstalValue;
            
            Long64_t nEntries = tree->GetEntries();
            for (Long64_t i = 0; i < nEntries; ++i) {
                tree->GetEntry(i);
            
                // hv = ev->GetCoBoTime();
                // sipmTemp = ev->GetUNIXTime();
                // cout << "HV size: " << hv << ", Temp size: " << sipmTemp << endl;
                SignalValue = ev->GetSignalValue();
                Pulse *pulse;

                
                
                // cout << PedstalValue << endl;
                cout << "SignalValue size: " << SignalValue[0].size() << endl;
                vector<Int_t> trace(1, 0);
                cout << "Trace Size: " << trace.size() << endl;
                for (int p = 0; p < SignalValue.size(); p++) {
                    pulse = new Pulse(SignalValue[p]);
                    // cout << pulse->GetPedestal() << endl;
                    // trace[0] = static_cast<Int_t>(pulse->GetPedestal());
                    
                    // To do subtract the pedestal from the signal value if it is not already done you need to look at that
                    trace[0] =SignalValue[p][239];
                    SignalValueUpdated.push_back(trace);
                }
                delete pulse;
                
                ev->SetSignalValue(SignalValueUpdated);

                
                outputTree->Fill();
            }
            outputTree->Write();
            // outputTree->Write("", TObject::kOverwrite);
        }
                


        file->Close();
        outputFile->Close();
        delete outputFile;
        delete file;
        std::cout << "Added SiPM branch to trees in: " << fileNamesVec[f] << std::endl;
    }

    return 0;
}