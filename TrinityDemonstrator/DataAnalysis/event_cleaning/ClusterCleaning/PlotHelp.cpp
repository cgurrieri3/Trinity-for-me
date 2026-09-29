#include "PlotHelp.h"


// Constructor
PlotHelp::PlotHelp() {
}


void PlotHelp::AddtoWL(double WLRatio){
    WLvector.push_back(WLRatio);
}

void PlotHelp::AddtoL(double L){
    Lvector.push_back(L);
}
void PlotHelp::AddtoW(double W){
    Wvector.push_back(W);
}

void PlotHelp::AddtoSize(double size){
    Svector.push_back(size);
}

void PlotHelp::AddtoConcentation(double conc){
    Cvector.push_back(conc);
}

void PlotHelp::AddtoSurvivingPixelCount(int count){
    SPCvector.push_back(count);
    
}

void PlotHelp::AddtoCOGx(double COGx){
    COGxvector.push_back(COGx);
}
void PlotHelp::AddtoCOGy(double COGy){
    COGyvector.push_back(COGy);
}

void PlotHelp::AddtoTriggeredPixelsID(int id){
    TPID.push_back(id);
}

void PlotHelp::AddtoEventDate(int date){
    Datevecctor.push_back(date);
}

void PlotHelp::AddtoRMSratioAVGamp(double rmsoveravg){
    RMSoverAVGvector.push_back(rmsoveravg);
}

void PlotHelp::AddtoCoreRatio(double cr){
    Corevector.push_back(cr);
}

void PlotHelp::AddtoNumberOfCores(double num){
    NCorevector.push_back(num);
}

void PlotHelp::AddEventFlags(int i) {
    Flagvector.push_back(i);
    // sims only: bin the current event's energy by whether it survived cleaning (flag 5)
    if (trackSimEnergy) {
        if (i == 5) {
            SimEnergySavedVector.push_back(currentSimEnergy);
        } else {
            SimEnergyNotSavedVector.push_back(currentSimEnergy);
        }
    }
}

void PlotHelp::SetSimEnergy(float energy, bool isSim) {
    currentSimEnergy = energy;
    trackSimEnergy = isSim;
}

void PlotHelp::AddtoDistance2MajorAxis(double d){
    Distance2MajorAxisVector.push_back(d);
}

void PlotHelp::AddtoRMSMajorAxis(double r){
    RMSMajorAxisVector.push_back(r);
}

void PlotHelp::AddtoWeightedRMSMajorAxis(double w){
    WeightedRMSMajorAxisVector.push_back(w);
}
void PlotHelp::AddtoPixelsonMajorAxis(int p){
    PixelsonMajorAxisVector.push_back(p);
}
void PlotHelp::AddtoPixelsoffMajorAxis(int p){
    PixelsoffMajorAxisVector.push_back(p);
}
void PlotHelp::AddtoRatioPixelsMajorAxis(double r){
    RatioPixelsMajorAxisVector.push_back(r);
}
void PlotHelp::AddtoRatioPixelsMajorAxisPerPixel(double r, int nPixels){
    for (int i = 0; i < nPixels; i++) {
        RatioPixelsMajorAxisPerPixelVector.push_back(r);
    }
}

void PlotHelp::AddtoSurvivingPixelPosition(double nx, double ny){
    SurvivingPixelXVector.push_back(nx);
    SurvivingPixelYVector.push_back(ny);
}

void PlotHelp::AddtoAngle(double angleRad){
    Anglevector.push_back(angleRad*(180.0/TMath::Pi()));
}

void PlotHelp::AddtoUpDownSym(double s){
    UpDownSymvector.push_back(s);
}

void PlotHelp::AddtoSaturatedPixels(int count){
    SatPixelCountVector.push_back(count);
}

void PlotHelp::AddtoSaturatedPixelPosition(double nx, double ny){
    SaturatedPixelXVector.push_back(nx);
    SaturatedPixelYVector.push_back(ny);
}
// flag = 0 default flag
    // 1, pre cleaned before first cut
    // 2, cleaned after  first cuts (panel2)
    // 3, cleaned after second  cuts(panel 3)
    // 4, cleaned after 3 set of  cut (panel 4)
    // 5 Suviving Event from the  event cleaning
    // 9, HLED event
std::map<std::string, int> PlotHelp::GetEventFlags() const {
    std::map<std::string, int> Eventdict;
    Eventdict["Total"] = Flagvector.size();
    Eventdict["Flasher"] = 0;
    Eventdict["Survived"] = 0;
    Eventdict["PreCleaned"] = 0;
    Eventdict["Panel2_Cleaned"] = 0;
    Eventdict["Panel3_Cleaned"] = 0;
    Eventdict["Panel4_Cleaned"] = 0;

    for (size_t i = 0; i < Flagvector.size(); ++i) {
        if (Flagvector[i] == 9) {
            Eventdict["Flasher"]++;
        } else if (Flagvector[i] == 5) {
            Eventdict["Survived"]++;
        } else if (Flagvector[i] == 1) {
            Eventdict["PreCleaned"]++;
        } else if (Flagvector[i] == 2) {
            Eventdict["Panel2_Cleaned"]++;
        } else if (Flagvector[i] == 3) {
            Eventdict["Panel3_Cleaned"]++;
        } else if (Flagvector[i] == 4) {
            Eventdict["Panel4_Cleaned"]++;
        }
    }
    
    
    for (auto it : Eventdict)
        std::cout << it.first << " Events: " << it.second << std::endl;

    return Eventdict;

}

void PlotHelp::PlotEventFlags(TCanvas* c, std::string pdf){
    TH1D* hEventFlag = new TH1D("hEventFlag", "Event Flags", 10, 0, 10);
    hEventFlag->SetStats(0);
    // hEventFlag->SetXTitle("Event Flag");
    hEventFlag->SetYTitle("Event");
    hEventFlag->SetFillColor(8);

    auto eventFlags = GetEventFlags();
    for (const auto& flag : eventFlags) {
        hEventFlag->Fill(flag.first.c_str(), flag.second);
    }

    c->cd(0);
    hEventFlag->Draw("B1 Text");
    c->Write("EventFlags");
    hEventFlag->Write("EventFlagsTH1D");
    c->Print(pdf.c_str());
    delete hEventFlag;
}

