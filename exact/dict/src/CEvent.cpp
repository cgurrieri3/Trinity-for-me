#include "CEvent.h"
#include <iostream>

// Constructor
CEvent::CEvent() : TriggeredMUSICID(-1) {
    // Initialize TriggeredMUSICID to -1 (default value)
}

void CEvent::LoadFlasherCalibration(std::string CalibrationFactorDir, std::string folString, double biasVoltage) {
    // Start by creating a histogram for panel 1 then use that to copy and create the 2nd panel
    // create the first panel, apply the Gain, apply the ADC to PE 
    TFile *flasher = new TFile(Form("%s%s_FlasherCalibration_Factor.root", CalibrationFactorDir.c_str(), folString.c_str()));
    if (!flasher || flasher->IsZombie()) {
        std::cerr << "Error: Could not open file " << Form("%s%s_FlasherCalibration_Factor.root", CalibrationFactorDir.c_str(), folString.c_str()) << std::endl;
    }

    std::string biasString = Form("CamFlasher%s", std::to_string(static_cast<int>(biasVoltage)).c_str());
    hflasher = (TH2F*)flasher->Get(biasString.c_str());
    if (!hflasher || hflasher->IsZombie()) {
        std::cerr << "Error: Could not find '" << biasString << "' in the file or it is invalid." << std::endl;
    }
    hflasher->SetDirectory(0);
    flasher->Close();
    delete flasher;
}

// this is for when you set all your gain and such when outside the script
void CEvent::SetAmplitudeValues(std::vector<float> amp) {
    // sets the peak amplitude  and assumes that gain calibration as already been set to the CEvent object
    AmplitudeValues = amp;
}

// setting the abs gain and the flasher calibration within the CEvent object for when you pull a time bin
void CEvent::SetAmplitudeValuesTimeBin(std::vector<float> amp, std::string CalibrationFactorDir, std::string folString, std::vector<double> absgain, double biasVoltage) {
    // sets the peak amplitude  and assumes that gain calibration as already been set to the CEvent object
    // Load the flasher calibration histogram
    LoadFlasherCalibration(CalibrationFactorDir, folString, biasVoltage);
    TH2F* hf = GetFlasherCalibration();
    
    // set the absolute gain from the ISiPM calss
    SetAbsoluteGain(absgain);

    AmplitudeValuesTimeBin = amp;
    for (int i = 0; i < MaxNofChannels; i++) {
        int nx, ny;
        IPlotTools::FindBin(i, &nx, &ny);
        double flashercalibratedamp = amp[i]*(1.0/hf->GetBinContent(nx + 1, ny + 1));
        // std::cout << "flasher calib: " << flashercalibratedamp << std::endl;
        AmplitudeValuesTimeBin[i] = flashercalibratedamp*(1.0/AbsoluteGain[i]);
        // std::cout << "AmplitudeValuesTimeBin[" << i << "]: " << AmplitudeValuesTimeBin[i] << std::endl;
    }
    delete hf;
}

void CEvent::SetRMS(std::vector<float> amp){
    float sumsq = 0;
    for (std::size_t i = 0; i < amp.size(); i++){
        sumsq += amp[i]*amp[i];
        // cout << "sumsq: " << sumsq << endl;
        RMS = sqrt(sumsq)/MaxNofChannels;
        // cout << "RMS: " << RMS << endl;
    }
}

void CEvent::SetPanel1(TH2F* hcam_panel1) {
    hcam_panel1->SetStats(0);
    hcam_panel1->GetXaxis()->SetLabelSize(0.03);
    hcam_panel1->GetYaxis()->SetLabelSize(0.03);
    hcam_panel1->SetXTitle("Pixel Bin along x axis of Camera");
    hcam_panel1->SetYTitle("Pixel Bin along y axis of Camera");
    hcam_panel1->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam_panel1->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam_panel1->SetMinimum(-1);
    
    for(int j = 0; j<MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        hcam_panel1->SetBinContent(nx + 1, ny + 1, AmplitudeValuesTimeBin[j]);
    }
    
    hcam_panel1->Draw("colz");
    IPlotTools::DrawMUSICBoundaries();
    
}

