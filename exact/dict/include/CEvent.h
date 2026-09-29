#ifndef CEvent_H
#define CEvent_H

#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <vector>
#include <TH2F.h>
#include <TFile.h>
#include <IPlotTools.h>
#include <IUtilities.h>
#include <fstream>
#include <sstream>



class CEvent { 
    public:
        CEvent();

        // These are set when you call the CEvent
        // Event GetEvent() const { return ev; }
        
        // int GetMaxMUSICID() const { return GetMaxMUSICID; }
        // int GetMaxAmplitudePixelID() const { return MaxAmplitdePixelID; }
        // float GetAverageAmplitude() const { return AverageAmplitude; }
        // int GetSumAmplitude() const { return SumAmplitude; }
        // int GetSaturedPixels() const { return SaturedPixels; }


        // float GetMaxAmplitude() const { return MaxEventAmplitde; }
        // int GetPeakTime() const { return GetPeakTime; }

        // float GetMaxAmplitduePixelIDAtTimeBin() const { return MaxAmplitduePixelIDAtTimeBin; }
        // float GetMaxAmplitudeAtTimeBin() const { return MaxAmplitudeAtTimeBin; }

        // // These will return an array with 256 pixels of data

        // std::vector<float> GetAmplitudeValues() const { return AmplitudeValues; }
        // std::vector<int> GetPeakTimeValues() const { return PeakTimeValues; }
        // std::vector<int> GetPedestalValues() const { return PedestalValues; }
        // std::vector<int> GetTrace() const { return Trace; }
        // // ADD CALIBRATION SOMEHOW

        // // These are set when you set them 
        // // HLED or Test Event
        // // 
        // // TH2F hcam_intial
        // // TH2F hcam_cut
        // // Surviving Pixel Amplitude array
        //     // remove duplicates on Get
        //     // Surviving Pixel ID array
        // // TH2F connected
        //     // Bring in Neighbor Pixel Stuff
        //     // Peak time bins earliest thing
        
        // // TH2F hcam_cleaned 


        // TVectorD GetEigenValues() const { return eigenValues; }
        // TMatrixD GetEigenVectors() const { return eigenVectors; }
        // std::vector<float> GetSigmas() const;
        // void AddRow(const std::vector<float>& row);
        // void ComputeCovarianceMatrix();
        // void ComputeEigenDecomposition();
        // void Set_Surviving_Pixels(std::vector<int> sp) const;
        // std::vector<int> Get_Surviving_Pixels() const {return surviving_pixels; }
        // void SetGainCalibration(std::vector<float> gain) const;
        // std::vector<float> GetGainCalibration() const {return gain; }

        

        // void SetPanel2() const;
        // void SetPanel3() const;
        // void SetPanel4() const;

        void SetTriggeredMUSICID(int id) {TriggeredMUSICID = id; } // Use const_cast to modify the member variable
        int GetTriggeredMUSICID() const {return TriggeredMUSICID; }  

        void SetAmplitudeValues(std::vector<float> amp);
        std::vector<float> GetAmplitudeValues() { return AmplitudeValues; }
        void SetAmplitudeValuesTimeBin(std::vector<float> amp, std::string CalibrationFactorDir, std::string folString, std::vector<double> absGain, double biasVoltage); 
        std::vector<float> GetAmplitudeValuesTimeBin() { return AmplitudeValuesTimeBin; }

        void SetPedestalValues(std::vector<float> ped) { PedestalValues = ped; }
        std::vector<float> GetPedestalValues() { return PedestalValues; }    
        
        void SetPeakTimeBin(std::vector<int> peak) { PeakTimeBin = peak; }
        std::vector<int> GetPeakTimeBin() { return PeakTimeBin; }    
        
        void SetMaxAmplitudePixelID(int id) { MaxAmplitdePixelID = id; } // aka triggered pixel
        int GetMaxAmplitudePixelID() const { return MaxAmplitdePixelID; }

        // void SetMaxAmplitudePixelAmp(int Amp) { MaxAmplitdePixelAmp = Amp; } // aka triggered pixel
        // int GetMaxAmplitudePixelAmp() const { return MaxAmplitdePixelAmp; }
        
        void SetSecondCoreID(int id) { SecondCoreID = id; }
        int GetSecondCoreID() const { return SecondCoreID; }
        void SetCoreIDs(std::vector<int> id) { SecondCoreIDs = id; }
        std::vector<int> GetCoreIDs() const { return SecondCoreIDs; }

        void SetTotalCoreAmp(float val) {totalCoreAmp  = val;}
        float GetTotalCoreAmp() const {return totalCoreAmp;}
        
        void SetSecondCoreAmp(int id) { SecondCoreAmp = id; }
        int GetSecondCoreAmp() const { return SecondCoreAmp; }

        void SetNumberofCorePixels(int num) { CoreNumber = num; }
        int GetNumberofCorePixels() const { return CoreNumber; }

        void SetMaxAmplitude(float amp) { MaxEventAmplitde = amp; }
        float GetMaxAmplitude() const { return MaxEventAmplitde; }

        void SetAbsoluteGain(std::vector<double> absgain) {AbsoluteGain = absgain; }
        double GetAbsoluteGain(int i) const { return AbsoluteGain[i]; }