// Sims only: overlay the energy distribution of saved (flag 5) vs removed events.
// Safe to call for non-sim runs: both vectors are empty so it returns without plotting.
void PlotHelp::PlothSimEnergySavedVsNotSaved(TCanvas* c, std::string pdf){
    if (SimEnergySavedVector.empty() && SimEnergyNotSavedVector.empty()) {
        return; // not a sim run, or no events processed
    }

    // common axis range across both samples; energy spans decades so use log10 (skip non-positive)
    bool first = true;
    double emin = 0.0, emax = 0.0;
    auto scan = [&](const std::vector<double>& v){
        for (double e : v){
            if (e <= 0) continue; // log10 undefined
            if (first){ emin = emax = e; first = false; }
            else { if (e < emin) emin = e; if (e > emax) emax = e; }
        }
    };
    scan(SimEnergySavedVector);
    scan(SimEnergyNotSavedVector);
    if (first) return; // no positive energies to plot

    // bin in log space, snapped to whole decades, 10 bins per decade
    double lo = TMath::Floor(TMath::Log10(emin));
    double hi = TMath::Ceil(TMath::Log10(emax));
    if (hi <= lo) hi = lo + 1.0; // at least one decade wide
    int nbins = static_cast<int>((hi - lo) * 10);
    if (nbins < 1) nbins = 1;

    // axis range is given in log10 exponents; LogBinning() converts edges to powers of ten
    TH1D* hSaved    = new TH1D("hSimEnergySaved",    "Sim Energy: Saved vs Removed;Neutrino Energy;Events", nbins, lo, hi);
    TH1D* hNotSaved = new TH1D("hSimEnergyNotSaved", "Sim Energy: Saved vs Removed;Neutrino Energy;Events", nbins, lo, hi);
    hSaved->SetStats(0);
    hNotSaved->SetStats(0);
    LogBinning(hSaved);
    LogBinning(hNotSaved);

    for (double e : SimEnergySavedVector)    hSaved->Fill(e);
    for (double e : SimEnergyNotSavedVector) hNotSaved->Fill(e);

    hNotSaved->SetLineColor(kRed);
    hNotSaved->SetFillColorAlpha(kRed+2, 0.35);
    hNotSaved->SetLineWidth(2);
    hSaved->SetLineColor(kGreen+2);
    hSaved->SetFillColorAlpha(kGreen+2, 0.35);
    hSaved->SetLineWidth(2);

    c->cd(0);
    double ymax = std::max(hSaved->GetMaximum(), hNotSaved->GetMaximum());
    hNotSaved->SetMaximum(ymax * 1.5); // headroom for log-y
    hNotSaved->SetMinimum(0.5);        // keep empty bins off the log-y axis
    hNotSaved->Draw("HIST");
    hSaved->Draw("HIST SAME");

    TLegend* leg = new TLegend(0.62, 0.75, 0.88, 0.88);
    leg->AddEntry(hSaved,    Form("Saved (%d)",   (int)SimEnergySavedVector.size()),    "f");
    leg->AddEntry(hNotSaved, Form("Removed (%d)", (int)SimEnergyNotSavedVector.size()), "l");
    leg->Draw();

    c->Update();
    c->SetLogx();
    c->SetLogy();
    c->Write("SimEnergySavedVsRemoved");
    hSaved->Write("SimEnergySavedTH1D");
    hNotSaved->Write("SimEnergyNotSavedTH1D");
    c->Print(pdf.c_str());

    delete leg;
    delete hSaved;
    delete hNotSaved;
    c->SetLogx(0); // reset for any plots that follow
    c->SetLogy(0);
}

void PlotHelp::PlothWL(TCanvas* c, std::string pdf,std::string outDir,std::string date){
    TH1D* hWL = new TH1D("hWL", "Distribution of Width Length Ratio",ONEstep, ONEmin, ONEmax);
    hWL->SetStats(0);
    hWL->SetXTitle("Width Length Ratio");
    for (std::vector<double>::size_type h = 0; h < WLvector.size(); h++) {
        hWL->Fill(WLvector[h]);
    }
    // hWL->SetXaxis()->SetLabelSize(0.03);
    // hWL->SetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    // hWL->SetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    c->cd(0);
    hWL->Draw("HIST E1");
    //Create a TLatex object to display the label
    TLatex *latex = new TLatex();
    latex->SetNDC(); // Use normalized coordinates
    latex->SetTextSize(0.02); // Set the text size
    std::map<std::string, int> eventFlagsDict = GetEventFlags();

    latex->DrawLatex(0.1, 0.83, Form("Flasher Events #: %i", eventFlagsDict["Flasher"]));
    latex->DrawLatex(0.1, 0.85, Form("Events #: %i", eventFlagsDict["Total"]-eventFlagsDict["Flasher"]));
    latex->DrawLatex(0.1, 0.81, Form("Survived Events #: %i", eventFlagsDict["Survived"]));
    latex->DrawLatex(0.1, 0.79, Form("PreCleaned Cleaned #: %i", eventFlagsDict["PreCleaned"]));
    latex->DrawLatex(0.1, 0.77, Form("Panel 2 Cleaned #: %i", eventFlagsDict["Panel2_Cleaned"]));
    latex->DrawLatex(0.1, 0.75, Form("Panel 3 Cleaned #: %i", eventFlagsDict["Panel3_Cleaned"]));
    delete latex;
    c->Update();
    c->SetLogy();
    c->Write("WL_Distribution");
    hWL->Write("WL_DistributionTH1D");
    c->Print(pdf.c_str());
    c->SetLogy(0);
    delete hWL;

    // std::ofstream outputFile1(Form("%sTotalEvents_%s.txt",outDir.c_str(),date.c_str()));
    // if (outputFile1.is_open()) {
    //     outputFile1 << "Triggered,Flasher,Survived,Pre,Panel2,Panel3" << "\n";
    //     outputFile1 << TotalEvents-HLEDEvents << "," << HLEDEvents<<","<< SurvivingEvents << "," << PreCleanedEvents << "," << Panel2CleanedEvents << "," << Panel3CleanedEvents <<"\n";
    //     outputFile1.close();
    // } else {
    //     std::cerr << "Unable to open file for writing." << std::endl;
    // }
}

void PlotHelp::PlothdistLandW(TCanvas* c, std::string pdf){
    TH1D* hW = new TH1D("hL", "Seperate Distribution of Width and Length",4*ONEstep, ONEmin, ONEmax+1);
    TH1D* hL = new TH1D("hW", "Distribution of Length ",4*ONEstep, ONEmin, ONEmax+1);
    hW->SetStats(0);
    hL->SetStats(0);
    hW->SetXTitle("Value");
    hL->SetXTitle("Value");
    for (std::vector<double>::size_type h = 0; h < Wvector.size(); h++) {
        hW->Fill(Wvector[h]);
        hL->Fill(Lvector[h]);
    }
    // hWL->SetXaxis()->SetLabelSize(0.03);
    // hWL->SetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    // hWL->SetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    c->cd(0);
    hW->SetLineColor(kBlue);
    hW->SetFillColor(38);
    hW->SetFillStyle(3017);
    hW->Draw("HIST E1");
    
    hL->SetLineColor(kRed);
    hL->SetFillColor(46);
    hL->SetFillStyle(3017);
    hL->Draw("HIST E1 SAME");
    // //Create a TLatex object to display the label
    // TLatex *latex = new TLatex();
    // latex->SetNDC(); // Use normalized coordinates
    // latex->SetTextSize(0.02); // Set the text size
    
    // latex->DrawLatex(0.1, 0.83, Form("Flasher Events #: %i", HLEDEvents));
    // latex->DrawLatex(0.1, 0.85, Form("Events #: %i", TotalEvents-HLEDEvents));
    // latex->DrawLatex(0.1, 0.81, Form("Survived Events #: %i", SurvivingEvents));
    // latex->DrawLatex(0.1, 0.79, Form("PreCleaned Cleaned #: %i", PreCleanedEvents));
    // latex->DrawLatex(0.1, 0.77, Form("Panel 2 Cleaned #: %i", Panel2CleanedEvents));
    // latex->DrawLatex(0.1, 0.75, Form("Panel 3 Cleaned #: %i", Panel3CleanedEvents));
    // delete latex;
    
    auto legend = new TLegend(0.1,0.8,0.34,0.9);
    // legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
    legend->AddEntry(hL,"Length","f");
    legend->AddEntry(hW,"Width","f");
    legend->Draw();
    c->Update();
    c->SetLogy();
    c->Write("WandL_Distribution");
    hW->SetTitle("Distribution of Width");
    hW->SetXTitle("Width");
    hL->SetXTitle("Length");
    hW->Write("W_DistributionTH1D");
    hL->Write("L_DistributionTH1D");
    c->Print(pdf.c_str());
    c->SetLogy(0);

}

