/* \file TrinitySimUtilities.cpp
   Implementation of the Utilities to simulate Trinity's sensitivity and all the other good stuff
*/

#include "TrinitySimUtilities.h"

using namespace std;

//---------------------------------------------------------------------------------------
//Constructor
TrinitySimUtilities::TrinitySimUtilities()
{
  cout<<"Initializing the Trinity Simulation Utilities"<<endl;

  iConfig = 2;

  NumSites = 3; //number of trinity sites

  // Trigger Timing information
  iTrigWin = 5; // index 5 means all PEs are counted regardless of time

  dExposureHours = 50;
  dLogEnergyStep = 1;
  dHalfEnergyBinWidth = 0.5;
  dMinEnu = 6;
  dMaxEnu = 10;
  nuIndex = 2; //power law index of the neutrino spectrum the minus sign is added later

  dST = 10; //km max height of shower tip above ground;
  yMin = 10;
  yMax = 250;
  yDelta = 20;
  MaxElevation = 15; //elevation angle (determines path through Earth)
  dMaxAzimuthSky = 10; //max limit for azimuth evaluation 
  DeltaAngleAz = 0.1; //steps in azimuth in the camera FOV
  DeltaAngleSky = 0.1; //this is the step size in elevation and azimuth in the sky to calculate the acceptance for a limited FOV telescope. Make sure it is smaller than then FOV of the camera  
  dMaxCherenkovAzimuthAngle = 20.0; //maximum azimuth angle for cherenkov 
  dMaxFluorescenceDistance = 70;

  //next three parameters are key to the instrument
  //plus the telescop height above ground which can be selected above
  blimFOV = kFALSE;
  tanFoV = tan(2/180.*pi); //Field of view of telescope above the horizon
  dFoVBelow = 3/180.*pi; //Field of view of telescope  below horizon 
  hFOV = 5.; // in degrees
  dMinLength = 0.3; //mimnimum length a shower has to have in the camera, in degrees. This is a conservative estimate because it assumes that the shower starts at a distance l from the detector, which is not necessarily tru for showers with shallow elevation angles.
  dMinimumNumberPhotoelectrons = 10;  //this is pe per sqm mirror 4.8 for full trinity at threshold use 100pe/mirror effective area for analysis threshold 

  bFluorescence = kFALSE;
  bCombined = kFALSE;
  bMonoNu = kFALSE; //simulate monoenergetic neutrinos, only good for acceptance calculation. For all other simulations set it to kFALSE
  //iNumTimesOutOfAnalogueTraceUpperEnd = 0;


  gROOT->ProcessLine("#include <vector>"); //need this otherwise we cannot save vectors in the root file
  gStyle->SetOptStat(0);

  //read tables to use NuTauSimResults	
  readFromTable() ;
  findAngleNumber() ;
  
  grsCC = new TGraph(19, Esig, sigma);
  grsNC = new TGraph(19, Esig, sigmaNC);


  hTriggeredAzimuthAngles = new TH1D("hTriggeredAzimuthAngles","",int(180/DeltaAngleSky),0,180);
  hTriggeredAzimuthAngles->GetXaxis()->SetTitle("angle #alpha [degrees]");
  hTriggeredAzimuthAngles->GetYaxis()->SetTitle("probability");
  hTriggeredAzimuthAngles->SetLineWidth(2);
  hTriggeredAzimuthAngles->SetLineColor(kBlue+3);
  
  hTau = new TH1D("hTau","",70,4,11);
  hTau->GetXaxis()->SetTitle("energy [GeV]");
  hTau->GetYaxis()->SetTitle("F_tau/F_nu");
  hTau->SetLineWidth(3);
  hTau->GetXaxis()->SetLabelSize(0.045);
  hTau->GetXaxis()->SetTitleOffset(1.5);
  hTau->GetYaxis()->SetTitleOffset(1.3);
  hTau->GetYaxis()->SetTitleSize(0.04);
  hTau->GetXaxis()->SetTitleSize(0.04);
  hTau->GetYaxis()->SetLabelSize(0.045);
  
  TAxis *axis = hTau->GetXaxis();
  int bins = axis->GetNbins();
  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins + 1];
  for (int i = 0; i <= bins; i++) {
    new_bins[i] = TMath::Power(10, from + i * width);
  }
  axis->Set(bins, new_bins);
  
}

void TrinitySimUtilities::DumpConfiguration()
{
cout<<"Simulation Configuration"<<endl<<endl;

cout<<"Trinity sites simulated: "<<NumSites<<endl;

cout<<"Detector altitude above ground: "<<DetectorAltitude[iConfig]<<" km"<<endl;

if(iTrigWin == 5)
  cout<<"No trigger window, all photons are accepted"<<endl;
else
  cout<<"Photons accounted for must arrive within "<<TriggerWindow[iTrigWin]<<" ns"<<endl;

if(blimFOV)
  cout<<"Simulate detector with limited horizontal FoV of: "<<hFOV<<" degrees"<<endl;
else
  cout<<"Simulate detector with unlimited horizontal FoV"<<endl;
cout<<"FoV above the horizon: "<<atan(tanFoV)*180/pi<<" degrees"<<endl; 
cout<<"FoV below the horizon: "<<dFoVBelow*180./pi<<" degrees"<<endl; 
cout<<"The minimum shower length: "<<dMinLength<<" degrees"<<endl; 
cout<<"The minimum number of photoelectrons per sqr meter mirror: "<<dMinimumNumberPhotoelectrons<<" photoelectrons"<<endl;
cout<<"Total mirror area = number of photoelectrons in the shower / minimum number of photoelectrons per sqr meter mirror = 100/ "<<dMinimumNumberPhotoelectrons<<" = "<<100/dMinimumNumberPhotoelectrons<<" sq meters"<<endl;

cout<<"Exposure time for point source sensitivity: "<<dExposureHours<<" hours"<<endl;
cout<<"Neutrino energy step size on log scale: "<<dLogEnergyStep<<" [ ] "<<endl;
cout<<"Sensitivities are integrated over log bins of width: "<<dHalfEnergyBinWidth*2<<endl;
cout<<"Scans over energy are done from "<<dMinEnu<<" to "<<dMaxEnu<<" log10(GeV) "<<endl;
cout<<"Power-law neutrino spectrum spectral index: -"<<nuIndex<<endl; 
if(bMonoNu)
cout<<"Only monoenergetic neutrinos are simulated: only good for acceptance calculation in compliance with neutrino community"<<endl;
else
cout<<"integrate energy bins (do not assume mono-energetic neutrinos)"<<endl<<endl;

cout<<"The tip of the shower can not be more than "<<dST<<" km above the ground"<<endl;
cout<<"Scans in distance from the telescope start at "<<yMin<<" km and end at "<<yMax<<" km in steps of "<<yDelta<<" km"<<endl<<endl;

if(bFluorescence || bCombined)
cout<<"Fluorescence emission is taken into account to "<<dMaxFluorescenceDistance<<" km"<<endl;
if(bCombined)
cout<<"Fluorescence and Cherenkov emission is taken into account"<<endl;
else
cout<<"Only Cherenkov emission is taken into account"<<endl;
if(bFluorescence)
cout<<"Only Fluorescence is taken into account"<<endl<<endl;

cout<<"Elevation is scanned from the horizon down to -"<<MaxElevation<<" degrees below the horizon"<<endl;
cout<<"Largest azimuth angle of emerging tau: "<<dMaxAzimuthSky<<" degrees (only used for point source acceptance with limited FoV telescopes)"<<endl; 
cout<<"Azimuth and elevation scan step size: "<<DeltaAngleSky<<" degrees"<<endl; 
cout<<"Camera FoV scan step size: "<<DeltaAngleAz<<" degrees"<<endl;
cout<<"For 360deg FoV and diffuse flux sensitivities, the azimuth is scanned to the maximum Cherenkov angle: "<<dMaxCherenkovAzimuthAngle<<" degrees"<<endl<<endl; 


}

void TrinitySimUtilities::ConfigureDemonstrator()
{
  cout<<"Setting up for the Demonstrator"<<endl;

  iConfig = 1;  //1km above ground
  iTrigWin = 5; // index 5 means all PEs are counted regardless of time
  yMin = 10;
  yMax = 250;
  yDelta = 10;
  MaxElevation = 11; //elevation angle (determines path through Earth)
  dMaxAzimuthSky = 6; //max limit for azimuth evaluation 
  DeltaAngleAz = 0.1; //steps in azimuth in the camera FOV
  DeltaAngleSky = 0.1; //this is the step size in elevation and azimuth in the sky to calculate the acceptance for a limited FOV telescope. Make sure it is smaller than then FOV of the camera  
  dMaxCherenkovAzimuthAngle = 20.0; //maximum azimuth angle for cherenkov 
  dMaxFluorescenceDistance = 70;

  //next three parameters are key to the instrument
  //plus the telescop height above ground which can be selected above
  blimFOV = kTRUE; //limited FOV
  tanFoV = tan(1.4/180.*pi); //Field of view of telescope above the horizon
  dFoVBelow = 2.4/180.*pi; //Field of view of telescope  below horizon 
  hFOV = 3.8; // in degrees
  dMinLength = 0.3; //mimnimum length a shower has to have in the camera, in degrees. This is a conservative estimate because it assumes that the shower starts at a distance l from the detector, which is not necessarily tru for showers with shallow elevation angles.
  dMinimumNumberPhotoelectrons = 100;  //this is pe per sqm mirror 



}

////////////////////////////////////////////////////////////////////////////////////
// A Function
string TrinitySimUtilities::Hold()
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

Double_t TrinitySimUtilities::myPEfunction(Double_t *x, Double_t *par)
{
   //par[0] Distance to where tau comes out in km
   //par[1] Elevation in rad
   //azimuth angle is our x
   Float_t xx =x[0]; //angle is rad here 
   //if angle is larger 40 degrees return 0
   if(xx>0.69813170)
     return 0;

   //Calculate azimuth angle in frame of master pe distribution (50km, 0ele,
   //0altitude
   Double_t dTelAngle = atan(DetectorAltitude[iConfig]*1e-3/par[0]);
   Double_t dAngle = sqrt(xx*xx + (par[1]-dTelAngle)*(par[1]-dTelAngle))*57.295780; //in deg
   //calculate how many PEs / per m2 per GeV
   Double_t f = 0;
   if(dAngle<1.3)
     f = parPEF[0]*exp(-1.1/parPEF[1])+parPEF[2]*exp(-1.1/(parPEF[3]+1.1*parPEF[4]));
   else
     f = parPEF[0]*exp(-dAngle/parPEF[1])+parPEF[2]*exp(-dAngle/(parPEF[3]+dAngle*parPEF[4]));

//other parameters to get PE intensity for different distance, azimuth and
//elevation
   //scale PE distribution to first PE distribution at 50km distance
   f*=   scalefirst[iConfig];
   //Get elevation dependence
   f*= (2-exp(-par[1]/eleScaling[iConfig]));
   //Get Distance dependence
   f*=  exp(-(par[0]-55)/
           (absorptionlength[iConfig]+(par[0]-55)*lincorr[iConfig])); //55km is the distance for which the normalized PE distribution is extracted

   return f;
}


/*
 Modeled after Nepomuk's PE function. This parameterization includes trigger timing using a 15,30,45 ns trigger window around the median of the timing distribution. This function is only valid for 2km and uses two master distributions. One taken at 45 km and 85km. 
*/ 
Double_t TrinitySimUtilities::myPEfunction_trigwindow(Double_t *x, Double_t *par)
{
  // par[0] is distance 
  Double_t l = par[0];
  // par[1] is elevation 
  Double_t elv = par[1];
  // par[2] is trig time window index
  Int_t ti = par[2];
  // x is azimuth angle in rad
  Double_t azi = x[0];
  
  //cout << par[0]<< " " << par[1]<< " " << par[2] << endl;
  //cout << "ti" << ti << "TW: " << TriggerWindow[ti] << " Fit Parameters: " << parPEF2[ti][0] << endl;
  
  if (azi > 0.69813170) // if > 40 degrees then return 0
    return 0; 
  
  
  Double_t dTelAngle = atan(DetectorAltitude[iConfig]*1e-3/l);
  Double_t dAngle = sqrt(azi*azi + (elv-dTelAngle)*(elv-dTelAngle))*57.295780; //in deg
  //calculate how many PEs / per m2 per GeV
  Double_t f = 0;

  //Calculate azimuth angle in frame of master pe distribution (40km, 0ele,2km altitude)
  // if l < 50 km
  if (l < 50){
    if(dAngle<1.3)
      f = parPEF2[ti][0]*exp(-1.1/parPEF2[ti][1])+parPEF2[ti][2]*exp(-1.1/(parPEF2[ti][3]+1.1*parPEF2[ti][4]));
    else
      f = parPEF2[ti][0]*exp(-(dAngle+0.19*azi)/parPEF2[ti][1])+parPEF2[ti][2]*exp(-(dAngle+0.19*azi)/(parPEF2[ti][3]+(dAngle+0.19*azi)*parPEF2[ti][4]));
    
    //scale PE distribution to first PE distribution at 50km distance
    f*=  3.3*scalefirst2[ti]; // Scale is 1 at master distribution height
    //Get elevation dependence
    f*= (2-exp(-elv));
    //Get Distance dependence
    f*=  exp(-(l-45)/
	     (absorptionlength2[ti]+(l-45)*lincorr2[ti])); //40km is the distance for which the normalized PE distribution is extracted
  } else {
    //Calculate azimuth angle in frame of master pe distribution (80km, 0ele,2km altitude)
    // if >= 50 km
    if(dAngle<1.3)
      f = parPEF3[ti][0]*exp(-1.1/parPEF3[ti][1])+parPEF3[ti][2]*exp(-1.1/(parPEF3[ti][3]+1.1*parPEF3[ti][4]));
    else
      f = parPEF3[ti][0]*exp(-dAngle/parPEF3[ti][1])+parPEF3[ti][2]*exp(-dAngle/(parPEF3[ti][3]+dAngle*parPEF3[ti][4]));
    
    //scale PE distribution to first PE distribution at 50km distance
    f*=   scalefirst3[ti]; // Scale is 1 at master distribution height
    //Get elevation dependence
    f*= (2-exp(-elv/eleScaling3[ti]));
    //Get Distance dependence
    f*=  exp(-(l-85)/
	     (absorptionlength3[ti]+(l-85)*lincorr3[ti])); //80km is the distance for which the normalized PE distribution is extracted
    }
  
  
  return f; // return number of photoelectrons arriving at camera in trigger window
}  

Double_t TrinitySimUtilities::DistanceThroughEarth(Double_t y, Double_t elevation, Double_t azimuth)
{

  elevation = elevation/180*pi; //elevation angle (determines path through Earth;
  azimuth = azimuth/180.*pi;  //azimuth angle

  Double_t l = y; //Distance from detector to where the tau comes out detector is always at z=0

  Double_t v = sqrt((REarth+DetectorAltitude[iConfig])*(REarth+DetectorAltitude[iConfig])-REarth*REarth);

  //shortest distance d between tau trajectory and detector
  Double_t nproj = y*sqrt( 1 + tan(azimuth)*tan(azimuth) ); //projection of trajectory to x-y plane
  Double_t denomsquared= y*tan(azimuth)*y*tan(azimuth) + y*y + nproj*nproj*tan(elevation)*tan(elevation)  ;
  
  //normalized trajectory vector of tau
  Double_t dNormalize = y/sqrt(denomsquared);
  //Double_t dNx = dNormalize * tan(azimuth);
  Double_t dNy = -dNormalize;
  Double_t dNz = dNormalize * sqrt( 1 + tan(azimuth)*tan(azimuth) ) * tan(elevation);


  Double_t p = 2 * ( REarth*dNz - (v-l)*dNy );
  Double_t q = (v-l)*(v-l);

  if(q-p*p/4>=0) //trajectory does not intersect with Earth
      return 0;
        
  Double_t i1 = p/2. - sqrt(p*p/4.-q);
  Double_t i2 = p/2. + sqrt(p*p/4.-q);

return fabs(i2-i1);

}

