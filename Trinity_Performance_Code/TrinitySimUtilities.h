#include <TF1.h>
#include <TGraph.h>
#include <TGaxis.h>
#include <TTimer.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TMath.h>
#include <TColor.h>
#include <TH1D.h>

#include <TROOT.h>
#include <TApplication.h>

// for sky plots
#include <TFile.h>
#include <TMarker.h>
#include <TText.h>
#include <TH2F.h>

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

//#include <ctime>
#include <TSystem.h>
#include <sys/stat.h>
//#include <filesystem> // Requires C++17


using namespace std;

class TrinitySimUtilities {

 private:
    std::string outputFolder;

 public:
 
  TrinitySimUtilities();

  //void RunFADC(TelescopeData *telData);
  string Hold();

  void ConfigureDemonstrator();

  //Calculations that can be done
  void PlotEmergenceProbability();

  Double_t GetDiffuseAcceptanceLimitedFoV(Double_t dMinEnu, Double_t dMaxEnu,TGraph *grDiffAcceptance);
  Double_t GetDiffuseAcceptance(Double_t dMinEnu, Double_t dMaxEnu,TGraph *grDiffAcceptance);

  void CalculateIntegralSensitivity();

  TH2F* GetPointSourceAcceptanceInFOV(Double_t dMinEnu, Double_t dMaxEnu);
  TH2F* GetPointSourceAcceptanceSingleAngle(Double_t dMinEnu, Double_t dMaxEnu);

  void CalculatePointSourceDifferentialSensitivity();
  void CalculateDiffuseFluxDifferentialSensitivity();

  void DumpConfiguration();
 
  //setter functions to configure the simulation
  void SetExposure(Double_t time){ dExposureHours = time; }; //exposure assumed for diffuse sensitivity

  void SetNumberOfTrinitySites(Int_t sites){ NumSites = sites; }; //used to calculate the diffuse acceptance assuming 360 FoV
  void SetLowerNeutrinoEnergy(Double_t minenu){ dMinEnu = minenu; }; //log10 [GeV]
  void SetUpperNeutrinoEnergy(Double_t maxenu){ dMaxEnu = maxenu; }; //log10 [GeV] 
  void SetNeutrinoEnergyStep(Double_t step){ dLogEnergyStep = step; }; //log10 [GeV] 
  void SetIntegrationRangeForSensitivity(Double_t range){ dHalfEnergyBinWidth = range/2; }; //log10 [GeV] 
  void SetMaximumShowerHeightAboveGround( Double_t maxheight){ dST = maxheight;}; //km max height of shower tip above ground;
 void SetMinimumDistance(Double_t mindist){ yMin = mindist;}; //km
 void SetMaximumDistance(Double_t maxdist){ yMax = maxdist;}; //km
 void SetDistanceStepSize(Double_t step){ yDelta = step;}; //km
 void SetMaximumElevationAngle(Double_t maxel){ MaxElevation = maxel;}; //deg elevation angle (determines path through Earth)
 //This sets the maximum azimuth angle simulated measured relative to a line
 //drawn from the telescope to where the tau emerges. It is only used in the
 //Point source acceptance of a limited FoV GetPointSourceAcceptanceInFOV
 void SetMaximumAzimuthAngle(Double_t maxaz){ dMaxAzimuthSky = maxaz;}; //deg max limit for azimuth evaluation 
 void SetStepSizeCameraAzimuth(Double_t step){ DeltaAngleAz = step;}; //deg steps in azimuth in the camera FOV
 void SetStepSizeSky(Double_t step){ DeltaAngleSky = step;}; //deg this is the step size in elevation and azimuth in the sky to calculate the acceptance for a limited FOV telescope. Make sure it is smaller than then FOV of the camera  
 void SetSourceSpectralIndex(Double_t index){ nuIndex = index;}; //power law index of the neutrino spectrum the minus sign is added later
 //used in the acceptance calculations for a 360deg FoV telescope.
 void SetMaximumShowerViewingAngle(Double_t maxangle){ dMaxCherenkovAzimuthAngle = maxangle;}; //deg maximum azimuth angle for cherenkov 
 void SetMaximumDistanceForFluorescence(Double_t maxdist){ dMaxFluorescenceDistance = maxdist;}; //km