void PlotHelp::PlothSize(TCanvas* c, std::string pdf){
    TH1D* hSize = new TH1D("hSize", "Size Distribution of Surviving Pixels", SIZEstep, SIZEmin,SIZEmax );
    hSize->SetStats(0);
    hSize->SetXTitle("SIZE (PE)");
    
    LogBinning(hSize);

    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hSize->Fill(Svector[h]);
    }
    c->cd(0);
    hSize->Draw("HIST E1");
    c->Update();
    c->SetLogx();
    c->SetLogy();
    c->Write("Size_Distribution");
    hSize->Write("Size_DistributionTH1D");
    c->Print(pdf.c_str());
    delete hSize;
    c->SetLogx(0);
    c->SetLogy(0);
}

void PlotHelp::PlothNumberofCores(TCanvas* c, std::string pdf){
    TH1D* hncore = new TH1D("hncore", "Distribution of NUmber of Cores", nCoreStep, nCoreMin,nCoreMax );
    hncore->SetStats(0);
    hncore->SetXTitle("Number of Cores");
    
    // LogBinning(hncore);

    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hncore->Fill(NCorevector[h]);
    }
    c->cd(0);
    hncore->Draw("HIST E1");
    c->Update();
    c->SetLogy();
    c->Write("Ncore_Distribution");
    hncore->Write("NCore_DistributionTH1D");
    c->Print(pdf.c_str());
    delete hncore;
    c->SetLogx(0);
    c->SetLogy(0);
}

void PlotHelp::PlothCoreRatioandNumberofCores(TCanvas* c, std::string pdf){
    TH2F* hcrvnc = new TH2F("hcrvnc", "Core Ratio vs Number of cores;Number of cores;Core Ratio",nCoreStep, nCoreMin,nCoreMax,4*ONEstep,4*ONEmin,4*ONEmax);
    hcrvnc->SetStats(0);

    for (std::vector<double>::size_type h = 0; h < Lvector.size(); h++) {
        hcrvnc->Fill(NCorevector[h],Corevector[h]);
    }
    c->cd(0);
    hcrvnc->Draw("COLZ");
    c->Update();
    c->Write("crvnc");
    hcrvnc->Write("crvncTH2F");
    c->Print(pdf.c_str());
    delete hcrvnc;
}



void PlotHelp::PlothRMSratioAvg(TCanvas* c, std::string pdf) {
    TH1D* hRMS = new TH1D("hRMS", "Ratio of RMS with Avg Amp", ONEstep*10, ONEmin,ONEmax );
    hRMS->SetStats(0);
    hRMS->SetXTitle("RMS/Avg Amplitude");
    for (std::vector<double>::size_type h = 0; h < RMSoverAVGvector.size(); h++) {
        hRMS->Fill(RMSoverAVGvector[h]);
    }
    c->cd(0);
    hRMS->Draw();
    c->Update();
    c->Write("RMSratioAvg_Distribution");
    hRMS->Write("RMSratioAvg_DistributionTH1D");
    c->Print(pdf.c_str());
    delete hRMS;
}

void PlotHelp::PlothLvW(TCanvas* c, std::string pdf){
    TH2F* hLvW = new TH2F("hLvW", "Length vs Width; Width;Length",4*ONEstep, ONEmin, ONEmax+1,4*ONEstep, ONEmin, ONEmax+1);
    hLvW->SetStats(0);

    for (std::vector<double>::size_type h = 0; h < Lvector.size(); h++) {
        hLvW->Fill(Wvector[h],Lvector[h]);
    }
    c->cd(0);
    hLvW->Draw("COLZ");
    c->Update();
    c->Write("LvW");
    hLvW->Write("LvWTH2F");
    c->Print(pdf.c_str());
    delete hLvW;
}



void PlotHelp::PlothSizeConc(TCanvas* c, std::string pdf){
    TH2F* hConc = new TH2F("hConc", "SIZE vs concentration; SIZE (PE); CONC", SIZEstep, SIZEmin,SIZEmax,ONEstep, ONEmin, ONEmax);
    hConc->SetStats(0);
    LogBinning(hConc);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hConc->Fill(Svector[h],Cvector[h]);
    }
    c->cd(0);
    hConc->Draw("COLZ");
    c->Update();
    c->SetLogx();
    c->Write("Conc_Size");
    hConc->Write("Conc_SizeTH2F");
    c->Print(pdf.c_str());
    delete hConc;
    c->SetLogx(0);
}

void PlotHelp::PlothWLConc(TCanvas* c, std::string pdf){
    TH2F* hWLConc = new TH2F("hWLConc", "WL vs concentration; WL; CONC",ONEstep, ONEmin, ONEmax,ONEstep, ONEmin, ONEmax);
    hWLConc->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hWLConc->Fill(WLvector[h],Cvector[h] );
    }
    c->cd(0);
    hWLConc->Draw("COLZ");
    c->Update();
    c->Write("WL_Conc");
    hWLConc->Write("WL_ConcTH2F");
    c->Print(pdf.c_str());
    delete hWLConc;
}

void PlotHelp::PlothncoreSIZE(TCanvas* c, std::string pdf){
    TH2F* ncoreSIZE = new TH2F("ncoreSIZE", "Number of Core pixels vs Size; Size (PE);Number of Core pixel ",SIZEstep, SIZEmin,SIZEmax, nCoreStep, nCoreMin,nCoreMax);
    ncoreSIZE->SetStats(0);
    LogBinning(ncoreSIZE);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        ncoreSIZE->Fill(Svector[h],NCorevector[h] );
    }
    c->cd(0);
    ncoreSIZE->Draw("COLZ");
    c->Update();
    c->SetLogx();
    c->Write("Ncore_SIZE");
    ncoreSIZE->Write("Ncore_SIZETH2F");
    c->Print(pdf.c_str());
    
    delete ncoreSIZE;
    c->SetLogx(0);
}