void CEvent::SetPanel2(TH2F* hcam_panel2, int CorePixelAmpCutOff) {

    
    for(int j = 0; j<MaxNofChannels; j++){
        
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent= hcam_panel2->GetBinContent(nx+1,ny+1);
        if (binContent >= CorePixelAmpCutOff) { 
            hcam_panel2->SetBinContent(nx + 1, ny + 1, binContent);
            // std::cout << "Surviving Panel 2 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            SurvivingPixelPanel2.push_back(j);
        } else {
            hcam_panel2->SetBinContent(nx + 1, ny + 1, 0);
        }
    }

    hcam_panel2->Draw("colz");
    hcam_panel2->SetMinimum(7.95);
    IPlotTools::DrawMUSICBoundaries();
}


// identifies if a neighbor to the triggered pixel is a core and then only checks the surrounding pixels of the core to be saved

void CEvent::SetPanel3(TH2F* hcam_panel3, std::string neighborDir, int NumberOfCores, float CoreCutOff , float Crosstalkallowance) {
    // std::vector<int> PixelPeakTimes = PeakTimeBin;
    // int TriggeredPixelPeakTime = PixelPeakTimes[MaxAmplitdePixelID];
    // std::cout << "Triggered Pixel Peak Time: " << TriggeredPixelPeakTime << std::endl;
    // Get neighboring pixels to triggered pixel
    
    // get the 2nd core pixel 
    
    // Check to see if there is a pixel that is connected to the triggered pixel that is at least PE_second > PE_Trig - 1.5sqrt(PE_Trig)
    isSecondCorePixel(hcam_panel3,MaxAmplitdePixelID, neighborDir, CoreCutOff ,NumberOfCores);
    if (SecondCoreID == -1){
        SurvivingPixelPanel3 = {MaxAmplitdePixelID};
        return;
    }

    std::cout << "Trig Id: " << MaxAmplitdePixelID << " Core ID; " << SecondCoreID << std::endl;

    // is you are using core avg
    // double coreAverageAmp = (MaxEventAmplitde +SecondCoreAmp)/2.0;
    // std::cout << "Core Average Amplitude: " << coreAverageAmp << std::endl;
    // set  them to the surving 
    // std::vector<int> neighborsurvivingPanel3Pixels = {MaxAmplitdePixelID, SecondCoreID};

    // max between the two core pixels
    double maxcoreAmp = std::max(MaxEventAmplitde, SecondCoreAmp);

    
    
    // SetCoreIDs({-300});
    // std::cout<< "Try getting Cores:" <<updateCore[0] << std::endl;

    // go around triggered pixel and find those that are 30% of core avg
    // go  around second pixel and find those that are   30% ofthe second core pixel 
    // CheckCloseNeigborsOnly(hcam_panel3, MaxAmplitdePixelID, SecondCoreID, 0.3 * maxcoreAmp, neighborDir);
    CheckCloseNeigborsOnly(hcam_panel3, MaxAmplitdePixelID, GetCoreIDs(), Crosstalkallowance * maxcoreAmp, neighborDir);
    
    

    // save all surviivng pixels to  hcam_panel3
    // std::cout << "Neightsbors after 1: ";
    // for (auto i: neighborsurvivingPanel3Pixels)
    //     std::cout << i;
    // std::cout << std::endl;
    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        
        if ((PeakTimeBin[j] >=237 && PeakTimeBin[j] <=240) && binContent > 0.0) {
            // std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            SurvivingPixelPanel3.push_back(j);
            SurvivingPixelTotalAmpPanel3 += binContent;
            hcam_panel3->SetBinContent(nx + 1, ny + 1, binContent);
        } else {
            if (j == MaxAmplitdePixelID || j == SecondCoreID) {
                MaxEventAmplitde = 0.0;
                SecondCoreAmp = 0.0;
            }
            hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
        }
    }


    std::vector<int> updateCore =  SecondCoreIDs;
    for (int i =0; i < updateCore.size(); i++){
        std::cout<< "Try getting Cores:" <<updateCore[i] << std::endl;
        
    }

    // Check lastly for only broadside connections to the triggered pixel so this mean you can clean the second core pixel if it is 
    // not connected to the triggered pixel

    std::vector<int> neighborsurvivingPanel3Pixels;
    TriggeredPixelNeighborhoodChecker(SurvivingPixelPanel3,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
    SurvivingPixelPanel3={};
    SurvivingPixelTotalAmpPanel3 = 0.0;
    totalCoreAmp = 0;
    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        
        if ( binContent > 0.0 && std::find(neighborsurvivingPanel3Pixels.begin(), neighborsurvivingPanel3Pixels.end(), j) != neighborsurvivingPanel3Pixels.end()) {
            // std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            SurvivingPixelPanel3.push_back(j);
            SurvivingPixelTotalAmpPanel3 += binContent;
            hcam_panel3->SetBinContent(nx + 1, ny + 1, binContent);
            if (std::find(updateCore.begin(), updateCore.end(), j) != updateCore.end()){
                totalCoreAmp += binContent;
            }
        } else {
            if (j == MaxAmplitdePixelID || j == SecondCoreID) {
                MaxEventAmplitde = 0.0;
                SecondCoreAmp = 0.0;
            }
            hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
            // TO DO:fix the core output int to reset the coreIDS to only include those plotted
        }
    }
    std::cout << "Total Core amp: " << totalCoreAmp << std::endl;
    // Check to see if there is a pixel that is connected to the triggered pixel that is at least PE_second > PE_Trig - 1.5sqrt(PE_Trig)
    // isSecondCorePixel(SurvivingPixelPanel3, hcam_panel3,MaxAmplitdePixelID, neighborDir);
    
    hcam_panel3->Draw("colz");
    IPlotTools::DrawMUSICBoundaries();

}


