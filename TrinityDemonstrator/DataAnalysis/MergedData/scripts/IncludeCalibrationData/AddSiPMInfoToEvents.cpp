//R__LOAD_LIBRARY(libExACT.so)
#include "AddSiPMInfoToEvents.h"



int main(int argc, char **argv) {
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
    std::string mount = "";
    mount = argv[2];
    std::string filename_argument = "";
    filename_argument = argv[3];

    if (mount != ""){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt=mount.c_str();
        dataDir = Form("%sDataAnalysis/MergedData/Output/",mnt.c_str());
    }

    // Get the Arguments
    std::string folString = argv[1];
    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
    std::vector<std::string>fileNamesVec;
    cout << "Folder Path: " << FolderPath << endl;
    if (filename_argument != "Merged_n"){ // if the file name not specified then do all files in the directory
        std::cout << "using specific file name" << std::endl;
        // std::string specificfile = Form("%s%s",FolderPath.c_str(),filename_argument.c_str());
        fileNamesVec.push_back(filename_argument);
    } else {
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    }
    cout << "Number of files: " << fileNamesVec.size() << endl;

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        cout << "Processing file: " << FilePath << endl;
        
        TFile* file = new TFile(FilePath.c_str(), "UPDATE");
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }
    

        const double GAIN_REF = 22.1; // at 25�C

        std::vector<std::string> treeNames = {"HLED", "BiFocal", "Forced", "Test"};

        for (const auto& treeName : treeNames) {
            TTree* tree = (TTree*)file->Get(treeName.c_str());
            if (!tree) {
                std::cout << "Tree " << treeName << " not found. Skipping.\n";
                continue;
            }

            std::cout << "Processing tree: " << treeName << std::endl;

            // Set up input branch
            IEvent* ev = new IEvent();
            tree->SetBranchAddress("Events", &ev);

            // Ensure the file is the current directory before creating new branches
            file->cd();

            // Create new branch
            ISiPM* sipmInfo = new ISiPM(256);
            TBranch* sipmBranch = tree->Branch("SiPM", "ISiPM", &sipmInfo, 64000, 99);

            std::vector<float> hv;
            std::vector<float> sipmTemp;
            std::vector<double> absoluteGain(256);
            std::vector<double> tempCorrection(256);
            std::vector<double> relOverVoltage(256);
            std::vector<double> amplitudeToPE(256);
            std::vector<double> pedestal(256);

            double hvSettingPx, relOverV, overV;

            Long64_t nEntries = tree->GetEntries();
            for (Long64_t i = 0; i < nEntries; ++i) {
                tree->GetEntry(i);

                hv = ev->Gethv();
                sipmTemp = ev->GetSiPMTemp();

                for (int j = 0; j < 256; ++j) {
                    hvSettingPx = hv[IUtilities::GetHVChannel(j) - 1];
                    relOverV = ICalibration::GetRelativeOverVoltage(j, hvSettingPx, 44);
                    overV = ICalibration::GetOverVoltage(j, hvSettingPx);
                    tempCorrection[j] = ICalibration::GetRelativeGain(j, sipmTemp[j / 16], hvSettingPx);

                    Pulse* pulse = new Pulse(ev->GetSignalValue(j));
                    double amplitude = pulse->GetAmplitude();
                    double ped = pulse->GetPedestal();
                    // cout << ped << endl;
                    delete pulse;

                    absoluteGain[j] = GAIN_REF * (1.0 / tempCorrection[j]);
                    amplitudeToPE[j] = amplitude / absoluteGain[j];
                    relOverVoltage[j] = relOverV;
                    pedestal[j] = ped;
                }

                sipmInfo->SetGain(absoluteGain);
                sipmInfo->SetTCorrection(tempCorrection);
                sipmInfo->SetAmplToPE(amplitudeToPE);
                // cout << pedestal[0] << endl;
                sipmInfo->SetPedestal(pedestal);
                sipmInfo->SetRelOverV(relOverVoltage);

                sipmBranch->Fill();

                // auto pedestalVec = sipmInfo->GetPedestal();
                // cout <<"yo" <<pedestalVec[0] << endl;
            }
            // tree->Write();
            tree->Write("", TObject::kOverwrite);

            delete ev;
            delete sipmInfo;
        }
        file->Close();
        std::cout << "Added SiPM branch to trees in: " << fileNamesVec[f] << std::endl;
    }

    return 0;
}