void TrinitySimUtilities::removeDuplicates()
{
	while(rem < enerNu.size()-1){
		if(enerNu[rem]==enerNu[rem+1]){
			enerNu.erase(enerNu.begin()+rem+1);
			removeDuplicates();
		}else if(enerNu[rem+1]==enerNu[rem+2]){
			rem ++;
			removeDuplicates();
		}
	}
}

void TrinitySimUtilities::readFromTable()
{
	ifstream ifs("table_with_e_05_a_1.txt") ;
	if(ifs.is_open()){
	ifs>>star;
		while(ifs.good()){
			ifs>>number;
			enerNu.push_back( pow(10,number-9.0) );
			ifs>>number;
			//angle.push_back(number);
			dist.push_back(cos((180-number)*pi/180)*2*REarth);
			for(int i=0;i<100;i++){
				ifs>>number;
				enerTau.push_back( pow(10,number-9.0) );
                                EtauNorm.push_back( pow(10,4+i*0.07) );
				ifs>>number;
				prob.push_back(number);
			}
			ifs>>number;
			enerTau.push_back( pow(10,number-9.0) );
                        EtauNorm.push_back( pow(10,4+100*0.07) );
			ifs>>star;
		}
		cout << "size of neutrino energy array: " << enerNu.size() << endl;
		removeDuplicates() ;
	}
}

void TrinitySimUtilities::findAngleNumber()
{
	for(unsigned i=1;i<enerNu.size();i++){
		if(dist[0]==dist[i]){
			distanceNumber = i;
			break;
		}else{
			continue ;
		}
	}
}


double TrinitySimUtilities::biLinearInterpolation(double a1,double n1,double q11,double a2,double n2,double q22,double x,double y){
	double q12 = (q11 + q22)/2 ;
	double q21 = q12 ;
	double p = (n2-y)/(n2-n1)*( (a2-x)/(a2-a1)*q11 + (x-a1)/(a2-a1)*q21 ) + (y-n1)/(n2-n1)*( (a2-x)/(a2-a1)*q12 + (x-a1)/(a2-a1)*q22 ) ;

return p ;
}

//Find index in lookuptable
int TrinitySimUtilities::FindLion(double dValue, vector<double> &vData,int iSize)
{
          int iWidth = iSize/2;
          int index = iWidth;
          //cout<<dValue<<": ";
          while(iWidth>1 && vData[index]!=dValue && index > 0  && index <iSize)
            {
              iWidth = iWidth/2+ iWidth%2;
              index = vData[index]<dValue ? index + iWidth :  index - iWidth;
              //cout<<index<<" "<<iWidth<<"  "<<vData[index]<<"; ";
            }
          while((dValue>vData[index] || index<0) && index < iSize)
             index++;
          index--;

          if(dValue==vData[index] || index>=iSize)
             index--;

          if(index<0)
            index=0; 
 
         //cout<<index<<endl;

return index;
}

//Calculates the probability of tau emergence using NuTauSim LUT
Double_t TrinitySimUtilities::PEtau(Double_t D,Double_t Etau, Double_t Enu) 
{
  //Enu = log10(Enu) + 9.0;
  //Etau = log10(Etau) + 9.0 ;
  //double zenithAngle = 180 - acos(D/2/REarth)/M_PI*180 ;
  //	if(zenithAngle >= angle[0] && zenithAngle <= angle[angleNumber-1] && Enu>=enerNu[0] && Enu <=enerNu[enerNu.size()-1]){
  if(D >= dist[0] && D <= dist[distanceNumber-1] && Enu>=enerNu[0] && Enu <=enerNu[enerNu.size()-1] && Etau>=enerTau[0] && Etau <=enerTau[100]){
    
    int indexEnu = FindLion(Enu,enerNu,enerNu.size());
    
    int indexDistance = FindLion(D,dist,distanceNumber);
    
    int indexEtau = FindLion(Etau,enerTau,100);
    
    int indexProb1 = indexEnu*distanceNumber*100+indexDistance*100+indexEtau;
    double p1 = prob[indexProb1] ;
    int indexProb2 = (indexEnu+1)*distanceNumber*100 + (indexDistance+1)*100 + indexEtau ;
    double p2 = prob[indexProb2] ;
    
    double Prob = biLinearInterpolation(dist[indexDistance],enerNu[indexEnu],p1,dist[indexDistance+1],enerNu[indexEnu+1],p2,D,Enu)/
      (EtauNorm[indexEtau+1]-EtauNorm[indexEtau]);
    //(pow(10,4+(indexEtau+1)*0.07)-pow(10,4+indexEtau*0.07));
    //cout<<Prob<<endl;
    return Prob ;
  }else{
    return 0 ;
  }
}

//Probability that Tau with Energy Etau emerges for initial nu energy Enu
//Thickness of matter d 
//Does not use energy loss of tau in Earth.
//Assumes the energy of the tau is 0.8*Enu
//Double_t PEtauNoTauEnergyLoss(Double_t D,Double_t Etau, Double_t Enu) 
//Double_t PEtauNoTauEnergyLoss(Double_t D,Double_t Etau, Double_t Enu) 
Double_t TrinitySimUtilities::PEtauNoTauEnergyLoss(Double_t D,Double_t Etau, Double_t Enu) 
{
  
  int n = hTau->FindBin(Etau);
  if(hTau->GetBinLowEdge(n)>0.8*Enu  || hTau->GetBinLowEdge(n+1)<0.8*Enu )
    return 0;
  
  Double_t sCC = grsCC->Eval(Enu); //crossection in pB
  Double_t sNC = grsNC->Eval(Enu); //crossection in pB
  Double_t rho = 2.65; //density in g/cm3
  Double_t NA = 6.022142e23;
  
  Double_t dInvConvCC = sCC*rho*NA*1e-31; // 1/km 1e-12*1e-28*1e4*1e5
  Double_t db = (sNC+sCC)*rho*NA*1e-31; // 1/km
  Double_t da = Mtau/(DecayTime*c*0.8*Enu); //1/km
  
  //cout<<dInvConvCC<<endl;
  //cout<<"neutrino interaction: "<<db<<" "<<exp(-1.0*db*D)<<endl;
  //cout<<"tau survival: "<<da<<"  "<<exp(-1.0*da*D)<<endl;
  
  Double_t Prob = dInvConvCC / (da-db) * ( exp(-1.0*db*D)-exp(-1.0*da*D) );  
  
  if(Prob<0)
    return 0;
  
  Prob /= (hTau->GetBinLowEdge(n+1)-hTau->GetBinLowEdge(n));
  
  return Prob;  
}

//Probability that Tau with Energy Etau emerges for initial nu energy Enu
//Thickness of matter d 
//follows description in Dutta 2005 in particular equation 28 with
//parameterization of beta in equation 13 case II
//energies in GeV distances in km at inptut
//Fails <1e8 GeV because energy loss (Beta) becoms <0
Double_t TrinitySimUtilities::PEtauDutta(Double_t D,Double_t Etau, Double_t Enu) 
{

if(Etau>0.8*Enu)
  return 0;

Double_t sCC = grsCC->Eval(Enu); //crossection in pB
Double_t sNC = grsNC->Eval(Enu); //crossection in pB
Double_t rho = 2.65; //density in g/cm3
Double_t NA = 6.022142e23;

Double_t dInvConvCC = sCC*rho*NA*1e-31; // 1/km 1e-12*1e-28*1e4*1e5
Double_t dInvConvtotal = (sNC+sCC)*rho*NA*1e-31; // 1/km
Double_t beta = 1.2e-6 + 0.16e-6 * log(Etau/1e10); //cm2/g Equation 13 case II in Dutta 
//cout<<"beta "<<beta<<endl;
Double_t prefactor = Mtau/(DecayTime*c*1e5*beta*rho*Etau); //dimensionless
//cout<<"Prefactor: "<<prefactor<<endl;

Double_t xDelta = log(Etau/(0.8*Enu))/(beta*rho)*1e-5+D; //where delta function is non zero; 1e-5 convert from cm to km 

if(xDelta<0)
  return 0;

//cout<<"beta: "<<beta<<" rho: "<<rho<<" Etau: "<<Etau<<endl;
Double_t Prob = 1.e-5/(beta*rho*Etau); //km/GeV
//cout<<"Prob: "<<Prob<<endl;
Double_t Pnu = dInvConvCC*exp(-xDelta*dInvConvtotal);  // 1/km
//cout<<"Pnu: "<<Pnu<<endl;
Prob *= Pnu; 
//cout<<"Prob2: "<<Prob<<endl;
if(Prob<0)
  return 0;

if(prefactor<0)
  return 0;
  
Double_t Ptau = exp(-prefactor*(1.0-exp(-beta*rho*(D-xDelta)*1e5))); //1e5 to convert from km to cm 
//cout<<"Ptau: "<<Ptau<<endl;
//cout<<1.0-exp(-beta*rho*(D-xDelta)*1e5)<<endl;
Prob *= Ptau;

return Prob;  
}