        void SetSaturatedPixels(int sat) { SaturedPixels = sat; }
        int GetSaturatedPixels() const { return SaturedPixels; }

        void SetAverageAmplitude(float amp) { AverageAmplitude = amp; }
        float GetAverageAmplitude() const { return AverageAmplitude; } 

        void SetRMS(std::vector<float> amp);
        float GetRMS() const { return RMS; } // RMS is calculated in the SetRMS function)
        float GetRMSoverAvgAmp() const { return RMS/AverageAmplitude; } // RMS is calculated in the SetRMS function)

        void SetCoreRatio(float ratio) {coreRatio = ratio;}
        float GetCoreRatio() const { return coreRatio; }

        void SetSumAmplitude(int sum) { SumAmplitude = sum; }
        int GetSumAmplitude() const { return SumAmplitude; }

        void SetEventNumber(int event) { EventNumber = event; }
        int GetEventNumber() const { return EventNumber; }

        void SetEventDate(int date) {EventDate = date; }
        int GetEventDate() const {return EventDate;}
        
        void SetFilename(const std::string& name) { Filename = name; }
        std::string GetFilename() const { return Filename; }

        void AddTotalEvent(int i) {TotalEvents += i; }
        void AddHLEDEvent(int i) {HLEDEvents += i; }
        void AddCleanedEvent(int i) {CleanedEvents += i; }
        void AddSurvivingEvent(int i) {SurvivingEvents += i; }

        int GetTotalEvent() {return TotalEvents; }
        int GetHLEDEvent() {return HLEDEvents; }
        int GetCleanedEvent() {return CleanedEvents; }
        int GetSurvivingEvent() {return SurvivingEvents; }

        void LoadFlasherCalibration(std::string CalibrationFactorDir, std::string folderName, double biasVoltage); 
        TH2F* GetFlasherCalibration() const { return hflasher; }
        void SetPanel1(TH2F* hcam_panel1);
        void SetPanel2(TH2F* hcam_panel2, int CorePixelAmpCutOff);
        void SetPanel3AllNeighboringPixels(TH2F* hcam_panel3, std::string neighborDir);
        void SetPanel3(TH2F* hcam_panel3, std::string neighborDir,int NumberOfCores,float CoreCutOff, float Crosstalkallowance);
        std::vector<int> GetSurvivingPixelPanel2() {return SurvivingPixelPanel2; }
        std::vector<int> GetSurvivingPixelPanel3() {return SurvivingPixelPanel3; }
        float GetSurvivingPixelTotalAmpPanel3() {return SurvivingPixelTotalAmpPanel3; }
        int isConfigureEvent();
        void isSecondCorePixel(TH2F* hcam_panel3, int triggeredpixel,std::string neighborDir,float CoreCutOff, int NumberOfCores);


    private:
        // std::sting Filename;
        // int EventNumber;
        // std::vector<int> surviving_pixels
        // TH2F* hcam_1;
        // TH2F* hcam_2;
        // TH2F* hcam_3;
        // TH2F* hcam_4;

        

        int TriggeredMUSICID = -1;
        std::vector<float> AmplitudeValues;
        std::vector<float> AmplitudeValuesTimeBin;
        std::vector<float> PedestalValues;
        std::vector<int> PeakTimeBin;
        std::vector<double> AbsoluteGain;
        int MaxAmplitdePixelID = -1;
        // int MaxAmplitdePixelAmp = -1;
        int SecondCoreID = -1; // ID of the second core pixel, if it exists
        std::vector<int> SecondCoreIDs; // ID of the second core pixel, if it exists
        float totalCoreAmp = -1.0;
        float SecondCoreAmp = -1.0; // Amplitude of the second core pixel, if it exists
        int CoreNumber = -1;
        float MaxEventAmplitde = -1;
        int SaturedPixels = -1;

        float AverageAmplitude = -1;
        float RMS = -1;
        float coreRatio = -1; // ratio of the amplitude of the triggered pixel to the second core pixel
        float RMSoverAvgAmp = -1;
        int SumAmplitude = -1;
        int EventNumber = -1;
        int EventDate = -1;
        std::string Filename;

        int TotalEvents = 0;
        int HLEDEvents = 0;
        int CleanedEvents = 0;
        int SurvivingEvents = 0;
        std::vector<int> SurvivingPixelPanel2 = {};
        std::vector<int> SurvivingPixelPanel3 = {};
        float SurvivingPixelTotalAmpPanel3 =0;
        int MaxNofChannels = 256;
        TH2F* hflasher;
        TH2F* hcam_panel1;
        
        void NeighborhoodCheckerHelper(int ID, std::vector<int>& IDSelected,std::vector<int>& cleaned_pixels, std::vector<int>& visited_ids, std::string neighborDir);
        void TriggeredPixelNeighborhoodChecker(std::vector<int>& IDSelected, int triggeredpixel,std::vector<int>& cleaned_pixels, std::string neighborDir);
        std::vector<int> GetNeighborArray(int id, std::string filename);
        void CheckCloseNeigborsOnly(TH2F* hcam_panel3, int pixel,std::vector<int> cores,double cutoff,std::string neighborDir);
        



};
#endif
