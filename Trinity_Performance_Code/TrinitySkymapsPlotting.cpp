#include <TF1.h>
#include <TGraph.h>
#include <TGaxis.h>
#include <TTimer.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TMath.h>
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

#include <TSystem.h>
#include <sys/stat.h>

using namespace std;

// Global variable to store the output folder name
std::string outputFolder;
std::string pdfPath;
std::string rootPath;

const Double_t pi = 3.14159265359;

// for sky plots
Double_t latitude;
Double_t tStep;
Double_t MaxAzimuth;

string Hold()
{
  string input;

  //hold the code;
  TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
  timer.TurnOn();
  cout<<"Press Enter to continue:"<<endl;
  getline(cin,input);
  timer.TurnOff();
  return input;
  
}

// Function to get the current date and time as a formatted string
std::string getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
    return std::string(buf);
}

// Function to create a directory if it doesn't exist
void createDirectory(const std::string& path) {
    if (gSystem->AccessPathName(path.c_str())) { // Check if directory exists
        gSystem->mkdir(path.c_str(), true);      // Create directory (with parents)
        std::cout << "Created directory: " << path << std::endl;
    } else {
        std::cout << "Directory already exists: " << path << std::endl;
    }
}
void PlotAcceptanceSkymaps()
{
  TString FilenamePointSourceAcceptanceLimitedFoV("PointSourceAcceptance_FoV3.8deg_Index2.0_Emin6.0_Emax10.0.root");
  TString FilenamePointSourceAcceptance360FoV("PointSourceAcceptance_FoV360deg_Index2.0_Emin6.0_Emax10.0.root");
  Bool_t blimFOV = kFALSE;
  Double_t logEmin = 6; //min energy log
  Double_t logEmax = 10; //max energy log
  Double_t nuIndex = 2.0;
  //up to here all parameters must match what is in the filename

  //let place and point the telescope in azimuth
  latitude = 38.52028; //lat of frisco peak, utah
  Double_t teleDirection = -80.; // Telescope direction relative to north (NofW)
  //Double_t teleDirection = -110; // Demonstartor point azimuth
 
  tStep = 2.5; //10 min step in degrees
  //Double_t Enaught = 1e8; //GeV
  //Double_t Fnaught = 6.694e-23; //GeV^-1 cm^-2 s^-1
  //Best Case for NGC1068 (2022, https://arxiv.org/abs/2211.09972)
  //Double_t Enaught = 1e3; // GeV (1 TeV)
  //Double_t Fnaught = 10.5e-14; // GeV^-1 cm^-2 s^-1
  //nuIndex = 3.0;
  //Flux for TXS0506+056 (9.5 year best fit, https://arxiv.org/abs/1807.08794)
  Double_t Enaught = 1e5; // GeV (100 TeV)
  Double_t Fnaught = 0.8e-17; // GeV^-1 cm^-2 s^-1
  Double_t normInverse = (pow(pow(10, logEmin), (1 - nuIndex)) - pow(pow(10, logEmax), (1 - nuIndex))) / (nuIndex - 1); // integral of E^-nuIndex from Emin to Emax to correct for the normalization in the GetTauDistibution function
  //~ normInverse = 1.0;
  //~ multNorm = kFALSE;

  Double_t LST = 0;
  Double_t degconv = pi/180.0;
  
  //new canvas for the horizontal skymaps, markers and labels for galactic landmarks
  int nMarks = 11;
  TCanvas *skyC = new TCanvas("skyC","Skymap of Acceptance",1600,750); 
  TMarker *galMarks[nMarks];
  TMarker *galMarksSup[nMarks];
  TMarker *galMarksEq[nMarks];
  TMarker *galMarksEqI[nMarks];
  TMarker *galMarksEqT[nMarks];
  TText *labels[nMarks];
  TText *labelsSup[nMarks];
  TText *labelsEq[nMarks];
  TText *labelsEqI[nMarks];
  TText *labelsEqT[nMarks];
  TPad *pad1 = new TPad("pad1","",0,0,1,1);
  
  //marker initialization
  for(int i = 0; i < nMarks; i++) {
    galMarks[i] = new TMarker(0.,0., 43);
    galMarksSup[i] = new TMarker(0.,0., 43);
    galMarksEq[i] = new TMarker(0.,0., 43);
    galMarksEqI[i] = new TMarker(0.,0., 43);
    galMarksEqT[i] = new TMarker(0.,0., 43);
    galMarks[i]->SetMarkerSize(1.5);
    galMarksSup[i]->SetMarkerSize(1.5);
    galMarksEq[i]->SetMarkerSize(1.5);
    galMarksEqI[i]->SetMarkerSize(1.5);
    galMarksEqT[i]->SetMarkerSize(1.5);
  }
  
  //label initialization
  for(int i = 0; i < nMarks; i++) {
    labels[i] = new TText();
    labels[i]->SetTextSize(20);
    labels[i]->SetTextFont(43);
    labels[i]->SetTextAlign(22);
    labelsSup[i] = new TText();
    labelsSup[i]->SetTextSize(20);
    labelsSup[i]->SetTextFont(43);
    labelsSup[i]->SetTextAlign(22);
    labelsEq[i] = new TText();
    labelsEq[i]->SetTextSize(20);
    labelsEq[i]->SetTextFont(43);
    labelsEq[i]->SetTextAlign(22);
    labelsEqI[i] = new TText();
    labelsEqI[i]->SetTextSize(20);
    labelsEqI[i]->SetTextFont(43);
    labelsEqI[i]->SetTextAlign(22);
    labelsEqT[i] = new TText();
    labelsEqT[i]->SetTextSize(20);
    labelsEqT[i]->SetTextFont(43);
    labelsEqT[i]->SetTextAlign(22);
  }
  
  //pad init
  pad1->SetFillStyle(4000);
  pad1->SetFillColor(0);
  pad1->SetBorderSize(0);
  pad1->SetFrameBorderMode(0);
  pad1->SetFrameLineColor(0); 
  pad1->SetFrameBorderMode(0);
  pad1->Range(-231,-111.875,283,111.875);
  
  //calculations done for the gridlines of each skymap
  float conv=TMath::Pi()/180; 
  float la, lo, x, yy, z;
  int Nl = 13; // Number of drawn latitudes
  int NL = 13; // Number of drawn longitudes
  int M  = 30;
  
  TGraph  *latitudes[Nl];
  TGraph  *longitudes[NL];
  
  //~ float xscale = 57.2;
  //~ float yscale = 57.2;
  
  for (int j=0;j<Nl;++j) {
    latitudes[j]=new TGraph();
    la = -90+180/(Nl-1)*j;
    for (int i=0;i<M+1;++i) {
      lo = -180+360/M*i;
      z  = sqrt(1+cos(la*conv)*cos(lo*conv/2));
      x  = 180*cos(la*conv)*sin(lo*conv/2)/z;
      yy  = 90*sin(la*conv)/z;
      //~ z = acos(cos(la * conv) * cos(0.5 * lo * conv));
      //~ if(z == 0.0)
      //~ {
      //~ x = xscale * (2.0 * cos(la * conv) * sin(0.5 * lo * conv));
      //~ yy = yscale * (sin(la * conv));
      //~ }
      //~ else
      //~ {
      //~ x = xscale * (2.0 * cos(la * conv) * sin(0.5 * lo * conv))/(sin(z) / z);
      //~ yy = yscale * (sin(la * conv)) / (sin(z) / z);
      //~ }
      latitudes[j]->SetPoint(i,x,yy);
    }
  }
  
  for (int j=0;j<NL;++j) {
    longitudes[j]=new TGraph();
    lo = -180+360/(NL-1)*j;
    for (int i=0;i<M+1;++i) {
      la = -90+180/M*i;
      z  = sqrt(1+cos(la*conv)*cos(lo*conv/2));
      x  = 180*cos(la*conv)*sin(lo*conv/2)/z;
      yy  = 90*sin(la*conv)/z;
      //~ z = acos(cos(la * conv) * cos(0.5 * lo * conv));
      //~ if(z == 0.0)
      //~ {
      //~ x = xscale * (2.0 * cos(la * conv) * sin(0.5 * lo * conv));
      //~ yy = yscale * (sin(la * conv));
      //~ }
      //~ else
      //~ {
      //~ x = xscale * (2.0 * cos(la * conv) * sin(0.5 * lo * conv))/(sin(z) / z);
      //~ yy = yscale * (sin(la * conv)) / (sin(z) / z);
      //~ }
      longitudes[j]->SetPoint(i,x,yy);
    }
  }
  
  //2D histograms for various skymaps + histogram to store number of neutrino events
  TH2F *skymapFull360Sweep = new TH2F("skymapFull360Sweep","Acceptance Skymap of 360 Degree Airshower Azimuth Sweep", 3601, -180.05, 180.05, 1801, -90.05, 90.05);
  TH2F *skymapFullProjection = new TH2F("skymapFullProjection","360 FoV Projection In Galactic Coordinates Over 1 Year of Exposure", 361, -180.05, 180.05, 181, -90.05, 90.05); //galactic
  TH2F *skymapProjSuperGal = new TH2F("skymapProjSuperGal","360 FoV Projection In Supergalactic Coordinates Over 1 Year of Exposure", 361, -180.05, 180.05, 181, -90.05, 90.05); //supergal
  TH2F *skymapProjEq = new TH2F("skymapProjEq","360 FoV Projection In Equatorial Coordinates Over 1 Year of Exposure", 361, -180.05, 180.05, 181, -90.05, 90.05); //equatorial
  TH2F *skymapInstantConverage = new TH2F("skymapInstantConverage","Instantanious Sky Coverage In Equatorial Coordinates", 361, -180.05, 180.05, 181, -90.05, 90.05); //histogram for instant sky coverage
  TH2F *nuevents = (TH2F*)skymapProjEq->Clone("nuevents");
  TH2F *TT = (TH2F*)skymapFull360Sweep->Clone("TT");
  TH2F *skymapTimeExp = new TH2F("skymapTimeExp","Souce Exposure Times", 361, -180.05, 180.05, 181, -90.05, 90.05);
  TH2F *sensEq = new TH2F("sensEq", "Sensitivity after 1 year of observation (10^{8} GeV Normalization)", 361, -180.05, 180.05, 181, -90.05, 90.05);
  TH2F *sensEqI = (TH2F*)sensEq->Clone("sensint");
  
  //histogram formatting
  //~ skymapSingleAngle->GetZaxis()->SetTitle("Acceptance [cm^2]");
  skymapFull360Sweep->GetXaxis()->SetTitle("Azimuth Angle [degrees]");
  skymapFull360Sweep->GetYaxis()->SetTitle("Elevation Angle [degrees]");
  //~ skymapFull360Sweep->GetZaxis()->SetTitle("Acceptance [cm^2]");
  skyC->Divide(2,1);


  TH2F *skymapSingleAngle = 0;
  if (blimFOV){
    //open 5 degree fov file
    TFile *fileDe = TFile::Open(FilenamePointSourceAcceptanceLimitedFoV.Data());
    skymapSingleAngle = (TH2F*)fileDe->Get("skyAcceptance");
    //projecting the single angle of acceptance over a 360 degree FoV
    for(int yBins = 1; yBins <= skymapSingleAngle->GetNbinsY(); yBins++)
      {
	for(int xBins = 250; xBins <= (skymapSingleAngle->GetNbinsX()-250); xBins++){
	  skymapFull360Sweep->SetBinContent(xBins, yBins, skymapSingleAngle->GetBinContent(xBins,yBins));
   	}
    }
  }else{
    //open single angle file for 360 degree fov
    TFile *fileDe = TFile::Open(FilenamePointSourceAcceptance360FoV.Data());
    skymapSingleAngle = (TH2F*)fileDe->Get("skyAcceptance");
    
    //projecting the single angle of acceptance over a 360 degree FoV
    for(int yBins = 1; yBins <= skymapSingleAngle->GetNbinsY(); yBins++)
      {
	Double_t comboBin = 0;
	for(int xBins = 1; xBins <= skymapSingleAngle->GetNbinsX(); xBins++)
	  comboBin += skymapSingleAngle->GetBinContent(xBins, yBins);
	for(int xBins = 1; xBins <= skymapSingleAngle->GetNbinsX(); xBins++)
	  skymapFull360Sweep->SetBinContent(xBins, yBins, comboBin);
      }
  }
  
  Double_t vFov = 0;
  

  for(int i = 1; i <= skymapFull360Sweep->GetNbinsY(); i++)
    {
      if(skymapFull360Sweep->GetBinContent(1801, i) > 0) {
  	vFov += 0.1;
  	//cout<<skymapFull360Sweep->GetBinContent(1801, i)<< " " << vFov<<endl;
      }
    }
  //~return;
  Double_t hFov = 0;
  for(int i = 1; i <= skymapFull360Sweep->GetNbinsX(); i++)
    {
      if(skymapFull360Sweep->GetBinContent(i, 900) > 0) {
  	hFov += 0.1;
  	//cout<<skymapFull360Sweep->GetBinContent(i, 900)<< " " << hFov<<endl;
      }
    }
  //~return;
  skyC->cd(1);
  gPad->SetLogz(1);
  gPad->SetRightMargin(0.15);
  skymapSingleAngle->Draw("COLZ"); //plot single angle skymap
  
  skyC->cd(2);
  gPad->SetLogz(1);
  gPad->SetRightMargin(0.15);
  skymapFull360Sweep->Draw("COLZ"); //plot 360 sweep skymap
  //return;
  for(int r = -180; r <= 180; r++) { //filling the instant sky converage histogram
    for(int d = -90; d <= 90; d++) {
      Double_t az = (atan2(sin((LST - r) * degconv), cos((LST - r) * degconv) * sin(latitude * degconv) - tan(d * degconv) * cos(latitude * degconv)) * 180 / pi) - 180;
      //~ Double_t az = (-1.0 * atan2(sin((LST - r) * degconv) * cos(d * degconv), cos(latitude * degconv) * sin(d * degconv) - sin(latitude * degconv) * cos(d * degconv) * cos((LST - r) * degconv)) * 180 / pi) - 180;
      Double_t alt = asin(sin(latitude * degconv) * sin(d * degconv) + cos(latitude * degconv) * cos(d * degconv) * cos((LST - r) * degconv)) * 180 / pi;
      az = az + teleDirection;
      if(az > 180.0)
	az = az - 360.0;
      else if(az < -180.0)
	az = az + 360.0;
      //~ Double_t dec = asin(sin(d * degconv) * sin(27.1284 * degconv) + cos(d * degconv) * cos(27.1284 * degconv) * cos((122.9320 - r) * degconv)) * 180 / pi;
      //~ Double_t ra = (atan2((cos(d * degconv) * sin((122.932 - r) * degconv)), (sin(d * degconv) * cos(27.1284 * degconv) - cos(d * degconv) * sin(27.1284 * degconv) * cos((122.9320 - r) * degconv))) * 180 / pi) + 192.8595;
      //~ if(ra > 180)
      //~ ra = ra - 360.;
      //~ if(ra < -180)
      //~ ra = ra + 360.;
      //~ Double_t az = (atan2(sin((LST - ra) * degconv), cos((LST - ra) * degconv) * sin(latitude * degconv) - tan(dec * degconv) * cos(latitude * degconv)) * 180 / pi) - 180;
      //~ Double_t alt = asin(sin(latitude * degconv) * sin(dec * degconv) + cos(latitude * degconv) * cos(dec * degconv) * cos((LST - ra) * degconv)) * 180 / pi;
      //~ if(az > 180.0)
      //~ az = az - 360.0;
      //~ if(az < -180.0)
      //~ az = az + 360.0;
      int xBin = (int)((az + 180.1) * 10);
      int yBin = (int)((alt + 90.1) * 10);
      cout <<"xBin: "<<xBin<<" yBin: "<<yBin<<" r: "<<r<<" d: "<<d<<"  "<<skymapFull360Sweep->GetBinContent(xBin, yBin)<<endl;
      skymapInstantConverage->Fill((-1 * r), d, skymapFull360Sweep->GetBinContent(xBin, yBin));
    }
  }

  TCanvas *test3 = new TCanvas("test3","test3",1500,750);
  test3->cd(1);
  skymapInstantConverage->Draw("colz");
  
  TCanvas *skyProjInstantEq = new TCanvas("skyProjInstantEq","Instant Skymap Coverage (Equatorial Coordinates)",1500,750); //canvas for instant converage skymap
  TPad *padI = (TPad*)pad1->Clone("padI");
  
  skyProjInstantEq->cd(1);
  skyProjInstantEq->SetRightMargin(0.2);
  
  skymapInstantConverage->GetXaxis()->SetTitle("Right Ascension [hours]");
  skymapInstantConverage->GetYaxis()->SetTitle("Declination [deg]");
  skymapInstantConverage->GetZaxis()->SetTitle("Acceptance [cm^{2}]");
  skymapInstantConverage->GetXaxis()->SetNdivisions(-512);
  skymapInstantConverage->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"12h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"10h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"8h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"6h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"4h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"2h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"22h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"20h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"18h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"16h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"14h");
  skymapInstantConverage->GetXaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"12h");
  skymapInstantConverage->Draw("z aitoff");
  padI->Draw();
  
  galMarksEqI[0]->SetX(84.7649); //galactic center
  galMarksEqI[0]->SetY(-32.1444);
  
  galMarksEqI[1]->SetX(-77.0868); //TXS
  galMarksEqI[1]->SetY(6.1484);
  
  galMarksEqI[2]->SetX(86.8957); //MRK 501
  galMarksEqI[2]->SetY(45.1052);
  
  galMarksEqI[3]->SetX(-132.496); //MRK 421
  galMarksEqI[3]->SetY(52.534);
  
  galMarksEqI[4]->SetX(161.754); //Virgo
  galMarksEqI[4]->SetY(-9.72109);
  
  galMarksEqI[5]->SetX(49.3004); //cygnus A
  galMarksEqI[5]->SetY(42.3715);
  
  galMarksEqI[6]->SetX(119.23); // Cen A
  galMarksEqI[6]->SetY(-56.6101);
  
  galMarksEqI[7]->SetX(-92.1939); //Auger Dipole
  galMarksEqI[7]->SetY(-29.1072);
  
  galMarksEqI[8]->SetX(-44.1018); //Fornax
  galMarksEqI[8]->SetY(-34.7217);
  
  galMarksEqI[9]->SetX(-102.613); //TA Hotspot
  galMarksEqI[9]->SetY(52.2853);
  
  galMarksEqI[10]->SetX(-40.6698); //NGC 1068
  galMarksEqI[10]->SetY(-0.0135745);
  
  //~ galMarksEqI[0]->SetX(0); //galactic center
  //~ galMarksEqI[0]->SetY(0);
  
  //~ galMarksEqI[1]->SetX(155.711); //TXS
  //~ galMarksEqI[1]->SetY(-28.0311);
  
  //~ galMarksEqI[2]->SetX(-53.1613); //MRK 501
  //~ galMarksEqI[2]->SetY(40.6421);
  
  //~ galMarksEqI[3]->SetX(-75.9516); //MRK 421
  //~ galMarksEqI[3]->SetY(81.5564);
  
  //~ galMarksEqI[4]->SetX(32.3546); //Virgo
  //~ galMarksEqI[4]->SetY(57.0701);
  
  //~ galMarksEqI[5]->SetX(-75.9174); //cygnus A
  //~ galMarksEqI[5]->SetY(6.20106);
  
  //~ galMarksEqI[6]->SetX(48.4862); // Cen A
  //~ galMarksEqI[6]->SetY(20.0392);
  
  //~ galMarksEqI[7]->SetX(122.036); //Auger Dipole
  //~ galMarksEqI[7]->SetY(-17.1514);
  
  //~ galMarksEqI[8]->SetX(75.2931); //Fornax
  //~ galMarksEqI[8]->SetY(-65.8552);
  
  //~ galMarksEqI[9]->SetX(-136.706); //TA Hotspot
  //~ galMarksEqI[9]->SetY(57.3636);
  
  labelsEqI[0]->SetText(galMarksEqI[0]->GetX() + 12, galMarksEqI[0]->GetY() -5, "Galactic Center");
  labelsEqI[1]->SetText(galMarksEqI[1]->GetX(), galMarksEqI[1]->GetY() -5, "TXS 0506+056");
  labelsEqI[2]->SetText(galMarksEqI[2]->GetX(), galMarksEqI[2]->GetY() -5, "MRK 501");
  labelsEqI[3]->SetText(galMarksEqI[3]->GetX() - 15, galMarksEqI[3]->GetY(), "MRK 421");
  labelsEqI[4]->SetText(galMarksEqI[4]->GetX(), galMarksEqI[4]->GetY() -5, "Virgo");
  labelsEqI[5]->SetText(galMarksEqI[5]->GetX(), galMarksEqI[5]->GetY() -5, "Cygnus A");
  labelsEqI[6]->SetText(galMarksEqI[6]->GetX(), galMarksEqI[6]->GetY() -5, "Cen A");
  labelsEqI[7]->SetText(galMarksEqI[7]->GetX() - 7, galMarksEqI[7]->GetY() -5, "Auger Dipole");
  labelsEqI[8]->SetText(galMarksEqI[8]->GetX(), galMarksEqI[8]->GetY() -5, "Fornax");
  labelsEqI[9]->SetText(galMarksEqI[9]->GetX() + 9, galMarksEqI[9]->GetY() -5, "TA Hotspot");
  labelsEqI[10]->SetText(galMarksEqI[10]->GetX(), galMarksEqI[10]->GetY() -5, "NGC 1068");
  
  padI->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nMarks; i++) {
    galMarksEqI[i]->Draw();
    labelsEqI[i]->Draw();
  }
  
  //~ gStyle->SetPalette(52);
  //~ return;//testing
  
  //~return;
  
  //~ TFile *f1 = new TFile("s360_2.2.root","RECREATE");
  //~ skymapFull360Sweep->Write();
  //~ f1->Close();
  
  //~ TFile *f2 = new TFile("singleangle_2.2.root","RECREATE");
  //~ skymapSingleAngle->Write();
  //~ f2->Close();
  
  //~return;
  
  Double_t totalT = 0, totalAcc = 0, maxT = 0, minT = 999999;
  int maxDay = -1, minDay = -1, noObsdays = 0;
  //calculations for the time evolution of the horizontal skymaps over various coordinate systems
  ifstream in;
  in.open("1yrmod.txt"); //open ephem file
  //in.open("3yrmod.txt");
  //in.open("5yrmod.txt");
  //in.open("10yrmod.txt");
  //~ return;
  if (in.is_open())
    {
      Double_t setTimeSun, riseTimeSun, riseTimeMoon, setTimeMoon, phaseMoon, deltaT = -1, tStepAdjusted; 
      //~ Double_t origLST;
      string sTimeS, rTimeS, rTimeM, sTimeM, phM;
      bool nestNone = false, nestSun = false, nestBoth = false;
      int nSteps, daycounter = 1;
      
      while(in.good()) 
	{
	  in >> sTimeS >> rTimeS >> rTimeM >> sTimeM >> phM; //read from the file that contains rise and set times of moon and sun in LST as well as phase of moon
	  
	  setTimeSun = stod(sTimeS);
	  riseTimeSun = stod(rTimeS);
	  riseTimeMoon = stod(rTimeM);
	  setTimeMoon = stod(sTimeM);
	  phaseMoon = stod(phM);
	  
	  nestNone = false;
	  nestSun = false;
	  nestBoth = false;
	  
	  //moon phase calculations
	  if(phaseMoon < 0.3) {
	    if(setTimeSun > riseTimeSun)
	      deltaT = (360.0 - setTimeSun) + riseTimeSun;
	    else
	      deltaT = riseTimeSun - setTimeSun;
	    LST = setTimeSun;
	  } else {
	    if( (riseTimeSun > setTimeSun) && (setTimeMoon > riseTimeMoon) ) { //both don't cross 0hr
	      if( (riseTimeMoon < setTimeSun) && (setTimeMoon < setTimeSun) && (setTimeMoon < riseTimeSun) ){
		LST = setTimeMoon;
		deltaT = riseTimeSun - setTimeMoon;
	      } else if( (riseTimeMoon > setTimeSun) && (setTimeMoon > riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = riseTimeMoon - setTimeSun;
	      } else if( (riseTimeMoon > setTimeSun) && (setTimeMoon < riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = (riseTimeMoon - setTimeSun) + (riseTimeSun - setTimeMoon);
		nestNone = true;
	      } else if( (riseTimeMoon < setTimeSun && setTimeMoon < setTimeSun) || (riseTimeMoon > riseTimeSun && setTimeMoon > riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = riseTimeSun - setTimeSun;
	      } else { deltaT = 0; }
	    } else if( (riseTimeSun < setTimeSun) && (setTimeMoon > riseTimeMoon) ) { //only sun crosses 0hr (impossible for moon to cover full night)
	      if( (riseTimeMoon < setTimeSun) && (setTimeMoon > setTimeSun) ) {
		LST = setTimeMoon;
		deltaT = (360. + riseTimeSun) - setTimeMoon;
	      } else if( (riseTimeSun > riseTimeMoon) && (riseTimeSun < setTimeMoon) ) {
		LST = setTimeSun;
		deltaT = (360. + riseTimeMoon) - setTimeSun;
	      } else if( (riseTimeMoon > setTimeSun && setTimeMoon < 360.) || (riseTimeMoon < riseTimeSun && setTimeMoon < riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = (riseTimeSun + 360. - setTimeSun) - (setTimeMoon - riseTimeMoon);
		nestSun = true;
	      } else if( (riseTimeMoon < setTimeSun && setTimeMoon < setTimeSun) || (riseTimeMoon > riseTimeSun && setTimeMoon > riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = (360. + riseTimeSun) - setTimeSun;
	      }
	    } else if( (riseTimeSun > setTimeSun) && (setTimeMoon < riseTimeMoon) ) { //only moon crosses 0hr
	      if( ((setTimeSun + 360.) > riseTimeMoon) && (setTimeMoon > setTimeSun) && (setTimeMoon < riseTimeSun) ) {
		LST = setTimeMoon;
		deltaT = riseTimeSun - setTimeMoon;
	      } else if( (riseTimeMoon < riseTimeSun) && (riseTimeSun < (setTimeMoon + 360.)) && (riseTimeMoon > setTimeSun) ) {
		LST = setTimeSun;
		deltaT = riseTimeMoon - setTimeSun;
	      } else if( (riseTimeMoon > riseTimeSun) && (setTimeMoon < setTimeSun) ) {
		LST = setTimeSun;
		deltaT = riseTimeSun - setTimeSun;
	      } else { deltaT = 0; }
	    } else if ( (riseTimeSun < setTimeSun)  && (setTimeMoon < riseTimeMoon) ) { //both cross 0 hr
	      if( (riseTimeMoon > setTimeSun) && (riseTimeSun < setTimeMoon) ) {
		LST = setTimeSun;
		deltaT = riseTimeMoon - setTimeSun;
	      } else if( (riseTimeMoon < setTimeSun) && (setTimeMoon < riseTimeSun) ) {
		LST = setTimeMoon;
		deltaT = riseTimeSun - setTimeMoon;
	      } else if( (setTimeSun < riseTimeMoon) && (setTimeMoon < riseTimeSun) ) {
		LST = setTimeSun;
		deltaT = (riseTimeSun + 360. - setTimeSun) - (setTimeMoon + 360. - riseTimeMoon);
		nestBoth = true;
	      } else { deltaT = 0; }
	    }
	  }
	  
	  // LST = 0;
	  
	  cout<<"Day: "<<daycounter<<" Hours of obs: "<<deltaT / 15.0<<endl;
	  
	  totalT += deltaT;
	  nSteps = (int)(deltaT / tStep);
	  tStepAdjusted = deltaT / nSteps;
	  
	  int yBinMax = 900;
	  int yBinMin = 662;
	  
	  for(int yBins = yBinMin; yBins <= yBinMax; yBins++)
	    {
	      for(int xBins = 1; xBins <= TT->GetNbinsX(); xBins++)
		{
		  if (skymapFull360Sweep->GetBinContent(xBins,yBins) > 0)
		    TT->SetBinContent(xBins, yBins, tStepAdjusted * (24.0 / 360.0));
		}
	    }
	  
	  if(deltaT > maxT)
	    {
	      maxT = deltaT;
	      maxDay = daycounter;
	    }
	  
	  if(deltaT < minT)
	    {
	      minT = deltaT;
	      minDay = daycounter;
	    }
	  
	  if(deltaT == 0)
	    {
	      noObsdays++;
	    }
	  
	  daycounter++;

	  //evolving the horizotal skymap over time and projecting onto equatorial coordinates
	  for(int i = 0; i < nSteps; i++) { //equatorial
	    for(int r = -180; r <= 180; r++) {
	      for(int d = -90; d <= 90; d++) {
		Double_t az = (atan2(sin((LST - r) * degconv), cos((LST - r) * degconv) * sin(latitude * degconv) - tan(d * degconv) * cos(latitude * degconv)) * 180 / pi) - 180;
		Double_t alt = asin(sin(latitude * degconv) * sin(d * degconv) + cos(latitude * degconv) * cos(d * degconv) * cos((LST - r) * degconv)) * 180 / pi;
		az = az + teleDirection;
		if(az > 180.0)
		  az = az - 360.0;
		else if(az < -180.0)
		  az = az + 360.0;
		int xBin = (int)((az + 180.1) * 10);
		int yBin = (int)((alt + 90.1) * 10);
		if( !(nestNone && LST > riseTimeMoon && LST < setTimeMoon) && 
		    !(nestSun && LST > riseTimeMoon && LST < setTimeMoon) && 
		    !(nestBoth && ( (LST > riseTimeMoon && LST < 360.) || (LST > 0 && LST < setTimeMoon) ) ) ) 
		  { 
		    skymapProjEq->Fill((-1 * r), d, skymapFull360Sweep->GetBinContent(xBin, yBin) * tStepAdjusted * 240.0); //240 sec = 1 degree of RA
		    if (TT->GetBinContent(xBin,yBin) > 0)
		      skymapTimeExp->Fill((-1 * r), d, TT->GetBinContent(xBin, yBin));
		    //~ nuevents->Fill((-1 * r), d, skymapFull360Sweep->GetBinContent(xBin, yBin) * tStepAdjusted * 240 * normInverse * Fnaught / pow(Enaught, -nuIndex));
		  } 
	      }
	    }
	    LST += tStepAdjusted;
	    if(LST > 360.0)
	      LST -= 360.0;
	  }
	  //break;
	}
      
      TCanvas *test1 = new TCanvas("test1","test1",1500,750);
      test1->cd(1);
      skymapProjEq->Draw("colz");
      //return;
      
       ///Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/test1.pdf";
    rootPath = outputFolder  + "/test1.root";
    test1->SaveAs(pdfPath.c_str());
    test1->SaveAs(rootPath.c_str());

      TCanvas *test = new TCanvas("test","test",1500,750);
      test->Divide(2,1);
      test->cd(1);
      TT->Draw("colz");
      test->cd(2);
      skymapTimeExp->Draw("colz");
      //return;
      
      // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/test.pdf";
    rootPath = outputFolder  + "/test.root";
    test->SaveAs(pdfPath.c_str());
    test->SaveAs(rootPath.c_str());
      
     

      //galactic & supergal projections done w.r.t the equatorial plot after time evolution has finished
      for(int l = -180; l <= 180; l++)
	{
	  for(int b = -90; b <= 90; b++)
	    {
	      Double_t dec = asin(sin(b * degconv) * sin(27.1284 * degconv) + cos(b * degconv) * cos(27.1284 * degconv) * cos((122.9320 - l) * degconv)) * 180 / pi;
	      Double_t ra = (atan2((cos(b * degconv) * sin((122.932 - l) * degconv)), (sin(b * degconv) * cos(27.1284 * degconv) - cos(b * degconv) * sin(27.1284 * degconv) * cos((122.9320 - l) * degconv))) * 180 / pi) + 192.8595;
	      if(ra > 180)
		ra = ra - 360.;
	      else if(ra < -180)
		ra = ra + 360.;
	      int xBin = (int)(ra + 181);
	      int yBin = (int)(dec + 91);
	      skymapFullProjection->Fill(-1.0 * l, b, skymapProjEq->GetBinContent(xBin, yBin));
	      
	      dec = asin(sin(b * degconv) * sin(15.70894274 * degconv) + cos(b * degconv) * cos(15.70894274 * degconv) * cos((26.45043911 - l) * degconv)) * 180 / pi;
	      ra = (atan2((cos(b * degconv) * sin((26.45043911 - l) * degconv)), (sin(b * degconv) * cos(15.70894274 * degconv) - cos(b * degconv) * sin(15.70894274 * degconv) * cos((26.45043911 - l) * degconv))) * 180 / pi) + 283.75418652;
	      if(ra > 180)
		ra = ra - 360.;
	      else if(ra < -180)
		ra = ra + 360.;
	      xBin = (int)(ra + 181);
	      yBin = (int)(dec + 91);
	      skymapProjSuperGal->Fill(-1.0 * l, b, skymapProjEq->GetBinContent(xBin, yBin));
	    }
	}
      //applying the IceCube parameters to the time-evolved acceptance to get the number of neutrino events in each bin
      for(int i = 1; i <= skymapProjEq->GetNbinsX(); i++)
	{
	  for(int j = 1; j <= skymapProjEq->GetNbinsY(); j++)
	    nuevents->SetBinContent(i, j, skymapProjEq->GetBinContent(i, j) * normInverse * 1/3. * Fnaught / pow(Enaught, -nuIndex));//1/3 because we only detect tau neutrinos
	}

      TCanvas *test2 = new TCanvas("test2","test2",1500,750);
      test2->cd(1);
      nuevents->Draw("colz");
      
      for(int xBins = 1; xBins <= skymapProjEq->GetNbinsX(); xBins++)
	{
	  for(int yBins = 1; yBins <= skymapProjEq->GetNbinsY(); yBins++)
	    {
	      if(skymapProjEq->GetBinContent(xBins, yBins) > 0)
		{
		  Double_t buffer = skymapProjEq->GetBinContent(xBins, yBins);
		  sensEq->SetBinContent(xBins, yBins, pow(Enaught, -nuIndex) / (buffer * normInverse));
		  sensEqI->SetBinContent(xBins, yBins, 1. / buffer);
		  //~ cout<<pow(Enaught, -nuIndex) / (buffer * normInverse)<<endl;
		}
	    }
	}
      
      in.close();
    } else {cout << "Unable to open file" << endl; return; }
  
  for(int i = 1; i <= skymapProjEq->GetNbinsX(); i++) {
    for(int j = 1; j <= skymapProjEq->GetNbinsY(); j++)
      totalAcc += skymapProjEq->GetBinContent(i, j);
  }
  
  //printing observation time, acceptance, and duty cycle
  cout<<"Total observation time: "<<totalT * (24.0 / 360.0)<<" hours."<<endl;
  cout<<"Total Acceptance over "<<totalT * (24.0 / 360.0)<<" hours: "<<totalAcc<<endl;
  cout<<"Duty Cycle: "<<totalT / (365.25 * 360.) * 100.<<" percent"<<endl; //1 year
  cout<<"Total events observed: "<<totalAcc * normInverse * Fnaught / pow(Enaught, -nuIndex)<<" events/sr"<<endl;
  //~ cout<<"Duty Cycle: "<<totalT * (24.0 / 360.0) * (1/87600.0) * 100.<<" percent"<<endl; //10 years
  cout<<"Longest observation window: "<<maxT * (24.0 / 360.0)<<" hours "<<"on day "<<maxDay<<endl;
  cout<<"Shortest observation window: "<<minT * (24.0 / 360.0)<<" hours "<<"on day "<<minDay<<endl;
  cout<<"Average observation time per night: "<<totalT * (24.0 / 360.0) / 365.25<<" hours."<<endl;
  cout<<"Number of days where no observation was possible: "<<noObsdays<<endl;
  
  cout<<"Vertical FoV of telescope (deg): "<<vFov<<endl;
  cout<<"Horizontal FoV of telescope (deg): "<< hFov<<endl;
  cout<<"Area of FoV: "<<(vFov * hFov) * (pi * pi) / (180. * 180.)<<" sr"<<endl;
  cout<<"Pointing Direction of Telescope (deg): "<< 360. + teleDirection <<endl; 
  cout<<"Rate of gamma-ray bursts observed per day: "<<((vFov * hFov) * (pi * pi) / (180. * 180.)) / (4. * pi)<<endl;
  cout<<"Rate of gamma-ray bursts observed per year: "<<((vFov * hFov) * (pi * hFov) / (180. * 180.)) / (4. * pi) * 365.25<<endl;
  cout<<"Rate of gamma-ray bursts observed per day with duty cycle: "<<((vFov * hFov) * (pi * pi) / (180. * 180.)) / (4. * pi) * totalT / (365.25 * 360.)<<endl;
  cout<<"Rate of gamma-ray bursts observed per year with duty cycle: "<<((vFov * hFov) * (pi * pi) / (180. * 180.)) / (4. * pi) * 365.25 * totalT / (365.25 * 360.)<<endl;
  
  
  //initializing and formatting graphical elements for the galactic coordinate skymap
  TCanvas *skyProjC = new TCanvas("skyProjC","Skymap Projection (Galactic Coordinates)",1500,750); //canvas for galactic skymap projections
  
  //pad and histogram formatting
  
  TPad *pad2 = (TPad*)pad1->Clone("pad2");
  TPad *pad3 = (TPad*)pad1->Clone("pad3");
  TPad *pad4 = (TPad*)pad1->Clone("pad4");
  
  skyProjC->cd(1);
  //~ gStyle->SetPalette(52);
  //~ TColor::InvertPalette();
  skyProjC->SetRightMargin(0.2);
  skymapFullProjection->GetXaxis()->SetTitle("Galactic Longitude [deg]");
  skymapFullProjection->GetYaxis()->SetTitle("Galactic Latitude [deg]");
  skymapFullProjection->GetZaxis()->SetTitle("Acceptance [cm^{2} s]");
  skymapFullProjection->Draw("z aitoff");
  pad1->Draw();
  
  //~ labels[0]->SetText(-29, -5, "Galactic Center");
  //~ labels[1]->SetText(-55., 5.0, "Cygnus A");
  //~ labels[2]->SetText(-35., 35., "MRK 501");
  //~ labels[3]->SetText(-97., 85., "MRK 421");
  //~ labels[4]->SetText(-125., 45., "TA Hotspot");
  //~ labels[5]->SetText(36., 55, "Virgo");
  //~ labels[6]->SetText(48, 25, "Cen A");
  //~ labels[7]->SetText(150, -10, "Auger Dipole");
  //~ labels[8]->SetText(135, -35, "TXS 0506+056");
  //~ labels[9]->SetText(75, -72, "Fornax");
  
  galMarks[0]->SetX(0); //galactic center
  galMarks[0]->SetY(0);
  
  galMarks[1]->SetX(155.711); //TXS
  galMarks[1]->SetY(-28.0311);
  
  galMarks[2]->SetX(-53.1613); //MRK 501
  galMarks[2]->SetY(40.6421);
  
  galMarks[3]->SetX(-75.9516); //MRK 421
  galMarks[3]->SetY(81.5564);
  
  galMarks[4]->SetX(32.3546); //Virgo
  galMarks[4]->SetY(57.0701);
  
  galMarks[5]->SetX(-75.9174); //cygnus A
  galMarks[5]->SetY(6.20106);
  
  galMarks[6]->SetX(48.4862); // Cen A
  galMarks[6]->SetY(20.0392);
  
  galMarks[7]->SetX(122.036); //Auger Dipole
  galMarks[7]->SetY(-17.1514);
  
  galMarks[8]->SetX(75.2931); //Fornax
  galMarks[8]->SetY(-65.8552);
  
  galMarks[9]->SetX(-136.706); //TA Hotspot
  galMarks[9]->SetY(57.3636);
  
  galMarks[10]->SetX(-107.824); //NGC
  galMarks[10]->SetY(-69.0034);
  
  labels[0]->SetText(galMarks[0]->GetX(), galMarks[0]->GetY() -5, "Galactic Center");
  labels[1]->SetText(galMarks[1]->GetX(), galMarks[1]->GetY() -5, "TXS 0506+056");
  labels[2]->SetText(galMarks[2]->GetX(), galMarks[2]->GetY() -5, "MRK 501");
  labels[3]->SetText(galMarks[3]->GetX(), galMarks[3]->GetY() -5, "MRK 421");
  labels[4]->SetText(galMarks[4]->GetX(), galMarks[4]->GetY() -5, "Virgo");
  labels[5]->SetText(galMarks[5]->GetX(), galMarks[5]->GetY() -5, "Cygnus A");
  labels[6]->SetText(galMarks[6]->GetX(), galMarks[6]->GetY() -5, "Cen A");
  labels[7]->SetText(galMarks[7]->GetX(), galMarks[7]->GetY() -5, "Auger Dipole");
  labels[8]->SetText(galMarks[8]->GetX(), galMarks[8]->GetY() -5, "Fornax");
  labels[9]->SetText(galMarks[9]->GetX(), galMarks[9]->GetY() -5, "TA Hotspot");
  labels[10]->SetText(galMarks[10]->GetX(), galMarks[10]->GetY() -5, "NGC 1068");
  
  pad1->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nMarks; i++) {
    galMarks[i]->Draw();
    labels[i]->Draw();
  }
  
  //initializing and formatting graphical elements for the supergalactic coordinate skymap
  TCanvas *skyProjCSuper = new TCanvas("skyProjCSuper","Skymap Projection (Supergalactic Coordinates)",1500,750); //canvas for supergalactic skymap projections
  
  skyProjCSuper->cd(1);
  skyProjCSuper->SetRightMargin(0.2);
  skymapProjSuperGal->GetXaxis()->SetTitle("Supergalactic Longitude [deg]");
  skymapProjSuperGal->GetYaxis()->SetTitle("Supergalactic Latitude [deg]");
  skymapProjSuperGal->GetZaxis()->SetTitle("Acceptance [cm^{2} s]");
  skymapProjSuperGal->Draw("z aitoff");
  pad2->Draw();
  
  galMarksSup[0]->SetX(129.873); //galactic center
  galMarksSup[0]->SetY(59.1844);
  
  galMarksSup[1]->SetX(17.194); //TXS
  galMarksSup[1]->SetY(-56.5666);
  
  galMarksSup[2]->SetX(45.5916); //MRK 501
  galMarksSup[2]->SetY(56.6805);
  
  galMarksSup[3]->SetX(-70.6736); //MRK 421
  galMarksSup[3]->SetY(-11.2826);
  
  galMarksSup[4]->SetX(-123.878); //Virgo
  galMarksSup[4]->SetY(1.7941);
  
  galMarksSup[5]->SetX(-.259471); //cygnus A
  galMarksSup[5]->SetY(61.3381);
  
  galMarksSup[6]->SetX(-159.147); // Cen A
  galMarksSup[6]->SetY(-7.42725);
  
  galMarksSup[7]->SetX(29.4819); //Auger Dipole
  galMarksSup[7]->SetY(-84.528);
  
  galMarksSup[8]->SetX(78.3434); //Fornax
  galMarksSup[8]->SetY(-42.7981);
  
  galMarksSup[9]->SetX(-46.701); //TA Hotspot
  galMarksSup[9]->SetY(-25.9257);
  
  galMarksSup[10]->SetX(51.7886); //NGC
  galMarksSup[10]->SetY(-26.825);
  
  labelsSup[0]->SetText(galMarksSup[0]->GetX(), galMarksSup[0]->GetY() -5, "Galactic Center");
  labelsSup[1]->SetText(galMarksSup[1]->GetX(), galMarksSup[1]->GetY() -5, "TXS 0506+056");
  labelsSup[2]->SetText(galMarksSup[2]->GetX(), galMarksSup[2]->GetY() -5, "MRK 501");
  labelsSup[3]->SetText(galMarksSup[3]->GetX(), galMarksSup[3]->GetY() -5, "MRK 421");
  labelsSup[4]->SetText(galMarksSup[4]->GetX(), galMarksSup[4]->GetY() -5, "Virgo");
  labelsSup[5]->SetText(galMarksSup[5]->GetX(), galMarksSup[5]->GetY() -5, "Cygnus A");
  labelsSup[6]->SetText(galMarksSup[6]->GetX(), galMarksSup[6]->GetY() -5, "Cen A");
  labelsSup[7]->SetText(galMarksSup[7]->GetX(), galMarksSup[7]->GetY() -5, "Auger Dipole");
  labelsSup[8]->SetText(galMarksSup[8]->GetX(), galMarksSup[8]->GetY() -5, "Fornax");
  labelsSup[9]->SetText(galMarksSup[9]->GetX(), galMarksSup[9]->GetY() -5, "TA Hotspot");
  labelsSup[10]->SetText(galMarksSup[10]->GetX(), galMarksSup[10]->GetY() -5, "NGC 1068");
  
  pad2->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nMarks; i++) {
    galMarksSup[i]->Draw();
    labelsSup[i]->Draw();
  }
  
  //initializing and formatting graphical elements for the equatorial coordinate skymap
  TCanvas *skyProjCEq = new TCanvas("skyProjCEq","Skymap Projection (Equatorial Coordinates)",1500,750); //canvas for equatorial skymap projections
  
  skyProjCEq->cd(1);
  skyProjCEq->SetRightMargin(0.2);
  skymapProjEq->GetXaxis()->SetTitle("Right Ascension [hours]");
  skymapProjEq->GetYaxis()->SetTitle("Declination [deg]");
  skymapProjEq->GetZaxis()->SetTitle("Acceptance [cm^{2} s]");
  skymapProjEq->GetXaxis()->SetNdivisions(-512);
  skymapProjEq->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"12h");
  skymapProjEq->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"10h");
  skymapProjEq->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"8h");
  skymapProjEq->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"6h");
  skymapProjEq->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"4h");
  skymapProjEq->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"2h");
  skymapProjEq->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0h");
  skymapProjEq->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"22h");
  skymapProjEq->GetXaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"20h");
  skymapProjEq->GetXaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"18h");
  skymapProjEq->GetXaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"16h");
  skymapProjEq->GetXaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"14h");
  skymapProjEq->GetXaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"12h");
  skymapProjEq->Draw("z aitoff");
  pad3->Draw();
  
  galMarksEq[0]->SetX(84.7649); //galactic center
  galMarksEq[0]->SetY(-32.1444);
  
  galMarksEq[1]->SetX(-77.0868); //TXS
  galMarksEq[1]->SetY(6.1484);
  
  galMarksEq[2]->SetX(86.8957); //MRK 501
  galMarksEq[2]->SetY(45.1052);
	
  galMarksEq[3]->SetX(-132.496); //MRK 421
  galMarksEq[3]->SetY(52.534);
  
  galMarksEq[4]->SetX(161.754); //Virgo
  galMarksEq[4]->SetY(-9.72109);
  
  galMarksEq[5]->SetX(49.3004); //cygnus A
  galMarksEq[5]->SetY(42.3715);
  
  galMarksEq[6]->SetX(119.23); // Cen A
  galMarksEq[6]->SetY(-56.6101);
  
  galMarksEq[7]->SetX(-92.1939); //Auger Dipole
  galMarksEq[7]->SetY(-29.1072);
  
  galMarksEq[8]->SetX(-44.1018); //Fornax
  galMarksEq[8]->SetY(-34.7217);
  
  galMarksEq[9]->SetX(-102.613); //TA Hotspot
  galMarksEq[9]->SetY(52.2853);
  
  galMarksEq[10]->SetX(-40.6698); //NGC
  galMarksEq[10]->SetY(-0.0135745);
  
  labelsEq[0]->SetText(galMarksEq[0]->GetX(), galMarksEq[0]->GetY() -5, "Galactic Center");
  labelsEq[1]->SetText(galMarksEq[1]->GetX(), galMarksEq[1]->GetY() -5, "TXS 0506+056");
  labelsEq[2]->SetText(galMarksEq[2]->GetX(), galMarksEq[2]->GetY() -5, "MRK 501");
  labelsEq[3]->SetText(galMarksEq[3]->GetX(), galMarksEq[3]->GetY() +5, "MRK 421");
  labelsEq[4]->SetText(galMarksEq[4]->GetX(), galMarksEq[4]->GetY() -5, "Virgo");
  labelsEq[5]->SetText(galMarksEq[5]->GetX(), galMarksEq[5]->GetY() -5, "Cygnus A");
  labelsEq[6]->SetText(galMarksEq[6]->GetX(), galMarksEq[6]->GetY() -5, "Cen A");
  labelsEq[7]->SetText(galMarksEq[7]->GetX(), galMarksEq[7]->GetY() -5, "Auger Dipole");
  labelsEq[8]->SetText(galMarksEq[8]->GetX(), galMarksEq[8]->GetY() -5, "Fornax");
  labelsEq[9]->SetText(galMarksEq[9]->GetX(), galMarksEq[9]->GetY() -5, "TA Hotspot");
  labelsEq[10]->SetText(galMarksEq[10]->GetX(), galMarksEq[10]->GetY() -5, "NGC 1068");
  
  pad3->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nMarks; i++) {
    galMarksEq[i]->Draw();
    labelsEq[i]->Draw();
  }
  
  TCanvas *skyTimeExpC = new TCanvas("skyTimeExpC","Skymap Time Exposure",1500,750);
  skyTimeExpC->cd(1);
  skyTimeExpC->SetRightMargin(0.2);
  
  skymapTimeExp->GetXaxis()->SetTitle("Right Ascension [hours]");
  skymapTimeExp->GetYaxis()->SetTitle("Declination [deg]");
  skymapTimeExp->GetZaxis()->SetTitle("Hours of Exposure");
  skymapTimeExp->GetXaxis()->SetNdivisions(-512);
  skymapTimeExp->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"12h");
  skymapTimeExp->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"10h");
  skymapTimeExp->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"8h");
  skymapTimeExp->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"6h");
  skymapTimeExp->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"4h");
  skymapTimeExp->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"2h");
  skymapTimeExp->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0h");
  skymapTimeExp->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"22h");
  skymapTimeExp->GetXaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"20h");
  skymapTimeExp->GetXaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"18h");
  skymapTimeExp->GetXaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"16h");
  skymapTimeExp->GetXaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"14h");
  skymapTimeExp->GetXaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"12h");
  skymapTimeExp->Draw("z aitoff");
  pad4->Draw();
  
  galMarksEqT[0]->SetX(84.7649); //galactic center
  galMarksEqT[0]->SetY(-32.1444);
  
  galMarksEqT[1]->SetX(-77.0868); //TXS
  galMarksEqT[1]->SetY(6.1484);
  
  galMarksEqT[2]->SetX(86.8957); //MRK 501
  galMarksEqT[2]->SetY(45.1052);
  
  galMarksEqT[3]->SetX(-132.496); //MRK 421
  galMarksEqT[3]->SetY(52.534);
  
  galMarksEqT[4]->SetX(161.754); //Virgo
  galMarksEqT[4]->SetY(-9.72109);
  
  galMarksEqT[5]->SetX(49.3004); //cygnus A
  galMarksEqT[5]->SetY(42.3715);
  
  galMarksEqT[6]->SetX(119.23); // Cen A
  galMarksEqT[6]->SetY(-56.6101);
  
  galMarksEqT[7]->SetX(-92.1939); //Auger Dipole
  galMarksEqT[7]->SetY(-29.1072);
  
  galMarksEqT[8]->SetX(-44.1018); //Fornax
  galMarksEqT[8]->SetY(-34.7217);
  
  galMarksEqT[9]->SetX(-102.613); //TA Hotspot
  galMarksEqT[9]->SetY(52.2853);
  
  galMarksEqT[10]->SetX(-40.6698); //NGC 1068
  galMarksEqT[10]->SetY(-0.0135745);
  
  labelsEqT[0]->SetText(galMarksEq[0]->GetX(), galMarksEq[0]->GetY() -5, "Galactic Center");
  labelsEqT[1]->SetText(galMarksEq[1]->GetX(), galMarksEq[1]->GetY() -5, "TXS 0506+056");
  labelsEqT[2]->SetText(galMarksEq[2]->GetX(), galMarksEq[2]->GetY() -5, "MRK 501");
  labelsEqT[3]->SetText(galMarksEq[3]->GetX(), galMarksEq[3]->GetY() +5, "MRK 421");
  labelsEqT[4]->SetText(galMarksEq[4]->GetX(), galMarksEq[4]->GetY() -5, "Virgo");
  labelsEqT[5]->SetText(galMarksEq[5]->GetX(), galMarksEq[5]->GetY() -5, "Cygnus A");
  labelsEqT[6]->SetText(galMarksEq[6]->GetX(), galMarksEq[6]->GetY() -5, "Cen A");
  labelsEqT[7]->SetText(galMarksEq[7]->GetX(), galMarksEq[7]->GetY() -5, "Auger Dipole");
  labelsEqT[8]->SetText(galMarksEq[8]->GetX(), galMarksEq[8]->GetY() -5, "Fornax");
  labelsEqT[9]->SetText(galMarksEq[9]->GetX(), galMarksEq[9]->GetY() -5, "TA Hotspot");
  labelsEqT[10]->SetText(galMarksEq[10]->GetX(), galMarksEq[10]->GetY() -5, "NGC 1068");
  
  pad4->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nMarks; i++) {
    galMarksEq[i]->Draw();
    labelsEq[i]->Draw();
  }
  
  int nPoints = 5;
  TCanvas *tele1 = new TCanvas("tele1","tele", 1600, 750);
  TPad *paddie = new TPad("paddie","",0,0,1,1);
  TMarker *srcMrks[nPoints];
  
  for(int i = 0; i < nPoints; i++)
    {
      srcMrks[i] = new TMarker(0.,0., 43);
      srcMrks[i]->SetMarkerSize(2.5);
    }
  
  paddie->SetFillStyle(4000);
  paddie->SetFillColor(0);
  paddie->SetBorderSize(0);
  paddie->SetFrameBorderMode(0);
  paddie->SetFrameLineColor(0); 
  paddie->SetFrameBorderMode(0);
  paddie->Range(-231,-111.875,283,111.875);
  
  tele1->cd(1);
  tele1->SetRightMargin(0.2);
  
  sensEq->GetXaxis()->SetTitle("Right Ascension [hours]");
  sensEq->GetYaxis()->SetTitle("Declination [deg]");
  sensEq->GetZaxis()->SetTitle("Sensitivity [GeV^{-1} cm^{-2} s^{-1}]");
  sensEq->GetXaxis()->SetNdivisions(-512);
  sensEq->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"12h");
  sensEq->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"10h");
  sensEq->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"8h");
  sensEq->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"6h");
  sensEq->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"4h");
  sensEq->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"2h");
  sensEq->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0h");
  sensEq->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"22h");
  sensEq->GetXaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"20h");
  sensEq->GetXaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"18h");
  sensEq->GetXaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"16h");
  sensEq->GetXaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"14h");
  sensEq->GetXaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"12h");
  
  sensEq->GetXaxis()->SetTickSize(0);
  sensEq->GetYaxis()->SetTickSize(0);
  sensEq->GetYaxis()->SetNdivisions(12,2,2, kFALSE);
  sensEq->GetYaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"-90");
  sensEq->GetYaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"-75");
  sensEq->GetYaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"-60");
  sensEq->GetYaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"-45");
  sensEq->GetYaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"-30");
  sensEq->GetYaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"-15");
  sensEq->GetYaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0");
  sensEq->GetYaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"15");
  sensEq->GetYaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"30");
  sensEq->GetYaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"45");
  sensEq->GetYaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"60");
  sensEq->GetYaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"75");
  sensEq->GetYaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"90");
  
  sensEq->GetZaxis()->SetRangeUser(sensEq->GetMinimum(0), sensEq->GetMaximum());
  
  srcMrks[0]->SetX(77.0967); 
  srcMrks[0]->SetY(-62.0723);
  
  srcMrks[1]->SetX(-23.7132);
  srcMrks[1]->SetY(49.0101);
  
  srcMrks[2]->SetX(-66.866); 
  srcMrks[2]->SetY(22.9632);
  
  srcMrks[3]->SetX(-91.0507); 
  srcMrks[3]->SetY(-4.75758);
  
  srcMrks[4]->SetX(-102.639); 
  srcMrks[4]->SetY(-35.3881);
  
  srcMrks[0]->SetMarkerColor(1);
  srcMrks[4]->SetMarkerColor(2);
  srcMrks[3]->SetMarkerColor(3);
  srcMrks[2]->SetMarkerColor(4);
  srcMrks[1]->SetMarkerColor(6);
  
  gPad->SetLogz(1);
  sensEq->Draw("z aitoff");
  paddie->Draw();
  paddie->cd();
  
  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nPoints; i++)
    {
      srcMrks[i]->Draw();
    }
  
  TCanvas *tele2 = new TCanvas("tele2", "tele1", 1600, 750);
  TPad *paddie1 = (TPad*)paddie->Clone("paddie1");
  
  tele2->cd(1);
  tele2->SetRightMargin(0.2);
  
  sensEqI->GetXaxis()->SetTitle("Right Ascension [hours]");
  sensEqI->GetYaxis()->SetTitle("Declination [deg]");
  sensEqI->GetZaxis()->SetTitle("Integral Sensitivity [cm^{-2} s^{-1}]");
  sensEqI->SetTitle("Integral Sensitivity after 1 year of observation");
  sensEqI->GetXaxis()->SetNdivisions(-512);
  sensEqI->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"12h");
  sensEqI->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"10h");
  sensEqI->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"8h");
  sensEqI->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"6h");
  sensEqI->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"4h");
  sensEqI->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"2h");
  sensEqI->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0h");
  sensEqI->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"22h");
  sensEqI->GetXaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"20h");
  sensEqI->GetXaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"18h");
  sensEqI->GetXaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"16h");
  sensEqI->GetXaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"14h");
  sensEqI->GetXaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"12h");
  
  sensEqI->GetXaxis()->SetTickSize(0);
  sensEqI->GetYaxis()->SetTickSize(0);
  sensEqI->GetYaxis()->SetNdivisions(12,2,2, kFALSE);
  sensEqI->GetYaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"-90");
  sensEqI->GetYaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"-75");
  sensEqI->GetYaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"-60");
  sensEqI->GetYaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"-45");
  sensEqI->GetYaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"-30");
  sensEqI->GetYaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"-15");
  sensEqI->GetYaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"0");
  sensEqI->GetYaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"15");
  sensEqI->GetYaxis()->ChangeLabel(9,-1,-1,-1,-1,-1,"30");
  sensEqI->GetYaxis()->ChangeLabel(10,-1,-1,-1,-1,-1,"45");
  sensEqI->GetYaxis()->ChangeLabel(11,-1,-1,-1,-1,-1,"60");
  sensEqI->GetYaxis()->ChangeLabel(12,-1,-1,-1,-1,-1,"75");
  sensEqI->GetYaxis()->ChangeLabel(13,-1,-1,-1,-1,-1,"90");

  sensEqI->GetZaxis()->SetRangeUser(sensEqI->GetMinimum(0), sensEqI->GetMaximum());

  gPad->SetLogz(1);
  sensEqI->Draw("z aitoff");
  paddie1->Draw();
  paddie1->cd();

  for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
  for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
  
  for(int i = 0; i < nPoints; i++)
    {
      srcMrks[i]->Draw();
    }
  
  //color formatting
  const Int_t Number = 9;
  Double_t Red[Number]    = { 242./255., 234./255., 237./255., 230./255., 212./255., 156./255., 99./255., 45./255., 0./255.};
  Double_t Green[Number]  = { 243./255., 238./255., 238./255., 168./255., 101./255.,  45./255.,  0./255.,  0./255., 0./255.};
  Double_t Blue[Number]   = { 230./255.,  95./255.,  11./255.,   8./255.,   9./255.,   3./255.,  1./255.,  1./255., 0./255.};
  Double_t Length[Number] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000};
  Int_t nb=99;
  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
  skymapProjEq->SetContour(nb);
  skymapProjSuperGal->SetContour(nb);
  skymapFullProjection->SetContour(nb);
  skymapInstantConverage->SetContour(nb);
  skymapTimeExp->SetContour(nb);
  sensEq->SetContour(nb);
  sensEqI->SetContour(nb);
  
  TString Filename;
  Filename.Form("eq_teleDir%0.1f.root",teleDirection);
  TFile *f = new TFile(Filename.Data(),"RECREATE");
  skymapProjEq->Write();
  f->Close();
  Filename.Form("gal_teleDir%0.1f.root",teleDirection);
  TFile *ff = new TFile(Filename.Data(),"RECREATE");
  skymapFullProjection->Write();
  ff->Close();
  Filename.Form("supgal_teleDir%0.1f.root",teleDirection);
  TFile *fff = new TFile(Filename.Data(),"RECREATE");
  skymapProjSuperGal->Write();
  fff->Close();
  Filename.Form("insteq_teleDir%0.1f.root",teleDirection);
  TFile *ffff = new TFile(Filename.Data(),"RECREATE");
  skyProjInstantEq->Write();
  ffff->Close();
  
  //Printing the specific acceptances and number of neutrino events of different galactic landmarks (commented out is for cross-checking acceptances over different coordinate systems to maintain consistancy)
  //~ cout<<"Equatorial:"<<endl;
  cout<<"Galacitc Center Acceptance: "<<skymapProjEq->GetBinContent((int)(93.5949 + 181), (int)(-28.9362 + 91))<<endl; 
  cout<<"TXS 0506+056 Acceptance: "<<skymapProjEq->GetBinContent((int)(-77.3581 + 181), (int)(5.69315 + 91))<<endl; 
  cout<<"MRK 501 Acceptance: "<<skymapProjEq->GetBinContent((int)(106.532 + 181), (int)(39.7602 + 91))<<endl; 
  cout<<"MRK 421 Acceptance: "<<skymapProjEq->GetBinContent((int)(-166.114 + 181), (int)(38.2089 + 91))<<endl; 
  cout<<"Virgo Acceptance: "<<skymapProjEq->GetBinContent((int)(162.797 + 181), (int)(-6.77748 + 91))<<endl; 
  cout<<"Cygnus A Acceptance: "<<skymapProjEq->GetBinContent((int)(60.1317 + 181), (int)(40.7339 + 91))<<endl; 
  cout<<"Cen A Acceptance: "<<skymapProjEq->GetBinContent((int)(158.635 + 181), (int)(-43.0192 + 91))<<endl; 
  cout<<"Auger Dipole Acceptance: "<<skymapProjEq->GetBinContent((int)(-99.7352 + 181), (int)(-25.7697 + 91))<<endl; 
  cout<<"Fornax Acceptance: "<<skymapProjEq->GetBinContent((int)(-50.1778 + 181), (int)(-33.73 + 91))<<endl; 
  cout<<"TA Hotspot Acceptance: "<<skymapProjEq->GetBinContent((int)(-133.503 + 181), (int)(43.1166 + 91))<<endl; 
  cout<<"NGC 1068 Acceptance: "<<skymapProjEq->GetBinContent((int)(-40.6698 + 181), (int)(-0.0132913 + 91))<<endl; 
  //~ cout<<"Supergalactic:"<<endl;
  //~ cout<<"Galacitc Center Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(174.214 + 181), (int)(42.3103 + 91))<<endl; 
  //~ cout<<"TXS 0506+056 Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(26.2644 + 181), (int)(-56.2196 + 91))<<endl; 
  //~ cout<<"MRK 501 Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-68.0962 + 181), (int)(54.3094 + 91))<<endl; 
  //~ cout<<"MRK 421 Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-71.5313 + 181), (int)(-10.5706 + 91))<<endl; 
  //~ cout<<"Virgo Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-123.91 + 181), (int)(1.46442 + 91))<<endl; 
  //~ cout<<"Cygnus A Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-0.443399 + 181), (int)(61.338 + 91))<<endl; 
  //~ cout<<"Cen A Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-159.753 + 181), (int)(-5.24987 + 91))<<endl; 
  //~ cout<<"Auger Dipole Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(133.749 + 181), (int)(-79.2626 + 91))<<endl; 
  //~ cout<<"Fornax Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(94.4711 + 181), (int)(-38.7337 + 91))<<endl; 
  //~ cout<<"TA Hotspot Acceptance: "<<skymapProjSuperGal->GetBinContent((int)(-50.0381 + 181), (int)(-25.1529 + 91))<<endl;
  //~ cout<<"Galactic:"<<endl;
  //~ cout<<"Galacitc Center Acceptance: "<<skymapFullProjection->GetBinContent((int)(-0 + 181), (int)(0 + 91))<<endl; 
  //~ cout<<"TXS 0506+056 Acceptance: "<<skymapFullProjection->GetBinContent((int)(164.595 + 181), (int)(-19.636 + 91))<<endl; 
  //~ cout<<"MRK 501 Acceptance: "<<skymapFullProjection->GetBinContent((int)(-63.6 + 181), (int)(38.8592 + 91))<<endl; 
  //~ cout<<"MRK 421 Acceptance: "<<skymapFullProjection->GetBinContent((int)(-179.832 + 181), (int)(65.0315 + 91))<<endl; 
  //~ cout<<"Virgo Acceptance: "<<skymapFullProjection->GetBinContent((int)(49.3718 + 181), (int)(55.8344 + 91))<<endl; 
  //~ cout<<"Cygnus A Acceptance: "<<skymapFullProjection->GetBinContent((int)(-76.1899 + 181), (int)(5.75539 + 91))<<endl; 
  //~ cout<<"Cen A Acceptance: "<<skymapFullProjection->GetBinContent((int)(50.4841 + 181), (int)(19.4173 + 91))<<endl; 
  //~ cout<<"Auger Dipole Acceptance: "<<skymapFullProjection->GetBinContent((int)(125 + 181), (int)(-14 + 91))<<endl; 
  //~ cout<<"Fornax Acceptance: "<<skymapFullProjection->GetBinContent((int)(126.161 + 181), (int)(-57.335 + 91))<<endl; 
  //~ cout<<"TA Hotspot Acceptance: "<<skymapFullProjection->GetBinContent((int)(-178 + 181), (int)(40 + 91))<<endl; 
  
  //printing the number of neutrino events in each source of interest
  cout<<"Galacitc Center Events: "<<nuevents->GetBinContent((int)(93.5949 + 181), (int)(-28.9362 + 91))<<endl; 
  cout<<"TXS 0506+056 Events: "<<nuevents->GetBinContent((int)(-77.3581 + 181), (int)(5.69315 + 91))<<endl; 
  cout<<"MRK 501 Events: "<<nuevents->GetBinContent((int)(106.532 + 181), (int)(39.7602 + 91))<<endl; 
  cout<<"MRK 421 Events: "<<nuevents->GetBinContent((int)(-166.114 + 181), (int)(38.2089 + 91))<<endl; 
  cout<<"Virgo Events: "<<nuevents->GetBinContent((int)(162.797 + 181), (int)(-6.77748 + 91))<<endl; 
  cout<<"Cygnus A Events: "<<nuevents->GetBinContent((int)(60.1317 + 181), (int)(40.7339 + 91))<<endl; 
  cout<<"Cen A Events: "<<nuevents->GetBinContent((int)(158.635 + 181), (int)(-43.0192 + 91))<<endl; 
  cout<<"Auger Dipole Events: "<<nuevents->GetBinContent((int)(-99.7352 + 181), (int)(-25.7697 + 91))<<endl; 
  cout<<"Fornax Events: "<<nuevents->GetBinContent((int)(-50.1778 + 181), (int)(-33.73 + 91))<<endl; 
  cout<<"TA Hotspot Events: "<<nuevents->GetBinContent((int)(-133.503 + 181), (int)(43.1166 + 91))<<endl;
  cout<<"NGC 1068 Events: "<<nuevents->GetBinContent((int)(-40.6698 + 181), (int)(-0.0132913 + 91))<<endl;
  
  cout<<"Sensitivity Max Bin: "<<skymapProjEq->GetMaximumBin()<<", Sensitivity Max: "<<1/skymapProjEq->GetBinContent(skymapProjEq->GetMaximumBin())<<endl;
  
  //~ int nPoints = 5;
  Double_t srcRA[nPoints];
  Double_t srcDec[nPoints];
  
  srcDec[0] = -0.013;
  srcDec[1] = 5.69315;
  srcDec[2] = -53;
  srcDec[3] = -29;
  srcDec[4] = 20;
  
  srcRA[0] = 40.6698; 
  srcRA[1] = 77.3581;
  srcRA[2] = 111;
  srcRA[3] = -107;
  srcRA[4] = -64;
  
  for(int i = 0; i < nPoints; i++)
    {
      cout<<"RA: "<<srcRA[i]<<" Dec: "<<srcDec[i]<<" Acceptance: "<<skymapProjEq->GetBinContent((int)(srcRA[i] + 181), (int)(srcDec[i] + 91))<<endl;
    }
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyC.pdf";
    rootPath = outputFolder  + "/skyC.root";
    skyC->SaveAs(pdfPath.c_str());
    skyC->SaveAs(rootPath.c_str());
   

    
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/test3.pdf";
    rootPath = outputFolder  + "/test3.root";
    test3->SaveAs(pdfPath.c_str());
    test3->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/tele1.pdf";
    rootPath = outputFolder  + "/tele1.root";
    tele1->SaveAs(pdfPath.c_str());
    tele1->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/tele2.pdf";
    rootPath = outputFolder  + "/tele2.root";
    tele2->SaveAs(pdfPath.c_str());
    tele2->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyTimeExpC.pdf";
    rootPath = outputFolder  + "/skyTimeExpC.root";
    skyTimeExpC->SaveAs(pdfPath.c_str());
    skyTimeExpC->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyProjCEq.pdf";
    rootPath = outputFolder  + "/skyProjCEq.root";
    skyProjCEq->SaveAs(pdfPath.c_str());
    skyProjCEq->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyProjCSuper.pdf";
    rootPath = outputFolder  + "/skyProjCSuper.root";
    skyProjCSuper->SaveAs(pdfPath.c_str());
    skyProjCSuper->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyProjC.pdf";
    rootPath = outputFolder  + "/skyProjC.root";
    skyProjC->SaveAs(pdfPath.c_str());
    skyProjC->SaveAs(rootPath.c_str());
    
    // Save the canvas as a .pdf and .root file inside the folder
    pdfPath = outputFolder  + "/skyProjInstantEq.pdf";
    rootPath = outputFolder  + "/skyProjInstantEq.root";
    skyProjInstantEq->SaveAs(pdfPath.c_str());
    skyProjInstantEq->SaveAs(rootPath.c_str());
    
    
    
}

////////////////////////////////////////////////////////////////////
// Main Program
//
//////////////////////////////////////////////////////////////////
int main (int argc, char **argv) {


   // Get the current date and time for the folder name
    std::string timestamp = getCurrentDateTime();
    outputFolder = "output_skymaps_" + timestamp;

    // Create the folder
    createDirectory(outputFolder);

  //initiate root
  TROOT root("DisplayEvts","Display Results");
  TApplication *theApp = new TApplication("App",&argc,argv);
  gROOT->ProcessLine("#include <vector>"); //need this otherwise we cannot save vectors in the root file

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Skyplots start here
  //
  PlotAcceptanceSkymaps();
 

 cout<<"done"<<endl;
 theApp->Run();
 
 return 0;
}
