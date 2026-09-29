// #include <TROOT.h>
#ifndef SEVENT_H
#define SEVENT_H
#include <vector>
#include "TROOT.h"     // general ROOT stuff
#include "TFile.h"     // for TFile
#include "TTree.h"     // for TTree
#include "TObject.h"
using namespace std;

class SEvent : public TObject {
    public:
        SEvent(int size = 1);

        ~SEvent();
        void SetSimRun(std::string temp);
        void SetDate(Int_t temp);
        void SetSimEventNumber(Int_t temp);
        void SetTriggered(Bool_t temp);
        void SetPolarAngle(Float_t temp);
        void SetAzimuthAngle(Float_t temp);
        void SetTelescope_Xpos(Float_t temp);
        void SetTelescope_Ypos(Float_t temp);
        void SetTelescope_Zpos(Float_t temp);
        void SetTelescope_Radius(Float_t temp);
        void SetXMax(Float_t temp);
        void SetNMax(Float_t temp);
        void SetX0Max(Float_t temp);
        void SetLambda(Float_t temp);
        void SetNeutrinoEnergy(Float_t temp);
        void SetDecayLength(Float_t temp);
        void SetShowerStartX(Float_t temp);
        void SetShowerStartY(Float_t temp);
        void SetShowerStartZ(Float_t temp);
        void SetTauEmergenceProb(Float_t temp);
        void SetEmergencePointX(Float_t temp);
        void SetEmergencePointY(Float_t temp);
        void SetEmergencePointZ(Float_t temp);
        void SetGlobeTheta(Float_t temp);
        void SetGlobePhi(Float_t temp);
        void SetTauEnergy(Float_t temp);

        void SetZenith(Float_t temp);
        void SetAzimuth(Float_t temp);
        // void SetBSM(Float_t temp);
        void SetTilt(Float_t temp);
        void SetPosX(vector<Float_t> temp);
        void SetPosY(vector<Float_t> temp);



        Int_t GetSimEventNumber();
        Bool_t GetTriggered();
        std::string GetSumRun();
        Int_t GetDate();
        Float_t GetPolarAngle();
        Float_t GetAzimuthAngle();


        Float_t GetTelescope_Xpos();
        Float_t GetTelescope_Ypos();
        Float_t GetTelescope_Zpos();
        Float_t GetTelescope_Radius();
        Float_t GetXMax();
        Float_t GetNMax();
        Float_t GetX0Max();
        Float_t GetLambda();
        Float_t GetNeutrinoEnergy();
        Float_t GetDecayLength();
        Float_t GetShowerStartX();
        Float_t GetShowerStartY();
        Float_t GetShowerStartZ();
        Float_t GetTauEmergenceProb();
        Float_t GetEmergencePointX();
        Float_t GetEmergencePointY();
        Float_t GetEmergencePointZ();
        Float_t GetGlobeTheta();
        Float_t GetGlobePhi();
        Float_t GetTauEnergy();

        Float_t GetZenith();
        Float_t GetAzimuth();
        // Float_t GetBSM();
        Float_t GetTilt();
        vector<Float_t> GetPosX();
        vector<Float_t> GetPosY();


    protected:

        std::string simrun;
        Int_t date;
        Int_t simEvent;
        Bool_t triggered;
        vector<Float_t> posX;
        vector<Float_t> posY;
        Float_t zenith;
        Float_t azimuth;
        // Float_t bsm;
        Float_t tilt;
        Float_t polarangle;
        Float_t azimuthangle;
        Float_t telposX;
        Float_t telposY;
        Float_t telposZ;
        Float_t telradius;
        Float_t XMax;
        Float_t NMax;
        Float_t X0Max;
        Float_t lambda;
        Float_t neutrinoenergy;
        Float_t decaylength;
        Float_t showerstartX;
        Float_t showerstartY;
        Float_t showerstartZ;
        Float_t tauemergenceprob;
        Float_t EmergencepointX;
        Float_t EmergencepointY;
        Float_t EmergencepointZ;
        Float_t globaltheta;
        Float_t globalphi;
        Float_t tauenergy;

        ClassDef(SEvent, 1);  // ROOT I/O support

};
#endif

