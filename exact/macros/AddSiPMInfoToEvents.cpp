R__LOAD_LIBRARY(libExACT.so)
#include <iostream>
#include <vector>
#include <string>

int AddSiPMInfoToEvents(std::string inputFileName) {
    // Open input file in UPDATE mode
    TFile* file = new TFile(inputFileName.c_str(), "UPDATE");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open input file!" << std::endl;
        return 1;
    }

    const double GAIN_REF = 22.1; // at 25°C

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

        // Create new branch
        ISiPM* sipmInfo = new ISiPM(256);
        TBranch* sipmBranch = tree->Branch("SiPM", "ISiPM", &sipmInfo, 64000, 99);

        std::vector<float> hv;
        std::vector<float> sipmTemp;
        std::vector<double> absoluteGain(256);
        std::vector<double> tempCorrection(256);
        std::vector<double> relOverVoltage(256);
        std::vector<double> amplitudeToPE(256);

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
                delete pulse;

                absoluteGain[j] = GAIN_REF * (1.0 / tempCorrection[j]);
                amplitudeToPE[j] = amplitude / absoluteGain[j];
                relOverVoltage[j] = relOverV;
                
                //cout<<i<<"    "<<j<<"    "<<amplitude<<"    "<<sipmTemp[j/16]<<"    "<<tempCorrection[j]<<"    "<<amplitudeToPE[j]<<"    "<<absoluteGain[j]<<endl;
            }

            sipmInfo->SetGain(absoluteGain);
            sipmInfo->SetTCorrection(tempCorrection);
            sipmInfo->SetAmplToPE(amplitudeToPE);
            sipmInfo->SetRelOverV(relOverVoltage);

            sipmBranch->Fill();
        }

        tree->Write("", TObject::kOverwrite);

        delete ev;
        delete sipmInfo;
    }

    file->Close();
    std::cout << "Added SiPM branch to trees in: " << inputFileName << std::endl;
    return 0;
}