  //next three parameters are key to the instrument
  //plus the telescope height above ground which can be selected above
 void SetTelescopeFoVAboveHorizon( Double_t above ){ tanFoV = tan(above/180.*pi);}; //deg Field of view of telescope above the horizon
 void SetTelescopeFoVBelowHorizon( Double_t below ){ dFoVBelow = below;}; //rad Field of view of telescope  below horizon 
 void SetMinimumShowerLength( Double_t minlength ){ dMinLength = minlength;}; //deg mimnimum length a shower has to have in the camera, in degrees. 
 void SetMinimumNumberPhotoelectrons( Double_t minpe ){ dMinimumNumberPhotoelectrons = minpe;};  //this is pe per sqm mirror 

 void SetCameraHorizontalFoV( Double_t fov ){ hFOV = fov;}; //degrees

 void SetLimitedFoV( Bool_t limited ){ blimFOV = limited; };

 void SetSimulateFluorescence( Bool_t fluor ){ bFluorescence = fluor; };
 void SetSimulateFluorescenceAndCherenkov( Bool_t both ){ bCombined = both; };
 void SetSimulateMonoNuEnergies( Bool_t mono){ bMonoNu = mono; }; //simulate monoenergetic neutrinos, only good for acceptance calculation. For all other simulations set it to kFALSE. Use if you want to be in compliance with the nu Community for calculating diffuse flux sensitivities and acceptance
 void SetDetectorAltitude( Int_t conf){ iConfig = conf; }; //pick one of the following altitudes above ground{ 0, 1, 2, 3};
 void SetTriggerWindow( Int_t window ){ iTrigWin = window; }; //5 mins all photons are counted Size of the trigger window to collect photons {30.00, 60.00, 150.00, 300.00, 1500.00} 

 void ConfigureForDemonstrator();
 
 // Method to set the output folder path
 std::string getCurrentDateTime();
 void createDirectory(const std::string& path);
 void SetOutputFolder();

 // Getter for folder path
 std::string GetOutputFolder() const;

 // Method to save a canvas to the output folder
 void SaveCanvas(TCanvas* canvas, const std::string& canvasName);

 protected:

  Double_t myPEfunction(Double_t *x, Double_t *par);
  Double_t myPEfunction_trigwindow(Double_t *x, Double_t *par);

  Double_t DistanceThroughEarth(Double_t y, Double_t elevation, Double_t azimuth);

  Double_t PEtau(Double_t D,Double_t Etau, Double_t Enu);
  Double_t PEtauNoTauEnergyLoss(Double_t D,Double_t Etau, Double_t Enu);
  Double_t PEtauDutta(Double_t D,Double_t Etau, Double_t Enu);

  Double_t PDecayFluorescence(Double_t Etau, Double_t y, Double_t elevation, Double_t azimuth, Bool_t blimFOV, Double_t dAzimuth);
  Double_t PDecay(Double_t Etau, Double_t y, Double_t elevation, Double_t azimuth, Bool_t blimFOV, Double_t dAzimuth);

  void GetTauDistribution(Double_t d, Double_t Enumin = 1e9, Double_t Enumax = 3.16e9);

  void removeDuplicates();
  void readFromTable();
  void findAngleNumber();
  double biLinearInterpolation(double a1,double n1,double q11,double a2,double n2,double q22,double x,double y);
  int FindLion(double dValue, vector<double> &vData,int iSize);