void PlotHelp::PlothCRSPC(TCanvas* c, std::string pdf){
    TH2F* hCRSPC = new TH2F("hCRSPC", "Core Ratio vs Size; SPC;Core Ratio ",SPstep, SPmin,SPmax, 4*ONEstep,4*ONEmin,4*ONEmax);
    hCRSPC->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hCRSPC->Fill(SPCvector[h],Corevector[h] );
    }
    c->cd(0);
    hCRSPC->Draw("COLZ");
    c->Update();
    c->Write("CR_SPC");
    hCRSPC->Write("CR_SPCTH2F");
    c->Print(pdf.c_str());
    delete hCRSPC;
    c->SetLogx(0);
}

void PlotHelp::PlothnCoreSPC(TCanvas* c, std::string pdf){
    TH2F* hnCoreSPC = new TH2F("hnCoreSPC", "Number of Core Pixels vs Surviving Pixels; Surviving Pixels; Number of Cores ", SPstep, SPmin, SPmax,nCoreStep, nCoreMin,nCoreMax);
    hnCoreSPC->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hnCoreSPC->Fill(SPCvector[h],NCorevector[h] );
    }
    c->cd(0);
    hnCoreSPC->Draw("COLZ");
    c->Update();
    c->Write("nCore_SPC");
    hnCoreSPC->Write("nCore_SPCTH2F");
    c->Print(pdf.c_str());
    delete hnCoreSPC;
}

void PlotHelp::PlothWLSPC(TCanvas* c, std::string pdf){
    TH2F* hWLSPC = new TH2F("hWLSPC", "Surviving pixels vs Width Length Ratio; Surviving Pixels;WL ratio ", SPstep, SPmin, SPmax,ONEstep, ONEmin, ONEmax);
    hWLSPC->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hWLSPC->Fill(SPCvector[h],WLvector[h] );
    }
    c->cd(0);
    hWLSPC->Draw("COLZ");
    c->Update();
    c->Write("WL_SPC");
    hWLSPC->Write("WL_SPCTH2F");
    c->Print(pdf.c_str());
    delete hWLSPC;
}

void PlotHelp::PlothSIZEWL(TCanvas* c, std::string pdf){
    TH2F* hWLsize = new TH2F("hWLsize", "Size vs Width Length Ratio; SIZE (PE);WL ratio ", SIZEstep, SIZEmin,SIZEmax,ONEstep, ONEmin, ONEmax);
    hWLsize->SetStats(0);
    LogBinning(hWLsize);

    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hWLsize->Fill(Svector[h],WLvector[h] );
    }
    c->cd(0);
    hWLsize->Draw("COLZ");
    c->Update();
    c->SetLogx();
    c->Write("WL_Size");
    hWLsize->Write("WL_SizeTH2F");
    c->Print(pdf.c_str());
    delete hWLsize;
    c->SetLogx(0);
}

void PlotHelp::PlothSIZEWandL(TCanvas* c, std::string pdf){
    TH2F* hWsize = new TH2F("hWsize", "Width vs Size; SIZE (PE);Width ", SIZEstep, SIZEmin,SIZEmax,4*ONEstep, ONEmin, ONEmax+1);
    TH2F* hLsize = new TH2F("hLsize", "Length vs Size; SIZE (PE);Length ", SIZEstep, SIZEmin,SIZEmax,4*ONEstep, ONEmin, ONEmax+1);
    hWsize->SetStats(0);
    hLsize->SetStats(0);
    LogBinning(hLsize);
    LogBinning(hWsize);

    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hWsize->Fill(Svector[h],Wvector[h] );
        hLsize->Fill(Svector[h],Lvector[h] );

    }
    hWsize->Write("W_SizeTH2F");
    hLsize->Write("L_SizeTH2F");
    hWsize->SetTitle("Width and Length vs Size; SIZE (PE);value");
    c->cd(0);
    gStyle->SetPalette(1);
    hWsize->SetLineColor(kBlue);
    hWsize->SetFillColor(kBlue);
    hWsize->Draw("BOX");
    
    hLsize->SetLineColor(kRed);
    hLsize->SetFillColor(kRed);
    hLsize->SetFillStyle(3001);
    hLsize->Draw("BOX SAME");
    
    auto legend = new TLegend(0.1,0.8,0.34,0.9);
    // legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
    legend->AddEntry(hLsize,"Length","f");
    legend->AddEntry(hWsize,"Width","f");
    legend->Draw();

    c->Update();
    c->SetLogx();

    c->Write("WandL_Size");
    c->Print(pdf.c_str());
    delete hWsize;
    delete hLsize;
    c->SetLogx(0);
}

void PlotHelp::PlothSPCConc(TCanvas* c, std::string pdf){
    TH2F* hSPCConc = new TH2F("hSPCConc", "Surviving pixels vs concentration; Surviving Pixels;Conc", SPstep, SPmin, SPmax,ONEstep, ONEmin, ONEmax);
    hSPCConc->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hSPCConc->Fill(SPCvector[h],Cvector[h]);
    }
    c->cd(0);
    hSPCConc->Draw("COLZ");
    c->Update();
    c->Write("SPC_Conc");
    hSPCConc->Write("SPC_ConcTH2F");
    c->Print(pdf.c_str());
    delete hSPCConc;
}

void PlotHelp::PlothSizeSPC(TCanvas* c, std::string pdf){
    TH2F* hSizeSPC = new TH2F("hSizeSPC", "Distribution of # of pixels that surived cleaning over events; SIZE (PE); Surviving Pixels", SIZEstep, SIZEmin,SIZEmax,SPstep,SPmin,SPmax);
    hSizeSPC->SetStats(0);
    LogBinning(hSizeSPC);

    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hSizeSPC->Fill(Svector[h],SPCvector[h]);
    }
    c->cd(0);
    hSizeSPC->Draw("COLZ");
    c->Update();
    c->SetLogx();
    c->Write("Size_SPC");
    hSizeSPC->Write("Size_SPCTH2F");
    c->Print(pdf.c_str());
    delete hSizeSPC;
    c->SetLogx(0);
}

void PlotHelp::PlothCoreRatio(TCanvas* c, std::string pdf){
    TH2F* hsizeCORE = new TH2F("hsizeCORE", "Core Ratio (Trigger/Second); SIZE (PE); Core Ratio", SIZEstep, SIZEmin,SIZEmax,4*ONEstep,4*ONEmin,4*ONEmax);
    hsizeCORE->SetStats(0);
    LogBinning(hsizeCORE);

    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hsizeCORE->Fill(Svector[h],Corevector[h]);
    }
    c->cd(0);
    hsizeCORE->Draw("COLZ");
    c->Update();
    c->SetLogx();
    c->Write("Size_CORE");
    hsizeCORE->Write("Size_CORETH2F");
    c->Print(pdf.c_str());
    delete hsizeCORE;
    c->SetLogx(0);
}

void PlotHelp::PlothCOG(TCanvas* c, std::string pdf){
    TH2F* hCOG = new TH2F("hCOG", "Center of Gravity;Pixel Bin along x axis of Camera;Pixel Bin along y axis of Camera", CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);
    hCOG->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < COGxvector.size(); h++) {
        hCOG->Fill(COGxvector[h], COGyvector[h]);
    }
    c->cd(0);
    hCOG->SetMarkerStyle(20);   // Set marker style
    hCOG->SetMarkerColor(kGreen); // Green color for certain points
    hCOG->SetLineColor(kGreen);
    hCOG->Draw("SCAT");
    plottools->DrawMUSICBoundaries();
    c->Update();
    c->Write("COG");
    hCOG->Write("COGTH2F");
    c->Print(pdf.c_str());
    delete hCOG;
}

