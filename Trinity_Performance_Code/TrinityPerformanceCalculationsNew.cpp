#include "TrinitySimUtilities.h"

using namespace std;


////////////////////////////////////////////////////////////////////
// Main Program
//
//////////////////////////////////////////////////////////////////
int main (int argc, char **argv) {


  //initiate root
  TROOT root("DisplayEvts","Display Results");
  TApplication *theApp = new TApplication("App",&argc,argv);
  gROOT->ProcessLine("#include <vector>"); //need this otherwise we cannot save vectors in the root file

  TrinitySimUtilities *trinityUtils = new TrinitySimUtilities();

/*
  trinityUtils->SetMinimumDistance(100);
  trinityUtils->SetMaximumDistance(110);
  trinityUtils->SetMaximumElevationAngle(5);
  trinityUtils->SetStepSizeSky(0.2);
  trinityUtils->SetMaximumShowerViewingAngle(1);
*/
/*
  trinityUtils->ConfigureDemonstrator(); 
  trinityUtils->SetCameraHorizontalFoV(5);
  trinityUtils->SetTelescopeFoVAboveHorizon(2);
  trinityUtils->SetTelescopeFoVBelowHorizon(3);
  trinityUtils->SetMaximumShowerViewingAngle(20);
*/
  //plot the probability that a tau emerges when a neutrino hits the Earth
  //trinityUtils->PlotEmergenceProbability(); 
  //cout<<"Done with Emergence Probability Plot"<<endl;
  
  //trinityUtils->SetLowerNeutrinoEnergy(6.0);
  //trinityUtils->SetUpperNeutrinoEnergy(10.0);

  Double_t logEmin = 6; //min energy log
  Double_t logEmax = 10; //max energy log
  TGraph *grDiffAcceptance = new TGraph();
  
  //trinityUtils->SetSimulateMonoNuEnergies(kTRUE);  

  trinityUtils->DumpConfiguration();
  //trinityUtils->GetDiffuseAcceptanceLimitedFoV(logEmin,logEmax,grDiffAcceptance);//ok
  //trinityUtils->GetDiffuseAcceptance(logEmin,logEmax,grDiffAcceptance); //ok
  trinityUtils->CalculateDiffuseFluxDifferentialSensitivity(); //ok
  //trinityUtils->CalculateIntegralSensitivity();

  //trinityUtils->SetMaximumAzimuthAngle(10); //log10 [GeV] 
  //trinityUtils->GetPointSourceAcceptanceInFOV(logEmin, logEmax); //running
  //trinityUtils->GetPointSourceAcceptanceSingleAngle(logEmin,logEmax); //ok
  //trinityUtils->SetNeutrinoEnergyStep(0.5); //log10 [GeV] 
  //trinityUtils->SetIntegrationRangeForSensitivity(0.5); //log10 [GeV] 
  //trinityUtils->CalculatePointSourceDifferentialSensitivity(); //ok
/*
  TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
  timer.TurnOn();
  Getline("Type <return> to go on: ");
  timer.TurnOff();
*/
 cout<<"done"<<endl;
 theApp->Run();
 
 return 0;
}