  //Constants and Variables
  const Double_t sigma[18] = { 660, 920, 1400, 1900, 2500, 3700, 4800, 6200, 8700, 11000, 14000, 19000, 24000, 30000, 39000, 48000, 59000, 75000 }; //pb cc cross section cooper-sarkar 2011
  const Double_t sigmaNC[18]  = { 240, 350, 530, 730, 980, 1400, 1900, 2400, 3400, 4400, 5600, 7600, 9600, 12000, 16000, 20000, 24000, 31000 }; //pb
  const Double_t Esig[18] = { 1e6, 2e6, 5e6, 1e7, 2e7, 5e7, 1e8, 2e8, 5e8, 1e9, 2e9, 5e9, 1e10, 2e10, 5e10, 1e11, 2e11, 5e11 }; //GeV

//Ghandi 1996
//Double_t sigma[] = { 634, 960, 1412, 1749, 2554, 3630, 4436, 6283, 8700, 10490, 14660, 20100, 23790, 32890, 44270, 53570, 73200, 99270, 117900 }; //pb
//Double_t sigmaNC[] = { 260, 402, 600, 748, 1104, 1581, 1939, 2763, 3837, 4641, 6490, 8931, 10660, 14650, 19950, 23770, 32470, 43770, 51960 }; //pb
//Double_t Esig[] = { 1e6, 2.5e6, 6e6, 1e7, 2.5e7, 6e7, 1e8, 2.5e8, 6e8, 1e9, 2.5e9, 6e9, 1e10, 2.5e10, 6e10, 1e11, 2.5e11, 6e11, 1e12 }; //GeV


  const int iColors[7] = {kBlue-3,kCyan-3,kGreen-3,kYellow-3,kRed-3,kRed+3,kMagenta-3};
  const int marker[7] = { 23, 22, 29, 21, 20, 28, 25};
  const double markerSize[7] = { 0.9, 0.9, 1.2, 0.7, 0.9, 1.3, 0.9};

  TF1 *fPE;

  TH1D *hTau;


  const Double_t c = 299792; //km/s
  const Double_t pi = 3.14159265359;
  const Double_t DecayTime = 0.290e-12; //s
  const Double_t Mtau = 1.7768; //GeV
  const Double_t REarth = 6371; //km

//All the coefficients to get the right PE intensity
///////////////////////////////

  Int_t iConfig;
  const double DetectorAltitude[4] = { 0, 1, 2, 3};

  Int_t NumSites; //number of trinity sites

  // Trigger Timing information
  Int_t iTrigWin; // index 5 means all PEs are counted regardless of time
  const double TriggerWindow[5] = {30.00, 60.00, 150.00, 300.00, 1500.00}; // ns
 
  //obtained from 3e4 GeV gamma rays
  //Parameterization of PE distribution at 50km, 0ele, and 0 Altitude
  //par[0]*exp(-xx/par[1])+par[2]*exp(-xx/(par[3]+xx*par[4]))
  const double lincorr[4] = { 0.0509251, 0.0522854, 0.0595455, 0.0642221};
  const double scalefirst[4] = { 1.00001, 1.03346, 1.53535, 2.36961};
  const double eleScaling[4] = { 0.00163848, 0.00191408, 0.0071185, 0.0513182};
  const double absorptionlength[4] = { 16.7049, 16.6106, 17.9808, 19.0274};
  const double parPEF[5] = { 0.000332267, 0.580756, 4.25751e-05, 1.9491, 0.0427249};

  /*
  //obtained from 1e6 GeV gamma rays
  double lincorr[] = { 0.0497124, 0.0493805, 0.0552341, 0.0604423};
  double scalefirst[] = { 1.00001, 1.02504, 1.42546, 1.73371};
  double eleScaling[] = { 0.00143344, 0.00164536, 0.00419904, 0.00479111};
  double absorptionlength[] = { 16.7647, 16.8572, 18.3202, 19.4597};
  //Parameterization of PE distribution at 50km, 0ele, and 0 Altitude
  //par[0]*exp(-xx/par[1])+par[2]*exp(-xx/(par[3]+xx*par[4]))
  double parPEF[] = { 0.00038827, 0.555588, 4.66631e-05, 1.90266, 0.0426453};
  */

  // Parameterization of PE distribution with timing at 40km,0 ele,and 2km altitude
  //obtained from 1e6 GeV gamma rays with various ns trigger window (30,60,150,300,1500)
  const double DetectorAltitude2[5] = { 2, 2, 2, 2, 2};
  const double lincorr2[5] = { 0.04,  0.04, 0.04, 0.0400153, 0.217309};
  const double scalefirst2[5] = { 1, 1, 1, 1.28155, 1.26299};
  const double eleScaling2[5] = { 0.00501046, 0.00499608, 0.00500683, 0.0230373, 0.0233402};
  const double absorptionlength2[5] = { 20, 20, 20, 19.9998, 19.8361};
  const double parPEF2[5][5] = {{ 2.48094e-09, 0.971087, 0.000211414, 1.04562, 0.0346362}, { 2.52288e-10, 0.379361, 0.000306272, 1.10666, 0.0341286}, { 1.95971e-08, 0.373425, 0.000276322, 1.33355, 0.0321227}, { 4.90836e-07, 0.22572, 0.000208922, 1.5886, 0.0298558}, { 5.64579e-08, 0.391996, 0.000111541, 2.13944, 0.0294045}};

