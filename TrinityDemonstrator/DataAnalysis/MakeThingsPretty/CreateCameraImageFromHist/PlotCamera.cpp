#include "PlotCamera.h"

int main(int argc, char **argv){
    if(argc < 3){
        cout << "Too few arguments; please include the date data directory and mount flag" << endl;
        return 1;
    }

    std::string FilePath = argv[1];
    std::string mount = argv[2];

    if (mount == "y"){
        std::cout << "using mounted directory path" << std::endl;
        mnt = "/mnt/";
        outDir = "/mnt/DataAnalysis/MakeThingsPretty/Images/";
    } else if (mount == "n"){
        std::cout << "using default directory path" << std::endl;
        mnt = "";
    } else {
        std::cout << "using specific directory path: " << mount << std::endl;
        mnt = mount.c_str();
        outDir = Form("%s/mnt/DataAnalysis/MakeThingsPretty/Images/",mnt.c_str());
    }
    
    std::vector<string> tempfiles = util->readFileToVectorString(Form("%s%s", mnt.c_str(), FilePath.c_str()));
    cout << tempfiles[0] << endl;

    auto files = splitCSVLines(tempfiles);

    for (const auto& row : files) {
        for (const auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    canvas = new TCanvas("Display","Event Display",1000,900);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);

    Int_t ci = TColor::GetFreeColorIndex();
    TColor *color = new TColor(ci, 238.f/255.f, 243.f/255.f, 246.f/255.f);
    Color_t thecolor = color->GetNumber();

    canvas->SetFillColor(thecolor);

    for(int i = 1; i < (int)files.size(); i++){
        FilePath = Form("%s",(files[i][3]).c_str());
        cout << "file path: " << FilePath << endl;

        canvas->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.15);

        TFile *f = new TFile(FilePath.c_str());
        if (!f || f->IsZombie()){
            cerr << "Error opening file: " << FilePath << endl;
            continue;
        }

        std::string HistTitle = files[i][2];
        std::replace(HistTitle.begin(), HistTitle.end(), '%', ' ');
        cout << HistTitle << endl;

        TH2F *h2 = (TH2F*)f->Get(HistTitle.c_str());
        if (!h2){
            cerr << "Error: histogram not found: " << HistTitle << endl;
            f->Close();
            continue;
        }

        if(files[i][4] == "PE"){
            h2->SetTitle("Temp;Pixels along camera x axis;Pixels along camera y axis;PE");
        } else {
            h2->SetTitle("Temp;Pixels along camera x axis;Pixels along camera y axis;Photons");
        }

        std::string Title = files[i][1];
        std::replace(Title.begin(), Title.end(), '_', ' ');
        h2->SetTitle("");
        h2->GetZaxis()->SetTitleOffset(1.4);

        TFile *outFile = new TFile(Form("%s%s.root", outDir.c_str(), (files[i][0]).c_str()), "RECREATE");
        canvas->Print(Form("%s%s.pdf[", outDir.c_str(), (files[i][0]).c_str()));

        h2->Draw("COLZ");
        canvas->Update();

        TPaletteAxis* palette = (TPaletteAxis*)h2->GetListOfFunctions()->FindObject("palette");
        if (palette) {
            palette->SetX1NDC(0.86);
            palette->SetX2NDC(0.90);
            palette->SetY1NDC(0.1);
            palette->SetY2NDC(0.9);
            canvas->Modified();
            canvas->Update();
        }

        TLine *l = new TLine(-0.5, 2.5, 15.5, 2.5);
        l->SetLineWidth(6);
        l->SetLineColor(kGray);

        TText *t1 = new TText(13.5, 2.93, "Ground");
        t1->SetTextAlign(22);
        t1->SetTextSize(0.05);
        t1->SetTextColor(kGray);

        TText *t2 = new TText(14.58, 1.9, "Sky");
        t2->SetTextAlign(22);
        t2->SetTextSize(0.05);
        t2->SetTextColor(kGray);

        if (files[i][0].find("Cleaned") != std::string::npos || files[i][0].find("Sim") != std::string::npos) {
            l->SetLineColor(kBlack);
            t1->SetTextColor(kBlack);
            t2->SetTextColor(kBlack);
        }

        if (files[i][0].find("Sim") == std::string::npos){
            l->Draw();
            t1->Draw();
            t2->Draw();
        }

        plottools->DrawSIABBoundaries();
        canvas->Update();

        outFile->cd();
        h2->Write("hist");
        canvas->Write(h2->GetTitle());

        canvas->Print(Form("%s%s.pdf",  outDir.c_str(), (files[i][0]).c_str()));
        canvas->SaveAs(Form("%s%s.png", outDir.c_str(), (files[i][0]).c_str()));
        canvas->Print(Form("%s%s.pdf]", outDir.c_str(), (files[i][0]).c_str()));

        f->Close();
        outFile->Close();
    }

    cout << "Completed!" << endl;
}