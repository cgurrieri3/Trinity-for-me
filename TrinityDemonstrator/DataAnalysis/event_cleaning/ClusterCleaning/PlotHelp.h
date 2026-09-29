#ifndef PlotHelp_H
#define PlotHelp_H

#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <vector>
#include <algorithm>
#include <TH2F.h>
#include <TH1D.h>
#include <TFile.h>
#include <IPlotTools.h>
#include <iostream>
#include <TCanvas.h>
#include <CEvent.h>
#include <TLatex.h>
#include <TMath.h>
#include <TDatime.h>
#include <TLegend.h>
#include "TStyle.h"
#include <THStack.h>
#include <cmath>


class PlotHelp { 
    public:
        PlotHelp();
        void AddtoWL(double WLRatio);
        void AddtoL(double L);
        void AddtoW(double W);
        void AddtoSize(double TotalAmp); // Distribution of Total Amplitude of surviving pixels
        void AddtoConcentation(double conc); // Distribution of Total Amplitude of surviving pixels
        void AddtoSurvivingPixelCount(int count); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGx(double COGx); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGy(double COGy); // Distribution of Total Amplitude of surviving pixels
        void AddtoTriggeredPixelsID(int id); // Distribution of Total Amplitude of surviving pixels
        void AddtoEventDate(int date);
        void AddtoRMSratioAVGamp(double rmsoveravg); // Distribution of Total Amplitude of surviving pixels
        void AddtoCoreRatio(double cr);
        void AddtoNumberOfCores(double num);

        void AddtoDistance2MajorAxis(double d);
        void AddtoRMSMajorAxis(double r);
        void AddtoWeightedRMSMajorAxis(double w);
        void AddtoPixelsonMajorAxis(int p);
        void AddtoPixelsoffMajorAxis(int p);
        void AddtoRatioPixelsMajorAxis(double r);
        // repeats the event's ratio once per pixel so it lines up with Distance2MajorAxisVector
        void AddtoRatioPixelsMajorAxisPerPixel(double r, int nPixels);
        // camera position of every pixel that survived cleaning, one entry per pixel
        void AddtoSurvivingPixelPosition(double nx, double ny);
        void AddtoAngle(double angleRad); // major axis angle, kept in degrees
        // major axis (oriented head-to-tail by M3Long) . vertical: +1 head up, -1 head down
        void AddtoUpDownSym(double s);
        void AddtoSaturatedPixels(int count); // saturated pixels in the event, kept aligned with SPCvector
        // camera position of every saturated pixel that survived cleaning, one entry per pixel
        void AddtoSaturatedPixelPosition(double nx, double ny);

