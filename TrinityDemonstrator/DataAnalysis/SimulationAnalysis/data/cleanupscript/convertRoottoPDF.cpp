#include <TFile.h>
#include <TH2.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TKey.h>
#include <TClass.h>
#include <TStyle.h>
#include <iostream>
#include <TBox.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TTree.h>
#include <string>
#include <IPlotTools.h>
#include <map>
#include <vector>
#include <cmath>
#include <limits>
#include <SEvent.h>
#include <IUtilities.h>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: canvases_to_pdf <input.root> <output.pdf>" << std::endl;
        return 1;
    }
    IPlotTools *plottools;
    void DrawMUSICBoundaries();

    const std::string infile = argv[1];
    std::string outpdf = infile.substr(0, infile.find_last_of('.')) + ".pdf";

    TFile *f = TFile::Open(infile.c_str(), "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: Cannot open " << infile << std::endl;
        return 1;
    }

    gStyle->SetOptStat(0);

    // --- Build shower-info lookup from the SEvent tree -----------------
    // The plots file (SIM_Plots_<DATE>.root) has a sibling data file
    // (SIM_DataFiles_<DATE>.root) holding one SEvent per shower.
    // Key = simrun + "_" + simEvent, which matches the histogram base name.
    struct ShowerInfo {
        double energy;            // neutrino energy [GeV]
        double emergenceAngle;    // [deg]
        double emergenceDistance; // [m]
    };
    std::map<std::string, ShowerInfo> infoMap;

    // Energy distributions over all events: triggered vs. non-triggered.
    // Log-spaced bins since neutrino energies span several decades.
    const int    nbins = 40;
    const double logLo = 6.0, logHi = 10.0;   // 1e6 .. 1e10 GeV
    double edges[nbins + 1];
    for (int i = 0; i <= nbins; ++i)
        edges[i] = std::pow(10.0, logLo + (logHi - logLo) * i / nbins);
    TH1F *hEtrig   = new TH1F("hEtrig",   "Neutrino energy distribution", nbins, edges);
    TH1F *hEnotrig = new TH1F("hEnotrig", "Neutrino energy distribution", nbins, edges);
    {
        std::string datafile = infile;
        const std::string tag = "SIM_Plots_";
        size_t p = datafile.find(tag);
        if (p != std::string::npos)
            datafile.replace(p, tag.size(), "SIM_DataFiles_");
        if (argc == 3) datafile = argv[2];   // optional explicit override

        TFile *df = TFile::Open(datafile.c_str(), "READ");
        if (!df || df->IsZombie()) {
            std::cerr << "WARNING: cannot open datafile " << datafile
                      << " - neutrino energy will not be shown." << std::endl;
        } else {
            TTree *t = (TTree*)df->Get("Sim");
            if (!t) {
                std::cerr << "WARNING: no 'Sim' tree in " << datafile << std::endl;
            } else {
                SEvent *d = nullptr;
                t->SetBranchAddress("GrOptics", &d);
                const Long64_t nentries = t->GetEntries();
                for (Long64_t i = 0; i < nentries; ++i) {
                    t->GetEntry(i);
                    std::string k = d->GetSumRun() + "_" +
                                    std::to_string(d->GetSimEventNumber());

                    // Emergence angle (azimuth wrapped to [-180,180] from the
                    // telescope axis) and distance to the emergence point
                    // (magnitude of the telescope position vector).
                    double emergenceAngle =
                        IUtilities::GetEmergenceAngle(d->GetAzimuthAngle());
                    double emergenceDistance =
                        IUtilities::GetEmergenceDistance(d->GetTelescope_Xpos(),
                                                         d->GetTelescope_Ypos(),
                                                         d->GetTelescope_Zpos());

                    infoMap[k] = { d->GetNeutrinoEnergy(),
                                   emergenceAngle, emergenceDistance };

                    if (d->GetTriggered())
                        hEtrig->Fill(d->GetNeutrinoEnergy());
                    else
                        hEnotrig->Fill(d->GetNeutrinoEnergy());
                }
                std::cout << "Loaded " << infoMap.size()
                          << " showers from " << datafile << std::endl;
            }
            df->Close();
        }
    }

    // Resolve the shower info for a histogram base name.
    auto findInfo = [&](std::string base) -> const ShowerInfo* {
        if (base.rfind("h_", 0) == 0) base = base.substr(2);  // drop "h_"
        while (!base.empty() && base.back() == '_') base.pop_back();
        auto it = infoMap.find(base);
        if (it != infoMap.end()) return &it->second;
        // Fallback: tolerate prefix differences (e.g. "datafiles_" stripped).
        for (auto& [k, v] : infoMap) {
            if (k.size() >= base.size() &&
                k.compare(k.size() - base.size(), base.size(), base) == 0) return &v;
            if (base.size() >= k.size() &&
                base.compare(base.size() - k.size(), k.size(), k) == 0) return &v;
        }
        return nullptr;
    };

    // Draw the shower info as a header on the canvas' mother pad.
    auto drawInfo = [&](TCanvas* c, const ShowerInfo* info, int totalPhotons) {
        c->cd(0);
        TLatex *tex = new TLatex();
        tex->SetNDC();
        tex->SetTextAlign(22);
        tex->SetTextSize(0.025);
        if (!info)
            tex->DrawLatex(0.5, 0.975, Form("Shower info: N/A, Total Photons: %i",
                                            totalPhotons));
        else
            tex->DrawLatex(0.5, 0.975,
                Form("Neutrino Energy: %.3g GeV, Emergence Angle: %.3g degrees, "
                     "Shower Distance: %.3g m ,Total Photons: %i",
                     info->energy, info->emergenceAngle,
                     info->emergenceDistance, totalPhotons));
    };

    // Categorise keys
    // base name -> {1mm key, 6mm key}
    std::map<std::string, std::string> map1mm, map6mm,mapPE;
    // std::vector<std::string>           pe_keys;

    TIter next(f->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {
        const std::string name = key->GetName();
        std::cout << "Found key: " << name << " (class: " << key->GetClassName() << ")" << std::endl;
        if (name.substr(0, 2) != "h_") continue;

        TClass *cl = TClass::GetClass(key->GetClassName());
        if (!cl || !cl->InheritsFrom("TH2")) continue;

        // Strip suffix to get base name
        const std::string s1mm = "1mm";
        const std::string s6mm = "6mm";
        const std::string spe  = "PixelsPE";

        auto ends_with = [&](const std::string& s, const std::string& suffix) {
            return s.size() >= suffix.size() &&
                   s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
        };

        if (ends_with(name, s1mm)) {
            std::string base = name.substr(0, name.size() - s1mm.size());
            map1mm[base] = name;
        } else if (ends_with(name, s6mm)) {
            std::string base = name.substr(0, name.size() - s6mm.size());
            map6mm[base] = name;
        } else if (ends_with(name, spe)) {
            std::string base = name.substr(0, name.size() - spe.size());
            mapPE[base]=name;
        }
    }

    int count = 0;

    auto print_page = [&](TCanvas* c) {
        if (count == 0)
            c->Print((outpdf + "(").c_str());
        else
            c->Print(outpdf.c_str());
        count++;
    };

    // --- Page: neutrino energy distribution, triggered vs. non-triggered ---
    {
        TCanvas *c = new TCanvas("c_edist", "c_edist", 1200, 800);
        c->SetLogx();
        c->SetLogy();
        gPad->SetLeftMargin(0.12);
        gPad->SetRightMargin(0.05);

        hEnotrig->SetLineColor(kRed);
        hEnotrig->SetLineWidth(2);
        hEtrig->SetLineColor(kBlue);
        hEtrig->SetLineWidth(2);

        hEnotrig->GetXaxis()->SetTitle("Neutrino energy [GeV]");
        hEnotrig->GetYaxis()->SetTitle("Number of showers");
        hEnotrig->SetMaximum(1.15 * std::max(hEtrig->GetMaximum(),
                                             hEnotrig->GetMaximum()));

        hEnotrig->Draw("HIST");
        hEtrig->Draw("HIST SAME");

        TLegend *leg = new TLegend(0.68, 0.78, 0.93, 0.90);
        leg->AddEntry(hEtrig,   Form("Triggered (%d)",     (int)hEtrig->GetEntries()),   "l");
        leg->AddEntry(hEnotrig, Form("Not triggered (%d)", (int)hEnotrig->GetEntries()), "l");
        leg->Draw();

        print_page(c);
        delete leg; delete c;
    }

    // --- Paired pages: 1mm | 6mm ---
    for (auto& [base, key1mm] : map1mm) {
        auto it = map6mm.find(base);
        if (it == map6mm.end()) continue;
        const std::string& key6mm = it->second;

        TH2 *h1 = (TH2*)f->Get(key1mm.c_str());
        TH2 *h6 = (TH2*)f->Get(key6mm.c_str());
        if (!h1 || !h6) continue;

        TCanvas *c = new TCanvas("c_pair", "c_pair", 1600, 800);
        c->SetRightMargin(0.02);
        c->Divide(2, 1);

        c->cd(1);
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.12);
        gPad->SetBottomMargin(0.12);
        gPad->SetTopMargin(0.10);
        h1->Draw("COLZ");
        h1->GetZaxis()->SetTitle("Photons");
        h1->GetZaxis()->SetTitleOffset(1.4);
        DrawMUSICBoundaries();

        c->cd(2);
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.12);
        gPad->SetBottomMargin(0.12);
        gPad->SetTopMargin(0.10);
        h6->GetZaxis()->SetTitle("Photons");
        h6->GetZaxis()->SetTitleOffset(1.4);
        h6->Draw("COLZ");
        DrawMUSICBoundaries();

        drawInfo(c, findInfo(base), (int)h6->Integral());
        print_page(c);
        delete h1; delete h6; delete c;
    }

    // --- Paired pages: 6mm | PE ---
    for (auto& [base, keyPE] : mapPE) {
        TH2 *h6 = nullptr;
        for (auto& [base6, key6mm] : map6mm) {
            if (base6.find(base) != std::string::npos || base.find(base6) != std::string::npos) {
                h6 = (TH2*)f->Get(key6mm.c_str());
                break;
            }
        }
        TH2 *h = (TH2*)f->Get(keyPE.c_str());
        if (!h6 || !h) continue;

        TCanvas *c = new TCanvas("c_pair", "c_pair", 1600, 800);
        c->SetRightMargin(0.02);
        c->Divide(2, 1);

        c->cd(1);
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.12);
        gPad->SetBottomMargin(0.12);
        gPad->SetTopMargin(0.10);
        h6->Draw("COLZ");
        h6->GetZaxis()->SetTitle("Photons");
        h6->GetZaxis()->SetTitleOffset(1.4);
        DrawMUSICBoundaries();

        c->cd(2);
        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.12);
        gPad->SetBottomMargin(0.12);
        gPad->SetTopMargin(0.10);
        h->GetZaxis()->SetTitle("PEs");
        h->GetZaxis()->SetTitleOffset(1.4);
        h->Draw("COLZ");
        plottools->DrawMUSICBoundaries();

        drawInfo(c, findInfo(base), (int)h6->Integral());
        print_page(c);
        delete h; delete h6; delete c;
    }

    if (count == 0) {
        std::cerr << "WARNING: No histograms found." << std::endl;
        f->Close();
        return 1;
    }

    // Close PDF with a blank canvas
    TCanvas dummy;
    dummy.Print((outpdf + ")").c_str());

    f->Close();
    std::cout << "Wrote " << count << " page(s) to " << outpdf << std::endl;
    return 0;
}

void DrawMUSICBoundaries()
{
        TBox *b = new TBox(-48.5,-48.5,-36.5,-24.5);
        b->SetFillStyle(0);
        b->SetLineColor(kRed);
        for(int i=1; i<32; i++)
        {
                TBox *bn = (TBox*)b->Clone();
                bn->SetX1((i%8)*12-48.5);
                bn->SetX2((i%8)*12-36.5);
                bn->SetY1((i/8)*24-48.5);
                bn->SetY2((i/8)*24-24.5);
                bn->Draw();
        }
}


 