Double_t TrinitySimUtilities::PDecayFluorescence(Double_t Etau, Double_t y, Double_t elevation, Double_t azimuth, Bool_t blimFOV, Double_t dAzimuth)
{
  //cout<<endl<<"elevation: "<<elevation<<" azimuth: "<<azimuth<<" distance: "<<y<<endl;
  elevation = elevation/180*pi; //elevation angle (determines path through Earth;
  azimuth = azimuth/180.*pi;  //azimuth angle
  dAzimuth = dAzimuth/180.*pi; //azimuth walk

  Double_t l = y; //Distance from detector to where the tau comes out detector is always at z=0

  //shortest distance d between tau trajectory and detector
  Double_t nproj = y*sqrt( 1 + tan(abs(azimuth))*tan(abs(azimuth)) ); //projection of trajectory to x-y plane
  Double_t denomsquared= y*tan(abs(azimuth))*y*tan(abs(azimuth)) + y*y + nproj*nproj*tan(elevation)*tan(elevation)  ;
  
  //normalized trajectory vector of tau
  Double_t dNormalize = y/sqrt(denomsquared);
  Double_t dNx = dNormalize * tan(abs(azimuth));
  Double_t dNy = -dNormalize;
  Double_t dNz = dNormalize * sqrt( 1 + tan(abs(azimuth))*tan(abs(azimuth)) ) * tan(elevation);

  if(abs(azimuth)>=pi/2.)
    {
      dNx*=-1;
      dNy*=-1;
    }

  //cout<<"trajectory vector normalized x: "<<dNx<<" y: "<<dNy<<" z: "<<dNz<<" normalization: "<<dNormalize<<endl;

  //crossproduct of trajectory vector and vector of where tau emerges. Gives the
  //distance between the to perpendicular to the trajecotory vector
  //Double_t dx = y*dNz; 
  //Double_t dy = 0;
  //Double_t dz = y*dNx;

  //Double_t d = sqrt(dx*dx+dy*dy+dz*dz); //shortest distance d between tau trajectory and detector

  //Double_t dem = sqrt(l*l-d*d);

  //maximum length of trajectory above horizon befor track leaves atmosphere (dST above ground)
  //calculation is not entirely correct but we do not max out on this distance
  //anyway
  Double_t v = sqrt((REarth+DetectorAltitude[iConfig])*(REarth+DetectorAltitude[iConfig])-REarth*REarth);

  Double_t phi = elevation + asin( REarth/sqrt(REarth*REarth+(l-v)*(l-v)) ); //if azimuth > 90

  Double_t alpha = asin( sin(phi) * sqrt(REarth*REarth+(l-v)*(l-v)) / (REarth+dST)  );

  Double_t gamma = pi - alpha - phi;

  Double_t dMaxDist = (REarth+dST) *  sin(gamma)/sin(phi);


 //trim the path length to be fully inside the atmosphere, should always be the
 //case
  Double_t dED = 0; //extra distance
  Double_t dInFoV=0; //Distance in between lower edge of FoV and line of sight to horizon

  //If the camera FoV below the horizon is larger than the maximum angle needed to
  //cover all the solid angle below the horizon, set the FoV below the horizon
  //to the maximum angle needed.
  Double_t dMaxFoVBelow = asin(REarth/(REarth+DetectorAltitude[iConfig]));
  if(dFoVBelow>dMaxFoVBelow)
      dFoVBelow=dMaxFoVBelow; 


  //length of the visible trajectory between the plane to horizon and the lower edge of the camera
  //FoV below the horizon
  dInFoV = l * sin(dFoVBelow) / ( dNy * sin(dFoVBelow) +  dNz * cos(dFoVBelow)  );

  //length of tau trajectory below the horizon and earth surface
  Double_t p = 2 * ( REarth*dNz - (v-l)*dNy );
  Double_t q = (v-l)*(v-l);

  if(q-p*p/4>=0) //trajectory does not intersect with Earth
      return 0;
        
  Double_t i1 = p/2. - sqrt(p*p/4.-q);
  Double_t i2 = p/2. + sqrt(p*p/4.-q);
            
  if(i1<0 || i2<0)
    cout<<"PDecay: i1 or i2 less than 0: "<<i1<<"  "<<i2<<endl;

  dED= i1<i2 ? i1 : i2;

  if( (dInFoV>dED && dInFoV>0 ) || dInFoV<0)
          dInFoV=dED;
 
  if(l>v) //if shower emerges beyond horizon
     dInFoV = 0;


  dMaxDist = dST/sin(elevation) + dInFoV; //takes into account distance visible below plane to horizon
 // cout<<"dMaxDist: "<<dMaxDist<<endl;

  Double_t dTermInSquareRoot = cos(elevation)*cos(abs(azimuth))*cos(elevation)*cos(abs(azimuth))
                               + sin(elevation)*sin(elevation)/tanFoV/tanFoV
                               - cos(elevation)*cos(elevation);

  if(dTermInSquareRoot>0) //if it is negative the shower is contained in the FoV anywhere along the track
    {
        //maximum trajectory length before the tip of the shower is not contained in the camera anymore
       Double_t dMaxDisttoSatisfyFovReq = l / ( cos(elevation)*cos(abs(azimuth)) + sqrt( dTermInSquareRoot ));
       //if the value is negative, the elevation angle is less than the Max FoV would have to point below the
       //horizon
       if(dMaxDist>dMaxDisttoSatisfyFovReq+dInFoV && dMaxDisttoSatisfyFovReq>0) //correct max. trajectory length
           dMaxDist = dMaxDisttoSatisfyFovReq+dInFoV;
   //   cout<<" dMaxDisttoSatisfyFovReq: "<<dMaxDisttoSatisfyFovReq<<endl;
    }

  
  //length of shower in camera plane
  Double_t dShwrLgth = 0.304 * log(Etau*0.5/0.088)/log(2); //0.304km radiation length at see level, 0.088GeV critical energy of electrons in air,only 0.5 of the energy goes into the electromagnetic shower
  //cout<<"Etau "<<Etau<<" size of shower in km: "<<dShwrLgth<<endl;
  //cout<<" dMaxDist: "<<dMaxDist<<endl;
  
  //ok have taken all requirements into account. Lets see if the trajectory
  //length allows for a full shower development. If not we quit.
  if(dMaxDist<dShwrLgth)
   return 0;


  //make sure the shower does not develop past the point where more than 90% of the
  //taus have decayed
  Double_t DecayLength = Etau * c * DecayTime / Mtau;
  Double_t d90PctDecayLength = -log(0.1)*DecayLength;
  //cout<<"90% of taus decayed after: "<<d90PctDecayLength<<endl;

  if(d90PctDecayLength+dShwrLgth<dMaxDist)
    dMaxDist = d90PctDecayLength+dShwrLgth;

  //cout<<" dMaxDist: "<<dMaxDist<<endl;
  //calculate how far away from the detector the shower can be to be still
  //detected
  //emitted light intensity
  Double_t dLight = 5.95844e3*Etau; //in photons. 5.958 comes from the macro FluorescenceDetectionYield.C and includes PDE of S14520-6050CN, it is the integral from 300 to 430nm 
  dLight /= 4 * pi; //so we do not have to do it in every loop below
  //and absorption 0.9 at 337nm is used in condition below
  Double_t dFluorescenceMaximumDistance = 10; //km

  while(1)
   {
      //the 1e-6 is for a 1m^2 mirror
      if(dLight*1e-6/dFluorescenceMaximumDistance/dFluorescenceMaximumDistance*exp(dFluorescenceMaximumDistance*log(0.9))>dMinimumNumberPhotoelectrons)
      dFluorescenceMaximumDistance++; 
     else
      break;
     //cout<<dFluorescenceMaximumDistance<<"   "<<dLight*1e-6/dFluorescenceMaximumDistance/dFluorescenceMaximumDistance*exp(dFluorescenceMaximumDistance*log(0.9))<<endl;

   }  
  dFluorescenceMaximumDistance--;
  //cout<<"Maximum Distance between Detector and Shower: "<<dFluorescenceMaximumDistance<<endl;
  //don't know if this is actually good
 
  //check if we need to increase limit 
  if(dMaxDist>dFluorescenceMaximumDistance) //too speed up calculations
    dMaxDist=dFluorescenceMaximumDistance;
  

  //now lets check if the size of the shower is fullfilling the minimum length
  //requirement

  //calculating length of shower in the camera assuming the shower happens late
  //and develops up to the maximumg possible point along the trajectory

  Double_t dLength = 0.0;
  Double_t m = dMaxDist;
  Double_t B = sqrt( m* dNx * m* dNx + (m*dNy+y) * (m*dNy+y) + m*dNz * m*dNz  );
  Double_t A = 0.0;


  //This is if the shower passed and develops behind
  while(dMaxDist>dShwrLgth && (dLength<dMinLength || B>dFluorescenceMaximumDistance ))
   {
     Double_t n = dMaxDist - dShwrLgth -dInFoV;
              m = dMaxDist;

     Double_t A = sqrt( n* dNx * n* dNx + (n*dNy+y) * (n*dNy+y) + n*dNz * n*dNz  );
              B = sqrt( m* dNx * m* dNx + (m*dNy+y) * (m*dNy+y) + m*dNz * m*dNz  );

     Double_t costheta = n*dNx * m*dNx + (n*dNy+y)*(m*dNy+y) + m*dNz * n*dNz;
     costheta = costheta / (A*B);
     dLength = acos(costheta)*180/pi;
 //    cout<<"l"<<l<<"az"<<azimuth*180/pi<<"size of shower in degrees: "<<dLength<<" cos of angle:  "<<costheta<<" dMaxDist "<<dMaxDist<<"dShwrLgth "<<dShwrLgth<<" minimum shower length in deg  "<<dMinLength<<" Distance of tip of shower to telescope  "<<B<<endl;
     if((dLength<dMinLength && B>l) || B>dFluorescenceMaximumDistance )
       dMaxDist-=1.0;
     else
       break;
   }
  dMaxDist+=1.0;

  if(dLength<dMinLength  || B>dFluorescenceMaximumDistance || A>dFluorescenceMaximumDistance) //shower is not long enough in the camera or either end of the shower does not produce sufficient intensity in the telescope
      return 0;

  //  need to check if the shower is pointing away from the telescope and if we find a distance in which the tau can decay and develop a shower which appears larger. Need to adjust the distance so the image has the minimal required size.

  //----------------------------------------------------
  // Take into account to the horizontal FoV of the telescope. Is the shower fully
  // contained in the horizontal FoV? If not then return 0
  if (blimFOV){
    
    // Using the law of sines we calculate the trajectory length of the tau that is in the 
    // limited FOV. Accounts for movement along the Tau emergence anulus, i.e. if dAzimuth 
    // is zero then we are in the center of the horizontal FoV.
    Double_t hDistInFOV = 0;
    Double_t halfHFOV = (hFOV/2.) * (pi/180.);// in radians
    Double_t eta0 = pi - (halfHFOV-abs(dAzimuth)) - (abs(azimuth) + abs(dAzimuth));
    Double_t eta1 = pi - (halfHFOV+abs(dAzimuth)) - (abs(azimuth) - abs(dAzimuth));
    if (eta0 <= 0 || eta1 <= 0){
      hDistInFOV = std::numeric_limits<double>::infinity(); // shower is fully contained in hFOV; this distance is infinity now
    }else{
      if (azimuth != 0 && dAzimuth==0) // single angle, tau emerges at dAzimuth = 0
	hDistInFOV = l*sin(halfHFOV)/sin(eta0);
      if (azimuth > 0 && dAzimuth > 0) // positive azimuth and an positive azimuthal walk
	hDistInFOV = l*sin(halfHFOV-dAzimuth)/sin(eta0);
      if (azimuth < 0 && dAzimuth > 0) // negative azimuth and an positive azimuthal walk
	hDistInFOV = l*sin(halfHFOV+dAzimuth)/sin(eta1);
      if (azimuth > 0 && dAzimuth < 0)// positive azimuth and an negative azimuthal walk
	hDistInFOV = l*sin(halfHFOV+abs(dAzimuth))/sin(eta1);
      if (azimuth < 0 && dAzimuth < 0 )// negative azimuth and an negative azimuthal walk
	hDistInFOV = l*sin(halfHFOV-abs(dAzimuth))/sin(eta0);
      if (azimuth == 0) // special condition tragectory points to the telescope  
	hDistInFOV = l;
    }
    
    Double_t hdShwrLgth = dShwrLgth*cos(elevation); // we are only interested in the horizontal component of the shower length
    Double_t hd90PctDecayLength = d90PctDecayLength*cos(elevation);
    
    // Check if the full horizontal component of the shower will be captured. 
    if (hdShwrLgth+hd90PctDecayLength > hDistInFOV){
      return 0;
      }
  }
  //-----------------------------------------------------


  
  //Ok finally we are there. Lets decay the tau in the remaining distance we
  //have
 
  Double_t ProbTauDecay = exp(-(dED-dInFoV)/DecayLength); //Tau has to to survive before it becomes visible to the detector

  ProbTauDecay *= 1-exp(-(dMaxDist-dShwrLgth)/DecayLength); // then it has to decay before it is out of the FoV

  ProbTauDecay*=0.8;//only 80% of taus make a shower

//if(azimuth*180/pi>90 && azimuth*180/pi<8.1)
//cout<<"Etau: "<<Etau<<" el: "<<elevation*180/pi<<" az: "<<azimuth*180/pi<<" alpha "<<alpha<<" beta "<<180/pi*(pi - asin(sinbeta))<<" Prob: "<<ProbTauDecay<<" l "<<l<<" v "<<v<<" below horizon: "<<dBH<<" MaxDistOfTrack: "<<dMaxDist<<" minimal distance between trajectory and telescope "<<d<<endl;
return ProbTauDecay;
}

Double_t TrinitySimUtilities::PDecay(Double_t Etau, Double_t y, Double_t elevation, Double_t azimuth, Bool_t blimFOV, Double_t dAzimuth)
{
  elevation = elevation/180*pi; //elevation angle (determines path through Earth;
  azimuth = azimuth/180.*pi;  //azimuth angle
  dAzimuth = dAzimuth/180.*pi; //azimuth angle seen from telescope to shower

  Double_t l = y; //Distance between the detector and the point where the tau emerges from the ground. The detector is always at z=0
  //Distance between telescope and horizon
  Double_t v = sqrt((REarth+DetectorAltitude[iConfig])*(REarth+DetectorAltitude[iConfig])-REarth*REarth);

  //Below: the shortest distance d between tau trajectory and detector d
  Double_t nproj = y*sqrt( 1 + tan(abs(azimuth))*tan(abs(azimuth)) ); //projection of trajectory to x-y plane
  Double_t denomsquared= y*tan(abs(azimuth))*y*tan(abs(azimuth)) + y*y + nproj*nproj*tan(elevation)*tan(elevation)  ;
  
  //normalized trajectory vector of tau
  Double_t dNormalize = y/sqrt(denomsquared);
  Double_t dNx = dNormalize * tan(abs(azimuth));
  Double_t dNy = -dNormalize;
  Double_t dNz = dNormalize * sqrt( 1 + tan(abs(azimuth))*tan(abs(azimuth)) ) * tan(elevation);

  //cout<<"trajectory vector normalized x: "<<dNx<<" y: "<<dNy<<" z: "<<dNz<<" normalization: "<<dNormalize<<endl;

  //crossproduct of the trajectory vector with the vector pointing to where the tau emerged from the ground. 
  //The magnitude of the cross product gives the distance of closest approach of the tau to the detector as it travels along 
  //its trajectory
  Double_t dx = y*dNz; 
  Double_t dy = 0;
  Double_t dz = y*dNx;

  Double_t d = sqrt(dx*dx+dy*dy+dz*dz); //shortest distance d between tau trajectory and detector

  //add some extra distance if the tau passes plane in between telescope and
  //horizon
  //using dED term assumes we see shower in camera but lower edge of FoV is aligned with line of sight to the horizon
  Double_t dED = 0; //extra distance
  Double_t dInFoV=0; //Distance in between lower edge of FoV and line of sight to horizon

  //Reset FoV below to maximum possible if it is larger
  Double_t dMaxFoVBelow = asin(REarth/(REarth+DetectorAltitude[iConfig]));
  if(dFoVBelow>dMaxFoVBelow)
      dFoVBelow=dMaxFoVBelow; 


  //length of trajectory between plane to horizon and lower edge of camera
  //FoV
  dInFoV = l * sin(dFoVBelow) / ( dNy * sin(dFoVBelow) +  dNz * cos(dFoVBelow)  );

  //length of trajectory between plane to horizon and earth surface
  Double_t p = 2 * ( REarth*dNz - (v-l)*dNy );
  Double_t q = (v-l)*(v-l);

  if(q-p*p/4>=0) //trajectory does not intersect with Earth
      return 0;
        
  Double_t i1 = p/2. - sqrt(p*p/4.-q);
  Double_t i2 = p/2. + sqrt(p*p/4.-q);
            
  if(i1<0 || i2<0)
    cout<<"PDecay: i1 or i2 less than 0: "<<i1<<"  "<<i2<<endl;

  dED= i1<i2 ? i1 : i2;

  if( (dInFoV>dED && dInFoV>0 ) || dInFoV<0)
          dInFoV=dED;
 
  if(l>v) //if shower emerges beyond horizon
     dInFoV = 0;

  //maximum distance from where tau emerges to that plane
  //modify this to take into account extra length if shower emerges before the
  //horizon
  //add extra length if trajectory crosses plane between telescope and horizon
  Double_t dem = sqrt(l*l-d*d) + dInFoV;

  
  //fix this to use elevation measured when shower emerges from ground
  fPE->FixParameter(1,elevation ); //Shower elevation in rad

  //length of shower in camera plane
  Double_t dShwrLgth = 0.304 * log(Etau*0.5/0.088)/log(2); //0.304km radiation length at see level, 0.088GeV critical energy of electrons in air,only 0.5 of the energy goes into the electromagnetic shower
  //cout<<"Etau "<<Etau<<" size of shower in km: "<<dShwrLgth<<endl;
  //cout<<" dMaxDist: "<<dMaxDist<<endl;

  //minimal distance from tip of shower to the plane that is normal to trajectory and goes through the origin (where the detector is located), constrained by maximum angle a sufficient Cherenkov light reaches the detector. values below are 90-a and a in the sines. Need to have functions of tau energy and distance for a 
  //dd is first used as the distance to the start of the shower not hte tip of
  //the shower
  Double_t dd = dShwrLgth+5; //we need to at least have the shower develop. note we neglect the decay length here, which does not really matter. That is taken care of later.
 
  if(dd>dem)
     dd = dShwrLgth;
  
  while(dd<dem)
   {
  //move back along trajectory from the point of latest possible decay and find spot 
  //where for the first time we see enough photoelectrons in the telescope
  Double_t dDistanceToWhereTauStarts = sqrt(d*d+dd*dd);
  fPE->FixParameter(0,dDistanceToWhereTauStarts); //Distance to where the tau starts shower
   //get new azimuth
   Double_t g = (dem-dd) * cos(elevation);
   Double_t az = 0;
   if(g>0 && abs(azimuth)>0)
     {
        Double_t xi = ( l - g * cos(abs(azimuth))) / (g * sin(abs(azimuth)));
        az =  pi*0.5 + abs(azimuth) - atan(xi);
        
        if (dem-dd-dInFoV < 0 ) //if we are below the plane
          az = abs(azimuth);
     }
   else
     {
       if (abs(azimuth) < 0) 
	 cout<<"in PDecay, azimuth is zero"<<endl;
       if (g<0)
	 cout<<"in PDecay, g is smaller zero"<<endl;
     }
   //get PE for new azimuth
     //double az = asin(d/dDistanceToWhereTauStarts); //azimuth for that distance
   if(fPE->Eval(az)*Etau*0.5<dMinimumNumberPhotoelectrons)
       dd++; //increasing dd by 1km
     else
      break;

   }
  
  //cout<<"maximum available length for decay and shower to happen (dem:) "<<dem<<" distance between trajectory and telescope d: "<<d<<" dd: "<<dd<<endl;
  //tip of the shower has to be inside the atmosphere. Check if that is the case. if not adjust dd
  //cout<<"need to takeaway dd so we can see all the Cherenkov light: "<<dd<<endl; 
  //if dem is less then dd, which means Cherenkov light will not hit the
  //telescope. return 0
  if(dd>dem) // the shower cannot be seen by the telescope because the cherenkov cone does not illuminate the telescope anywhere along the track
   return 0;

  //dd below is used as the distance between the plane perp. to the trajectory
  //and the tip of the shower so lets subtract the length of the shower and the
  //5 km again
  if(dd<dShwrLgth+5)
    dd -= dShwrLgth;
  else
    dd -= (dShwrLgth+5);
  
  
  //maximum length of trajectory above horizon before track leaves atmosphere (dST above ground)
  Double_t phi = elevation + asin( REarth/sqrt(REarth*REarth+(l-v)*(l-v)) );
  
  Double_t alpha = asin( sin(phi) * sqrt(REarth*REarth+(l-v)*(l-v)) / (REarth+dST)  );

  Double_t gamma = pi - alpha - phi;

  Double_t dMaxDist = (REarth+dST) *  sin(gamma)/sin(phi);

 //trim the path length to be fully inside the atmosphere, should always be the
 //case
 if(l>v)//if the shower is not seen over the entire track in the atmosphere and l>v. Reset the maximum possible track length to the portion that can be seen
  {
    dMaxDist = dMaxDist>dem-dd ? dem-dd : dMaxDist;
  }
 else//do the same if the shower emerges l<v from telescope v is where the tangent touches earth.
  {
    dMaxDist = dST/sin(elevation) + dInFoV;
    dMaxDist = dMaxDist>dem-dd ? dem-dd : dMaxDist;
  }

 // Double_t delta = acos(-dNy);
 // cout<<"Delta: "<<delta*180/pi<<" angle from tip of shower to telescope: "<<asin(y*sin(delta)/sqrt(y*y+dMaxDist*dMaxDist-2*dMaxDist*y*cos(delta)))*180/pi  <<" shortest distance so Cherenkov light goes to camera: "<<dd<<endl;


  Double_t dTermInSquareRoot = cos(elevation)*cos(abs(azimuth))*cos(elevation)*cos(abs(azimuth))
                               + sin(elevation)*sin(elevation)/tanFoV/tanFoV
                               - cos(elevation)*cos(elevation);

  if(dTermInSquareRoot>0) //if it is negative the shower is contained in the FoV anywhere along the track
    {
        //maximum trajectory length before the tip of the shower is not contained in the camera anymore
       Double_t dMaxDisttoSatisfyFovReq = l / ( cos(elevation)*cos(abs(azimuth)) + sqrt( dTermInSquareRoot )) + dInFoV;

       if(dMaxDist>dMaxDisttoSatisfyFovReq+dInFoV && dMaxDisttoSatisfyFovReq>0) //correct max. trajectory length
           dMaxDist = dMaxDisttoSatisfyFovReq+dInFoV;
   //    cout<<" dMaxDisttoSatisfyFovReq: "<<dMaxDisttoSatisfyFovReq<<endl;
    }

 
 
  
  //ok have taken all requirements into account. Lets see if the trajectory
  //length allows for a full shower development.
  //If not we quit.
  if(dMaxDist<dShwrLgth)
   return 0;

  //make sure the shower does not develop past the point where more than 90% of the
  //taus have decayed
  Double_t DecayLength = Etau * c * DecayTime / Mtau;
  Double_t d90PctDecayLength = -log(0.1)*DecayLength;
  //cout<<"90% of taus decayed after: "<<d90PctDecayLength<<endl;
  if(d90PctDecayLength+dShwrLgth<dMaxDist)
    dMaxDist = d90PctDecayLength+dShwrLgth;

  //now lets check if the size of the shower is fullfilling the minimum length
  //requirement

  //calculating length of shower in the camera assuming the shower happens late
  //and develops up to the maximumg possible point along the trajectory
  //add that distance is within maximum distance like we do for the Fluorescence
  //part
  Double_t n = dMaxDist - dShwrLgth -dInFoV;
  Double_t m = dMaxDist;

  Double_t A = sqrt( n* dNx * n* dNx + (n*dNy+y) * (n*dNy+y) + n*dNz * n*dNz  );
  Double_t B = sqrt( m* dNx * m* dNx + (m*dNy+y) * (m*dNy+y) + m*dNz * m*dNz  );

  Double_t costheta = n*dNx * m*dNx + (n*dNy+y)*(m*dNy+y) + m*dNz * n*dNz;
  costheta = costheta / (A*B);
  Double_t dLength = acos(costheta)*180/pi;
  //cout<<"size of shower in degrees: "<<dLength<<" cos of angle:  "<<costheta<<endl;

  if(dLength<dMinLength) //shower image is too short
   return 0;

  //----------------------------------------------------
  // Take into account the horizontal FoV of the telescope. 
  // return 0 if the shower is not fully contained in the horizontal FoV
  if (blimFOV){

    // Using the law of sines we calculate the trajectory length of the tau that is in the 
    // limited FOV. Accounts for movement along the Tau emergence anulus, i.e. if dAzimuth 
    // is zero then we are in the center of the horizontal FoV.
    Double_t hDistInFOV = 0;
    Double_t halfHFOV = (hFOV/2.) * (pi/180.);// in radians
    //Double_t eta0 = pi - (halfHFOV-abs(dAzimuth)) - abs(azimuth);
    //Double_t eta1 = pi - (halfHFOV+abs(dAzimuth)) - abs(azimuth);
    Double_t eta0 = pi - (halfHFOV-abs(dAzimuth)) - (abs(azimuth) + abs(dAzimuth));
    Double_t eta1 = pi - (halfHFOV+abs(dAzimuth)) - (abs(azimuth) - abs(dAzimuth));

    if (azimuth != 0 && dAzimuth==0) // single angle, tau emerges at dAzimuth = 0
      hDistInFOV = l*sin(halfHFOV)/sin(eta0);
    if (azimuth > 0 && dAzimuth > 0) // positive azimuth and an positive azimuthal walk
      hDistInFOV = l*sin(halfHFOV-dAzimuth)/sin(eta0);
    if (azimuth < 0 && dAzimuth > 0) // negative azimuth and an positive azimuthal walk
      hDistInFOV = l*sin(halfHFOV+dAzimuth)/sin(eta1);
    if (azimuth > 0 && dAzimuth < 0)// positive azimuth and an negative azimuthal walk
      hDistInFOV = l*sin(halfHFOV+abs(dAzimuth))/sin(eta1);
    if (azimuth < 0 && dAzimuth < 0 )// negative azimuth and an negative azimuthal walk
      hDistInFOV = l*sin(halfHFOV-abs(dAzimuth))/sin(eta0);
    if (azimuth == 0) // special condition tragectory points to the telescope  
      hDistInFOV = l;
    
    Double_t hdShwrLgth = dShwrLgth*cos(elevation); // we are only interested in the horizontal component of the shower length
    Double_t hd90PctDecayLength = d90PctDecayLength*cos(elevation);
    
    // Check if the full horizontal component of the shower will be captured. 
    if (hdShwrLgth+hd90PctDecayLength > hDistInFOV){
      return 0;
      }
  }
  //-----------------------------------------------------

  //Ok finally we are there. Lets decay the tau in the remaining distance we
  //have
 
  //Probabilty that tau survives if it is not in the field of view
  //below works for tau emerging beyond horizon and before horizon 
  Double_t ProbTauDecay = exp(-(dED-dInFoV)/DecayLength); //Tau has to to survive before it becomes visible to the detector

  
  ProbTauDecay *= 1-exp(-(dMaxDist-dShwrLgth)/DecayLength); // then it has to decay before it is out of the FoV

  ProbTauDecay*=0.8;//only 80% of taus make a shower


//if(azimuth*180/pi>90 && azimuth*180/pi<8.1)
//cout<<"Etau: "<<Etau<<" el: "<<elevation*180/pi<<" az: "<<azimuth*180/pi<<" alpha "<<alpha<<" beta "<<180/pi*(pi - asin(sinbeta))<<" Prob: "<<ProbTauDecay<<" l "<<l<<" v "<<v<<" below horizon: "<<dBH<<" MaxDistOfTrack: "<<dMaxDist<<" minimal distance between trajectory and telescope "<<d<<endl;
return ProbTauDecay;
}