void CEvent::SetPanel3AllNeighboringPixels(TH2F* hcam_panel3, std::string neighborDir) {
    // std::vector<int> PixelPeakTimes = PeakTimeBin;
    // int TriggeredPixelPeakTime = PixelPeakTimes[MaxAmplitdePixelID];
    // std::cout << "Triggered Pixel Peak Time: " << TriggeredPixelPeakTime << std::endl;
    // Get neighboring pixels to triggered pixel
    std::vector<int> neighborsurvivingPanel3Pixels;
    std::vector<int> tempSurvivingPixelPanel3;

    // std::cout << "Neightsbors panel2:  ";
    // for (auto i: SurvivingPixelPanel2)
    //     std::cout << i;
    // std::cout << std::endl;
    
    TriggeredPixelNeighborhoodChecker(SurvivingPixelPanel2,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
    
    // std::cout << "Neightsbors before before: ";
    // for (auto i: neighborsurvivingPanel3Pixels)
    //     std::cout << i;
    // std::cout << std::endl;
    
    
    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        if ((PeakTimeBin[j] == 239 || PeakTimeBin[j] == 238 || PeakTimeBin[j] == 237) && std::find(std::begin(neighborsurvivingPanel3Pixels), std::end(neighborsurvivingPanel3Pixels), j) != std::end(neighborsurvivingPanel3Pixels)) {
            // std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            tempSurvivingPixelPanel3.push_back(j);
            //hcam_panel3->SetBinContent(nx + 1, ny + 1, AmplitudeValuesTimeBin[j]);
        } //else {
            // hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
            //}
    }
    
    // std::cout << "Neightsbors before 1: ";
    // for (auto i: tempSurvivingPixelPanel3)
    //     std::cout << i;
    // std::cout << std::endl;
    neighborsurvivingPanel3Pixels={};
    TriggeredPixelNeighborhoodChecker(tempSurvivingPixelPanel3,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
    tempSurvivingPixelPanel3 = {};
    // Remove elements from neighbors that are not in surviving2
    for (int p = 0; p < neighborsurvivingPanel3Pixels.size(); p++) {
        // std::cout << "Pixel " << neighborsurvivingPanel3Pixels[p] << std::endl;
        for (int s=0; s<SurvivingPixelPanel2.size(); s++) {
            if (neighborsurvivingPanel3Pixels[p] == SurvivingPixelPanel2[s]) {
                // std::cout << "Found surviving pixel: " << neighborsurvivingPanel3Pixels[p] << std::endl;
                tempSurvivingPixelPanel3.push_back(neighborsurvivingPanel3Pixels[p]);
            }
            
        }
        
    }
    neighborsurvivingPanel3Pixels = tempSurvivingPixelPanel3;
    // std::cout << "Neightsbors after 1: ";
    // for (auto i: neighborsurvivingPanel3Pixels)
    //     std::cout << i;
    // std::cout << std::endl;
    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        if ((PeakTimeBin[j] ==237 || PeakTimeBin[j] ==238 || PeakTimeBin[j] == 239) && std::find(std::begin(neighborsurvivingPanel3Pixels), std::end(neighborsurvivingPanel3Pixels), j) != std::end(neighborsurvivingPanel3Pixels)) {
            std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            SurvivingPixelPanel3.push_back(j);
            SurvivingPixelTotalAmpPanel3 += binContent;
            hcam_panel3->SetBinContent(nx + 1, ny + 1, binContent);
        } else {
            hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
        }
    }
    
    // Check to see if there is a pixel that is connected to the triggered pixel that is at least PE_second > PE_Trig - 1.5sqrt(PE_Trig)
    // isSecondCorePixel(hcam_panel3,MaxAmplitdePixelID, neighborDir, NumberOfCores);
    
    hcam_panel3->Draw("colz");
    IPlotTools::DrawMUSICBoundaries();

}

