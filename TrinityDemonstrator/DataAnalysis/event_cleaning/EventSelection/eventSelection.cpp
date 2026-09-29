#include "eventSelection.h"

// ---------------------------------------------------------------------------
// Which half of the data to keep. Change this by hand and recompile.
//   'o' door open   : only events from files that ARE in the file list
//   'c' door closed : only events from files that are NOT in the file list
//   'a' all         : every event, list membership is only counted
// Ignored when there is no file list (file_list == "n") or for sims.
// ---------------------------------------------------------------------------
const char selection = 'o';

// Door open / door closed cut for one event, based on the cleaning file it came from.
// Sims and runs with no list have no door state so they always pass. Every event that
// reaches here is counted, so the door open/closed totals cover the whole run in all
// three modes.
bool PassesDoorSelection(const std::string& currentfilename) {
    if (isSim || file_list == "n") return true;

    bool inList = std::find(allowedFilesVec.begin(), allowedFilesVec.end(), currentfilename)
                  != allowedFilesVec.end();
    if (inList) { dooropen++; } else { doorclosed++; }

    if (selection == 'o') return inList;
    if (selection == 'c') return !inList;
    return true; // 'a'
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <mount> <folder> <filelist>\n", argv[0]);
        printf("  filelist : path to the door open list, or n for no list\n");
        printf("  door open/closed selection is the 'selection' constant at the top of eventSelection.cpp\n");
        return 1;
    }

    mount = argv[1];
    folder = argv[2];
    file_list = argv[3]; // set globals declared in eventSelection.h
    // read in the file list and then only allow events that are within the file list to be saved to
    // the output pdf/ root file. 
    // if no file_list is given then all files will go to the pdf/root file

    // ensure the file is mounted correctly in the container
    std::cout << "mount: " << mount << " folder: " << folder << "File_list:" << file_list << std::endl;
    
    std::cout << "using standard directory path: " << mnt << std::endl;
    if (mount != "y"){
        std::cout << "using specific directory path: " << mnt << std::endl;
        mnt=mount.c_str();
    }
    // add the mount path to the any paths
    // if the file list refers to simulation data (contains "sim") read from OutputSim
    isSim = (file_list.find("sim") != std::string::npos);
    if (isSim) {
        std::cout << "simulation file list detected, using OutputSim path" << std::endl;
        FolderPath = Form("%s/DataAnalysis/event_cleaning/OutputSim/%s/", mnt.c_str(), folder.c_str());
    } else {
        FolderPath = Form("%s/DataAnalysis/event_cleaning/Output/%s/", mnt.c_str(), folder.c_str());
    }
    AllowedFilesPath = Form("%s/%s", mnt.c_str(), file_list.c_str());
    std::cout << "folder path: " << FolderPath << "\nAllowed path: " << AllowedFilesPath <<std::endl;

    fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    cout << "This is the first file: " << fileNamesVec[0] << endl;

    if (file_list != "n" && file_list != "sim"){
        allowedFilesVec=util->readFileToVectorString(AllowedFilesPath.c_str());
        cout << "Number of allowed files: " << allowedFilesVec.size() << endl;
        cout << "Sample file from allowed Files " << allowedFilesVec[50] << endl;
    }

    TCanvas* c_cleaned = new TCanvas("c_cleaned", "c", 10, 10, 800, 600);
    c_cleaned->Print((OutputPdfFile + "[").c_str());
    TFile *file = new TFile(OutputRootFile.c_str(), "RECREATE");
    file->Close();

    for (auto f: fileNamesVec) {
        
        std::string fname = Form("%s%s",FolderPath.c_str(), f.c_str());
        cout << fname << endl;
        TFile* file = TFile::Open(fname.c_str(), "READ");
        if (!file || file->IsZombie()) {
            printf("Error opening file %s\n", f.c_str());
            return 1;
        }

        // Get the tree containing EventInfo objects
        TTree* tree = nullptr;
        file->GetObject("EventCleaning", tree); // Replace "Events" with your actual tree name
        if (!tree) {
            printf("TTree 'EventCleaning' not found in file.\n");
            file->Close();
            return 1;
        }

        EventInfo* event = nullptr;
        tree->SetBranchAddress("Cleaned", &event); // Replace "EventInfo" with your branch name

        // Older EventCleaning files were written before the major axis quantities existed, so the
        // branch is missing and the getter would silently hand back the EventInfo default of 0.
        // When it is absent fall back to recomputing the ratio the same way CheckCrossPoints does
        // in ../ClusterCleaning/EventCleaning.h; when it is there just read the stored value.
        bool hasRatioBranch = (tree->GetBranch("RatioPixelsMajorAxis") != nullptr) ||
                              (tree->FindLeaf("RatioPixelsMajorAxis") != nullptr);
        if (!hasRatioBranch) {
            printf("No RatioPixelsMajorAxis branch in %s, recomputing it from the panel 4 histogram.\n", f.c_str());
        }

        // For simulation data, EventCleaning copies the input "Sim" (GrOptics / SEvent)
        // tree into the output file aligned 1:1 with the EventCleaning entries. Read it so the
        // per-event neutrino energy is available for the saved-vs-removed plot.
        TTree* simTree = nullptr;
        SEvent* simRunData = nullptr;
        if (isSim) {
            simTree = (TTree*)file->Get("Sim");
            if (simTree) {
                simRunData = new SEvent();
                simTree->SetBranchAddress("GrOptics", &simRunData);
            } else {
                printf("TTree 'Sim' not found in sim file %s; energies will be skipped.\n", f.c_str());
            }
        }

        Long64_t nEntries = tree->GetEntries();
        // load into the PlotHelp class 
        //  nEntries = 10;
        for (Long64_t i = 0; i < nEntries; ++i) {
            // Testing on My Machine
            tree->GetEntry(i);
            if (!event) continue;

            // sims: read the neutrino energy for this event (aligned by entry) and track it so
            // AddEventFlags can bin saved (flag 5) vs removed energies. Non-sim: do not track.
            if (isSim && simTree && simRunData && i < simTree->GetEntries()) {
                simTree->GetEntry(i);
                plothelp->SetSimEnergy(simRunData->GetNeutrinoEnergy(), true);
            } else {
                plothelp->SetSimEnergy(0.0, false);
            }

            // For actual use
            // plothelp->AddTotalEvent(1);
            // Add the logic for these events so i can practice counting them
            // print the before and after histgrams to the pdf/root file
            
            // if all the pre cleaning/ cleanedpanel are false then print the pdf/roof 2 panel image
            
            
            
            std::string currentfilename = (event->GetFile()).erase(0, 7);
            cout << "Current File Name: " << currentfilename << endl;

            // door open / door closed cut, set by the 'selection' constant at the top of the file.
            // Everything past this point belongs to the half of the data that was asked for.
            TotalEvents++;
            if (!PassesDoorSelection(currentfilename)) {
                continue;
            }
            
            plothelp->AddtoEventDate(event->GetDate());
            plothelp->AddtoRMSratioAVGamp(event->GetRMS_AvgAmp());
            plothelp->AddEventFlags(event->GetEventFlag());
            
            
            if (event->GetEventFlag() == 9) {
                HLEDEvents++;   
            }

            // anything that is not flag 5 was cleaned away (9 = flasher) so there is nothing to select on
            if (event->GetEventFlag() != 5) {
                continue;
            }
            SurvivngEventCount++;

            cout << "Event Flag: " << event->GetEventFlag() << endl;
            // Event was never cleaned away so that means it survived and now needs
            // Event selection
            // create the 2 panel image
            auto c = new TCanvas("c", "2-d options",10,10, 800, 600);
            c->Divide(2, 1);
            c->SetRightMargin(0.15); // Adjust right margin for color palette
            c->SetBottomMargin(0.15); // Adjust bottom margin for x-axis labels
            c->SetLeftMargin(0.15); // Adjust left margin for y-axis labels
            
            TH2F* hist = event->GetHPanel1();
            TH2F* hist4 = event->GetHPanel4();
            // ratio of pixels on the major axis: read from the file when present, otherwise
            // filled in below from hist4. Stays 0 for events with no panel 4 histogram.
            double ratioPixelsMajorAxis  = hasRatioBranch ? event->GetRatioPixelsMajorAxis()  : 0.0;
            int    pixelsonMajorAxis     = hasRatioBranch ? event->GetPixelsonMajorAxis()    : 0;
            int    pixelsoffMajorAxis    = hasRatioBranch ? event->GetPixelsoffMajorAxis()   : 0;
            double rmsMajorAxis          = hasRatioBranch ? event->GetRMSMajorAxis()         : 0.0;
            double weightedRmsMajorAxis  = hasRatioBranch ? event->GetWeightedRMSMajorAxis() : 0.0;
            // std::vector<float> hvcurrent = event->Gethvc();
            // float hvsum = 0.0;
            // for (const auto& hv : hvcurrent) {
            //     hvsum += hv;
            // }
            // float hvavg = hvsum / hvcurrent.size();
            // cout << "Average HV: " << hvavg << " V" << endl;
            // if (hist4 && hvavg > 4.0) {
            if (hist4) {
                // printf("I Exist");
                c_cleaned->cd(0);
                c_cleaned->Divide(2, 1);
                c_cleaned->cd(1);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                TLatex *subtitle = new TLatex();
                subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
                subtitle->SetTextSize(0.03);
                
                subtitle->DrawLatex(0.1, 0.93, Form("Avg Amp Whole Camera : %.2f - Avg RMS: %.2f", event->GetAvgAmp(), event->GetRMS()));
                subtitle->DrawLatex(0.1, 0.91, Form("Triggered MUSIC: %i  Triggered Pixel: %i", event->GetTriggeredMUSICID(), event->GetTriggeredPixelID()));




                
                c_cleaned->Update();
                c->cd(1);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c->Update();
                c_cleaned->cd(2);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist4->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c->cd(2);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist4->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c_cleaned->cd(2);
                
                
                double meanx = event->GetCOGx(); 
                double meany = event->GetCOGy(); 
                cout << "MEAN " << meanx << "," << meany << endl;
                double anglerad = event->GetAngle();
                double angledeg = anglerad*(180.0/3.141592653589793238463);
                
                
                double r1 = event->Getr1();
                double r2 = event->Getr2();    
                TArrow* arrow = new TArrow(meanx - 2*(r1*cos(anglerad)), meany - 2*(r1*sin(anglerad)), meanx + 2*(r1*cos(anglerad)), meany + 2*(r1*sin(anglerad)), 0.01, "|"); // "|>" option gives an arrowhead
                TArrow* arrow1 = new TArrow(meanx - 2*(r2*cos(anglerad+1.5708)), meany - 2*(r2*sin(anglerad+1.5708)), meanx + 2*(r2*cos(anglerad+1.5708)), meany + 2*(r2*sin(anglerad+1.5708)), 0.01, "|"); // "|>" option gives an arrowhead
                
                arrow->SetLineColor(kRed); // Optional: Set the color of the arrow
                arrow->Draw("SAME");             // Draw the arrow on the same canvas
                arrow1->SetLineColor(kBlue); // Optional: Set the color of the arrow
                arrow1->Draw("SAME");             // Draw the arrow on the same canvas

                // Same major axis geometry as CheckCrossPoints in ../ClusterCleaning/EventCleaning.h.
                // The per pixel distances are never stored in EventInfo (Distance2MajorAxis is a
                // single value overwritten per pixel by the cleaning) so they are always recomputed
                // here; the event level values are only recomputed when the file predates them.
                // get the arrow slope
                double x1 = arrow->GetX1();
                double y1 = arrow->GetY1();
                double x2 = arrow->GetX2();
                double y2 = arrow->GetY2();

                // Line vector components
                double dx = x2 - x1;
                double dy = y2 - y1;
                double len = std::sqrt(dx*dx + dy*dy);

                // Degenerate line guard
                if (len != 0.0) {
                    int cLineIntersectPixels    = 0;
                    int cLineNonIntersectPixels = 0;

                    int nBinsX = hist4->GetNbinsX();
                    int nBinsY = hist4->GetNbinsY();

                    double RMSMajorAxis = 0.0;
                    double WeightedRMSMajorAxis = 0.0;
                    double totalWeight = 0.0;

                    for (int ix = 1; ix <= nBinsX; ++ix) {
                        for (int iy = 1; iy <= nBinsY; ++iy) {

                            if (hist4->GetBinContent(ix, iy) <= 0) continue;

                            // Bin center in axis coordinates
                            double cx = hist4->GetXaxis()->GetBinCenter(ix);
                            double cy = hist4->GetYaxis()->GetBinCenter(iy);

                            // Perpendicular distance from bin center to the infinite line
                            double dist = std::abs((cy - y1)*dx - (cx - x1)*dy) / len;
                            plothelp->AddtoDistance2MajorAxis(dist);
                            RMSMajorAxis += dist*dist;
                            WeightedRMSMajorAxis += dist*dist*hist4->GetBinContent(ix, iy);
                            totalWeight += hist4->GetBinContent(ix, iy);
                            // Half-diagonal of the bin as intersection threshold
                            double hw = 0.5 * hist4->GetXaxis()->GetBinWidth(ix);
                            double hh = 0.5 * hist4->GetYaxis()->GetBinWidth(iy);
                            double threshold = 0.905*std::sqrt(hw*hw + hh*hh);
                            if (dist <= threshold) {
                                ++cLineIntersectPixels;
                            }
                            else {
                                ++cLineNonIntersectPixels;
                            }
                        }
                    }
                    int nPixels = cLineIntersectPixels + cLineNonIntersectPixels;
                    RMSMajorAxis = std::sqrt(RMSMajorAxis/nPixels);
                    WeightedRMSMajorAxis = std::sqrt(WeightedRMSMajorAxis/(totalWeight));

                    // only take the recomputed values when the file could not supply them
                    if (!hasRatioBranch && nPixels > 0) {
                        ratioPixelsMajorAxis = (double)cLineIntersectPixels/nPixels;
                        pixelsonMajorAxis    = cLineIntersectPixels;
                        pixelsoffMajorAxis   = cLineNonIntersectPixels;
                        rmsMajorAxis         = RMSMajorAxis;
                        weightedRmsMajorAxis = WeightedRMSMajorAxis;
                    }

                    plothelp->AddtoPixelsonMajorAxis(pixelsonMajorAxis);
                    plothelp->AddtoPixelsoffMajorAxis(pixelsoffMajorAxis);
                    plothelp->AddtoRMSMajorAxis(rmsMajorAxis);
                    plothelp->AddtoWeightedRMSMajorAxis(weightedRmsMajorAxis);
                    // repeat this event's ratio once per pixel so it lines up with the distances
                    plothelp->AddtoRatioPixelsMajorAxisPerPixel(ratioPixelsMajorAxis, nPixels);
                }


                TEllipse* ell = new TEllipse(meanx, meany,r1,r2,0,360,angledeg);
                ell->SetFillColorAlpha(kGreen,0.00);
                ell->Draw("SAME");
                
                TLatex* title = new TLatex();
                title->SetNDC(); // Set to Normalized Device Coordinates (NDC)
                title->SetTextSize(0.03);
                
                // cev->GetSurvivingPixelTotalAmpPanel3()/util->GetADCtoPEratio(),
                title->DrawLatex(0, 0.93, Form("WL:%.2f Pixels:%i Area:%.2f Size:%.2f Conc:%.2f M3Long:(%s,  %s)",
                    event->GetWLRatio(),
                    event->GetSurvivngPixels(),
                    event->GetArea(),
                    event->GetSize(),
                    event->GetConc(),
                    event->GetM3Longx().c_str(),
                    event->GetM3Longy().c_str()));

                // For sims, draw the emergence angle and distance to the emergence point at the
                // bottom of the canvas. Drawn before the pdf Print and root Write so it appears in both.
                TPaveText* simInfo = nullptr;
                if (isSim && simRunData) {
                    // distance to the emergence point from the telescope position vector
                    double Rx = simRunData->GetTelescope_Xpos();
                    double Ry = simRunData->GetTelescope_Ypos();
                    double Rz = simRunData->GetTelescope_Zpos();
                    double emergenceDistance = sqrt(Rx*Rx + Ry*Ry + Rz*Rz);

                    // emergence angle: wrap azimuth to [-180,180] to measure from the telescope axis
                    double azimuthDeg = simRunData->GetAzimuthAngle();
                    double emergenceAngle = (azimuthDeg > 180.0) ? (azimuthDeg - 360.0) : azimuthDeg;

                    c_cleaned->cd(0);
                    simInfo = new TPaveText(0.2, 0.0, 0.8, 0.06, "NDC");
                    simInfo->SetFillColorAlpha(0, 0.0);
                    simInfo->SetFillStyle(0);
                    simInfo->SetBorderSize(0);
                    simInfo->SetTextSize(0.02);
                    simInfo->AddText(Form("Energy: %.3g GeV    Emergence Angle: %.3g deg    Distance to Emergence Point: %.3g m",
                        simRunData->GetNeutrinoEnergy(), emergenceAngle, emergenceDistance));
                    simInfo->Draw();
                }

                c->Update();
                c_cleaned->Update();
                c_cleaned->Print(OutputPdfFile.c_str());
                file = new TFile(OutputRootFile.c_str(), "UPDATE");
                c_cleaned->Write(Form("N%i-F%s-E%i", event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                hist->Write(Form("PreN%i-F%s-E%iTH2F", event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                hist4->Write(Form("PostN%i-F%s-E%iTH2F",event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                file->Close();
                c_cleaned->Clear();
                    delete subtitle;
                delete title;
                if (simInfo) delete simInfo;
                // c->SaveAs(Form("hist_entry_%lld.png", i));
            } else {
                printf("Histogram not found in EventInfo object at entry %lld.\n", i);
            }

            


            
            int date = event->GetDate();
            cout << "Date: " << date << endl;
            cout << "Event Number: " << event->GetEvent() << endl;
            
            // add to test scirpt
            plothelp->AddtoWL(event->GetWLRatio());
            plothelp->AddtoL(event->GetL());
            plothelp->AddtoW(event->GetW());
            plothelp->AddtoSize(event->GetSize());
            plothelp->AddtoSurvivingPixelCount(event->GetSurvivngPixels());
            plothelp->AddtoConcentation(event->GetConc());
            plothelp->AddtoCOGx(event->GetCOGx());
            plothelp->AddtoCOGy(event->GetCOGy());
            plothelp->AddtoTriggeredPixelsID(event->GetTriggeredPixelID());
            plothelp->AddtoCoreRatio(event->GetCoreRatio());
            // cout << "Ratio core:" << event->GetCoreRatio() << endl;
            // sleep(1);
            plothelp->AddtoNumberOfCores(event->GetNumberOfCores());
            plothelp->AddtoRatioPixelsMajorAxis(ratioPixelsMajorAxis);
            // cout << "# core:" << event->GetNumberOfCores() << endl;
            // sleep(1);



            delete c; // Clean up the canvas after use
            // delete hist; // Clean up the histogram after use
            // delete hist4; // Clean up the histogram after use
        }
        if (simRunData) { delete simRunData; } // simTree is owned by file and freed on Close
        file->Close();
    }
        // Make cuts

    // TCanvas* c_cleaned = new TCanvas("c", "c", 800, 600);
    // c_cleaned->Print("EventSelectionPlots.pdf[");
    // Make Plots
    
   file = new TFile(OutputRootFile.c_str(), "UPDATE");
    
    plothelp->PlothWL(c_cleaned, OutputPdfFile);
    plothelp->PlotEventFlags(c_cleaned, OutputPdfFile);
    plothelp->PlothdistLandW(c_cleaned, OutputPdfFile);
    plothelp->PlothSize(c_cleaned, OutputPdfFile);    
    plothelp->PlothSizeConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothSizeSPC(c_cleaned, OutputPdfFile);    
    plothelp->PlothCoreRatio(c_cleaned, OutputPdfFile);    
    plothelp->PlothNumberofCores(c_cleaned, OutputPdfFile);    
    plothelp->PlothCoreRatioandNumberofCores(c_cleaned, OutputPdfFile);    
    plothelp->PlothWLConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothWLSPC(c_cleaned, OutputPdfFile); 
    plothelp->PlothSIZEWL(c_cleaned, OutputPdfFile);   
    plothelp->PlothLvW(c_cleaned, OutputPdfFile);
    plothelp->PlothSPCConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothCOG(c_cleaned, OutputPdfFile);    
    plothelp->PlothTPID(c_cleaned, OutputPdfFile);
    plothelp->PlothRMSratioAvg(c_cleaned, OutputPdfFile);
    plothelp->PlothEventDateTriggeredPixel(c_cleaned, OutputPdfFile);
    plothelp->PlothncoreSIZE(c_cleaned, OutputPdfFile);
    plothelp->PlothnCoreSPC(c_cleaned, OutputPdfFile);
    plothelp->PlothCRSPC(c_cleaned, OutputPdfFile);
    plothelp->PlothSIZEWandL(c_cleaned, OutputPdfFile);
    plothelp->PlothSimEnergySavedVsNotSaved(c_cleaned, OutputPdfFile); // sims only: saved vs removed neutrino energy
    plothelp->PlotPixelsDistanceToMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlotPixelsRatioDistanceToMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlotdistRMSandWeightedRMS(c_cleaned, OutputPdfFile);
    plothelp->PlotPixelsOnMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlotPixelsOffMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlothRMSvsRatioDistance(c_cleaned, OutputPdfFile);
    plothelp->PlothWRMSvsRatioDistance(c_cleaned, OutputPdfFile);
    plothelp->PlothWLvsRatioPixelsMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlothDistancevsRatioPixelsMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlothCoreOverSPCvsRatioPixelsMajorAxis(c_cleaned, OutputPdfFile);
    plothelp->PlothSPCvsRatioPixelsMajorAxis(c_cleaned, OutputPdfFile);
    
    file->Close();
    // c_cleaned->Print("EventSelectionPlots.pdf"Close.c_str());
    util->setFilePermissions(OutputRootFile.c_str());
    // cout << "Completed!" << endl;   
    // // cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    // delete plothelp;
    
    c_cleaned->Print((OutputPdfFile + "]").c_str());

    // add this instead to a txt file
    ofstream filetxt;
    filetxt.open("OutputSummary.txt");
    if (filetxt.is_open())
    {
        filetxt << "Total Events in door open and door closed + flasher: " << TotalEvents << endl;
        // filetxt << "Total Surviving Events: " << plothelp->GetSurvivingEvent() << endl;
        // filetxt << "Total Pre Cleaned Events: " << plothelp->GetPreCleanedEvent() << endl;
        // filetxt << "Total Panel 2 Cleaned Events: " << plothelp->GetPanel2CleanedEvent() << endl;
        // filetxt << "Total Panel 3 Cleaned Events: " << plothelp->GetPanel3CleanedEvent() << endl;

        filetxt << "Door Open Event Total with Flasher: " << dooropen << endl;
        filetxt << "Door Closed Event Total with Flasher: " << doorclosed << endl;
        

        if (selection == 'o') {
            filetxt << "Door Open selection" << endl;
            filetxt << "Door Open Event Total without Flasher: " << dooropen - HLEDEvents << endl;
            filetxt << "Total Flasher Only Events: " << HLEDEvents << endl;
            filetxt << "Total Surviving Events: " << SurvivngEventCount << endl;
        } else if (selection == 'c') {
            filetxt << "Door Closed selection" << endl;
            filetxt << "Door Closed Event Total without Flasher: " << doorclosed - HLEDEvents << endl;
            filetxt << "Total Flasher Only Events: " << HLEDEvents << endl;
            filetxt << "Total Surviving Events: " << SurvivngEventCount << endl;
        } else {
            filetxt << "All Events selection" << endl;
            filetxt << "All events without Flasher: " << TotalEvents - HLEDEvents << endl;
            filetxt << "Total Flasher Only Events: " << HLEDEvents << endl;
            filetxt << "Total Surviving Events: " << SurvivngEventCount << endl;
        }

        // Close the file to free up resources.
        filetxt.close();
        cout << "File output summary created successfully." << endl;
    }
    else
    {
        cout << "ERROR: could not open OutputSummary.txt for writing." << endl;
    }

    
    
    
    return 0;
}