void PlotHelp::PlothTPID(TCanvas* c, std::string pdf){
    TH2F* hTPID = new TH2F("hTPID", "Triggered Pixels Of Surviving Events;Pixel Bin along x axis of Camera;Pixel Bin along y axis of Camera", CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);
    hTPID->SetStats(0);
    for (std::vector<int>::size_type h = 0; h < TPID.size(); h++) {
        int nx, ny;
        plottools->FindBin(TPID[h], &nx, &ny);
        hTPID->Fill(nx,ny, 1);
    }
    c->cd(0);
    hTPID->Draw("COLZ");
    plottools->DrawMUSICBoundaries();
    c->Update();
    c->Write("TPID");
    hTPID->Write("TPIDTH2F");
    c->Print(pdf.c_str());
    delete hTPID;
}

void PlotHelp::PlothEventDateTriggeredPixel(TCanvas* c, std::string pdf){
    int enddates = 500;
    TH2I* hDate = new TH2I("hDate", "Triggered Pixel on each date;Date;Pixel", enddates,-0.5,499.5,256,-0.5,255.5);
    hDate->SetStats(0);
    // hDate->SetXTitle("RMS/Avg Amplitude");
    for (std::vector<int>::size_type h = 0; h < TPID.size(); h++) {
        std::cout << "Date: " << Form("%i",GetSequentialDayIndex(Datevecctor[h])) << " Pixel Triggered: " << TPID[h] << std::endl;
        hDate->Fill(GetSequentialDayIndex(Datevecctor[h]), TPID[h],1);
    }
    c->cd(0);
    TDatime startDate(2024, 6, 11, 0, 0, 0);
    TDatime current = startDate;

    for (int i = 0; i < enddates; ++i) {
        int day = current.GetDay();
        int month = current.GetMonth();
        int year = current.GetYear();

        // Set tick marks for day 1 and 15
        if (day == 1 || day == 15) {
            hDate->GetXaxis()->SetTickLength(-0.03); // Negative = tick on opposite side
            // (tick marks are automatic — this just ensures visibility)
        }

        // Label only on every 2nd month (day == 1)
        if (day == 1 && (month % 2 == 0)) {
            TString label = TString::Format("%04d-%02d-%02d", year, month, day);
            hDate->GetXaxis()->SetBinLabel(i + 1, label);
        }

        current.Set(current.Convert() + 86400); // move to next day
    }
    gStyle->SetPalette(72);
    hDate->Draw("COLZ");
    c->Update();
    c->Write("EventDateTriggeredPixel");
    hDate->Write("EventDateTriggeredPixelTH2D");
    c->Print(pdf.c_str());
    delete hDate;
}

void PlotHelp::PlotPixelsDistanceToMajorAxis(TCanvas* c, std::string pdf){
    TH1D* hDistance = new TH1D("hDistance", "Every Surviving Pixels Distance to Major Axis; Distance to Major Axis (pixels)", dStep, dMin, dMax);
    hDistance->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Distance2MajorAxisVector.size(); h++) {
        hDistance->Fill(Distance2MajorAxisVector[h]);
        //std::cout << "Distance to Major Axis: " << Distance2MajorAxisVector[h] << std::endl;
    }
    c->cd(0);
    hDistance->Draw("HIST E1");
    c->SetLogy();
    c->Update();
    c->Write("Distance2MajorAxis");
    hDistance->Write("Distance2MajorAxisTH1D");
    c->Print(pdf.c_str());
    delete hDistance;
}

void PlotHelp::PlotPixelsRatioDistanceToMajorAxis(TCanvas* c, std::string pdf){
    TH1D* hDistance = new TH1D("hDistance", "Ratio of Pixels to Major Axis; ratio", ONEstep/2, ONEmin, ONEmax);
    hDistance->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < RatioPixelsMajorAxisVector.size(); h++) {
        hDistance->Fill(RatioPixelsMajorAxisVector[h]);
        //std::cout << "Distance to Major Axis: " << Distance2MajorAxisVector[h] << std::endl;
    }
    c->cd(0);
    hDistance->Draw("HIST E1");
    c->SetLogy();
    c->Update();
    c->Write("Ratio2MajorAxis");
    hDistance->Write("Ratio2MajorAxisTH1D");
    c->Print(pdf.c_str());
    delete hDistance;
}

void PlotHelp::PlotdistRMSandWeightedRMS(TCanvas* c, std::string pdf){
    TH1D* hWRMS = new TH1D("hWRMS", "Weighted distance RMS",dStep, dMin, dMax);
    TH1D* hRMS = new TH1D("hRMS", "distance RMS ",dStep, dMin, dMax);
    hWRMS->SetStats(0);
    hRMS->SetStats(0);
    hWRMS->SetXTitle("Value");
    hRMS->SetXTitle("Value");
    for (std::vector<double>::size_type h = 0; h < RMSMajorAxisVector.size(); h++) {
        hWRMS->Fill(WeightedRMSMajorAxisVector[h]);
        hRMS->Fill(RMSMajorAxisVector[h]);
    }
    // hWL->SetXaxis()->SetLabelSize(0.03);
    // hWL->SetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    // hWL->SetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    c->cd(0);
    hWRMS->SetLineColor(kBlue);
    hWRMS->SetFillColor(38);
    hWRMS->SetFillStyle(3017);
    hWRMS->Draw("HIST E1");
    
    hRMS->SetLineColor(kRed);
    hRMS->SetFillColor(46);
    hRMS->SetFillStyle(3017);
    hRMS->Draw("HIST E1 SAME");

    
    auto legend = new TLegend(0.1,0.8,0.34,0.9);
    // legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
    legend->AddEntry(hRMS,"RMS","f");
    legend->AddEntry(hWRMS,"Weighted RMS","f");
    legend->Draw();
    c->Update();
    c->SetLogy();
    c->Write("WandL_Distribution");
    hWRMS->SetTitle("Distribution of Weighted RMS");
    hWRMS->SetXTitle("Weighted RMS");
    hRMS->SetXTitle("RMS");
    hWRMS->Write("WRMS_DistributionTH1D");
    hRMS->Write("RMS_DistributionTH1D");
    c->Print(pdf.c_str());
    c->SetLogy(0);

}


void PlotHelp::PlotPixelsOnMajorAxis(TCanvas* c, std::string pdf){
    TH1D* hOn = new TH1D("hOn", "Pixels on Major Axis",pOnStep, pOnMin, pOnMax);

    hOn->SetStats(0);
    hOn->SetXTitle("Pixels");
    for (std::vector<double>::size_type h = 0; h < PixelsonMajorAxisVector.size(); h++) {
        hOn->Fill(PixelsonMajorAxisVector[h]);
    }
    c->cd(0);
    hOn->SetLineColor(kBlue);
    hOn->SetFillColor(38);
    hOn->SetFillStyle(3017);
    hOn->Draw("HIST E1");
    
    c->Update();
    c->SetLogy();
    c->Write("pixelsOnMajorAxis_Distribution");
    hOn->SetXTitle("Pixels on Major Axis");
    hOn->Write("OnMajorAxis_DistTH1D");
    c->Print(pdf.c_str());
    c->SetLogy(0);

}


