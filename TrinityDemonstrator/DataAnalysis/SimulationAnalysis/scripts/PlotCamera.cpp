#include "PlotCamera.h"


int main(int argc, char **argv){
        if(argc < 4){
                cout << "Too few arguments; usage: PlotCamera <YYYYMMDD> <simrun> <mount> [GROPT|CARE]" << endl;
                return 1;
        }
    
        // Get the Arguments
        std::string folString = argv[1];
        std::string simrun = argv[2];

        std::string mount = argv[3];

        // 4th argument selects which simulation stage to read. Defaults to GROPT
        // so the existing calls keep working unchanged.
        if (argc > 4) simType = argv[4];
        std::transform(simType.begin(), simType.end(), simType.begin(), ::toupper);
        if (simType != "GROPT" && simType != "CARE"){
                cout << "Unknown simulation type '" << simType << "'; expected GROPT or CARE" << endl;
                return 1;
        }
        cout << "Simulation type: " << simType << endl;

        // ROOT scales the histogram title to fit its box, so the box is widened
        // here; setting the font size alone has no effect on a long title.
        gStyle->SetTitleFontSize(kTitleFontSize);
        gStyle->SetTitleW(0.98);
        gStyle->SetTitleH(0.055);
    
        // The two stages currently live under different simdata folders:
        // GROPT under daviddata, CARE under sofiadata.
        std::string simDataDir = (simType == "CARE") ? "sofiadata" : "daviddata";

        if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
                std::cout << "using mounted directory path" << std::endl;
                mnt="/mnt/";
                dataDir = Form("/mnt/simdata/%s/",simDataDir.c_str());
                outDir = "/mnt/DataAnalysis/SimulationAnalysis/data/";
        } else if (mount != "n"){
                std::cout << "using specific directory path" << std::endl;
                mnt=mount.c_str();
                dataDir = Form("%ssimdata/%s/",mnt.c_str(),simDataDir.c_str());
                outDir = Form("%sDataAnalysis/SimulationAnalysis/data/",mnt.c_str());
        }

        // GROPT, CARE and ISOG all live side by side under the same run folder
        std::string RunDir = Form("%s%s_%s/",dataDir.c_str(),folString.c_str(),simrun.c_str());
        std::string FolderPath = ResolveTiltDir(RunDir, simType);
        if (FolderPath.empty()){
                cout << "No " << simType << " tilt folder under " << RunDir << ", moving on..." << endl;
                cout << "Completed!"<< endl;
                return 0;
        }
        cout << FolderPath << endl;
        std::vector<std::string> fileNamesVec;
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
        for (auto i: fileNamesVec)
                std::cout << i << ' ';
        cout << endl;
        // GROPT and CARE get their own folder under the night so the two sets of
        // outputs stay separate: <outDir>/<YYYYMMDD>/<GROPT|CARE>/
        mkdir(Form("%s%s",outDir.c_str(),folString.c_str()), 0775); // night folder, may already exist
        std::string outRunDir = Form("%s%s/%s/",outDir.c_str(),folString.c_str(),simType.c_str());
        if (mkdir(Form("%s%s/%s",outDir.c_str(),folString.c_str(),simType.c_str()), 0775) == -1)
        {
                cout << "Error : possibly directory already exists" << endl;

        } else{
                cout << "Directory created" <<  endl;

        }
        cout << "Output Folder: " << outRunDir << endl;
        if (fileNamesVec.size()==0){
                cout << "Folder empty moving on..." << endl;
                cout << "Completed!"<< endl;
                return 0;
        }

        // The ISOG output is the same for GROPT and CARE, so resolve it once here
        // instead of re-reading the directory on every event.
        std::string ISOGFolderPath = ResolveTiltDir(RunDir, "ISOG");
        std::string ISOGFilePath;
        if (!ISOGFolderPath.empty()){
                std::vector<std::string> ISOGfileNamesVec;
                ISOGfileNamesVec=util->GetFilesInDirectory(ISOGFolderPath,".txt");
                if (ISOGfileNamesVec.size()>0)
                        ISOGFilePath = Form("%s%s",ISOGFolderPath.c_str(),ISOGfileNamesVec[0].c_str());
        }
        if (ISOGFilePath.empty()){
                cout << "ISOG Folder empty moving on..." << endl;
                cout << "Completed!"<< endl;
                return 0;
        }
        cout << "ISOG file path: " << ISOGFilePath << endl;

        fileOutputPlots = new TFile(Form("%splots_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str()), "RECREATE");
        fileOutputPlots->Close();
        fileOutput = new TFile(Form("%sdatafiles_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str()), "RECREATE");

        // The output trees are made once for the whole run. Previously they were
        // recreated inside the file loop, so only the last file survived the Write.
        fileOutput->cd();
        simRun = new SEvent(1);
        treeSims = new TTree("Sim", "Events tree");
        treeSims->Branch("GrOptics","SEvent",&simRun);
        ev = new IEvent();
        treeNew = new TTree("Test", "Events tree");
        treeNew->Branch("Events","IEvent",&ev);

        canvas = new TCanvas("Display","Event Display",2*kPadPixelsW,kPadPixelsH);
        canvas->Divide(2,1);
        for (int ipad = 1; ipad <= 2; ipad++){
                canvas->cd(ipad)->SetRightMargin(kPadRight);
                canvas->cd(ipad)->SetTopMargin(kPadTop);
        }

        // The square PE camera images and the wide two pane views do not sit well
        // in one document, so each goes to its own PDF.
        //   pdf_<date>_<simrun>_PE.pdf      square 16x16 PE camera images, both stages
        //   pdf_<date>_<simrun>_2panel.pdf  wide 1mm / 6mm views, GROPT only
        // The PE canvas width is picked so the plot area comes out square once the
        // margins are removed: W*(1-kPadLeft-kPadRight) = H*(1-kPadBottom-kPadTop),
        // so with the taller header band W = H*0.72/0.75 = 960 for H = 1000.
        // GROPT gets two documents, both built on the wide two pane canvas:
        //   pdf_<date>_<simrun>_1mm6mm.pdf   1mm next to 6mm, in photons
        //   pdf_<date>_<simrun>_6mmPE.pdf    6mm next to the PE pixel map, both in PE
        // CARE has no 6mm view, so it gets the single square PE camera image:
        //   pdf_<date>_<simrun>_PE.pdf
        std::string pdf1mm6mmPath = Form("%spdf_%s_%s_1mm6mm.pdf",outRunDir.c_str(),folString.c_str(),simrun.c_str());
        std::string pdf6mmPEPath  = Form("%spdf_%s_%s_6mmPE.pdf",outRunDir.c_str(),folString.c_str(),simrun.c_str());
        std::string pdfPEPath     = Form("%spdf_%s_%s_PE.pdf",outRunDir.c_str(),folString.c_str(),simrun.c_str());

        // Square canvas for CARE's standalone PE image. Width chosen so the plot
        // area is square once the margins come out:
        // W*(1-kPadLeft-kPadRight) = H*(1-kPadBottom-kPadTop).
        TCanvas* canvasPE = 0;
        if (simType == "CARE"){
                canvasPE = new TCanvas("PE","PE Camera Display",kPadPixelsW,kPadPixelsH);
                canvasPE->SetLeftMargin(kPadLeft);
                canvasPE->SetRightMargin(kPadRight);
                canvasPE->SetBottomMargin(kPadBottom);
                canvasPE->SetTopMargin(kPadTop);
                SetPdfPageToCanvas(canvasPE);
                canvasPE->Print(Form("%s[", pdfPEPath.c_str()));
        } else {
                SetPdfPageToCanvas(canvas);
                canvas->Print(Form("%s[", pdf1mm6mmPath.c_str()));
                canvas->Print(Form("%s[", pdf6mmPEPath.c_str()));
        }

        // CARE also keeps an overlay of every FADC trace. The branch wrote one
        // traces_*.root per input file; here they accumulate into a single run
        // level histogram so a 4000 file night stays manageable.
        TH2F* hTraces = 0;
        TH2F* hTracesCorr = 0;
        if (simType == "CARE"){
                hTraces = new TH2F("hTraces", "All FADC Traces;Timebin;ADC Counts",
                                   200, -0.5, 199.5, 150, -0.5, 1499.5);
                hTraces->SetStats(0);
                hTraces->SetDirectory(0);

                // the same traces after the full correction that create_root_file_care
                // writes into the IEvent: inverted and shifted onto a pedestal+3400
                // baseline, so pulses run downwards the way the demonstrator records
                // them. The range covers the whole band that correction produces.
                hTracesCorr = new TH2F("hTracesCorrected",
                                       "All FADC Traces AFTER correction;Timebin;ADC Counts (inverted, pedestal+3400 baseline)",
                                       200, -0.5, 199.5, 450, -0.5, 4499.5);
                hTracesCorr->SetStats(0);
                hTracesCorr->SetDirectory(0);
        }

        // GROPT stores ray traced photons in T1, CARE stores FADC traces in Events/T0
        std::string treeName = (simType == "CARE") ? "Events/T0" : "T1";

        for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
                std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
                cout << "file path: " <<FilePath << endl;

                if (!util->isBranchPresentInFile(FilePath, treeName)) {
                        continue; // Skip to the next branch if not present

                }

                TFile* file = TFile::Open(FilePath.c_str(), "READ");
                TTree* tree = (TTree*)file->Get(treeName.c_str());
                if (!tree){
                        cout << "No tree " << treeName << " in " << FilePath << endl;
                        file->Close();
                        delete file;
                        continue;
                }

                Int_t iPhotonsInFocalPlane = 0;
                if (simType == "CARE"){
                        // One branch per pixel, each is a vector<int> of 200 timebins
                        for (int pix = 0; pix < kNPixels; pix++) {
                                tree->SetBranchAddress(Form("vFADCTraces%d", pix), &vFADCTraces[pix]);
                        }
                        // CARE's own photon count, the counterpart of the GROPT photon integral
                        if (tree->GetBranch("iPhotonsInFocalPlane"))
                                tree->SetBranchAddress("iPhotonsInFocalPlane", &iPhotonsInFocalPlane);
                } else {
                        tree->SetBranchAddress("photonX", &vPhotonX);
                        tree->SetBranchAddress("photonY", &vPhotonY);
                        tree->SetBranchAddress("photonDcosX", &vPhotonDcosX);
                        tree->SetBranchAddress("photonDcosY", &vPhotonDcosY);
                }

                // titles carry the file name, so trim the extension to leave the
                // title box room to render at a readable size
                std::string fileLabel = fileNamesVec[f];
                size_t dotPos = fileLabel.rfind(".root");
                if (dotPos != std::string::npos) fileLabel = fileLabel.substr(0,dotPos);

                int nEntries = tree->GetEntries();
                if (nEntries==0) {
                        cout << "No events" << endl;
                        file->Close();
                        delete file;
                        continue;
                }
                cout << "Total number of entries: " << nEntries << endl;

                for(int EventCounter = 0; EventCounter < (nEntries); EventCounter++){
                        cout << "Event number: " << EventCounter << endl;
                        tree->GetEntry(EventCounter);

                        // The PE camera image. Same binning, same axes and same object
                        // name for both pipelines, only the contents are built differently.
                        TH2F* hPhotonDist1xPixels = new TH2F("hPhotonDist1xPixels", Form("Camera View %s, %s, %s;Pixels along camera x axis;Pixels along camera y axis;PE", folString.c_str(),simrun.c_str(),fileLabel.c_str()), CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);

                        double totalPhotons = 0;
                        double totalPE = 0;   // SIZE: summed PE over the camera
                        bool triggered = false;

                        if (simType == "GROPT"){
                                hPhotonX = new TH1F("hist photonX","PhotonX Distribution", 192, -96, 96);
                                hPhotonX->GetXaxis()->SetTitle("Photon X Position (mm)");
                                hPhotonX->GetYaxis()->SetTitle("Number of Photons");
                                hPhotonX->SetStats(0);

                                hPhotonY = new TH1F("hist photonY","PhotonY Distribution", 96, -48, 48);
                                hPhotonY->GetXaxis()->SetTitle("Photon Y Position (mm)");
                                hPhotonY->GetYaxis()->SetTitle("Number of Photons");
                                hPhotonY->SetStats(0);

                                hPhotonDcosX = new TH1F("hist photonDcosX","PhotonDcosX Distribution", 100, -1, 1);
                                hPhotonDcosX->GetXaxis()->SetTitle("PhotonDcosX");
                                hPhotonDcosX->GetYaxis()->SetTitle("Number of Photons");
                                hPhotonDcosX->SetStats(0);

                                hPhotonDcosY = new TH1F("hist photonDcosY","PhotonDcosY Distribution", 100, -1, 1);
                                hPhotonDcosY->GetXaxis()->SetTitle("PhotonDcosY");
                                hPhotonDcosY->GetYaxis()->SetTitle("Number of Photons");
                                hPhotonDcosY->SetStats(0);

                                hPhotonZ = new TH1F("hist photonZ","Photon Incidence Angle Distribution", 180, 0, 90);
                                hPhotonZ->GetXaxis()->SetTitle("Photon Incidence Angle (Deg)");
                                hPhotonZ->GetYaxis()->SetTitle("Number of Photons");
                                hPhotonZ->SetStats(0);

                                hPhotonDist1x = new TH2F("hist1x", Form("Camera View %s, %s, %s, 1mm bins", folString.c_str(),simrun.c_str(),fileLabel.c_str()), 96, -48.5, 47.5, 96, -48.5, 47.5);
                                hPhotonDist1x->GetXaxis()->SetTitle("X (mm)");
                                hPhotonDist1x->GetYaxis()->SetTitle("Y (mm)");
                                hPhotonDist1x->GetZaxis()->SetTitle("Photons");
                                hPhotonDist1x->GetZaxis()->SetTitleOffset(kZTitleOffset);

                                hPhotonDist1x6mm = new TH2F("hist1x6mm", Form("Camera View %s, %s, %s, 6mm bins", folString.c_str(),simrun.c_str(),fileLabel.c_str()), 16, -48.5, 47.5, 16, -48.5, 47.5);
                                hPhotonDist1x6mm->GetXaxis()->SetTitle("X (mm)");
                                hPhotonDist1x6mm->GetYaxis()->SetTitle("Y (mm)");
                                hPhotonDist1x6mm->GetZaxis()->SetTitle("Photons");
                                hPhotonDist1x6mm->GetZaxis()->SetTitleOffset(kZTitleOffset);

                                hPhotonDist4x = new TH2F("hist4x", Form("Camera View %s, %s, %s (4x)", folString.c_str(),simrun.c_str(),fileLabel.c_str()), 32*nResMult, -96.5, 95.5, 16*nResMult, -48.5, 47.5);
                                hPhotonDist4x->GetXaxis()->SetTitle("X (mm)");
                                hPhotonDist4x->GetYaxis()->SetTitle("Y (mm)");
                                hPhotonDist4x->GetZaxis()->SetTitle("Photons");
                                hPhotonDist4x->GetZaxis()->SetTitleOffset(kZTitleOffset);
                                hPhotonDist4x->SetStats(0);

                                for (size_t j = 0; j < vPhotonX->size(); j++)
                                {
                                        hPhotonX->Fill((*vPhotonX)[j]);
                                        hPhotonY->Fill((*vPhotonY)[j]);
                                        hPhotonDcosX->Fill((*vPhotonDcosX)[j]);
                                        hPhotonDcosY->Fill((*vPhotonDcosY)[j]);
                                        hPhotonZ->Fill(TMath::ASin(sqrt(pow((*vPhotonDcosX)[j],2)+pow((*vPhotonDcosY)[j],2)))*RadtoDeg);
                                        hPhotonDist1x->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                                        hPhotonDist1x6mm->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                                        hPhotonDist4x->Fill((*vPhotonX)[j], (*vPhotonY)[j]);

                                }

                                totalPhotons = hPhotonDist1x6mm->Integral();
                                // SIZE, summed the same way the PE map is built below:
                                // photons scaled to PE and clipped at the 160 ceiling
                                for (int ix = 1; ix <= hPhotonDist1x6mm->GetNbinsX(); ++ix)
                                        for (int iy = 1; iy <= hPhotonDist1x6mm->GetNbinsY(); ++iy){
                                                double pe = hPhotonDist1x6mm->GetBinContent(ix,iy)*PtoPEconversion;
                                                totalPE += (pe > 160) ? 160 : pe;
                                        }
                                triggered = (hPhotonDist1x6mm->GetMaximum()*PtoPEconversion >= kTriggerPE);

                        } else { // CARE
                                // Ported from the Sofia_CARE_Simanalysis branch: build the
                                // camera image from the FADC sample at kCareTimeBin, after
                                // subtracting a pedestal taken from the leading samples.
                                for (int pix = 0; pix < kNPixels; pix++) {

                                        if (!vFADCTraces[pix]) continue;

                                        int nSamples = (int)vFADCTraces[pix]->size();
                                        int nPed = std::min(kCarePedBins, nSamples);
                                        int pedestal = 0;
                                        for (int t = 0; t < nPed; t++){
                                                pedestal += (*vFADCTraces[pix])[t];
                                        }
                                        if (nPed > 0) pedestal /= nPed; // average of the first bins
                                        // This stays the RAW pedestal. The demonstrator's ~3400 ADC
                                        // baseline is added where a demonstrator style trace is built
                                        // (the corrected overlay below and create_root_file_care), not
                                        // here: the camera image needs the signal measured above the
                                        // raw baseline, and folding 3400 in here drives every pixel
                                        // negative so nothing ever triggers.
                                        // cout << "Pixel " << pix << ": Pedestal = " << pedestal << " (raw), demonstrator baseline = " << pedestal+3400 << endl;
                                        float val = (nSamples > kCareTimeBin)
                                                        ? (*vFADCTraces[pix])[kCareTimeBin] - pedestal
                                                        : 0.0f;
                                        int nx, ny;
                                        plottools->FindBin(pix, &nx, &ny);
                                        if (val <= 0.0){
                                                val = 0.001;
                                        }
                                        hPhotonDist1xPixels->SetBinContent(nx + 1, ny + 1, val/kCareADCtoPE);

                                        // both trace overlays are filled here, where the
                                        // pedestal for this pixel is already known, and
                                        // before any trigger decision so they stay raw
                                        // readout diagnostics
                                        for (int t = 0; t < nSamples; t++){
                                                hTraces->Fill(t, (*vFADCTraces[pix])[t]);
                                                // exactly what create_root_file_care stores
                                                hTracesCorr->Fill(t, (-1*(*vFADCTraces[pix])[t])+2*pedestal+3400);
                                        }
                                }

                                totalPhotons = iPhotonsInFocalPlane;
                                totalPE = hPhotonDist1xPixels->Integral();
                                triggered = (hPhotonDist1xPixels->GetMaximum() >= kTriggerPE);
                        }

                        // First create the root file for this event
                        simRun->SetSimRun(simrun);
                        simRun->SetDate(std::stoi(folString)); // placeholder for now
                        simRun->SetSimEventNumber(EventCounter);

                        // Flag whether the event has enough signal to trigger. Untriggered
                        // events are still saved to the output, just marked Triggered = false.
                        cout << "Number of photons detected: " << totalPhotons << endl;

                        simRun->SetTriggered(triggered);
                        int pos =  std::string(fileNamesVec[f]).find("E_");
                        pos =  std::string(FilePath).find("Tilt_");
                        simRun->SetTilt(std::stof(std::string(FilePath).substr(pos+5, 9)));
                        if (simType == "GROPT"){
                                simRun->SetPosX(*vPhotonX);
                                simRun->SetPosY(*vPhotonY);
                        } else {
                                // CARE has no per photon positions; clear them so the event
                                // does not inherit the previous fill.
                                simRun->SetPosX(std::vector<Float_t>());
                                simRun->SetPosY(std::vector<Float_t>());
                        }

                        // Same ISOG row for both pipelines, keyed on the entry counter
                        LoadISOG(simRun, ISOGFilePath, EventCounter);

                        treeSims->Fill();
                        if (!triggered) {
                                cout << "Not enough photons detected in this simulation run, saving as untriggered..." << endl;
                                // Clean up this iteration's histograms before skipping so they
                                // are not left registered in the directory for the next event
                                delete hPhotonDist1xPixels;
                                if (simType == "GROPT"){
                                        delete hPhotonX;
                                        delete hPhotonY;
                                        delete hPhotonDcosX;
                                        delete hPhotonDcosY;
                                        delete hPhotonZ;
                                        delete hPhotonDist1x;
                                        delete hPhotonDist1x6mm;
                                        delete hPhotonDist4x;
                                }
                                continue;
                        }

                        // Emergence angle (azimuth wrapped to [-180,180] from the telescope axis)
                        // and distance to the emergence point (magnitude of the telescope position vector)
                        double emergenceAngle = util->GetEmergenceAngle(simRun->GetAzimuthAngle());
                        double emergenceDistance = util->GetEmergenceDistance(simRun->GetTelescope_Xpos(), simRun->GetTelescope_Ypos(), simRun->GetTelescope_Zpos());
                        cout << "Emergence Angle (from telescope axis): " << emergenceAngle << " deg" << endl;
                        cout << "Distance to Emergence Point: " << emergenceDistance << " (units of telescope coords)" << endl;

                        // Second create the plots for each event and save them to a root file

                        // create a lable with neutrino energy

                        fileOutputPlots = new TFile(Form("%splots_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str()), "UPDATE");
                        // two lines: the single line ran off the side of the page
                        TPaveText *ptEnergy = new TPaveText(0.05,kLabelY1,0.95,kLabelY2,"NDC");
                        ptEnergy->SetFillColor(0);
                        ptEnergy->SetTextAlign(12);
                        ptEnergy->SetTextSize(kLabelTextSize);
                        ptEnergy->AddText(Form("Event: %i, Neutrino Energy: %.3g GeV, Emergence Angle: %.3g degrees",
                                               EventCounter, simRun->GetNeutrinoEnergy(), emergenceAngle));
                        ptEnergy->AddText(Form("Shower Distance: %.3g m, Total Photons: %i, SIZE = %.4g PE",
                                               emergenceDistance, int(totalPhotons), totalPE));

                        if (simType == "GROPT"){
                                canvas->Clear();
                                canvas->Divide(2,1);
                                for (int ipad = 1; ipad <= 2; ipad++){
                                        canvas->cd(ipad)->SetRightMargin(kPadRight);
                                        canvas->cd(ipad)->SetTopMargin(kPadTop);
                                }
                                canvas->cd(1);
                                hPhotonDist1x->SetMarkerSize(200.0);
                                hPhotonDist1x->SetStats(0);
                                hPhotonDist1x->Draw("colz");
                                hPhotonDist1x->Write(Form("h_%s_%s_%i_1mm",folString.c_str(),simrun.c_str(),EventCounter));
                                DrawMUSICBoundaries();
                                ptEnergy->Draw("SAME");
                                canvas->cd(2);
                                hPhotonDist1x6mm->SetMarkerSize(200.0);
                                hPhotonDist1x6mm->SetStats(0);
                                hPhotonDist1x6mm->Draw("colz");
                                hPhotonDist1x6mm->Write(Form("h_%s_%s_%i_6mm",folString.c_str(),simrun.c_str(),EventCounter));
                                DrawMUSICBoundaries();

                                canvas->Write(Form("C_%s_%s_%i",folString.c_str(),simrun.c_str(),EventCounter));
                                canvas->Print(pdf1mm6mmPath.c_str()); // 1mm / 6mm page

                                // Scale to PE into the pixel map only. The 6mm histogram is
                                // left in photons so it still reads as 6mm bins when it is
                                // drawn beside the PE map below.
                                // Loop through all bins (excluding underflow/overflow)
                                int nBinsX = hPhotonDist1x6mm->GetNbinsX();
                                int nBinsY = hPhotonDist1x6mm->GetNbinsY();
                                for (int ix = 1; ix <= nBinsX; ++ix) {
                                        for (int iy = 1; iy <= nBinsY; ++iy) {
                                                double content = hPhotonDist1x6mm->GetBinContent(ix, iy)*PtoPEconversion;
                                                if (content > 160){
                                                        content = 160;
                                                }
                                                hPhotonDist1xPixels->SetBinContent(ix,iy,content);
                                        }
                                }
                                // ---- second page: the PE scaled 6mm view beside the PE pixel map,
                                // so the same event can be read off in the same units side by side
                                canvas->Clear();
                                canvas->Divide(2,1);
                                for (int ipad = 1; ipad <= 2; ipad++){
                                        canvas->cd(ipad)->SetRightMargin(kPadRight);
                                        canvas->cd(ipad)->SetTopMargin(kPadTop);
                                }
                                canvas->cd(1);
                                hPhotonDist1x6mm->SetStats(0);
                                hPhotonDist1x6mm->GetXaxis()->SetTitle("X (mm)");
                                hPhotonDist1x6mm->GetYaxis()->SetTitle("Y (mm)");
                                hPhotonDist1x6mm->GetZaxis()->SetTitle("Photons");
                                hPhotonDist1x6mm->GetZaxis()->SetTitleOffset(kZTitleOffset);
                                hPhotonDist1x6mm->Draw("colz");
                                DrawMUSICBoundaries();
                                ptEnergy->Draw("SAME");

                                canvas->cd(2);
                                hPhotonDist1xPixels->SetMarkerSize(200.0);
                                hPhotonDist1xPixels->SetStats(0);
                                hPhotonDist1xPixels->Draw("colz");
                                hPhotonDist1xPixels->Write(Form("h_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                                hPhotonDist1xPixels->GetXaxis()->SetTitle("Pixels along camera x axis");
                                hPhotonDist1xPixels->GetYaxis()->SetTitle("Pixels along camera y axis");
                                hPhotonDist1xPixels->GetZaxis()->SetTitleOffset(kZTitleOffset);
                                hPhotonDist1xPixels->GetZaxis()->SetTitle("PE");
                                plottools->DrawSIABBoundaries();

                                canvas->Write(Form("C_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                                canvas->Print(pdf6mmPEPath.c_str()); // 6mm / PE page

                        } else {
                                // CARE: square canvas, so the 16x16 camera map is not stretched
                                canvasPE->cd();
                                canvasPE->Clear();
                                canvasPE->SetLeftMargin(kPadLeft);
                                canvasPE->SetRightMargin(kPadRight);
                                canvasPE->SetBottomMargin(kPadBottom);
                                canvasPE->SetTopMargin(kPadTop);
                                hPhotonDist1xPixels->SetMarkerSize(200.0);
                                hPhotonDist1xPixels->SetStats(0);
                                hPhotonDist1xPixels->Draw("colz");
                                hPhotonDist1xPixels->Write(Form("h_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                                hPhotonDist1xPixels->GetXaxis()->SetTitle("Pixels along camera x axis");
                                hPhotonDist1xPixels->GetYaxis()->SetTitle("Pixels along camera y axis");
                                hPhotonDist1xPixels->GetZaxis()->SetTitleOffset(kZTitleOffset);
                                hPhotonDist1xPixels->GetZaxis()->SetTitle(Form("PE at timebin %i", kCareTimeBin));
                                hPhotonDist1xPixels->SetMinimum(0.0);
                                plottools->DrawSIABBoundaries();
                                ptEnergy->Draw("SAME");
                                canvasPE->Write(Form("C_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                                canvasPE->Print(pdfPEPath.c_str()); // PE camera page
                        }
                        gROOT->cd(); // restore root directory before touching the output files

                        // close the plots file for now
                        fileOutputPlots->Close();

                        if (simType == "CARE")
                                create_root_file_care(hPhotonDist1xPixels,treeNew,vFADCTraces);
                        else
                                create_root_file(hPhotonDist1xPixels,treeNew);

                        delete ptEnergy;
                        delete hPhotonDist1xPixels;
                        if (simType == "GROPT"){
                                delete hPhotonX;
                                delete hPhotonY;
                                delete hPhotonDcosX;
                                delete hPhotonDcosY;
                                delete hPhotonZ;
                                delete hPhotonDist1x;
                                delete hPhotonDist1x6mm;
                                delete hPhotonDist4x;
                        }

                }

                file->Close();
                delete file;

        }
        // close the PDFs
        if (simType == "CARE"){
                canvasPE->Print(Form("%s]", pdfPEPath.c_str()));
                util->setFilePermissions(pdfPEPath);
                cout << "PE PDF written: " << pdfPEPath << endl;
        } else {
                canvas->Print(Form("%s]", pdf1mm6mmPath.c_str()));
                util->setFilePermissions(pdf1mm6mmPath);
                cout << "1mm/6mm PDF written: " << pdf1mm6mmPath << endl;
                canvas->Print(Form("%s]", pdf6mmPEPath.c_str()));
                util->setFilePermissions(pdf6mmPEPath);
                cout << "6mm/PE PDF written: " << pdf6mmPEPath << endl;
        }

        if (simType == "CARE"){
                // The trace overlay gets its own file, matching the traces_*.root
                // convention, so it is not buried among the event displays.
                std::string tracesPath = Form("%straces_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str());
                TFile* fileTraces = new TFile(tracesPath.c_str(), "RECREATE");
                TCanvas* canvasTraces = new TCanvas("Traces","FADC Traces",1600,800);
                canvasTraces->SetRightMargin(0.15);
                hTraces->Draw("colz");
                hTraces->Write(Form("hTraces_%s_%s",folString.c_str(),simrun.c_str()));
                canvasTraces->Write(Form("CTraces_%s_%s",folString.c_str(),simrun.c_str()));

                // the same overlay after the pedestal correction
                TCanvas* canvasTracesCorr = new TCanvas("TracesCorrected","FADC Traces AFTER correction",1600,800);
                canvasTracesCorr->SetRightMargin(0.15);
                hTracesCorr->Draw("colz");
                hTracesCorr->Write(Form("hTracesCorrected_%s_%s",folString.c_str(),simrun.c_str()));
                canvasTracesCorr->Write(Form("CTracesCorrected_%s_%s",folString.c_str(),simrun.c_str()));
                fileTraces->Close();
                util->setFilePermissions(tracesPath);
                cout << "Trace overlay written: " << tracesPath << endl;
                delete canvasTraces;
                delete canvasTracesCorr;
                delete fileTraces;
                delete hTraces;
                delete hTracesCorr;
        }
        if (canvasPE) delete canvasPE;

        fileOutput->cd();
        treeSims->Write();
        treeNew->Write();
        fileOutput->Close();
        util->setFilePermissions(Form("%sdatafiles_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str()));
        util->setFilePermissions(Form("%splots_%s_%s.root",outRunDir.c_str(),folString.c_str(),simrun.c_str()));

        delete simRun;
        delete ev;
        delete fileOutput;
        delete fileOutputPlots;
        delete canvas;
        cout << "Completed!" << endl;   
}


void create_root_file(TH2F* hist, TTree* treeNew) {


    ev->SetCoBoTime(ULong64_t( 174000000000000ULL));
    ev->SetUNIXTime(ULong64_t(0ULL));
    ev->SetTBTime(ULong64_t( 174000000000000ULL));
    ev->SetEventType(ULong64_t(3));
    std::vector<vector<Int_t>> pixels;
    for(int i =0; i < 256; i++){
        std::vector<Int_t> trace;    
        for(int j = 0; j<512; j++) {
            trace.push_back(160.0);
        }
        // get the pixel nx, and ny

        int nx,ny;
        plottools->FindBin(i,&nx,&ny);
        // cout << "pixel:" << i << "bin content: " << hist->GetBinContent(nx+1,ny+1) << endl;
        // trace[239] = 160.0 - hist->GetBinContent(nx+1,ny+1);
        if (hist->GetBinContent(nx+1,ny+1) > 160){
                trace[239] = 160.0 - 160.0;

        } else {
                trace[239] = 160.0 - hist->GetBinContent(nx+1,ny+1);

        }
        // cout << trace[239] << endl;
        pixels.push_back(trace);
    }
    ev->SetSignalValue(pixels);
    int maxBinX, maxBinY, dummy;
    hist->GetMaximumBin(maxBinX, maxBinY, dummy);
    hist->GetBinContent(maxBinX, maxBinY);
//     cout << "Max bin content: " << hist->GetBinContent(maxBinX, maxBinY) << endl;
    int MusicID = plottools->FindMUSIC(maxBinX-1, maxBinY-1);

    ev->SetROIMusicID({MusicID,-1});
    ev->SetROIPixelID({1});
    std::vector<float> temp = {1.0};
    std::string temp_str = "";
    ev->SetParametersFromTimestamp(temp_str,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp);
    
    
    treeNew->Fill();

}


// CARE writes the real FADC waveforms into the IEvent, ported from the
// Sofia_CARE_Simanalysis branch. Each 200 sample trace is inverted and shifted
// so the pulse reads the way the demonstrator records it, then centred inside
// the 512 sample window with the rest held at the pixel baseline.
void create_root_file_care(TH2F* hist, TTree* treeNew, std::vector<int>* traces[]) {

    ev->SetCoBoTime(ULong64_t( 174000000000000ULL));
    ev->SetUNIXTime(ULong64_t(0ULL));
    ev->SetTBTime(ULong64_t( 174000000000000ULL));
    ev->SetEventType(ULong64_t(3));

    const int traceLen = 512;
    const int fadcLen  = 200;
    const int offset   = (traceLen - fadcLen) / 2; // = 156, centers the 200-bin trace in 512

    std::vector<vector<Int_t>> pixels;
    for (int i = 0; i < 256; i++) {

        // pedestal baseline for this pixel's trace, same as the camera image uses
        int nSamples = traces[i] ? (int)traces[i]->size() : 0;
        int nPed = std::min(kCarePedBins, nSamples);
        int pedestal = 0;
        for (int t = 0; t < nPed; t++){
                pedestal += (*traces[i])[t];
        }
        if (nPed > 0) pedestal /= nPed; // average of the first bins

        std::vector<Int_t> trace(traceLen, pedestal+3400); // fill with pedestal baseline

        // Embed the 200-bin FADC trace centered in the 512-bin trace
        for (int t = 0; t < fadcLen && t < nSamples; t++) {
            trace[offset + t] = (-1*(*traces[i])[t])+2*pedestal+3400; // subtract pedestal to center around zero and then shift to match the demonstrator
        }
        pixels.push_back(trace);
    }
    ev->SetSignalValue(pixels);

    int maxBinX, maxBinY, dummy;
    hist->GetMaximumBin(maxBinX, maxBinY, dummy);
    int MusicID = plottools->FindMUSIC(maxBinX-1, maxBinY-1);

    ev->SetROIMusicID({MusicID,-1});
    ev->SetROIPixelID({1});
    std::vector<float> temp = {1.0};
    std::string temp_str = "";
    ev->SetParametersFromTimestamp(temp_str,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp);

    treeNew->Fill();

}


// ROOT always writes an A4 MediaBox for PDF, but gStyle's paper size still sets
// how large the canvas is drawn on it. Left at the default the wide two pane
// canvas is letterboxed into a strip; set larger than the sheet and it runs off
// the edge. So the drawing area is fitted to the canvas aspect inside the usable
// A4 area, which ROOT orients by that same aspect (wide -> landscape).
void SetPdfPageToCanvas(TCanvas *c) {
    double aspect = (double)c->GetWw() / (double)c->GetWh();
    // usable drawing area, mirroring ROOT's own 20x26 default for portrait
    double maxW = (aspect >= 1.0) ? kPdfAreaLongCm  : kPdfAreaShortCm;
    double maxH = (aspect >= 1.0) ? kPdfAreaShortCm : kPdfAreaLongCm;
    double w = std::min(maxW, maxH * aspect);
    gStyle->SetPaperSize(w, w / aspect);
}