void TrinitySimUtilities::PlotEmergenceProbability()
{

  TH1D *hTau = new TH1D("hTauS","",70,4,11);
  //hTau->SetMaximum(1);
  hTau->GetXaxis()->SetTitle("energy [GeV]");
  hTau->GetYaxis()->SetTitle("F_tau/F_nu");
  TAxis *axis = hTau->GetXaxis();
  int bins = axis->GetNbins();
  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins + 1];
  for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
  }
  axis->Set(bins, new_bins);
  TMultiGraph *mg = new TMultiGraph();
  TLegend *leg = new TLegend(0.7,0.4,0.89,0.88,"neutrino energy");


  double Enulog = 11;
  double Enuminlog = 5.9;
  double Enusteplog = 0.5;
  int s=0;
  while(Enulog>Enuminlog)
    {
      //cout<<Enulog<<endl;
      double Enu = pow(10,Enulog);
      TGraph *grProb = new TGraph(); 
      grProb->SetMarkerStyle(20+s);
      TString title;
      title.Form("%0.0e GeV",Enu);
      leg->AddEntry(grProb,title.Data(),"p"); 
      mg->Add(grProb,"lp");
      s++;

      // loop over target thickness
      double d = 0; //in 10^dmin km
      double dmax = 4;
      double dstep = 0.2;
      int p=0;
      while(d<dmax)
       {          
        double targetthickness = pow(10,d); 
        double dSumProb = 0;
        hTau->Reset();
        for(int i=1;i<hTau->GetNbinsX();i++)
           {

               Double_t Etau = hTau->GetBinCenter(i+1); 
               if(hTau->GetBinLowEdge(i+2)<=Enu)
                 {
                    Double_t P = PEtau(targetthickness,Etau,Enu);
                    //cout<<i+1<<" "<<targetthickness<<" . "<<Etau<<"  "<<Enu<<" P "<<P<<endl;
                  P *= (hTau->GetBinLowEdge(i+1)-hTau->GetBinLowEdge(i));

                    hTau->Fill(Etau,P); 
                    hTau->SetBinError(i,0);
                    dSumProb+=P;
                 }

            }//got the energy spectrum of the taus for this azimuth and elevation

         grProb->SetPoint(p,targetthickness,dSumProb);
         p++;
         d+=dstep;            
        }    
      Enulog-=Enusteplog;
     }
  TCanvas *cProbOfEmergence = new TCanvas("cProbOfEmergence","Probability of emergence",750,500);
  cProbOfEmergence->Draw();
  cProbOfEmergence->SetLogx();
  cProbOfEmergence->SetLogy();
  mg->Draw("a");
  mg->GetXaxis()->SetTitle("target thickness [km]");
  mg->GetXaxis()->SetTitleSize(0.045);
  mg->GetXaxis()->SetTitleOffset(1.1);
  mg->GetXaxis()->SetLabelSize(0.045);
  mg->GetYaxis()->SetTitle("probability of #tau emergence");
  mg->GetYaxis()->SetTitleOffset(1.0);
  mg->GetYaxis()->SetTitleSize(0.045);
  mg->GetYaxis()->SetLabelSize(0.045);
  mg->GetYaxis()->SetRangeUser(1e-6,1);
 // mg->GetXaxis()->SetRangeUser(1,5e3);
  leg->Draw();
  //TF1 *fdeg=new TF1("fdeg","90-180/3.1415*TMath::ASin(0.5*x/6371)",0,1e4);
  cout<<mg->GetXaxis()->GetXmin()<<endl;
  cout<<180/3.1415*TMath::ASin(0.5*mg->GetXaxis()->GetXmin()/6371)<<"  "<<180/3.1415*TMath::ASin(0.5*mg->GetXaxis()->GetXmax()/6371)<<endl;
  TF1 *fdeg=new TF1("fdeg","x",180/3.1415*TMath::ASin(0.5*mg->GetXaxis()->GetXmin()/6371),180/3.1415*TMath::ASin(0.5*mg->GetXaxis()->GetXmax()/6371));
  fdeg->Eval(0);
  TGaxis *degaxis = new TGaxis(mg->GetXaxis()->GetXmin(),1,mg->GetXaxis()->GetXmax(),1,"fdeg",510,"-G");
  degaxis->SetTitle("elevation angle [degrees]");
  degaxis->SetTitleFont(42);
  degaxis->SetLabelFont(42);
  degaxis->Draw();
}

void TrinitySimUtilities::GetTauDistribution(Double_t d, Double_t Enumin, Double_t Enumax)
{
   hTau->Reset(); //get the energy spectrum of taus coming out of the Earth, starting with nus in the range expmin expMax
   //int nEnuSteps = 20;
   //Double_t DeltaEnu = (Enumax - Enumin)/nEnuSteps;
   Double_t DeltaEnu = 0.1; //logscale
   int nEnuSteps = (0.0001+log10(Enumax) - log10(Enumin))/DeltaEnu; //the 0.0001 is due to small uncertainties making sure we get the right number of steps
   Double_t Normalization = (nuIndex-1)/(pow(Enumin,1-nuIndex)-pow(Enumax,1-nuIndex)); 
   vector<double> Enu;
   vector<double> EnuWeight;
   for(int i=0;i<nEnuSteps;i++)
      {
        //Enu.push_back(Enumin+i*DeltaEnu+0.5*DeltaEnu);
        //NO Enu.push_back(pow(10,(log10(Enumin+i*DeltaEnu)+log10(Enumin+(i+1)*DeltaEnu))*0.5));
        Enu.push_back(pow(10,log10(Enumin)+i*DeltaEnu+DeltaEnu*0.5));
        //EnuWeight.push_back(pow(Enumin+i*DeltaEnu+0.5*DeltaEnu,-nuIndex)*Normalization*DeltaEnu);
        //NO EnuWeight.push_back(pow(Enu[Enu.size()-1],-nuIndex)*Normalization*DeltaEnu);
        EnuWeight.push_back(pow(Enu[Enu.size()-1],-nuIndex)*Normalization*
                       (pow(10,log10(Enumin)+(i+1)*DeltaEnu)-pow(10,log10(Enumin)+i*DeltaEnu)));
      }

   for(int i=1;i<=hTau->GetNbinsX();i++)
       {

          Double_t Etau = hTau->GetBinCenter(i); 

         //loop over all Enu in this energy bin to calculate the sensitivity
          if(bMonoNu) //if we want to simulate only monoenergetic neutrinos (only good for acceptance calculations
            {
               //NO removed because that is transferred into PEtau 
               //if(Etau<=0.8*Enu) //0.8 because I need to account for part of the energy being transferred to the also produced neutrinos
                 {
                    Double_t Enu = pow(10,(log10(Enumin)+log10(Enumax))*0.5);
                    Double_t P = PEtau(d,Etau,Enu);
                    hTau->Fill(Etau,P);
                 } 
            }
          else
            {
              //while(Enu<Enumax)//loop over nu energy bin
              for(int n = 0; n<nEnuSteps;n++)
                {
                  //NO removed because that is transferred into PEtau 
                  //if(Etau<=0.8*Enu) //0.8 because I need to account for part of the energy being transferred to the also produced neutrinos
                    {
                      Double_t P = 0;
                      //if(Enu==Enumin)  //mod
                      P = PEtau(d,Etau,Enu[n]);
                      //if(Enu==Enumin) //mod
                      //cout<<P<<"  "<<d<<"  "<<Etau<<" "<<Enu[n]<<"  "<<EnuWeight[n]<<endl; //mod
                      //P *= DeltaEnu/(Enumax-Enumin); 
                      P *= EnuWeight[n];

                      //do not know how the below is calculated
                      //P *= ( pow(Enu,1-nuIndex) - pow(Enu+DeltaEnu,1-nuIndex) ) 
                      // / ( pow(Enumin,1-nuIndex) - pow(Enumax,1-nuIndex) );//multiplying in the with
                      hTau->SetBinContent(i,hTau->GetBinContent(i)+P); 
                      //hTau->Fill(Etau,P); 
                    }
                //Enu += DeltaEnu;  
               }
           }    
      }//got the energy spectrum of the taus for this azimuth and elevation

 /*
               for(int i=0;i<hTau->GetNbinsX();i++)
                  {
cout<<i+1<<"  "<<hTau->GetBinCenter(i+1)  <<" taus cont: "<<hTau->GetBinContent(i+1)<<endl;
}
*/
      //finding lowest and highest bin that is non-zero
      //bin with maximum content
      Int_t iBinMax = hTau->GetMaximumBin();
      Double_t dContInMax = hTau->GetBinContent(iBinMax);
      //   cout<<"iBinMax: "<<iBinMax<<" maxcontent: "<<dContInMax<<endl;       
   
      //Double_t dHighE = hTau->GetBinLowEdge(iBinMax+1);
      //finding lowest bin
      Int_t iBinMin = iBinMax;
      while(hTau->GetBinContent(iBinMin)>1e-4*dContInMax)
        {
           iBinMin--;   
        }
      // cout<<"iBinMin: "<<iBinMin<<endl;

     //finding highest bin
      while(hTau->GetBinContent(iBinMax+1)>0)
         iBinMax++;
 
      //cout<<"iBinMax: "<<iBinMax<<endl;       


       //Double_t dLowE = hTau->GetBinLowEdge(iBinMin);
       //Double_t dDeltaE = dHighE-dLowE;

       for(int i=1;i<hTau->GetNbinsX();i++) //weight each bin with energy scale need to do that because of logarithmic energy scale
          {
            Double_t dContent = hTau->GetBinContent(i);
            dContent *= (hTau->GetBinLowEdge(i+1)-hTau->GetBinLowEdge(i));
            if(i<iBinMin || i>iBinMax)
                 dContent = 0;
            hTau->SetBinContent(i,dContent);
            hTau->SetBinError(i,0);
          }

}