void PlotHelp::PlotPixelsOffMajorAxis(TCanvas* c, std::string pdf){
    
    TH1D* hOff = new TH1D("hOff", "Pixels off Major Axis",pOnStep, pOnMin, pOnMax);
    
    hOff->SetStats(0);
    hOff->SetXTitle("Pixels");
    for (std::vector<double>::size_type h = 0; h < PixelsoffMajorAxisVector.size(); h++) {
        
        hOff->Fill(PixelsoffMajorAxisVector[h]);
    }
    c->cd(0);

    hOff->SetLineColor(kRed);
    hOff->SetFillColor(46);
    hOff->SetFillStyle(3017);
    hOff->Draw("HIST E1");
  
    
    
    c->Update();
    c->SetLogy();
    c->Write("pixelsOffMajorAxis_Distribution");
    hOff->SetTitle("Distribution of Pixels off Major Axis");
    hOff->SetXTitle("Pixels off Major Axis");
    hOff->Write("OffMajorAxis_DistTH1D");
    
    c->Print(pdf.c_str());
    c->SetLogy(0);

}

void PlotHelp::PlothRMSvsRatioDistance(TCanvas* c, std::string pdf){
    TH2F* hRMSd = new TH2F("hRMSd", "RMS vs ratio Distance; ratio;RMS ",ONEstep/2, ONEmin,ONEmax, 2*ONEstep,ONEmin,2*ONEmax);
    hRMSd->SetStats(0);
    for (std::vector<double>::size_type h = 0; h <RMSMajorAxisVector.size(); h++) {
        hRMSd->Fill(RatioPixelsMajorAxisVector[h],RMSMajorAxisVector[h]);
    }
    c->cd(0);
    hRMSd->Draw("COLZ");
    c->Update();
    c->Write("RMSd");
    hRMSd->Write("RMSd_TH2F");
    c->Print(pdf.c_str());
    delete hRMSd;
    c->SetLogx(0);
}

void PlotHelp::PlothWRMSvsRatioDistance(TCanvas* c, std::string pdf){
    TH2F* hWRMSd = new TH2F("hWRMSd", "WRMS vs ratio Distance; ratio; weighted RMS ",ONEstep/2, ONEmin,ONEmax, 4*ONEstep,ONEmin,10*ONEmax);
    hWRMSd->SetStats(0);
    for (std::vector<double>::size_type h = 0; h <WeightedRMSMajorAxisVector.size(); h++) {
        hWRMSd->Fill(RatioPixelsMajorAxisVector[h],WeightedRMSMajorAxisVector[h] );
    }
    c->cd(0);
    hWRMSd->Draw("COLZ");
    c->Update();
    c->Write("WRMSd");
    hWRMSd->Write("WRMSd_TH2F");
    c->Print(pdf.c_str());
    delete hWRMSd;
    c->SetLogx(0);
    c->SetLogz(0);
}

void PlotHelp::PlothWLvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf){
    TH2F* hWLRatio = new TH2F("hWLRatio", "WL vs ratio of pixels on Major Axis; ratio;WL ratio ",ONEstep/2, ONEmin,ONEmax, ONEstep,ONEmin,ONEmax);
    hWLRatio->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < RatioPixelsMajorAxisVector.size(); h++) {
        hWLRatio->Fill(RatioPixelsMajorAxisVector[h],WLvector[h] );
    }
    c->cd(0);
    hWLRatio->Draw("COLZ");
    c->Update();
    c->SetLogz();
    c->Write("WL_RatioPixelsMajorAxis");
    hWLRatio->Write("WL_RatioPixelsMajorAxisTH2F");
    c->Print(pdf.c_str());
    delete hWLRatio;
    c->SetLogx(0);
    c->SetLogz(0);
}

void PlotHelp::PlothDistancevsRatioPixelsMajorAxis(TCanvas* c, std::string pdf){
    // one entry per surviving pixel: its distance to the major axis vs its event's ratio
    TH2F* hdRatio = new TH2F("hdRatio", "Distance to Major Axis vs ratio; ratio;Distance to Major Axis (pixels) ",ONEstep/2, ONEmin,ONEmax, dStep,dMin,dMax);
    hdRatio->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < RatioPixelsMajorAxisPerPixelVector.size(); h++) {
        hdRatio->Fill(RatioPixelsMajorAxisPerPixelVector[h],Distance2MajorAxisVector[h] );
    }
    c->cd(0);
    hdRatio->Draw("COLZ");
    c->Update();
    c->SetLogz();
    c->Write("Distance2MajorAxis_RatioPixelsMajorAxis");
    hdRatio->Write("Distance2MajorAxis_RatioPixelsMajorAxisTH2F");
    c->Print(pdf.c_str());
    delete hdRatio;
    c->SetLogx(0);
    c->SetLogz(0);
}

void PlotHelp::PlothCoreOverSPCvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf){
    TH2F* hCoreSPCRatio = new TH2F("hCoreSPCRatio", "Core Pixels over Surviving Pixels vs ratio; ratio;Core Pixels / Surviving Pixels ",ONEstep/2, ONEmin,ONEmax, ONEstep,ONEmin,ONEmax);
    hCoreSPCRatio->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < RatioPixelsMajorAxisVector.size(); h++) {
        if (SPCvector[h] == 0) continue; // no surviving pixels, nothing to divide by
        hCoreSPCRatio->Fill(RatioPixelsMajorAxisVector[h],NCorevector[h]/(double)SPCvector[h] );
    }
    c->cd(0);
    hCoreSPCRatio->Draw("COLZ");
    c->Update();
    c->SetLogz();
    c->Write("CoreOverSPC_RatioPixelsMajorAxis");
    hCoreSPCRatio->Write("CoreOverSPC_RatioPixelsMajorAxisTH2F");
    c->Print(pdf.c_str());
    delete hCoreSPCRatio;
    c->SetLogx(0);
    c->SetLogz(0);
}

void PlotHelp::PlothSPCvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf){
    TH2F* hSPCRatio = new TH2F("hSPCRatio", "Surviving Pixels vs ratio; ratio;Surviving Pixels ",ONEstep/2, ONEmin,ONEmax, SPstep,SPmin,SPmax);
    hSPCRatio->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < RatioPixelsMajorAxisVector.size(); h++) {
        hSPCRatio->Fill(RatioPixelsMajorAxisVector[h],SPCvector[h] );
    }
    c->cd(0);
    hSPCRatio->Draw("COLZ");
    c->Update();
    c->SetLogz();
    c->Write("SPC_RatioPixelsMajorAxis");
    hSPCRatio->Write("SPC_RatioPixelsMajorAxisTH2F");
    c->Print(pdf.c_str());
    delete hSPCRatio;
    c->SetLogx(0);
    c->SetLogz(0);
}