void CEvent::NeighborhoodCheckerHelper(int ID, std::vector<int>& IDSelected,std::vector<int>& cleaned_pixels, std::vector<int>& visited_ids, std::string neighborDir) {
    bool already_searched = std::find(visited_ids.begin(), visited_ids.end(), ID) != visited_ids.end();
    visited_ids.push_back(ID);
    if (already_searched == false) {
        for(std::vector<int>::size_type i=0; i < IDSelected.size(); i++){
            //cout << "ID: " << ID << " IDSelected: " << IDSelected[i] << endl;

            if (ID == IDSelected[i] ){
                cleaned_pixels.push_back(IDSelected[i]);

                // cout << "ID selected: " << ID << endl;
                // check recursilely the board side of these pixels
                // Removes the ID that has been seleceted in the array so that the loops end
                

                for (int arrayID: GetNeighborArray(ID,Form("%sneighborbroadside.csv",neighborDir.c_str()))) {
                    // cout << "ArrayID: NCH: " << arrayID << endl;
                    NeighborhoodCheckerHelper(arrayID,IDSelected, cleaned_pixels,visited_ids, neighborDir);
                }
            }

        }
    }
    //cout << ID <<", ";
}


void CEvent::TriggeredPixelNeighborhoodChecker(std::vector<int>& IDSelected, int triggeredpixel,std::vector<int>& cleaned_pixels, std::string neighborDir){
    // we already know that the triggered pixel has a high enough amplitude 
    // We have a list of amplitudes that survied the first part of the the cleaning the straight amplitude cut across the whole camera
    // that will have 256 pixels and if it is to low the pixel will be 0 otherwise it will have the correct amplitude value if it meets the amplitude requirement
    //IDSelected.erase(std::remove(IDSelected.begin(), IDSelected.end(), triggeredpixel), IDSelected.end());
    
    std::vector<int> visited_ids;
    // get the Trigger pixels neighbors
    cleaned_pixels.push_back(triggeredpixel);
    for (int arrayID : GetNeighborArray(triggeredpixel,Form("%sneighborbroadside.csv",neighborDir.c_str()))) {
        // cout << "arrayID: Triggered() " << arrayID << endl;
        NeighborhoodCheckerHelper(arrayID,IDSelected,cleaned_pixels,visited_ids, neighborDir);
        // cout << "completed ID: "<< endl;
    }



}