//Calculates the acceptance by looping over distance, azimuth and elevation for
//a given energy bin
//returns the integral acceptance
//and returns a graph that holds the acceptance for each step in distance in the
//loop
Double_t TrinitySimUtilities::GetDiffuseAcceptanceLimitedFoV(Double_t dMinEnu, Double_t dMaxEnu,TGraph *grDiffAcceptance)
{
  cout<<"Running GetDiffuseAcceptanceLimitedFoV from "<<dMinEnu<<" to "<<dMaxEnu<<" in log10(E/GeV)"<<endl; 
  TH2F *skyAcceptance = new TH2F("skyAcceptance","Diffuse Flux Acceptance", 360/DeltaAngleSky+1, -180,180, 180/DeltaAngleSky, -90, 90); //histo for single angle acceptance plot
  skyAcceptance->GetXaxis()->SetTitle("azimuth angle [degrees]");
  skyAcceptance->GetYaxis()->SetTitle("elevation angle [degrees]");
  skyAcceptance->GetZaxis()->SetTitle("effective area [cm^{2} sr]");

  TH1F *hAcceptanceVsDistance = new TH1F("hAcceptanceVsDistance","Diffuse Flux Acceptance vs. Distance from the telescope",(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsDistance->GetXaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsDistance->GetZaxis()->SetTitle("effective area [cm^{2}]");

  TH2F *hAcceptanceVsAzimuthAndDistance = new TH2F("hAzimuthVsDistance","Diffuse FLux Acceptance vs. Azimuth and Distance",(int)(180/DeltaAngleSky)+1,0,180,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsAzimuthAndDistance->GetXaxis()->SetTitle("azimuth (viewing angle) [degrees]");
  hAcceptanceVsAzimuthAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsAzimuthAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2} sr]");
	
  TH2F *hAcceptanceVsElevationAndDistance = new TH2F("hElevationVsDistance","Diffuse Flux Acceptance vs. Elevation and Distance",
                            (int)(MaxElevation / DeltaAngleSky),-1*MaxElevation,0,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsElevationAndDistance->GetXaxis()->SetTitle("elevation [degrees]");
  hAcceptanceVsElevationAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsElevationAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2} sr]");

  if (iTrigWin==5){
    cout<<"///// Time Window: All time" <<endl;
    fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction,0,40,2);
  }else{
    cout<<"///// Time Window: " << TriggerWindow[iTrigWin] <<endl;
    fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction_trigwindow,0,40,3);
    fPE->FixParameter(2,iTrigWin);
  }
  
  dMinEnu = pow(10,dMinEnu);
  dMaxEnu = pow(10,dMaxEnu);
  
  //area of cell
  Double_t dConversion=yDelta*DeltaAngleAz/180*pi; //multiply area of cell taking into account that we go in steps of DeltaAngleAz in the camera
  dConversion*=1e10; //from km2 to cm2
  //solid angle
  dConversion*=DeltaAngleSky/180.*pi*DeltaAngleSky/180.*pi; //multiply area of solidangle cell
  
  dConversion*=2; //because we only scan over one side of the camera  
  
  Double_t dIntegratedAcceptance=0;
  Int_t p = 0;
  Double_t y = yMin; //y distance from telescope where tau comes out of the ground;
  while(y<yMax) //loop over distance to telescope
    {
      //cout<<"Distance from Detector: "<<y<<endl;
      //calculate for given elevation the length of the trajectory through earth.
      Double_t dAcceptance=0.0;
      
      Double_t MaxAzimuth = dMaxCherenkovAzimuthAngle;
      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) ) // so we can make full use of fluoresence events
	MaxAzimuth = 180;
      
      Double_t elevation=DeltaAngleSky*0.5;    
      while(elevation<MaxElevation) //loop over elevation
	{
	  Double_t dWeightForTriggeredAzimuth = sin(elevation/180.*pi)*y;
	  Double_t AbsAzimuth = DeltaAngleSky*0.5;
	  while(AbsAzimuth<MaxAzimuth) // loop over azimuth
	    {
                    
	      if(AbsAzimuth>MaxAzimuth && y>dMaxFluorescenceDistance)
		cout<<" Azimuth:" <<AbsAzimuth<<" should not be here "<<endl;

              //We add for +/- azimuth acceptance. 
	      Double_t dDeltaAcceptance=0;
	       
              for(int iSign= -1;iSign<=1;iSign+=2)
              {
                 Double_t azimuth = iSign * AbsAzimuth;
	      
	         Double_t dEarth = DistanceThroughEarth(y,elevation,azimuth);
	         //cout<<"   length of trajectory in Earth: "<<dEarth<<" km"<<endl;
	      
	         //cout<<dEarth<<"  "<<dMinEnu<<"  "<<dMaxEnu<<endl; 
	         GetTauDistribution(dEarth,dMinEnu,dMaxEnu);                
	         //cout<<"done GetTau"<<endl;
	      

                 //looping over azimuth in FoV in steps of DeltaAngleAz. That is
                 //an offset of the shower from the center of the camera.
	         for (int iFoVazi= 0; iFoVazi <= (hFOV/2/DeltaAngleAz); iFoVazi++) 
		   {
		  
		      Double_t dAzimuthFoV = iFoVazi * DeltaAngleAz;
 
	              Double_t dP = 0;
	              for(int i=0;i<hTau->GetNbinsX();i++)
		         {
		           if(hTau->GetBinContent(i+1)>0)
		             {
		               Double_t dPFluorescence = 0.0;
		               Double_t dPCherenkov = 0.0;
		               if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
			           dPFluorescence = PDecayFluorescence(hTau->GetBinCenter(i+1),y,elevation,azimuth,kTRUE,dAzimuthFoV);
		               if( ( !bFluorescence || bCombined) ) 
	           	   	   dPCherenkov = PDecay(hTau->GetBinCenter(i+1),y,elevation,azimuth,kTRUE,dAzimuthFoV);
		      
		               if(bCombined)
			          dP = dPFluorescence > dPCherenkov ? dPFluorescence : dPCherenkov;
		               else if(bFluorescence)
			          dP = dPFluorescence;
		               else
			          dP = dPCherenkov;
		               dDeltaAcceptance+=hTau->GetBinContent(i+1)*dP;
		               if(!bMonoNu)
			         hTriggeredAzimuthAngles->Fill(azimuth,hTau->GetBinContent(i+1)*dP*dWeightForTriggeredAzimuth);
		             }
		         }
                   }//iFoVazi
	         }//iSign
                 dDeltaAcceptance*=sin(elevation/180.*pi); //projection of area cell to trajectory
                 //multiply with area of cell (note that the yDelta*DeltaAngleSky/180*pi is included in dConversion
                 dDeltaAcceptance*=y;

                 hAcceptanceVsAzimuthAndDistance->Fill(AbsAzimuth,y,dDeltaAcceptance);
                 hAcceptanceVsElevationAndDistance->Fill(-1*elevation,y,dDeltaAcceptance);
	         skyAcceptance->Fill(AbsAzimuth,-1*elevation,dDeltaAcceptance);
                 hAcceptanceVsDistance->Fill(y,dDeltaAcceptance);

	         dAcceptance+=dDeltaAcceptance; 
	          //   cout<<"distance "<<y<<" prob"<<dDeltaAcceptance<<" elevation  "<<elevation<<endl;
	         AbsAzimuth+=DeltaAngleSky;
	         if(AbsAzimuth>=MaxAzimuth && y>dMaxFluorescenceDistance)
                      cout<<"The "<<MaxAzimuth<<" degree maximum azimuth angle dMaxCherenkovAzimuthAngle has been reached. Consider making it bigger using SetMaximumShowerViewingAngle(Double_t maxangle)."<<endl; 

	         if(dDeltaAcceptance<1e-3 && y>dMaxFluorescenceDistance) //won't get any more acceptance. The >60 is to make sure we do not miss fluorescence events whic can be seen from the back
                  {
                     cout<<"Breaking after "<<AbsAzimuth<<" azimuth"<<endl;
		      break;
                  }
	      
	    }//finished looping over all azimuth angles
	  elevation+=DeltaAngleSky;    
	  //cout<<azimuth<<"  "<<dAcceptance<<endl;
	  //multiply with dOmega  DeltaAngleSky*DeltaAngleSky
        }//finished looping over all elevation angles
      dIntegratedAcceptance+=dAcceptance;
      
      grDiffAcceptance->SetPoint(p,y,dAcceptance*dConversion); 
      p++;
      cout<<"distance: "<<y<<" differ. acceptance "<<(dAcceptance*dConversion)<<" cm^2 sr; integr. acceptance.: "<<(dIntegratedAcceptance*dConversion)<<" cm^2 sr"<<endl; 
      //cout<<dAcceptance<<"  "<<dConversion<<endl;
      y+=yDelta;
      if(dAcceptance<1e-10 && y>dMaxFluorescenceDistance) //no sense to increase in distance if we can't see any showers now
	break;
    }//end looping over distances


  TCanvas *cAcceptance = new TCanvas("cPointSourceAcceptance","Diffuse Flux Acceptance Limited Field of View",1700,800);
  cAcceptance->Divide(2,2);
  cAcceptance->cd(1);
  skyAcceptance->Scale(dConversion);
  skyAcceptance->Draw("COLZ");
  cAcceptance->cd(2);
  hAcceptanceVsDistance->Scale(dConversion);
  hAcceptanceVsDistance->Draw("L");
  cAcceptance->cd(3);
  hAcceptanceVsElevationAndDistance->Scale(dConversion); 
  hAcceptanceVsElevationAndDistance->Draw("COLZ"); 
  cAcceptance->cd(4);
  hAcceptanceVsAzimuthAndDistance->Scale(dConversion); 
  hAcceptanceVsAzimuthAndDistance->Draw("COLZ"); 
  cAcceptance->Modified();
  cAcceptance->Update();
  //Save the acceptance into a file
  TString Filename;
  Filename.Form("DiffuseFluxAcceptance_FoV%0.1fdeg_Index%0.1f_Emin%0.1f_Emax%0.1f.root",hFOV,nuIndex,log10(dMinEnu),log10(dMaxEnu));
  TFile *f = new TFile(Filename.Data(),"RECREATE");
  f->WriteObject(hAcceptanceVsDistance,"hAcceptanceVsDistance");
  f->WriteObject(skyAcceptance,"skyAcceptance");
  f->WriteObject(hAcceptanceVsAzimuthAndDistance,"hAcceptanceVsAzimuthAndDistance");
  f->WriteObject(hAcceptanceVsElevationAndDistance,"hAcceptanceVsElevationAndDistance");
  f->Close();


  return (dIntegratedAcceptance*dConversion);
}