  // Parameterization of PE distribution with timing at 80km,0 ele,and 2km altitude
  //obtained from 1e6 GeV gamma rays with various trigger windows (30,60,150,300,1500)
  const double DetectorAltitude3[5] = { 2, 2, 2, 2, 2};
  const double lincorr3[5] = { 0.0786967, 0.0779326, 0.0770613, 0.0761429, 0.0736504};
  const double scalefirst3[5] = { 1, 1, 1, 1, 1};
  const double eleScaling3[5] = { 0.00170477, 0.00173431, 0.00175404, 0.00178948, 0.00186624};
  const double absorptionlength3[5] = { 20, 20, 20, 20, 20};
  const double parPEF3[5][5] = {{ 3.41281e-05, 0.911229, 1.30399e-05, 1.24591, 0.032254}, { 1.0245e-09, 0.321118, 3.32187e-05, 1.19095, 0.0326761}, { 2.97373e-13, 0.472895, 2.50814e-05, 1.48642, 0.0297992}, { 1.34544e-08, 0.519933, 1.77321e-05, 1.79327, 0.0268114}, { 2.37947e-05, 0.9659, 6.80081e-06, 2.57612, 0.0237208}};

///////////////////////

  Double_t dExposureHours;
  Double_t dMinEnu;
  Double_t dMaxEnu;
  Double_t dLogEnergyStep; //was 1
  Double_t dHalfEnergyBinWidth; //in log was 1/2
  Double_t nuIndex; //power law index of the neutrino spectrum the minus sign is added later
  Double_t dST; //km max height of shower tip above ground;

  Double_t yMin;
  Double_t yMax;
  Double_t yDelta;
  Double_t MaxElevation; //elevation angle (determines path through Earth)
  Double_t dMaxAzimuthSky; //max limit for azimuth evaluation 
  Double_t DeltaAngleAz; //steps in azimuth in the camera FOV
  Double_t DeltaAngleSky; //this is the step size in elevation and azimuth in the sky to calculate the acceptance for a limited FOV telescope. Make sure it is smaller than then FOV of the camera  
  Double_t dMaxCherenkovAzimuthAngle; //maximum azimuth angle for cherenkov 
  Double_t dMaxFluorescenceDistance;

  //next three parameters are key to the instrument
  //plus the telescop height above ground which can be selected above
  Double_t tanFoV; //Field of view of telescope above the horizon
  Double_t dFoVBelow; //Field of view of telescope  below horizon 
  Double_t dMinLength; //mimnimum length a shower has to have in the camera, in degrees. This is a conservative estimate because it assumes that the shower starts at a distance l from the detector, which is not necessarily tru for showers with shallow elevation angles.
  Double_t dMinimumNumberPhotoelectrons;  //this is pe per sqm mirror 

/*
  Double_t dMirrorA[4] = {1.0, 5.0, 10.0, 100.0}; //m^2 
  //Double_t dThreshold[] = {8*3, 19*3, 22*3, 120*3}; //pe //three fold coincidence. 
  Double_t dThreshold[4] = {10*2, 22*2, 24*2, 155*2}; //pe //two fold coincidence
*/

  Double_t hFOV;
  Bool_t blimFOV;


  Bool_t bFluorescence;
  Bool_t bCombined;
  Bool_t bMonoNu; //simulate monoenergetic neutrinos, only good for acceptance calculation. For all other simulations set it to kFALSE

  TH1D *hTriggeredAzimuthAngles;

  TGraph *grsCC;
  TGraph *grsNC;
  
  string star ;
  double number;
  int distanceNumber;
  vector<double> enerNu,enerTau,prob,dist,EtauNorm;
  unsigned rem = 0 ;
};