void PlotHelp::PlothOnOffMajorAxisvsratio(TCanvas* c, std::string pdf){
    TH2F* hOnRatio = new TH2F("hOnRatio", "On Major Axis vs Ratio; Ratio; On Major Axis ", ONEstep/2, ONEmin, ONEmax,pOnStep, pOnMin, pOnMax);
    TH2F* hOffRatio = new TH2F("hOffRatio", "Off Major Axis vs Ratio; Ratio; Off Major Axis ",  ONEstep/2, ONEmin, ONEmax,pOnStep, pOnMin, pOnMax);
    hOnRatio->SetStats(0);
    hOffRatio->SetStats(0);
    // LogBinning(hOffRatio);
    // LogBinning(hOnRatio);

    for (std::vector<double>::size_type h = 0; h < PixelsonMajorAxisVector.size(); h++) {
        hOnRatio->Fill(RatioPixelsMajorAxisVector[h], PixelsonMajorAxisVector[h] );
        hOffRatio->Fill(RatioPixelsMajorAxisVector[h], PixelsoffMajorAxisVector[h] );

    }
    hOnRatio->Write("OnRatio_TH2F");
    hOffRatio->Write("OffRatio_TH2F");
    hOnRatio->SetTitle("Pixels On/off Major Axis vs Ratio; Ratio; Pixels ");
    c->cd(0);
    gStyle->SetPalette(1);
    hOnRatio->SetLineColor(kBlue);
    hOnRatio->SetFillColor(kBlue);
    hOnRatio->Draw("BOX");
    
    hOffRatio->SetLineColor(kRed);
    hOffRatio->SetFillColor(kRed);
    hOffRatio->SetFillStyle(3001);
    hOffRatio->Draw("BOX SAME");
    
    auto legend = new TLegend(0.1,0.8,0.34,0.9);
    // legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
    legend->AddEntry(hOnRatio,"On Major Axis","f");
    legend->AddEntry(hOffRatio,"Off Major Axis","f");
    legend->Draw();

    c->Update();
    // c->SetLogx();

    c->Write("OnOffMajorAxisvsRatio");
    c->Print(pdf.c_str());
    delete hOnRatio;
    delete hOffRatio;
    c->SetLogx(0);
}


void PlotHelp::PlothSurvivingPixelsX(TCanvas* c, std::string pdf){
    TH1D* hSurvX = new TH1D("hSurvX", "Every Surviving Pixel along x axis of Camera", CameraStep, CameraMin, CameraMax);
    hSurvX->SetStats(0);
    hSurvX->SetXTitle("Pixel Bin along x axis of Camera");
    for (std::vector<double>::size_type h = 0; h < SurvivingPixelXVector.size(); h++) {
        hSurvX->Fill(SurvivingPixelXVector[h]);
    }
    c->cd(0);
    hSurvX->SetLineColor(kBlue);
    hSurvX->SetFillColor(38);
    hSurvX->SetFillStyle(3017);
    hSurvX->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SurvivingPixelsX_Distribution");
    hSurvX->Write("SurvivingPixelsX_DistTH1D");
    c->Print(pdf.c_str());
    delete hSurvX;
    c->SetLogy(0);
}

void PlotHelp::PlothSurvivingPixelsY(TCanvas* c, std::string pdf){
    TH1D* hSurvY = new TH1D("hSurvY", "Every Surviving Pixel along y axis of Camera", CameraStep, CameraMin, CameraMax);
    hSurvY->SetStats(0);
    hSurvY->SetXTitle("Pixel Bin along y axis of Camera");
    for (std::vector<double>::size_type h = 0; h < SurvivingPixelYVector.size(); h++) {
        hSurvY->Fill(SurvivingPixelYVector[h]);
    }
    c->cd(0);
    hSurvY->SetLineColor(kRed);
    hSurvY->SetFillColor(46);
    hSurvY->SetFillStyle(3017);
    hSurvY->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SurvivingPixelsY_Distribution");
    hSurvY->Write("SurvivingPixelsY_DistTH1D");
    c->Print(pdf.c_str());
    delete hSurvY;
    c->SetLogy(0);
}

void PlotHelp::PlothCOGx(TCanvas* c, std::string pdf){
    // one bin per camera pixel, so the COG lands in the bin of the pixel it sits on
    TH1D* hCOGx = new TH1D("hCOGx", "Center of Gravity along x axis of Camera", CameraStep, CameraMin, CameraMax);
    hCOGx->SetStats(0);
    hCOGx->SetXTitle("Pixel Bin along x axis of Camera");
    for (std::vector<double>::size_type h = 0; h < COGxvector.size(); h++) {
        hCOGx->Fill(COGxvector[h]);
    }
    c->cd(0);
    hCOGx->SetLineColor(kBlue);
    hCOGx->SetFillColor(38);
    hCOGx->SetFillStyle(3017);
    hCOGx->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("COGx_Distribution");
    hCOGx->Write("COGx_DistTH1D");
    c->Print(pdf.c_str());
    delete hCOGx;
    c->SetLogy(0);
}

void PlotHelp::PlothCOGy(TCanvas* c, std::string pdf){
    // one bin per camera pixel, so the COG lands in the bin of the pixel it sits on
    TH1D* hCOGy = new TH1D("hCOGy", "Center of Gravity along y axis of Camera", CameraStep, CameraMin, CameraMax);
    hCOGy->SetStats(0);
    hCOGy->SetXTitle("Pixel Bin along y axis of Camera");
    for (std::vector<double>::size_type h = 0; h < COGyvector.size(); h++) {
        hCOGy->Fill(COGyvector[h]);
    }
    c->cd(0);
    hCOGy->SetLineColor(kRed);
    hCOGy->SetFillColor(46);
    hCOGy->SetFillStyle(3017);
    hCOGy->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("COGy_Distribution");
    hCOGy->Write("COGy_DistTH1D");
    c->Print(pdf.c_str());
    delete hCOGy;
    c->SetLogy(0);
}

void PlotHelp::PlothAngle(TCanvas* c, std::string pdf){
    // the angle comes from atan of the major axis eigenvector so it lives in (-90, 90)
    TH1D* hAngle = new TH1D("hAngle", "Major Axis Angle", AngleStep, AngleMin, AngleMax);
    hAngle->SetStats(0);
    hAngle->SetXTitle("Major Axis Angle (deg)");
    for (std::vector<double>::size_type h = 0; h < Anglevector.size(); h++) {
        // a single pixel event has no axis to fit, which comes back as a NaN
        if (std::isnan(Anglevector[h])) continue;
        hAngle->Fill(Anglevector[h]);
    }
    c->cd(0);
    hAngle->SetLineColor(kBlue);
    hAngle->SetFillColor(38);
    hAngle->SetFillStyle(3017);
    hAngle->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("Angle_Distribution");
    hAngle->Write("Angle_DistTH1D");
    c->Print(pdf.c_str());
    delete hAngle;
    c->SetLogy(0);
}