//Calculates the acceptance by looping over distance, azimuth and elevation for
//a given energy bin
//returns the integral acceptance
//and returns a graph that holds the acceptance for each step in distance in the
//loop
Double_t TrinitySimUtilities::GetDiffuseAcceptance(Double_t dMinEnu, Double_t dMaxEnu,TGraph *grDiffAcceptance)
{
 cout<<"Running GetDiffuseAcceptance for 360 deg horizontal FoV from "<<dMinEnu<<" to "<<dMaxEnu<<" in log10(E/GeV)"<<endl; 

  TH2F *skyAcceptance = new TH2F("skyAcceptance","Diffuse Flux Acceptance", 360/DeltaAngleSky+1, -180,180, 180/DeltaAngleSky, -90, 90); //histo for single angle acceptance plot
  skyAcceptance->GetXaxis()->SetTitle("azimuth angle [degrees]");
  skyAcceptance->GetYaxis()->SetTitle("elevation angle [degrees]");
  skyAcceptance->GetZaxis()->SetTitle("effective area [cm^{2} sr]");

  TH1F *hAcceptanceVsDistance = new TH1F("hAcceptanceVsDistance","Diffuse Flux Acceptance vs. Distance from the telescope",(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsDistance->GetXaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsDistance->GetZaxis()->SetTitle("effective area [cm^{2}]");

  TH2F *hAcceptanceVsAzimuthAndDistance = new TH2F("hAzimuthVsDistance","Diffuse FLux Acceptance vs. Azimuth and Distance",(int)(180/DeltaAngleSky)+1,0,180,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsAzimuthAndDistance->GetXaxis()->SetTitle("azimuth (viewing angle) [degrees]");
  hAcceptanceVsAzimuthAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsAzimuthAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2} sr]");
	
  TH2F *hAcceptanceVsElevationAndDistance = new TH2F("hElevationVsDistance","Diffuse Flux Acceptance vs. Elevation and Distance",
                            (int)(MaxElevation / DeltaAngleSky),-1*MaxElevation,0,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsElevationAndDistance->GetXaxis()->SetTitle("elevation [degrees]");
  hAcceptanceVsElevationAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsElevationAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2} sr]");

  if (iTrigWin==5){
    cout<<"///// Time Window: All time" <<endl;
    fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction,0,40,2);
  }else{
    cout<<"///// Time Window: " << TriggerWindow[iTrigWin] <<endl;
    fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction_trigwindow,0,40,3);
    fPE->FixParameter(2,iTrigWin);
  }
  
  dMinEnu = pow(10,dMinEnu);
  dMaxEnu = pow(10,dMaxEnu);
  
  //area of cell
  Double_t dConversion=yDelta*2*pi; //multiply area of cell taking into account that we have a 360 degree FoV
  dConversion*=1e10; //from km2 to cm2
  //solid angle
  dConversion*=DeltaAngleSky/180.*pi*DeltaAngleSky/180.*pi; //multiply area of solidangle cell
  
  dConversion*=2; //because we only calculate for azimuth angles 0 to azimuth max. There are also negative azimuth values due to symmetry of the problem 
  
  dConversion*=NumSites; // multiply by the number of sites
  
  Double_t dIntegratedAcceptance=0;
  Int_t p = 0;
  Double_t y = yMin; //y distance from telescope where tau comes out of the ground;
  while(y<yMax) //loop over distance to telescope
    {
      //cout<<"Distance from Detector: "<<y<<endl;
      //calculate for given elevation the length of the trajectory through earth.
      Double_t dAcceptance=0.0;
      
      Double_t MaxAzimuth = dMaxCherenkovAzimuthAngle;
      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) ) // so we can make full use of fluoresence events
	MaxAzimuth = 180;
      
      Double_t elevation=DeltaAngleSky*0.5;    
      while(elevation<MaxElevation) //loop over elevation
	{
	  Double_t dWeightForTriggeredAzimuth = sin(elevation/180.*pi)*y;
	  Double_t azimuth = DeltaAngleSky*0.5;
	  while(azimuth<MaxAzimuth) // loop over azimuth
	    {
	      
	      if(azimuth>MaxAzimuth && y>dMaxFluorescenceDistance)
		cout<<" Azimuth:" <<azimuth<<" should not be here "<<endl;
	      
	      Double_t dEarth = DistanceThroughEarth(y,elevation,azimuth);
	      //cout<<"   length of trajectory in Earth: "<<dEarth<<" km"<<endl;
	      
	      //cout<<dEarth<<"  "<<dMinEnu<<"  "<<dMaxEnu<<endl; 
	      GetTauDistribution(dEarth,dMinEnu,dMaxEnu);                
	      //cout<<"done GetTau"<<endl;
	      
	      Double_t dDeltaAcceptance=0;
	      
	      //Calculate probability that taus with E convert before they are 150 km away from detector when they make it out of the earth. 
	      //150km is for an angle of 5 degrees assuming 10 degree opening angle. If the angle is free the distance of closest approach depends on
	      // where the tau comes out of the earth and under what angle (vertical and horizontal. 
	      //That is probably dependend on the initial nu energy
	      Double_t dP = 0;
	      //     if(elevation>0.5 && elevation<1 && azimuth>9 && azimuth<9.2)
	      for(int i=0;i<hTau->GetNbinsX();i++)
		{
		  //cout<<dMinEnu<<"  "<<dMaxEnu<<endl;
		  //cout<<"taus cont: "<<hTau->GetBinContent(i+1)<<endl;
		  if(hTau->GetBinContent(i+1)>0)
		    {
		      Double_t dPFluorescence = 0.0;
		      Double_t dPCherenkov = 0.0;
		      //cout<<bFluorescence<<" "<<bCombined<<"  "<<y<<"<"<<dMaxFluorescenceDistance<<endl;
		      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
			dPFluorescence = PDecayFluorescence(hTau->GetBinCenter(i+1),y,elevation,azimuth,kFALSE,0);
		      if( (!bFluorescence || bCombined) && azimuth<dMaxCherenkovAzimuthAngle  )
			dPCherenkov = PDecay(hTau->GetBinCenter(i+1),y,elevation,azimuth,kFALSE,0);
		      
		      if(bCombined)
			dP = dPFluorescence > dPCherenkov ? dPFluorescence : dPCherenkov;
		      else if(bFluorescence)
			dP = dPFluorescence;
		      else
			dP = dPCherenkov;
		      dDeltaAcceptance+=hTau->GetBinContent(i+1)*dP;
		      if(!bMonoNu)
			hTriggeredAzimuthAngles->Fill(azimuth,hTau->GetBinContent(i+1)*dP*dWeightForTriggeredAzimuth);
		    }
		  //if(hTau->GetBinContent(i+1)*dP>0 )
		  //cout<<hTau->GetBinCenter(i+1)<<"  "<<hTau->GetBinContent(i+1)<<" y:  "<<y<<"  el: "<<elevation<<" az: "<<azimuth<<" dp: "<<dP<<" dDeltaAccept: "<<dDeltaAcceptance<<" prod: "<<hTau->GetBinContent(i+1)*dP<<endl;
		}


	         dDeltaAcceptance*=sin(elevation/180.*pi)*y; //projection of area cell to trajectory
       	         dAcceptance+=dDeltaAcceptance;

                 //if(dDeltaAcceptance>1e-3)
		 //   cout<<"Y: "<<y<<", Elevation: "<<elevation <<", Azimuth: " << azimuth<< ""<<", dDeltaAcceptance: " << dDeltaAcceptance <<" cm^2 "<<endl;

                 hAcceptanceVsAzimuthAndDistance->Fill(azimuth,y,dDeltaAcceptance);
                 hAcceptanceVsElevationAndDistance->Fill(-1*elevation,y,dDeltaAcceptance);
                 hAcceptanceVsDistance->Fill(y,dDeltaAcceptance);
	         skyAcceptance->Fill(azimuth,-1*elevation,dDeltaAcceptance);


	         if(dDeltaAcceptance<1e-10 && y>dMaxFluorescenceDistance) //won't get any more acceptance. The >60 is to make sure we do not miss fluorescence events whic can be seen from the back
                   {
                     cout<<"Breaking after "<<azimuth<<" azimuth"<<endl;
		     break;
		    }
	          //cout<<"distance "<<y<<" prob"<<dDeltaAcceptance<<" elevation  "<<elevation<<endl;
	          
	          azimuth+=DeltaAngleSky;

	          if(azimuth>=MaxAzimuth && y>dMaxFluorescenceDistance)
                       cout<<"The "<<MaxAzimuth<<" degree maximum azimuth angle dMaxCherenkovAzimuthAngle has been reached. Consider making it bigger using SetMaximumShowerViewingAngle(Double_t maxangle)."<<endl; 
	      //Add absorption in the atmosphere between shower and observer
	      //Go over target area and calculate acceptance angle for each dA. Integrate over energy spectrum of taus coming out of the earth at that point. multiplied with detection efficiency(absorption).
	    }//finished looping over all azimuth angles

	  elevation+=DeltaAngleSky;    
	  //cout<<azimuth<<"  "<<elevation<<" "<<dAcceptance<<endl;
	  //multiply with dOmega  DeltaAngleSky*DeltaAngleSky
        }//finished looping over all elevation angles
       //cout<<"finished looping over all elevation angles"<<endl;
      //dAcceptance*=yDelta*DeltaAngleSky/180*pi*y; //multiply area of cell
      //dAcceptance*=DeltaAngleSky/180*pi*DeltaAngleSky/180*pi; //multiply area of solidangle cell
      dIntegratedAcceptance+=dAcceptance;
      
      grDiffAcceptance->SetPoint(p,y,dAcceptance*dConversion); 
      p++;
      cout<<"distance: "<<y<<" differ. acceptance "<<(dAcceptance*dConversion)<<" cm^2 sr; integr. acceptance.: "<<(dIntegratedAcceptance*dConversion)<<" cm^2 sr"<<endl; 
      //cout<<dAcceptance<<"  "<<dConversion<<endl;
      y+=yDelta;
      if(dAcceptance<1e-10 && y>dMaxFluorescenceDistance) //no sense to increase in distance if we can't see any showers now
	break;
    }//end looping over distances



  TCanvas *cAcceptance = new TCanvas("cPointSourceAcceptance","Diffuse Flux Acceptance 360 deg FoV",1700,800);
  cAcceptance->Divide(2,2);
  cAcceptance->cd(1);
  skyAcceptance->Scale(dConversion);
  skyAcceptance->Draw("COLZ");
  cAcceptance->cd(2);
  hAcceptanceVsDistance->Scale(dConversion);
  hAcceptanceVsDistance->Draw("L");
  cAcceptance->cd(3);
  hAcceptanceVsElevationAndDistance->Scale(dConversion); 
  hAcceptanceVsElevationAndDistance->Draw("COLZ"); 
  cAcceptance->cd(4);
  hAcceptanceVsAzimuthAndDistance->Scale(dConversion); 
  hAcceptanceVsAzimuthAndDistance->Draw("COLZ"); 
  cAcceptance->Modified();
  cAcceptance->Update();
  //Save the acceptance into a file
  TString Filename;
  Filename.Form("DiffuseFluxAcceptance_FoV360deg_Index%0.1f_Emin%0.1f_Emax%0.1f.root",nuIndex,log10(dMinEnu),log10(dMaxEnu));
  TFile *f = new TFile(Filename.Data(),"RECREATE");
  f->WriteObject(hAcceptanceVsDistance,"hAcceptanceVsDistance");
  f->WriteObject(skyAcceptance,"skyAcceptance");
  f->WriteObject(hAcceptanceVsAzimuthAndDistance,"hAcceptanceVsAzimuthAndDistance");
  f->WriteObject(hAcceptanceVsElevationAndDistance,"hAcceptanceVsElevationAndDistance");
  f->Close();




  return (dIntegratedAcceptance*dConversion);
}

////////////////////////////////////////////////////////////////////
//
//
// Calculate Integral Sensitiivty
//
//
void TrinitySimUtilities::CalculateIntegralSensitivity()
{

  cout<<"Calculate the integral sensitivity to diffuse fluxes"<<endl;
  TGraph *grSensitivity = new TGraph();

  TGraph *grDiffAcceptance = new TGraph();

  TCanvas *cIntSensitivity = new TCanvas("cIntSensitivity","Integral Sensitivity",750,500);
  cIntSensitivity->Draw();
  cIntSensitivity->SetLogy();
  cIntSensitivity->SetLogx();

  //Move in steps from highest energy to lowest adding acceptance
  Double_t dLogE = dMaxEnu;
  Int_t n=0;
  Double_t dIntegratedAcceptance=0;
  while(dLogE>dMinEnu)
       {
	 Double_t dAcceptance = GetDiffuseAcceptance(dLogE,dLogE+dLogEnergyStep,grDiffAcceptance);
          Double_t dEIndexed = pow(10,-dLogE*nuIndex);
          dIntegratedAcceptance+=dAcceptance
             *dEIndexed*(pow(10,dLogE+dLogEnergyStep)-pow(10,dLogE));

          Double_t dnuFnu = 3 * pow(10,dLogE*2) * dEIndexed / dIntegratedAcceptance;
          cout<<"Energy "<<dLogE<<" integrated acceptance:  "<<dIntegratedAcceptance<<" converted to nuFnu: "<<dnuFnu<<" for power law with index -"<<nuIndex<<endl;
          grSensitivity->SetPoint(n,pow(10,dLogE),dnuFnu);

          n++;
          dLogE-=dLogEnergyStep;
          cIntSensitivity->cd();
          grSensitivity->Draw("alp");
          cIntSensitivity->Modified();
          cIntSensitivity->Update();
       }

  grSensitivity->SetLineWidth(3);
  grSensitivity->SetLineColor(kBlue+3);
  grSensitivity->GetXaxis()->SetTitle("energy [GeV]");
  grSensitivity->GetYaxis()->SetTitle("E^{2} dN/dE [ GeV cm^{-2} s^{-1} sr^{-1} ]");
  grSensitivity->GetYaxis()->SetTitleSize(0.04);
  grSensitivity->GetYaxis()->SetLabelSize(0.04);
  grSensitivity->GetXaxis()->SetTitleSize(0.04);
  grSensitivity->GetXaxis()->SetLabelSize(0.04);
}

