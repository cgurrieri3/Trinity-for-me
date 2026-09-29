#include "TFile.h"
#include "TH1F.h"

int FailedRFile(){
    TFile *file = new TFile("/storage/hive/project/phy-otte/nlew3/exact/incomplete.root", "RECREATE");
    TH1F *hist = new TH1F("h1", "Test Histogram", 100, -4, 4);
    hist->FillRandom("gaus", 1000);

    hist->Write();  // Start writing
    // Program ends here without calling file->Close() or deleting the file.
    while (true);  // Infinite loop (terminate with Ctrl+C or a kill command)
    return 0;
}