// The major axis, oriented head-to-tail by M3Long, dotted with the vertical. +1 means the
// bright head of the image points straight up, -1 straight down and 0 means the image lies
// along the camera x axis. Events whose head-tail direction could not be determined are
// entered as NaN by CompletePanel4 and are skipped here rather than piling up at 0.
void PlotHelp::PlothUpDownSym(TCanvas* c, std::string pdf){
    TH1D* hUD = new TH1D("hUD", "Up-Down Symmetry (Major Axis #upoint Vertical, head-tail from M3Long)", UDstep, UDmin, UDmax);
    hUD->SetStats(0);
    hUD->SetXTitle("Major Axis #upoint Vertical (+ = head up)");
    for (std::vector<double>::size_type h = 0; h < UpDownSymvector.size(); h++) {
        // no head-tail direction to orient by, so there is no up or down to report
        if (std::isnan(UpDownSymvector[h])) continue;
        hUD->Fill(UpDownSymvector[h]);
    }
    c->cd(0);
    hUD->SetLineColor(kRed);
    hUD->SetFillColor(46);
    hUD->SetFillStyle(3017);
    hUD->Draw("HIST E1");
    c->Update();
    c->Write("UpDownSym_Distribution");
    hUD->Write("UpDownSym_DistTH1D");
    c->Print(pdf.c_str());
    delete hUD;
    c->SetLogy(0);
}

void PlotHelp::PlothSaturatedPixels(TCanvas* c, std::string pdf){
    TH1D* hSat = new TH1D("hSat", "Saturated Pixels per Event", SatStep, SatMin, SatMax);
    hSat->SetStats(0);
    hSat->SetXTitle("Saturated Pixels");
    for (std::vector<int>::size_type h = 0; h < SatPixelCountVector.size(); h++) {
        hSat->Fill(SatPixelCountVector[h]);
    }
    c->cd(0);
    hSat->SetLineColor(kBlue);
    hSat->SetFillColor(38);
    hSat->SetFillStyle(3017);
    hSat->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SaturatedPixels_Distribution");
    hSat->Write("SaturatedPixels_DistTH1D");
    c->Print(pdf.c_str());
    delete hSat;
    c->SetLogy(0);
}

void PlotHelp::PlothSaturatedOverSurviving(TCanvas* c, std::string pdf){
    // The stored saturated pixels are filtered down to those that survived cleaning, so this
    // ratio is bounded by 1. Events with no surviving pixels have no ratio to form and are skipped.
    TH1D* hSatRatio = new TH1D("hSatRatio", "Saturated Pixels over Surviving Pixels", 2*ONEstep, ONEmin, ONEmax);
    hSatRatio->SetStats(0);
    hSatRatio->SetXTitle("Saturated Pixels / Surviving Pixels");
    for (std::vector<int>::size_type h = 0; h < SatPixelCountVector.size() && h < SPCvector.size(); h++) {
        if (SPCvector[h] <= 0) continue;
        hSatRatio->Fill((double)SatPixelCountVector[h]/SPCvector[h]);
    }
    c->cd(0);
    hSatRatio->SetLineColor(kRed);
    hSatRatio->SetFillColor(46);
    hSatRatio->SetFillStyle(3017);
    hSatRatio->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SaturatedOverSurviving_Distribution");
    hSatRatio->Write("SaturatedOverSurviving_DistTH1D");
    c->Print(pdf.c_str());
    delete hSatRatio;
    c->SetLogy(0);
}

void PlotHelp::PlothSaturatedPixelsX(TCanvas* c, std::string pdf){
    // Where saturation lands along the camera x axis. Compare against PlothSurvivingPixelsX: a
    // column that saturates out of proportion to how often it survives is a hardware suspect.
    TH1D* hSatX = new TH1D("hSatX", "Every Saturated Pixel along x axis of Camera", CameraStep, CameraMin, CameraMax);
    hSatX->SetStats(0);
    hSatX->SetXTitle("Pixel Bin along x axis of Camera");
    for (std::vector<double>::size_type h = 0; h < SaturatedPixelXVector.size(); h++) {
        hSatX->Fill(SaturatedPixelXVector[h]);
    }
    c->cd(0);
    hSatX->SetLineColor(kBlue);
    hSatX->SetFillColor(38);
    hSatX->SetFillStyle(3017);
    hSatX->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SaturatedPixelsX_Distribution");
    hSatX->Write("SaturatedPixelsX_DistTH1D");
    c->Print(pdf.c_str());
    delete hSatX;
    c->SetLogy(0);
}

void PlotHelp::PlothSaturatedPixelsY(TCanvas* c, std::string pdf){
    TH1D* hSatY = new TH1D("hSatY", "Every Saturated Pixel along y axis of Camera", CameraStep, CameraMin, CameraMax);
    hSatY->SetStats(0);
    hSatY->SetXTitle("Pixel Bin along y axis of Camera");
    for (std::vector<double>::size_type h = 0; h < SaturatedPixelYVector.size(); h++) {
        hSatY->Fill(SaturatedPixelYVector[h]);
    }
    c->cd(0);
    hSatY->SetLineColor(kRed);
    hSatY->SetFillColor(46);
    hSatY->SetFillStyle(3017);
    hSatY->Draw("HIST E1");
    c->Update();
    // c->SetLogy();
    c->Write("SaturatedPixelsY_Distribution");
    hSatY->Write("SaturatedPixelsY_DistTH1D");
    c->Print(pdf.c_str());
    delete hSatY;
    c->SetLogy(0);
}

void PlotHelp::LogBinning(TH2F* hist){
    TAxis *axis = hist->GetXaxis();
    int bins = axis->GetNbins();
    Axis_t from = axis->GetXmin();
    Axis_t to = axis->GetXmax();
    Axis_t width = (to - from) / bins;
    Axis_t *new_bins = new Axis_t[bins + 1];
    for (int i = 0; i <= bins; i++) {
        new_bins[i] = TMath::Power(10, from + i * width);
    }
    axis->Set(bins, new_bins);
    hist->SetBins(bins,new_bins);
    delete[] new_bins;
}

void PlotHelp::LogBinning(TH1D* hist){
    TAxis *axis = hist->GetXaxis();
    int bins = axis->GetNbins();
    Axis_t from = axis->GetXmin();
    Axis_t to = axis->GetXmax();
    Axis_t width = (to - from) / bins;
    Axis_t *new_bins = new Axis_t[bins + 1];
    for (int i = 0; i <= bins; i++) {
        new_bins[i] = TMath::Power(10, from + i * width);
    }
    axis->Set(bins, new_bins);
    hist->SetBins(bins,new_bins);
    delete[] new_bins;
}

int PlotHelp::GetSequentialDayIndex(int dateInt) {
    // Start date (e.g., your base reference)
    TDatime start(2024, 6, 11, 0, 0, 0);

    // Convert int date to year/month/day
    int year  = dateInt / 10000;
    int month = (dateInt / 100) % 100;
    int day   = dateInt % 100;

    // Target date
    TDatime target(year, month, day, 0, 0, 0);

    // Compute index as difference in days
    return (target.Convert() - start.Convert()) / 86400;
}