void CEvent::isSecondCorePixel(TH2F* hcam_panel3, int triggeredpixel,std::string neighborDir,float CoreCutOff, int NumberOfCores) {
    // gather all pixels that match the core requirement 
    std::vector<int> cores = {};
    int nx, ny;
    IPlotTools::FindBin(triggeredpixel, &nx, &ny);
    float TriggererContent = hcam_panel3->GetBinContent(nx+1,ny+1);
    std::cout << "triggered pXiel: " << triggeredpixel << " Amp:  " << TriggererContent << std::endl;
    for(int j = 0; j< MaxNofChannels; j++) {
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        // std::cout << "binContent: " << binContent <<std::endl;
        if (binContent >= (TriggererContent - CoreCutOff*sqrt(TriggererContent))) {
            std::cout << " core: " << j << " Amplitude: " << binContent << std::endl;
            cores.push_back(j);
        }
    }
    
    std::vector<int> neighborsurvivingPanel3Pixels={};
    // then ensure they are all attached to the triggered pixel via broadside and adjecent neighborhold to eachother as fasr out at core pixels needed 
    TriggeredPixelNeighborhoodChecker(cores,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
    
    SetCoreIDs(cores);
    SetNumberofCorePixels(GetCoreIDs().size());
    sort(cores.begin(), cores.end());
    for (auto i : cores)
    std::cout << i << " ";


    float secondcoreamp = -1;
    for (int j = 0; j < GetCoreIDs().size(); j++){
        if (GetCoreIDs()[j] == triggeredpixel){
            continue;
        }
        IPlotTools::FindBin(GetCoreIDs()[j], &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        std::cout << "pixel core: " << cores[j] <<std::endl;
        std::cout << "binContent: " << binContent <<std::endl;
        // will set the first pixel that has a bin content as the highest core pixel if there are multiple at 160
        if (secondcoreamp < binContent) {
                secondcoreamp = binContent;
                SetSecondCoreID(GetCoreIDs()[j]);
                SetSecondCoreAmp(binContent);
        }
            
    }


    std::cout  << "Second Core pixe; " << GetSecondCoreID() << " amp: " << GetSecondCoreAmp() << std::endl;

    SetCoreRatio((1.0*TriggererContent)/(1.0*GetSecondCoreAmp()));
    std::cout << "Core ratio" << GetCoreRatio();

    std::cout << "Number of cores: " << GetCoreIDs().size()<< std::endl;
    if (GetCoreIDs().size() < NumberOfCores || TriggererContent == 0) {
        std::cout << "Second core pixel was not found: "<< std::endl;
        SurvivingPixelPanel3={triggeredpixel};
        SetSecondCoreID(-1);
        SetSecondCoreAmp(-1);
    }
    
    // Get  setsecond core through second max pixel

    
    // // check all the neighbors of the triggered pixel to see if there is another pixel that is within the range of the triggered pxiel to be a second core pixel
    // int nx, ny;
    // IPlotTools::FindBin(triggeredpixel, &nx, &ny);
    // float TriggererContent = hcam_panel3->GetBinContent(nx+1,ny+1);
    // // std::cout << "triggered pXiel: " << triggeredpixel << " Amp:  " << TriggererContent << std::endl;
    // int scount = 0;
    // float secondcoreamp = 0;
    
    // for (int arrayID : GetNeighborArray(triggeredpixel,Form("%sneighbor.csv",neighborDir.c_str()))) {
    //     // std::cout << "arrayID: Second() " << arrayID << std::endl;
    //     int nx, ny;
    //     IPlotTools::FindBin(arrayID, &nx, &ny);
    //     float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
    //     // std::cout << "binContent: " << binContent <<std::endl;
    //     if (binContent >= (TriggererContent - CoreCutOff*sqrt(TriggererContent))) {
    //         std::cout << "Second core: " << arrayID << " Amplitude: " << binContent << std::endl;
    //         scount++;
    //         if (secondcoreamp < binContent) {
    //             secondcoreamp = binContent;
    //             SetSecondCoreID(arrayID);
    //             SetSecondCoreAmp(binContent);
    //         }
    //     }

    // }
    // SetNumberofCorePixels(scount+1);
    // if (NumberOfCores == 0 && scount == 0) {
    //     SetSecondCoreID(MaxAmplitdePixelID);
    //     SetSecondCoreAmp(TriggererContent);       
    // }

    // std::cout << "scount: " << scount << std::endl;
    // if (scount < NumberOfCores || TriggererContent == 0) {
    //     std::cout << "Second core pixel was not found: "<< std::endl;
    //     SurvivingPixelPanel3={triggeredpixel};
    //     SetSecondCoreID(-1);
    //     SetSecondCoreAmp(-1);
    // }

    // SetCoreRatio(TriggererContent/secondcoreamp);
}

void CEvent::CheckCloseNeigborsOnly(TH2F* hcam_panel3, int pixel,std::vector<int> cores, double cutoff,std::string neighborDir) {
    // got through all the pixels adn see if they are neightbors tothe  triggered and second core pixel

    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
        // check if the pixel is even 20% of the core average amplitude if yes save it if not set it to 0
        if (binContent >= cutoff) {
            // std::cout << "Saved: " << j<< " Amplitude: " << binContent << std::endl;
            // check if this pixel is in the triggered pixel neighborhood
            // std::cout <<  "check trigger pixel :"  << pixel << std::endl;
            std::vector<int> neighborsp1=GetNeighborArray(pixel,Form("%sneighbor.csv",neighborDir.c_str()));
            std::vector<int> neighborsp2={};
            for (int i=0; i < cores.size(); i++){
                std::vector<int> tempneigh = GetNeighborArray(cores[i],Form("%sneighbor.csv",neighborDir.c_str()));
                for (auto k : tempneigh){
                    neighborsp2.push_back(k);
                }
            }
            // std::cout << "Size: " << neighborsp1.size() << std::endl;
            auto temp = std::find(neighborsp1.begin(), neighborsp1.end(), pixel);
            std::cout << temp - neighborsp1.begin() << std::endl;
            if ( std::find(neighborsp1.begin(), neighborsp1.end(), j) != neighborsp1.end() || std::find(neighborsp2.begin(), neighborsp2.end(), j) != neighborsp2.end() ||  j == pixel) {
                // std::cout << "YES" << std::endl;
                hcam_panel3->SetBinContent(nx + 1, ny + 1, binContent);
            } else {
                // std::cout << "NOT SAVED no" << std::endl;
                hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
            }
        } else {
            // std::cout <<  "Not Saved: " << j<< " Amplitude: " << binContent << std::endl;
            hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
        }
    }
}


// Function to open a lookup table for the neighbors of each pixel ////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> CEvent::GetNeighborArray(int id, std::string filename) {

	// give name of lookup table file
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open the neighbor.csv file for reading." << std::endl;
        return std::vector<int>();
    }

    std::string line;
	// load each line from file
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char delimiter = ',';

        int neighborID;
        std::string neighborArrayStr;

        if (iss >> neighborID >> delimiter >> neighborArrayStr) {
            if (neighborID == id) {
                std::vector<int> neighborArray;

                // Handle the first value separately
                size_t pos = 0;
                int value;

                try {
                    value = std::stoi(neighborArrayStr, &pos);
                    neighborArray.push_back(value);
                } catch (const std::invalid_argument& e) {
                 //   std::cerr << "Invalid argument: " << e.what() << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Out of range: " << e.what() << std::endl;
                }

                // Continue with the rest of the values
                while (pos < neighborArrayStr.length()) {
                    size_t nextPos;
                    try {
                        value = std::stoi(neighborArrayStr.substr(pos + 1), &nextPos);
                        neighborArray.push_back(value);
                        pos += nextPos + 1;
                    } catch (const std::invalid_argument& e) {
                     //   std::cerr << "Invalid argument: " << e.what() << std::endl;
                        break;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Out of range: " << e.what() << std::endl;
                        break;
                    }
                }

                file.close();
                return neighborArray;
            }
        }
    }

    file.close();
    return std::vector<int>(); // returns array of neighbors for given pixel
}

int CEvent::isConfigureEvent(){
    // std::cout << "Pixels: " << SurvivingPixelPanel3.size() <<" And size: " << SurvivingPixelTotalAmpPanel3 << std::endl;
    if (SurvivingPixelPanel3.size() < 15 || SurvivingPixelTotalAmpPanel3 < 2080){
        // std::cout << "Exitting" << std::endl;
        return 0;
    }
    std::vector<int> survivingSIABS; 
    for(unsigned int i=0; i < SurvivingPixelPanel3.size(); i++){
        int nx, ny;
        IPlotTools::FindBin(SurvivingPixelPanel3[i],&nx,&ny);
        survivingSIABS.push_back(IPlotTools::FindSIAB(nx,ny));
    }

    std::sort(survivingSIABS.begin(), survivingSIABS.end());
    auto unique=std::unique(survivingSIABS.begin(), survivingSIABS.end());
    survivingSIABS.erase(unique, survivingSIABS.end());

    if (survivingSIABS.size() > 1){
        return 0;
    }
    return 1;

}