        void PlothSize(TCanvas* c, std::string pdf);
        void PlothSizeConc(TCanvas* c, std::string pdf);
        void PlothSizeSPC(TCanvas* c, std::string pdf);
        void PlothWL(TCanvas* c, std::string pdf,std::string outDir,std::string date);
        void PlothdistLandW(TCanvas* c, std::string pdf);
        void PlothLvW(TCanvas* c, std::string pdf);
        void PlothWLConc(TCanvas* c, std::string pdf);
        void PlothWLSPC(TCanvas* c, std::string pdf);
        void PlothSIZEWL(TCanvas* c, std::string pdf);
        void PlothSPCConc(TCanvas* c, std::string pdf);
        void PlothCOG(TCanvas* c, std::string pdf);
        void PlothTPID(TCanvas* c, std::string pdf);
        void PlothRMSratioAvg(TCanvas* c, std::string pdf);
        void PlothEventDateTriggeredPixel(TCanvas* c, std::string pdf);
        void PlothCoreRatio(TCanvas* c, std::string pdf);
        void PlothNumberofCores(TCanvas* c, std::string pdf);
        void PlothCoreRatioandNumberofCores(TCanvas* c, std::string pdf);
        void PlothncoreSIZE(TCanvas* c, std::string pdf);
        void PlothnCoreSPC(TCanvas* c, std::string pdf);
        void PlothCRSPC(TCanvas* c, std::string pdf);
        void PlothSIZEWandL(TCanvas* c, std::string pdf);
        void PlotEventFlags(TCanvas* c, std::string pdf);
        void PlotPixelsDistanceToMajorAxis(TCanvas* c, std::string pdf);
        void PlotdistRMSandWeightedRMS(TCanvas* c, std::string pdf);
        void PlotPixelsOnMajorAxis(TCanvas* c, std::string pdf);
        void PlotPixelsOffMajorAxis(TCanvas* c, std::string pdf);
        void PlothRMSvsRatioDistance(TCanvas* c, std::string pdf);
        void PlothWRMSvsRatioDistance(TCanvas* c, std::string pdf);
        void PlothOnOffMajorAxisvsratio(TCanvas* c, std::string pdf);
        void PlotPixelsRatioDistanceToMajorAxis(TCanvas* c, std::string pdf);
        void PlothWLvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf);
        void PlothDistancevsRatioPixelsMajorAxis(TCanvas* c, std::string pdf);
        void PlothCoreOverSPCvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf);
        void PlothSPCvsRatioPixelsMajorAxis(TCanvas* c, std::string pdf);
        void PlothSurvivingPixelsX(TCanvas* c, std::string pdf);
        void PlothSurvivingPixelsY(TCanvas* c, std::string pdf);
        void PlothCOGx(TCanvas* c, std::string pdf);
        void PlothCOGy(TCanvas* c, std::string pdf);
        void PlothAngle(TCanvas* c, std::string pdf);
        void PlothUpDownSym(TCanvas* c, std::string pdf);
        void PlothSaturatedPixels(TCanvas* c, std::string pdf);
        void PlothSaturatedOverSurviving(TCanvas* c, std::string pdf);
        void PlothSaturatedPixelsX(TCanvas* c, std::string pdf);
        void PlothSaturatedPixelsY(TCanvas* c, std::string pdf);

        void AddEventFlags(int i);
        void SetSimEnergy(float energy, bool isSim); // sims only: energy of the current event + whether to track it
        void PlothSimEnergySavedVsNotSaved(TCanvas* c, std::string pdf); // sims only: saved vs removed energy distribution
        std::map<std::string, int> GetEventFlags() const;
        void LogBinning(TH2F* hist);
        void LogBinning(TH1D* hist);

    private:
        
        
        int GetSequentialDayIndex(int dateStr);

        std::vector<double> WLvector = {}; //Length Width Ratio
        std::vector<double> Lvector = {}; // Length
        std::vector<double> Wvector = {}; // Width 
        std::vector<double> Svector = {}; //Size
        std::vector<double> Cvector = {}; //Concentration
        std::vector<double> Corevector = {}; //Core Ratio
        std::vector<double> NCorevector = {}; //Core Ratio
        std::vector<int> SPCvector = {}; //Surviving Pixel Count
        std::vector<double> COGxvector = {}; //Center of Gravity X
        std::vector<double> COGyvector = {}; //Center of Gravity Y
        std::vector<int> TPID = {}; //Triggered Pixel ID
        std::vector<int> Datevecctor = {}; // date of events
        std::vector<double> RMSoverAVGvector = {}; //RMS over Average Amplitude
        std::vector<double> Flagvector = {};
        std::vector<double> Distance2MajorAxisVector = {};
        std::vector<double> RMSMajorAxisVector = {};
        std::vector<double> WeightedRMSMajorAxisVector = {};
        std::vector<int> PixelsonMajorAxisVector = {};
        std::vector<int> PixelsoffMajorAxisVector = {};
        std::vector<double> RatioPixelsMajorAxisVector = {};
        std::vector<double> RatioPixelsMajorAxisPerPixelVector = {}; // one entry per pixel, matches Distance2MajorAxisVector
        std::vector<double> SurvivingPixelXVector = {}; // one entry per surviving pixel
        std::vector<double> SurvivingPixelYVector = {}; // one entry per surviving pixel
        std::vector<double> Anglevector = {}; // major axis angle in degrees
        std::vector<double> UpDownSymvector = {}; // signed major axis . vertical per event, in [-1,1]
        std::vector<int> SatPixelCountVector = {}; // saturated pixels per event, index matches SPCvector
        std::vector<double> SaturatedPixelXVector = {}; // one entry per saturated surviving pixel
        std::vector<double> SaturatedPixelYVector = {}; // one entry per saturated surviving pixel

        // sims only: neutrino energy of saved (flag 5) vs removed events
        std::vector<double> SimEnergySavedVector = {};
        std::vector<double> SimEnergyNotSavedVector = {};
        double currentSimEnergy = 0.0; // energy of the event currently being processed
        bool trackSimEnergy = false;   // only true for sim events so other data is ignored

        IPlotTools *plottools;

        // int TotalEvents = 0;
        // int HLEDEvents = 0;
        // int PreCleanedEvents = 0;
        // int Panel2CleanedEvents = 0;
        // int Panel3CleanedEvents = 0;
        // int SurvivingEvents = 0;

        // plot help with step size, min and max for TH*
        int SIZEstep =50;
        float SIZEmin = 1; // in log scale
        float SIZEmax = 5;

        int ONEstep = 22;
        float ONEmin = -0.05;
        float ONEmax = 1.05;
        
        int SPstep = 28;
        float SPmin = -0.5;
        float SPmax = 27.5;

        int CameraStep = 16;
        float CameraMin = -0.5;
        float CameraMax = 15.5;

        int nCoreStep = 21;
        float nCoreMin = 0.5;
        float nCoreMax = 21.5;

        int dStep = 21;
        float dMin = -0.125;
        float dMax = 5.125;

        int pOnStep = 11;
        float pOnMin = -0.5;
        float pOnMax = 10.5;

        int AngleStep = 36;
        float AngleMin = -90.0;
        float AngleMax = 90.0;

        // up/down symmetry is signed once M3Long orients the axis, so it spans [-1,1]
        int UDstep = 42;
        float UDmin = -1.05;
        float UDmax = 1.05;

        // saturated pixels are a subset of the surviving pixels, so they share that scale
        int SatStep = SPstep;
        float SatMin = SPmin;
        float SatMax = SPmax;
        

        
    };  
#endif