//This function calculates the acceptance for point sources 
TH2F* TrinitySimUtilities::GetPointSourceAcceptanceInFOV(Double_t dMinEnu, Double_t dMaxEnu)
{
  cout<<"Running GetPointSourceAcceptanceInFOV for limited FoV from "<<dMinEnu<<" to "<<dMaxEnu<<"log10(E/GeV)"<<endl;
  TH2F *skyAcceptance = new TH2F("skyAcceptance","Integral Point Source Acceptance", 360/DeltaAngleSky+1, -180-DeltaAngleSky/2., 180+DeltaAngleSky/2., 180/DeltaAngleSky+1, -90-DeltaAngleSky/2., 90+DeltaAngleSky/2.); //histo for single angle acceptance plot
  skyAcceptance->GetXaxis()->SetTitle("azimuth angle [degrees]");
  skyAcceptance->GetYaxis()->SetTitle("elevation angle [degrees]");
  skyAcceptance->GetZaxis()->SetTitle("effective area [cm^{2}]");

  TH1F *hAcceptanceVsDistance = new TH1F("hAcceptanceVsDistance","Acceptance vs. Distance from the telescope",(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsDistance->GetXaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsDistance->GetZaxis()->SetTitle("effective area [cm^{2}]");

  TH2F *hAcceptanceVsAzimuthAndDistance = new TH2F("hAzimuthVsDistance","Point Source Acceptance vs. Azimuth and Distance",(int)(360/DeltaAngleAz)+1,-180,180,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsAzimuthAndDistance->GetXaxis()->SetTitle("azimuth (viewing angle) [degrees]");
  hAcceptanceVsAzimuthAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsAzimuthAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2}]");
	
  TH2F *hAcceptanceVsElevationAndDistance = new TH2F("hElevationVsDistance","Point Source Acceptance vs. Elevation and Distance",
                            (int)(MaxElevation / DeltaAngleSky)+1,-1*MaxElevation-0.5*DeltaAngleSky,0.5*DeltaAngleSky,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsElevationAndDistance->GetXaxis()->SetTitle("elevation [degrees]");
  hAcceptanceVsElevationAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsElevationAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2}]");
	
  // Set To all time parameterization
  fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction,0,40,2);
  
  //set the proper values for the energy
  dMinEnu = pow(10,dMinEnu);
  dMaxEnu = pow(10,dMaxEnu);
  Double_t dConversion = yDelta*DeltaAngleAz*pi/180.0; //multiply area of cell 
  dConversion *= 1e10; //from km2 to cm2
  
  Double_t dDeltaAcceptance = 0;

  //looping over distance from telescope w/ incrememnts of yDelta
  Double_t y = yMin;
  Double_t MaxAzimuth = dMaxAzimuthSky;
  
  while(y < yMax)
    {	
            
      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
       	MaxAzimuth = 180.0;
      else
        MaxAzimuth = dMaxAzimuthSky;

      //looping over elevation w/ steps of DeltaAngleSky
      for(int elv = 1; elv <= (int)(MaxElevation / DeltaAngleSky); elv++)
	{
 	  Double_t elevation = elv * DeltaAngleSky;
	  
          //looping over azimuth w/ steps of DeltaAngleSky (-180 to 180)
	  for(int azi = (int)(-1*(MaxAzimuth/DeltaAngleAz)); azi <= (int)(MaxAzimuth / DeltaAngleAz); azi++)
	    {
	      Double_t azimuth = azi * DeltaAngleAz;
	      
	      Double_t dEarth = DistanceThroughEarth(y, elevation, azimuth);
	      GetTauDistribution(dEarth,dMinEnu,dMaxEnu); //tau distribution is calculated
	      
              //looping over azimuth in FoV in steps of DeltaAngleAz. That is
              //an offset of the shower from the center of the camera.
	      for (int iFoVazi= 0; iFoVazi <= (int)(hFOV/2/DeltaAngleAz); iFoVazi++) 
		{
		  
		  Double_t dAzimuthFoV = iFoVazi * DeltaAngleAz;
		  dDeltaAcceptance = 0;
		  Double_t dP = 0;
		  
		  for(int i=0;i<hTau->GetNbinsX();i++)
		    {
		      if(hTau->GetBinContent(i+1)>0)
			{
			  Double_t dPFluorescence = 0.0;
			  Double_t dPCherenkov = 0.0;
			  
			  if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
			    dPFluorescence = PDecayFluorescence(hTau->GetBinCenter(i+1),y,elevation,azimuth,kTRUE,dAzimuthFoV);
			  if( (!bFluorescence || bCombined) )
			    dPCherenkov = PDecay(hTau->GetBinCenter(i+1),y,elevation,azimuth,kTRUE,dAzimuthFoV);
			  if(bCombined)
			    dP = dPFluorescence > dPCherenkov ? dPFluorescence : dPCherenkov;
			  else if(bFluorescence)
			    dP = dPFluorescence;
			  else
			    dP = dPCherenkov;
			  
			  dDeltaAcceptance+=hTau->GetBinContent(i+1)*dP;
			}
		    }
		  //cout<<"dDeltaAcceptance "<<dDeltaAcceptance<<endl;


		  //Get out of the while loop if we do not get any more acceptance. 
		  //The >50 is to make sure we do not miss fluorescence events which can be seen from the back
                  //if(dDeltaAcceptance<1e-10 && y>dMaxFluorescenceDistance)
		  //   continue;

                  //apply the conversion and geometry factors to the acceptance
		  dDeltaAcceptance*=sin(elevation/180.*pi)*y*dConversion;
		  
                  //the acceptances are loaded into the histogram with conversion factors applied
                  if(dDeltaAcceptance>1e-3)
		    cout<<"Y: "<<y<<", Elevation: "<<elevation <<", Azimuth: " << azimuth<< ""<< ", dAzimuthFoV: "<< dAzimuthFoV << ", dDeltaAcceptance: " << dDeltaAcceptance <<" cm^2 "<<endl;
                  
                  hAcceptanceVsAzimuthAndDistance->Fill(azimuth,y,dDeltaAcceptance);
                  hAcceptanceVsElevationAndDistance->Fill(-1*elevation,y,dDeltaAcceptance);
 
                  hAcceptanceVsDistance->Fill(y,dDeltaAcceptance);


                  //Fill the instanteneous acceptance into the skymap
                  //Here we need to pay attention to fill in the right azimuth
                  //angle because PDecay and PDecayFluorescence have azimuth as
                  //as an angle to the line drawn between the telescope and where the tau comes out.
                  //It only references the sky with dAzimuthFoV=0 
                  if (iFoVazi == 0)
		       skyAcceptance->Fill(azimuth, (-1 * elevation), dDeltaAcceptance);

                  //2. case when the neutrino source is in the camera center 
                  //but the shower is not viewed in the camera center
		  else if (iFoVazi != 0 && azi == 0)
		    {// no tau azimuth
		      skyAcceptance->Fill(-1 * dAzimuthFoV, (-1 * elevation),dDeltaAcceptance);
		      skyAcceptance->Fill(dAzimuthFoV, (-1 * elevation),dDeltaAcceptance);
		    }

                   //3. case when the neutrino source is left of the camera center
                   //and the shower is not viewed in the camera center 
		  else if (iFoVazi > 0 && azi < 0)
		    {// gamma2 and gamma 4 triangle
		      skyAcceptance->Fill(-1*(abs(azimuth)+dAzimuthFoV), (-1 * elevation),dDeltaAcceptance);
		      skyAcceptance->Fill((abs(azimuth)+dAzimuthFoV), (-1 * elevation),dDeltaAcceptance);
		    }

                  //4. case when the neutrino source is right of the camera center
                  //and the shower is not viewed in the camera center
		  else if (iFoVazi > 0 && azi > 0)
		    { // gamma1 triangle
		      skyAcceptance->Fill(azimuth-dAzimuthFoV, (-1 * elevation),dDeltaAcceptance);
		      skyAcceptance->Fill(-1*(azimuth-dAzimuthFoV), (-1 * elevation),dDeltaAcceptance);
		    }
                  else
                    cout<<"We should never be here in GetPointSourceAcceptanceInFOV"<<endl;

                  
		}
	    }
	}
      //cout<<"Tau emergence distance: "<<y<<endl;
      y += yDelta; //distance from telescope counter increased by yDelta
    }

  TCanvas *cPointSourceAcceptance = new TCanvas("cPointSourceAcceptance","Point Source Acceptance Limited Field of View",1700,800);
 cPointSourceAcceptance->Divide(2,2);
 cPointSourceAcceptance->cd(1);
 skyAcceptance->Draw("COLZ");
 cPointSourceAcceptance->cd(2);
 hAcceptanceVsDistance->Draw("L");
 cPointSourceAcceptance->cd(3);
 hAcceptanceVsElevationAndDistance->Draw("COLZ"); 
 cPointSourceAcceptance->cd(4);
 hAcceptanceVsAzimuthAndDistance->Draw("COLZ"); 
 cPointSourceAcceptance->Modified();
 cPointSourceAcceptance->Update();
 //Save the acceptance into a file
 TString Filename;
 Filename.Form("PointSourceAcceptance_FoV%0.1fdeg_Index%0.1f_Emin%0.1f_Emax%0.1f.root",hFOV,nuIndex,log10(dMinEnu),log10(dMaxEnu));
 TFile *f = new TFile(Filename.Data(),"RECREATE");
 f->WriteObject(hAcceptanceVsDistance,"hAcceptanceVsDistance");
 f->WriteObject(skyAcceptance,"skyAcceptance");
 f->WriteObject(hAcceptanceVsAzimuthAndDistance,"hAcceptanceVsAzimuthAndDistance");
 f->WriteObject(hAcceptanceVsElevationAndDistance,"hAcceptanceVsElevationAndDistance");
 f->Close();
 return skyAcceptance;
}


