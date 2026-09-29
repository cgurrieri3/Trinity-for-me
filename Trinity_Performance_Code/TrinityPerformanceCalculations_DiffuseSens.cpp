#include "TrinitySimUtilities.h"
#include "TMath.h"

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
  
  // Set the output folder
  trinityUtils->SetOutputFolder();

  //trinityUtils->ConfigureDemonstrator(); 
  trinityUtils->SetMaximumShowerViewingAngle(20);

  //plot the probability that a tau emerges when a neutrino hits the Earth
  //trinityUtils->PlotEmergenceProbability(); 
  
  
  //cout<<"Done with Emergence Probability Plot"<<endl;
  
  

  Double_t logEmin = 6; //min energy log
  Double_t logEmax = 10.5; //max energy log
  
  trinityUtils->SetLowerNeutrinoEnergy(logEmin);
  trinityUtils->SetUpperNeutrinoEnergy(logEmax);
  trinityUtils->SetNeutrinoEnergyStep(1); //log10 [GeV]
  
  
  
  trinityUtils->SetTelescopeFoVAboveHorizon(2); //deg Field of view of telescope above the horizon
  trinityUtils->SetTelescopeFoVBelowHorizon(TMath::Pi()*3/180); //rad Field of view of telescope  below horizon 
  trinityUtils->SetMinimumNumberPhotoelectrons(10);  //this is pe per sqm mirror 
  trinityUtils->SetNumberOfTrinitySites(1);
  
 
 

 
  trinityUtils->DumpConfiguration();
  //trinityUtils->GetPointSourceAcceptanceInFOV(logEmin, logEmax); //ok
  //trinityUtils->GetPointSourceAcceptanceSingleAngle(logEmin,logEmax); //ok
  //trinityUtils->SetNeutrinoEnergyStep(0.5); //log10 [GeV] 
  //trinityUtils->SetIntegrationRangeForSensitivity(0.5); //log10 [GeV] 
  trinityUtils->CalculateDiffuseFluxDifferentialSensitivity(); //diffuse sensitivity plot


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
