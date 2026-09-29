#include "PlotSizeSPC.h"


int main(int argc, char **argv){
        if(argc < 1){
                cout << "Too few arguments; please include the date data directory to summarize" << endl;
                        return 1;
	}
       
        std::string FilePath = argv[1];

        std::vector<string> tempfiles=util->readFileToVectorString("/storage/hive/project/phy-otte/sstepanoff3/MakeThingsPretty/MakeSizeAndImagePixels/Hist.txt");
        
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
        
        // Int_t ci = TColor::GetFreeColorIndex();
        // TColor *color = new TColor(ci, 238, 243, 246);
        // Color_t thecolor = color->GetNumber();
        
        // canvas->SetFillColor(thecolor);
        
        // // Margins
        // Float_t lMargin = 0.12;
        // Float_t rMargin = 0.05;
        // Float_t bMargin = 0.15;
        // Float_t tMargin = 0.05;
        for(int i = 1; i < files.size(); i++){
                FilePath = Form("%s",(files[i][3]).c_str());
                cout << "file path: " <<FilePath << endl;
                
                canvas->cd();
                gPad->SetLeftMargin(0.15);
                gPad->SetRightMargin(0.15);


                
                TH2F *h2 = 0;
                TH2F* h2more =0;
                //  new TH2F("h2","Single Pixel Event" , 16, -0.5, 15.5,16, -0.5, 15.5);
                TFile *f = new TFile(FilePath.c_str());
                // h2 = (TH2F*)f->Get("PreN20250424-F04:21-E0TH2F");
                std::string HistTitle = files[i][2].c_str();
                std::replace(HistTitle.begin(), HistTitle.end(), '%', ' ');
                cout << HistTitle << endl;
                // h2->SetTitle(Title.c_str());

                // h2 = (TH2F*)f->Get(Form("%s", (files[i][2]).c_str()));
                h2 = (TH2F*)f->Get(HistTitle.c_str());
                h2more = new TH2F("h2more", "; Size; Image Pixels;Events", SIZEstep, SIZEmin,SIZEmax,SPstep,SPmin,SPmax);
                h2more->SetStats(0);
                
                for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
                        for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
                                h2more->SetBinContent(ix, iy, h2->GetBinContent(ix, iy));
                                // h2more->SetBinError(ix, iy, h2->GetBinError(ix, iy)); // optional
                        }
                }
                LogBinning(h2more);

                cout << files[i][4] << endl;
                if(files[i][4]== "PE"){
                        h2->SetTitle("Temp;Size;Image Pixels;Events");
                } else {
                        h2->SetTitle("Temp;Pixels along camera x axis;Pixels along camera y axis;Photons");

                }
                std::string Title = files[i][1].c_str();
                std::replace(Title.begin(), Title.end(), '_', ' ');
                h2->SetTitle(Title.c_str());
                h2->GetZaxis()->SetTitleOffset(1.4);
                
                
                
                TFile *outFile = new TFile(Form("%s%s.root",outDir.c_str(),(files[i][0]).c_str()), "RECREATE");
                canvas->Print(Form("%s%s.pdf[",outDir.c_str(),(files[i][0]).c_str()));
                h2more->Draw("COLZ");
                canvas->SetLogx();
                // canvas->SetLogz();

                // Access the TPaletteAxis and move it
                TPaletteAxis* palette = (TPaletteAxis*)h2->GetListOfFunctions()->FindObject("palette");
                if (palette) {
                palette->SetX1NDC(0.86); // new left X position in NDC
                palette->SetX2NDC(0.90); // new right X position in NDC
                palette->SetY1NDC(0.1); // optional: new bottom Y position
                palette->SetY2NDC(0.9); // optional: new top Y position
                canvas->Modified();  // mark the canvas as modified
                canvas->Update();    // redraw with the new palette position
                }

                // TLine *l=new TLine(185,-0.5,185,21.5);
                // l->SetLineWidth(6);
                // l->SetLineColor(kBlack);
                // l->Draw();
                TLine *l2=new TLine(-0.5,4.5,100000,4.5);
                l2->SetLineWidth(6);
                l2->SetLineColor(kBlack);
                l2->Draw();

                // auto ar5 = new TArrow(180.0, 5.0,450.0, 5.0,0.02);
                auto ar5 = new TArrow(450.0, 5.0,450.0, 7.5,0.02);
                // ar5->SetAngle(60);
                ar5->SetLineWidth(2);
                ar5->SetLineColor(kBlack);
                // ar5->SetFillStyle(3008);
                // ar5->SetFillColor(kBlack);
                ar5->Draw();
                
                // TText *t1 = new TText(140, 12.5, "Size Cut");
                // t1->SetTextAngle(90.0);
                // t1->SetTextAlign(22);
                // t1->SetTextSize(0.03);
                // t1->SetTextColor(kBlack);
                // t1->Draw();
                
                TText *t2 = new TText(5000, 4.0, "Image Pixels Cut");
                t2->SetTextAlign(22);
                t2->SetTextSize(0.03);
                t2->SetTextColor(kBlack);
                t2->Draw();
                
                Int_t ci2 = TColor::GetFreeColorIndex();
                TColor *color2 = new TColor(ci2, 207, 181, 59);
                Color_t thecolor2 = color2->GetNumber();
                TText *t3 = new TText(2520, 18.0, "*");
                t3->SetTextAlign(22);
                t3->SetTextSize(0.1);
                t3->SetTextColor(thecolor2);
                t3->Draw();
                // plottools->DrawMUSICBoundaries();
                canvas->Update();
                h2->Write("hist");
                canvas->Write(h2->GetTitle());
                canvas->Print(Form("%s%s.pdf",outDir.c_str(),(files[i][0]).c_str()));
                
                
                
                canvas->SaveAs(Form("%s%s.root",outDir.c_str(),(files[i][0]).c_str()));
                canvas->SaveAs(Form("%s%s.png",outDir.c_str(),(files[i][0]).c_str()));
                canvas->Print(Form("%s%s.pdf]",outDir.c_str(),(files[i][0]).c_str()));
                f->Close();
                outFile->Close();
                // util->setFilePermissions(Form("%s%s/%s",outDir.c_str().c_str(),folString.c_str(),fileNamesVec[f].c_str()));
                // util->setFilePermissions(Form("%s%s/plots_%s",outDir.c_str().c_str(),folString.c_str(),fileNamesVec[f].c_str()));
        }
                
        cout << "Completed!" << endl;   
}