//Calculates the acceptance to point sources in the FoV of the telescope with
//360 deg FOV
TH2F* TrinitySimUtilities::GetPointSourceAcceptanceSingleAngle(Double_t dMinEnu, Double_t dMaxEnu)
{
  cout<<"Running GetPointSourceAcceptanceSingleAngle for 360deg horizontal FoV from "<<dMinEnu<<" to "<<dMaxEnu<<"log10(E/GeV)"<<endl;

  TH2F *skyAcceptance = new TH2F("skyAcceptance","Integral Point Source Acceptance for 360 degree FoV", 360/DeltaAngleSky+1, -180-DeltaAngleSky/2., 180+DeltaAngleSky/2., 180/DeltaAngleSky+1, -90-DeltaAngleSky/2., 90+DeltaAngleSky/2.); //histo for single angle acceptance plot
  skyAcceptance->GetXaxis()->SetTitle("Azimuth Angle [degrees]");
  skyAcceptance->GetYaxis()->SetTitle("Elevation Angle [degrees]");
  skyAcceptance->GetZaxis()->SetTitle("Effective Area [cm^{2}]");

  TH1F *hAcceptanceVsDistance = new TH1F("hAcceptanceVsDistance","Acceptance vs. Distance from the telescope",(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsDistance->GetXaxis()->SetTitle("Distance from telescope [km]");
  hAcceptanceVsDistance->GetYaxis()->SetTitle("Effective Area [cm^{2}]");
  
  TH2F *hAcceptanceVsAzimuthAndDistance = new TH2F("hAzimuthVsDistance","Point Source Acceptance vs. Azimuth and Distance",(int)(180/DeltaAngleAz)+2,-0.5*DeltaAngleAz,180+0.5*DeltaAngleAz,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsAzimuthAndDistance->GetXaxis()->SetTitle("azimuth (viewing angle) [degrees]");
  hAcceptanceVsAzimuthAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsAzimuthAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2}]");
	
  TH2F *hAcceptanceVsElevationAndDistance = new TH2F("hElevationVsDistance","Point Source Acceptance vs. Elevation and Distance",
                            (int)(MaxElevation / DeltaAngleSky)+1,-1*MaxElevation-0.5*DeltaAngleSky,0.5*DeltaAngleSky,(yMax-yMin)/yDelta+1,yMin-yDelta/2.,yMax+yDelta/2);
  hAcceptanceVsElevationAndDistance->GetXaxis()->SetTitle("elevation [degrees]");
  hAcceptanceVsElevationAndDistance->GetYaxis()->SetTitle("distance from telescope [km]");
  hAcceptanceVsElevationAndDistance->GetZaxis()->SetTitle("acceptance [cm^{2}]");
	

  // Set the PE parametric function
  fPE = new TF1("fPE",this,&TrinitySimUtilities::myPEfunction,0,40,2);
  
  //set the proper values for the energy
  dMinEnu = pow(10,dMinEnu);
  dMaxEnu = pow(10,dMaxEnu);
  
  Double_t dConversion = yDelta*DeltaAngleAz*pi/180.0; //multiply area of cell taking into account that we have a 360 degree FoV
  dConversion *= 1e10; //from km2 to cm2


  Double_t y = yMin;
  //looping over distance from telescope w/ incrememnts of yDelta
  Double_t MaxAzimuth = dMaxCherenkovAzimuthAngle;
  while(y < yMax) 
    {	
      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
	MaxAzimuth = 180.0;
      else
        MaxAzimuth = dMaxCherenkovAzimuthAngle;


      //looping over elevation w/ steps of DeltaAngleSky
      for(int elv = 1; elv <= (int)(MaxElevation / DeltaAngleSky); elv++) 
	{
	  Double_t elevation = elv * DeltaAngleSky;
	  
	  for(int azi = 0; azi <= (int)(MaxAzimuth / DeltaAngleAz); azi++) //looping over azimuth w/ steps of DeltaAngleAz
	    {
	      Double_t azimuth = azi * DeltaAngleAz;

	      Double_t dEarth = DistanceThroughEarth(y, elevation, azimuth);
	      GetTauDistribution(dEarth,dMinEnu,dMaxEnu); //tau distribution is calculated

	      Double_t dDeltaAcceptance = 0;
	      Double_t dP = 0;
	      for(int i=0;i<hTau->GetNbinsX();i++)
		{
		  if(hTau->GetBinContent(i+1)>0)
		    {
		      Double_t dPFluorescence = 0.0;
		      Double_t dPCherenkov = 0.0;
		      
		      if( bFluorescence || (bCombined && y<dMaxFluorescenceDistance) )
			dPFluorescence = PDecayFluorescence(hTau->GetBinCenter(i+1),y,elevation,azimuth,kFALSE,0);
		      if( (!bFluorescence || bCombined) )
			dPCherenkov = PDecay(hTau->GetBinCenter(i+1),y,elevation,azimuth,kFALSE,0);
		      
		      if(bCombined)
			dP = dPFluorescence > dPCherenkov ? dPFluorescence : dPCherenkov;
		      else if(bFluorescence)
			dP = dPFluorescence;
		      else
			dP = dPCherenkov;
		      
		      dDeltaAcceptance+=hTau->GetBinContent(i+1)*dP;
		    }
		}
	      // cout<<"dDeltaAcceptance "<<dDeltaAcceptance<<endl;
	      dDeltaAcceptance*=sin(elevation/180.*pi)*y*dConversion;


              //apply the conversion and geometry factors to the acceptance

	      cout<<"Y: "<<y<<", Elevation: "<<elevation <<", Azimuth: " << azimuth<< ", dDeltaAcceptance: " << dDeltaAcceptance <<" cm^2"<<endl;
	      //the acceptances are loaded into the histogram with conversion factors applied
	      if(azimuth != 0.0)
		skyAcceptance->Fill(azimuth, (-1 * elevation), dDeltaAcceptance); 
	      skyAcceptance->Fill((-1 * azimuth), (-1 * elevation), dDeltaAcceptance);

              hAcceptanceVsDistance->Fill(y,dDeltaAcceptance);
              hAcceptanceVsAzimuthAndDistance->Fill(azimuth,y,dDeltaAcceptance);
              hAcceptanceVsElevationAndDistance->Fill(-1*elevation,y,dDeltaAcceptance);

	      if(dDeltaAcceptance<1 && y>dMaxFluorescenceDistance)
                { //won't get any more acceptance. The >60 is to make sure we do not miss fluorescence events whic can be seen from the back
                     cout<<"Breaking after "<<azimuth<<" azimuth"<<endl;
		     break;
                }
              
	    }
	}
      cout<<"Tau emergence distance: "<<y<<endl;
      y += yDelta; //distance from telescope counter increased by yDelta
    }
  TCanvas *cPointSourceAcceptance = new TCanvas("cPointSourceAcceptance","Point Source Acceptance For 360 deg FoV",1700,800);
  cPointSourceAcceptance->Divide(2,2);
  cPointSourceAcceptance->cd(1);
  skyAcceptance->Draw("COLZ");
  cPointSourceAcceptance->cd(2);
  hAcceptanceVsDistance->Draw("L");
  cPointSourceAcceptance->cd(3);
  hAcceptanceVsElevationAndDistance->Draw("COLZ"); 
  cPointSourceAcceptance->cd(4);
  hAcceptanceVsAzimuthAndDistance->Draw("COLZ"); 
  cPointSourceAcceptance->Modified();
  cPointSourceAcceptance->Update();
  //Save the acceptance into a file
  TString Filename;
  Filename.Form("PointSourceAcceptance_FoV360deg_Index%0.1f_Emin%0.1f_Emax%0.1f.root",nuIndex,log10(dMinEnu),log10(dMaxEnu));
  TFile *f = new TFile(Filename.Data(),"RECREATE");
  f->WriteObject(hAcceptanceVsDistance,"hAcceptanceVsDistance");
  f->WriteObject(skyAcceptance,"skyAcceptance");
  f->WriteObject(hAcceptanceVsAzimuthAndDistance,"hAcceptanceVsAzimuthAndDistance");
  f->WriteObject(hAcceptanceVsElevationAndDistance,"hAcceptanceVsElevationAndDistance");
  f->Close();
  return skyAcceptance;
}

////////////////////////////////////////////////////////////////////
//
//
// Calculate Point Source Differential Sensitiivty
// The Source is assumed to go through the center of the FoV and the telescope
// is pointing at Declination zero and is located at the equator
//  restricted FoV and 360 deg FoV telescope can be simulated
//
void TrinitySimUtilities::CalculatePointSourceDifferentialSensitivity()
{
    //exposure
    Double_t dExposure=dExposureHours*3600; //exposure time in seconds

    TGraph *grSensitivity = new TGraph();
    grSensitivity->SetLineWidth(3);
    grSensitivity->SetLineColor(kBlue+3);

    TCanvas *cDiffSensitivity = new TCanvas("cDiffSensitivity","Differential Sensitivity",750,500);
    cDiffSensitivity->Draw();
    cDiffSensitivity->SetLogy();
    cDiffSensitivity->SetLogx();

    TGraph *grAcceptance = new TGraph();
    grAcceptance->SetLineWidth(3);
    grAcceptance->SetLineColor(kBlue+3);

    TCanvas *cAcceptance = new TCanvas("cAcceptance","Acceptance",750,500);
    cAcceptance->Draw();
    cAcceptance->SetLogy();
    cAcceptance->SetLogx();
    
    
  

    //Move in steps from lowest to highest energy
    Double_t dLogE = dMinEnu;
    Int_t n=0;
    while(dLogE<=dMaxEnu)
       {
          Double_t dAcceptance = 0;
          Int_t iNonZeroBins = 0;


          TH2F *hAcceptance = 0;
          if(blimFOV)
              hAcceptance = GetPointSourceAcceptanceInFOV(dLogE-dHalfEnergyBinWidth,dLogE+dHalfEnergyBinWidth);
           else
              hAcceptance = GetPointSourceAcceptanceSingleAngle(dLogE-dHalfEnergyBinWidth,dLogE+dHalfEnergyBinWidth);
                    

           //Lets walk down from elevation 0 down to where the effective area
           //becomes again 10% of the peak value.
           Double_t dMaxArea = 0;
           for(int yBins = hAcceptance->GetYaxis()->FindBin(0.0); yBins >1; yBins--)
              {
	        Double_t dSum = 0.0;
                if(blimFOV)
                  {
                    //Find the bin that is one to the right of the center
                    //no integration in X. The point source acceptance already
                    //integrates over the FoV and we assume the source to go
                    //right through the middle of the camera.
                    Int_t xBin = hAcceptance->GetXaxis()->FindBin(0.0);
	            dSum = hAcceptance->GetBinContent(xBin, yBins);
                  }
                else
	          {
	             for(int xBins = 1; xBins <= hAcceptance->GetNbinsX(); xBins++)
                	dSum += hAcceptance->GetBinContent(xBins, yBins);
                  }
                  
                  dMaxArea = dSum>dMaxArea ? dSum : dMaxArea;	 

                  if( dSum>0 )
                      iNonZeroBins++;

                  dAcceptance += dSum; 
                  //cout<<dSum<<"  "<<iNonZeroBins<<endl;

                  //get out of the loop if the effective area reaches 10% of the
                  //peak value seen so far.
                  if(dSum<0.1*dMaxArea)
                       break;
               }
          
         //average the effective area
         dAcceptance= dAcceptance / iNonZeroBins;

         if(dAcceptance>1)
          {
             grAcceptance->SetPoint(n,pow(10,dLogE),dAcceptance);

             Double_t dnuFnu = 3 * pow(10,dLogE*2) / dAcceptance / dExposure / (pow(10,dLogE+dHalfEnergyBinWidth)- pow(10,dLogE-dHalfEnergyBinWidth));
              cout<<"Energy "<<dLogE-dHalfEnergyBinWidth<<" to "<<dLogE+dHalfEnergyBinWidth<<" acceptance:  "<<dAcceptance<<" nuFnu: "<<dnuFnu<<" for power law with index -"<<nuIndex<<endl;
              grSensitivity->SetPoint(n,pow(10,dLogE),dnuFnu);
              cDiffSensitivity->cd();
              grSensitivity->Draw("alp");
              cDiffSensitivity->Modified();
              cDiffSensitivity->Update();

              cAcceptance->cd();
              grAcceptance->Draw("alp");
         //     grAcceptanceMonoEnergy->Draw("lp");
              cAcceptance->Modified();
              cAcceptance->Update();

              n++;
            }
          dLogE+=dLogEnergyStep;
      }


  cDiffSensitivity->cd();
  TLegend *legend = new TLegend(0.53,0.7,0.75,0.88);
  TString legstr;
  legend->AddEntry(grSensitivity,"Integration","l");
//  legend->AddEntry(grSensitivityNuCommunity,"Approximation","l");
  legend->Draw();

  grSensitivity->GetXaxis()->SetTitle("energy [GeV]");
  grSensitivity->GetYaxis()->SetTitle("E^{2} dN/dE [ GeV cm^{-2} s^{-1} ]");
  grSensitivity->GetYaxis()->SetTitleSize(0.04);
  grSensitivity->GetYaxis()->SetLabelSize(0.04);
  grSensitivity->GetXaxis()->SetTitleSize(0.04);
  grSensitivity->GetXaxis()->SetLabelSize(0.04);

  cAcceptance->cd();
  legend = new TLegend(0.755,0.49,0.84,0.63);
  legend->AddEntry(grAcceptance,"Averaged","l");
//  legend->AddEntry(grAcceptanceMonoEnergy,"Mono","l");
  legend->Draw();

  grAcceptance->GetXaxis()->SetTitle("energy [GeV]");
  grAcceptance->GetYaxis()->SetTitle("acceptance [ cm^{2} ]");
  grAcceptance->GetYaxis()->SetTitleSize(0.04);
  grAcceptance->GetYaxis()->SetLabelSize(0.04);
  grAcceptance->GetXaxis()->SetTitleSize(0.04);
  grAcceptance->GetXaxis()->SetLabelSize(0.04);

  TString Filename;
  Double_t dhorizontalFOV = blimFOV ? hFOV : 360; 
     
 SaveCanvas(cDiffSensitivity, "DiffSensitivity");
SaveCanvas(cAcceptance, "Acceptance");

Filename.Form("DifferentialSensitivityPointSourcesTrinity_NuTauSim_%ikmAboveGround_%0.0fPEperSqrmMirror_%0.1fdeghorizontalFOV_%0.1fdegUpperFoV_%0.1fdegLowerFoV_%0.1fdegMinShowerLength_%0.1fns.root",iConfig,dMinimumNumberPhotoelectrons,dhorizontalFOV,atan(tanFoV)/pi*180,dFoVBelow/pi*180.,dMinLength,TriggerWindow[iTrigWin]);
  cDiffSensitivity->SaveAs(Filename.Data());
  Filename.Form("EffectiveAreaTrinityPointSources_NuTauSim_%ikmAboveGround_%0.0fPEperSqrmMirror_%0.1fdeghorizontalFOV_%0.1fdegUpperFoV_%0.1fdegLowerFoV_%0.1fdegMinShowerLength_%0.1fns.root",iConfig,dMinimumNumberPhotoelectrons,dhorizontalFOV,atan(tanFoV)*180/pi,dFoVBelow/pi*180.,dMinLength,TriggerWindow[iTrigWin]);
  cAcceptance->SaveAs(Filename.Data());

  Double_t *dE = grSensitivity->GetX(); 
  Double_t *dF = grSensitivity->GetY(); 
  for(int i=0;i<grSensitivity->GetN();i++)
     {
       cout<<dE[i]<<"  "<<dF[i]<<endl;;
     }
     
  
}

////////////////////////////////////////////////////////////////////
//
//
// Calculate Diffuse Flux Differential Sensitiivty
// for 360 deg FoV and restricted FoV
//
void TrinitySimUtilities::CalculateDiffuseFluxDifferentialSensitivity()
{
    cout<<"Calculating the differential sensitivity for diffuse flux for 10 years and 20% duty cycle"<<endl;

    //exposure
    Double_t dExposure=10*365*24*3600*0.20; //exposure time 10 years in seconds with 20% duty cycle

    TGraph *grDiffAcceptance = new TGraph();

    TGraph *grSensitivity = new TGraph();
    grSensitivity->SetLineWidth(3);
    grSensitivity->SetLineColor(kBlue+3);

    TGraph *grSensitivityNuCommunity = new TGraph();
    grSensitivityNuCommunity->SetLineWidth(3);
    grSensitivityNuCommunity->SetLineColor(kRed+3);

    TCanvas *cDiffSensitivity = new TCanvas("cDiffSensitivity","Differential Sensitivity",750,500);
    cDiffSensitivity->Draw();
    cDiffSensitivity->SetLogy();
    cDiffSensitivity->SetLogx();

    TGraph *grAcceptance = new TGraph();
    grAcceptance->SetLineWidth(3);
    grAcceptance->SetLineColor(kBlue+3);

    TCanvas *cAcceptance = new TCanvas("cAcceptance","Acceptance",750,500);
    cAcceptance->Draw();
    cAcceptance->SetLogy();
    cAcceptance->SetLogx();
  
    TCanvas *cTriggeredAzimuthAngles = new TCanvas("cTriggeredAzimuthAngles","Triggered Azimuth Angles",750,500);
    hTriggeredAzimuthAngles->Draw("HIST");

    //Move in steps from lowest to highest energy
    Double_t dLogE = dMinEnu;
    Int_t n=0;
    while(dLogE<=dMaxEnu)
       {
          Double_t dAcceptance = 0;
          if(blimFOV)
              dAcceptance = GetDiffuseAcceptanceLimitedFoV(dLogE-dHalfEnergyBinWidth,dLogE+dHalfEnergyBinWidth,grDiffAcceptance);
           else
              dAcceptance = GetDiffuseAcceptance(dLogE-dHalfEnergyBinWidth,dLogE+dHalfEnergyBinWidth,grDiffAcceptance);

          if(dAcceptance>1)
           {
              grAcceptance->SetPoint(n,pow(10,dLogE),dAcceptance);

              Double_t dnuFnu = 0;
              if(bMonoNu)
                {
                    dnuFnu = 3 * 2.44 / dAcceptance / dExposure / log(10) / (2*dHalfEnergyBinWidth) * pow(10,dLogE); //2.44 is from Feldman Cousin 90% confidence upper limit
                    cout<<"NuCommunity Definition: "<<dnuFnu<<" acceptance at center energy: "<<dAcceptance<<endl;
                    cout<<"Energy "<<dLogE-dHalfEnergyBinWidth<<" to "<<dLogE+dHalfEnergyBinWidth<<" acceptance:  "<<dAcceptance<<" nuFnu: "<<dnuFnu<<" for power law with index -"<<nuIndex<<endl;
                }
               else
                {
                    dnuFnu = 3 * 2.44 * pow(10,dLogE*2) / dAcceptance / dExposure / (pow(10,dLogE+dHalfEnergyBinWidth)- pow(10,dLogE-dHalfEnergyBinWidth));
                    cout<<"Energy "<<dLogE-dHalfEnergyBinWidth<<" to "<<dLogE+dHalfEnergyBinWidth<<" acceptance:  "<<dAcceptance<<" nuFnu: "<<dnuFnu<<" for power law with index -"<<nuIndex<<endl;
                }
              grSensitivity->SetPoint(n,pow(10,dLogE),dnuFnu);
              cDiffSensitivity->cd();
              grSensitivity->Draw("alp");
              cDiffSensitivity->Modified();
              cDiffSensitivity->Update();

              cAcceptance->cd();
              grAcceptance->Draw("alp");
              cAcceptance->Modified();
              cAcceptance->Update();

              cTriggeredAzimuthAngles->cd();
              cTriggeredAzimuthAngles->Modified();
              cTriggeredAzimuthAngles->Update();

              n++;
            }
          dLogE+=dLogEnergyStep;
       }


  cDiffSensitivity->cd();
  TLegend *legend = new TLegend(0.53,0.7,0.75,0.88);
  TString legstr;
  legend->AddEntry(grSensitivity,"Integration","l");
  legend->AddEntry(grSensitivityNuCommunity,"Approximation","l");
  legend->Draw();

  grSensitivity->GetXaxis()->SetTitle("energy [GeV]");
  grSensitivity->GetYaxis()->SetTitle("E^{2} dN/dE [ GeV cm^{-2} s^{-1} sr^{-1} ]");
  grSensitivity->GetYaxis()->SetTitleSize(0.04);
  grSensitivity->GetYaxis()->SetLabelSize(0.04);
  grSensitivity->GetXaxis()->SetTitleSize(0.04);
  grSensitivity->GetXaxis()->SetLabelSize(0.04);

  cAcceptance->cd();
  legend = new TLegend(0.755,0.49,0.84,0.63);
  legend->AddEntry(grAcceptance,"Averaged","l");
  legend->Draw();

  grAcceptance->GetXaxis()->SetTitle("energy [GeV]");
  grAcceptance->GetYaxis()->SetTitle("acceptance [ cm^{2} sr ]");
  grAcceptance->GetYaxis()->SetTitleSize(0.04);
  grAcceptance->GetYaxis()->SetLabelSize(0.04);
  grAcceptance->GetXaxis()->SetTitleSize(0.04);
  grAcceptance->GetXaxis()->SetLabelSize(0.04);

  hTriggeredAzimuthAngles->SetLineWidth(3);
  hTriggeredAzimuthAngles->SetLineColor(kBlue+3);
  hTriggeredAzimuthAngles->GetYaxis()->SetTitleSize(0.04);
  hTriggeredAzimuthAngles->GetYaxis()->SetLabelSize(0.04);
  hTriggeredAzimuthAngles->GetXaxis()->SetTitleSize(0.04);
  hTriggeredAzimuthAngles->GetXaxis()->SetLabelSize(0.04);
  hTriggeredAzimuthAngles->Scale(1.0/hTriggeredAzimuthAngles->Integral(),"nosw2");

  TString Filename;
  Double_t dhorizontalFOV = blimFOV ? hFOV : 360; 
  Filename.Form("DifferentialSensitivityDiffuseFluxTrinity_NuTauSim_%ikmAboveGround_%0.0fPEperSqrmMirror_%0.1fdeghorizontalFOV_%0.1fdegUpperFoV_%0.1fdegLowerFoV_%0.1fdegMinShowerLength_%0.1fns.root",iConfig,dMinimumNumberPhotoelectrons,dhorizontalFOV,atan(tanFoV)/pi*180,dFoVBelow/pi*180.,dMinLength,TriggerWindow[iTrigWin]);
  cDiffSensitivity->SaveAs(Filename.Data());
  Filename.Form("AcceptanceDiffuseFluxTrinity_NuTauSim_%ikmAboveGround_%0.0fPEperSqrmMirror_%0.1fdeghorizontalFOV_%0.1fdegUpperFoV_%0.1fdegLowerFoV_%0.1fdegMinShowerLength_%0.1fns.root",iConfig,dMinimumNumberPhotoelectrons,dhorizontalFOV,atan(tanFoV)/pi*180,dFoVBelow/pi*180.,dMinLength,TriggerWindow[iTrigWin]);
  cAcceptance->SaveAs(Filename.Data());
  Filename.Form("TriggeredAzimuthDiffuseFluxTrinity_NuTauSim_%ikmAboveGround_%0.0fPEperSqrmMirror_%0.1fdeghorizontalFOV_%0.1fdegUpperFoV_%0.1fdegLowerFoV_%0.1fdegMinShowerLength_%0.1fns.root",iConfig,dMinimumNumberPhotoelectrons,dhorizontalFOV,atan(tanFoV)/pi*180,dFoVBelow/pi*180.,dMinLength,TriggerWindow[iTrigWin]);
  cTriggeredAzimuthAngles->SaveAs(Filename.Data());

  Double_t *dE = grSensitivity->GetX(); 
  Double_t *dF = grSensitivity->GetY(); 
  for(int i=0;i<grSensitivity->GetN();i++)
     {
       cout<<dE[i]<<"  "<<dF[i]<<endl;;
     }
     
  SaveCanvas(cDiffSensitivity, "DiffSensitivity");
  SaveCanvas(cAcceptance, "Acceptance");

}

// Function to get the current date and time as a formatted string
std::string TrinitySimUtilities::getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
    return std::string(buf);
}

// Function to create a directory if it doesn't exist
void TrinitySimUtilities::createDirectory(const std::string& path) {
    if (gSystem->AccessPathName(path.c_str())) { // Check if directory exists
        gSystem->mkdir(path.c_str(), true);      // Create directory (with parents)
        std::cout << "Created directory: " << path << std::endl;
    } else {
        std::cout << "Directory already exists: " << path << std::endl;
    }
}

// Method to set the folder path based on current date and time
void TrinitySimUtilities::SetOutputFolder() {
    // Get the current date and time for the folder name
    std::string timestamp = getCurrentDateTime();
    outputFolder = "output_" + timestamp;

    // Create the folder
    createDirectory(outputFolder);
    
    std::cout << "Output folder created: " << outputFolder << std::endl;
}

// Getter for the output folder path
std::string TrinitySimUtilities::GetOutputFolder() const {
    return outputFolder;
}

// Method to save a canvas to the output folder
void TrinitySimUtilities::SaveCanvas(TCanvas* canvas, const std::string& canvasName) {
    std::string filePath_pdf = outputFolder + "/" + canvasName + ".pdf";
    canvas->SaveAs(filePath_pdf.c_str());
    std::string filePath_root = outputFolder + "/" + canvasName + ".root";
    canvas->SaveAs(filePath_root.c_str());
    std::cout << "Canvas saved to: " << outputFolder << std::endl;
}

