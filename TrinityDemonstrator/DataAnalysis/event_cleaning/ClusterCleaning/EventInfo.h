// #include <TROOT.h>
#ifndef EventInfo_H
#define EventInfo_H
#include <vector>
#include "TROOT.h"     // general ROOT stuff
#include "TFile.h"     // for TFile
#include "TTree.h"     // for TTree
#include "TObject.h"
#include "TH2F.h"
using namespace std;

class EventInfo : public TObject {
    public:
        EventInfo(int size = 1);

        ~EventInfo();

        void Clear();

        void SetDate(Int_t v);
        void SetFile(string v) ;
        void SetEvent(Int_t v) ;
        void SetHPanel1(TH2F* ptr) ;
        void SetEventFlag(Int_t v) ;
        void SetEventBranch(Int_t v) ;
        void SetHPanel4(TH2F* ptr) ;
        void SetW(Float_t v);
        void SetL(Float_t v);
        void SetWLRatio(Float_t v);
        void SetAngle(Float_t v);
        void Setr1(Float_t v);
        void Setr2(Float_t v);
        void SetSurvivngPixels(Int_t v);
        void SetSize(Float_t v);
        void SetConc(Float_t v);

        void SetTriggeredPixelID(Int_t v);
        void SetTriggeredMUSICID(Int_t v);
        void SetRMS_AvgAmp(Float_t v);
        void SetRMS(Float_t v);
        void SetAvgAmp(Float_t v);
        void SetArea(Float_t v);
        // IDs of the pixels that saturated; the size is how many, the values are where
        void SetSaturatedPixels(std::vector<Int_t> v);
        void SetCOGx(Float_t v);
        void SetCOGy(Float_t v);
        void SetCoreRatio(Float_t v);
        void SetNumberOfCores(Float_t v);
        void SetM3Long(Float_t v);
        void SetM3LongPow3(Float_t v);
        // |major axis . vertical|, 0 = axis along camera x, 1 = axis vertical
        void SetUpDownSym(Float_t v);

        void SetDistance2MajorAxis(Float_t v);
        void SetRMSMajorAxis(Float_t v);
        void SetWeightedRMSMajorAxis(Float_t v);
        void SetPixelsonMajorAxis(Int_t v);
        void SetPixelsoffMajorAxis(Int_t v);
        void SetRatioPixelsMajorAxis(Float_t v);
        


        // Getters
        Int_t GetDate() const;
        string GetFile() const;
        Int_t GetEvent() const;
        TH2F* GetHPanel1() const;
        Int_t GetEventFlag() const;
        Int_t GetEventBranch() const ;
        TH2F* GetHPanel4() const;
        Float_t GetW() const;
        Float_t GetL() const;
        Float_t GetWLRatio() const;
        Float_t GetAngle() const;
        Float_t Getr1() const;
        Float_t Getr2() const;
        Int_t GetSurvivngPixels() const;
        Float_t GetSize() const;
        Float_t GetConc() const;

        Int_t GetTriggeredPixelID() const;
        Int_t GetTriggeredMUSICID() const;
        Float_t GetRMS_AvgAmp() const;
        Float_t GetAvgAmp() const;
        Float_t GetRMS() const;
        Float_t GetArea() const;
        std::vector<Int_t> GetSaturatedPixels() const;
        Float_t GetCOGx() const;
        Float_t GetCOGy() const;
        Float_t GetCoreRatio() const;
        Float_t GetNumberOfCores() const;
        Float_t GetM3Long() const;
        Float_t GetM3LongPow3() const;
        Float_t GetUpDownSym() const;

        Float_t GetDistance2MajorAxis() const;
        Float_t GetRMSMajorAxis() const;
        Float_t GetWeightedRMSMajorAxis() const;
        Int_t GetPixelsonMajorAxis() const;
        Int_t GetPixelsoffMajorAxis() const;
        Float_t GetRatioPixelsMajorAxis() const;

    protected:

        Int_t date;
        string file;
        Int_t event;
        TH2F* hpanel1;
        Int_t flag;
        Int_t eventBranch; // Branch name for the event in the TTree
        TH2F* hpanel4;
        
        Float_t w;
        Float_t l;
        Float_t WLratio;
        Float_t Angle;
        Float_t r1;
        Float_t r2;
        Int_t SurvivngPixels; 
        Float_t Size;
        Float_t Conc;
        Int_t TriggeredPixelID;
        Int_t TriggeredMUSICID;
        Float_t RMS_AvgAmp;
        Float_t RMS;
        Float_t AvgAmp;
        Float_t Area;
        std::vector<Int_t> SaturatedPixelIDs;
        Float_t COGx;
        Float_t COGy;
        Float_t CoreRatio;
        Float_t NumberOfCores;
        Float_t M3Long;     // standard Hillas 3rd moment along the major axis, q^1 / sum(q)
        Float_t M3LongPow3; // thesis variant, q^3 / N -- sign is NOT the same as M3Long
        Float_t UpDownSym;  // |major axis . vertical| = |sin(Angle)|, in [0,1]

        Float_t Distance2MajorAxis;
        Float_t RMSMajorAxis;
        Float_t WeightedRMSMajorAxis;
        Int_t PixelsonMajorAxis;
        Int_t PixelsoffMajorAxis;
        Float_t RatioPixelsMajorAxis;

        ClassDef(EventInfo, 5);  // ROOT I/O support

};
#endif