#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TBox.h>
#include <set>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
#include <sstream>
#include <TPaletteAxis.h>
#include <TEllipse.h>
#include <IUtilities.h>
#include <TObjString.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <TArrow.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <TLatex.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TBox.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TFile.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TMath.h>
#include <TPrincipal.h>
#include "Math/Integrator.h"
#include <TPaveText.h>
#include <random>
#include <utility>
#include <CEvent.h>
#include <Event.h>
#include <IEvent.h>
#include <IPlotTools.h>
#include <ISiPM.h>
#include <SEvent.h>




IUtilities *util;
IPlotTools *plottools;
SEvent *simRun=0;
IEvent *ev=0;
CEvent *cev;
TFile *file;
TFile *fileOutput;
TFile *fileOutputPlots;
TTree *treeSims;
TTree *treeNew;

TH1F *hPhotonX=0;
TH1F *hPhotonY=0;
TH1F *hPhotonDcosX=0;
TH1F *hPhotonDcosY=0;
TH1F *hPhotonZ=0;
TH2F* hPhotonDist1x=0;
TH2F* hPhotonDist1x6mm=0;
TH2F* hPhotonDist4x=0;

TCanvas *canvas=0;



std::vector<float>* vPhotonX = 0;
std::vector<float>* vPhotonY = 0;
std::vector<float>* vPhotonDcosX = 0;
std::vector<float>* vPhotonDcosY = 0;
int nResMult = 100;

int CameraStep = 16;
float CameraMin = -0.5;
float CameraMax = 15.5;
float RadtoDeg = 57.2958;
float PtoPEconversion = 0.4*0.83;
// 0.4? photon detection effiency 
// 0.83 is the average mirror reflectivity 
// simulations accounts for atmopsheric absorbtion

// ---------------------------------------------------------------------------
// Simulation source. Selected with the 4th argument, GROPT or CARE.
//   GROPT -> ray traced photon positions, tree T1, photonX/photonY/...
//   CARE  -> simulated FADC traces,       tree Events/T0, vFADCTraces0..255
// The two produce different histogram contents but share every label, the
// ISOG derived shower information and the output file/tree layout.
// ---------------------------------------------------------------------------
std::string simType = "GROPT";

// CARE readout constants (taken from the Sofia_CARE_Simanalysis branch)
const int   kNPixels     = 256;   // one vFADCTraces<N> branch per pixel
const int   kCareTimeBin = 25;    // sample used to build the camera image
const int   kCarePedBins = 10;    // leading samples averaged for the pedestal
const float kCareADCtoPE = 24.1;  // ADC counts per photo electron

std::vector<int>* vFADCTraces[kNPixels] = {};

// Size of the event label drawn over the displays. The label is split across
// two lines, so each half fits across a PDF page at this size.
const float kLabelTextSize = 0.025;

// Pad margins. The top one is opened up so the two line label sits above the
// frame instead of over the top of the image. The histogram title still draws
// above the label, hence the gap left between kLabelY2 and 1.
const float kPadLeft   = 0.10;
const float kPadRight  = 0.15;   // room for the colour palette
const float kPadBottom = 0.10;
const float kPadTop    = 0.18;   // header band for the label
const float kLabelY1   = 0.83;   // just above the frame at 1 - kPadTop = 0.82
const float kLabelY2   = 0.92;   // just below the histogram title

// Histogram title size. ROOT auto-shrinks long titles to fit the pad, which
// made them unreadable, so it is pinned here instead.
const float kTitleFontSize = 0.030;

// Gap between the colour palette's numbers and its axis title. At the ROOT
// default of 1.0 the title sits on top of the numbers.
const float kZTitleOffset = 1.4;

// Canvas geometry. Square plot area needs W*(1-kPadLeft-kPadRight) to equal
// H*(1-kPadBottom-kPadTop), i.e. W = H*0.72/0.75 = 0.96*H per pad. Height is
// set first so each pad stays wide enough for the title.
const int kPadPixelsH  = 1300;                 // canvas height
const int kPadPixelsW  = 1248;                 // one square pad (0.96 * 1300)

// Usable drawing area on the A4 sheet, in cm. The canvas is fitted inside this
// at its own aspect so it fills the page without running off the edge.
const double kPdfAreaLongCm  = 28.0;
const double kPdfAreaShortCm = 19.0;

// Trigger threshold in PE on the brightest pixel, shared by both pipelines.
const float kTriggerPE = 20.0;

// Tilt sub-directory. daviddata writes 6 decimals, the CARE branch wrote 7,
// so both are tried and the one that exists is used.
const char* kTiltDirs[] = {"Tilt_91.560000", "Tilt_91.5600000"};
const int   kNTiltDirs  = 2;


// std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Simulations/TDemSims/";
std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator/";
std::string dataDir = "/storage/osg-otte1/shared/TrinityDemonstrator/simdata/roots/diffuse/";
std::string outDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/SimulationAnalysis/data/";

// std::string OutputFileRoot=Form("%sSimulations20250529.root",outDir.c_str());

// Define a struct to represent one simulation event
// struct SimEventData {
//     float tb_time;
//     float cobo_time;
//     float unix_time;
//     float eventType;
//     float signalValue;
//     float roiMusicID;
//     float roiPixelID;
// };

// struct SimRunData{
//     float energy;
//     float time;
//     float posX;
//     float posY;
//     // std::vector<float> float posZ;
//     float zenith;
//     float azimuth;
//     float bsm;
//     float tilt;
// };

// struct SimAuxData {
//     Float_t IcorrectionFactor;
//     Float_t sunAzimuth;
//     Float_t sunElevation;
//     Float_t moonAzimuth;
//     Float_t moon_Elevation;
//     Float_t moon_Illumination;
//     Float_t CameraRA;
//     Float_t CameraDEC;
//     Float_t TrigEvent;
//     Float_t hv;
//     Float_t hvc;
//     Float_t ucTemp;
//     Float_t siabMPWR;
//     Float_t hvSW;
//     Float_t sipmTemp;
//     Float_t siabCurr;
//     Float_t ASADcurr;
//     Float_t TBcurr;
//     Float_t outsideTempature;
//     Float_t humidity;
// };
void DrawMUSICBoundaries();
void create_root_file(TH2F* hist, TTree* treeNew);
void create_root_file_care(TH2F* hist, TTree* treeNew, std::vector<int>* traces[]);
void SetPdfPageToCanvas(TCanvas *c);
std::ifstream& GotoLine(std::ifstream& file, unsigned int num);
std::string ResolveTiltDir(const std::string &runDir, const std::string &stage);
bool LoadISOG(SEvent *simRun, const std::string &ISOGFilePath, int showerIndex);


// Returns <runDir>/<stage>/<tilt>/ for whichever tilt spelling is on disk,
// or an empty string when the stage folder is missing altogether.
std::string ResolveTiltDir(const std::string &runDir, const std::string &stage){
    for(int i = 0; i < kNTiltDirs; i++){
        std::string candidate = Form("%s%s/%s/", runDir.c_str(), stage.c_str(), kTiltDirs[i]);
        DIR *d = opendir(candidate.c_str());
        if(d){
            closedir(d);
            return candidate;
        }
    }
    return std::string();
}


// Reads one shower row out of the ISOG text file into the SEvent. The file has
// two header lines, so event <showerIndex> lives on line <showerIndex>+3.
// Shared by GROPT and CARE so both carry identical shower information.
bool LoadISOG(SEvent *simRun, const std::string &ISOGFilePath, int showerIndex){
    std::ifstream ISOGfile(ISOGFilePath.c_str());
    if(!ISOGfile.is_open()){
        cout << "Could not open ISOG file: " << ISOGFilePath << endl;
        return false;
    }

    std::ifstream& file = GotoLine(ISOGfile, showerIndex + 3);
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    cout << "ISOG line " << showerIndex + 3 << ": " << line << endl;

    std::vector<Float_t> isogValues;
    Float_t tmpValue;
    while (iss >> tmpValue) {
            isogValues.push_back(tmpValue);
    }
    if(isogValues.empty()){
        cout << "ISOG line empty for shower " << showerIndex << endl;
        return false;
    }

    if (isogValues.size() > 0) simRun->SetPolarAngle(isogValues[0]);
    cout << "Polar Angle: " << isogValues[0] << endl;
    if (isogValues.size() > 1) simRun->SetAzimuthAngle(isogValues[1]);
    cout << "Azimuth Angle: " << isogValues[1] << endl;
    if (isogValues.size() > 2) simRun->SetTelescope_Xpos(isogValues[2]);
    cout << "Telescope X Position: " << isogValues[2] << endl;
    if (isogValues.size() > 3) simRun->SetTelescope_Ypos(isogValues[3]);
    cout << "Telescope Y Position: " << isogValues[3] << endl;
    if (isogValues.size() > 4) simRun->SetTelescope_Zpos(isogValues[4]);
    cout << "Telescope Z Position: " << isogValues[4] << endl;
    if (isogValues.size() > 5) simRun->SetTelescope_Radius(isogValues[5]);
    cout << "Telescope Radius: " << isogValues[5] << endl;
    if (isogValues.size() > 6) simRun->SetXMax(isogValues[6]);
    cout << "XMax: " << isogValues[6] << endl;
    if (isogValues.size() > 7) simRun->SetNMax(isogValues[7]);
    cout << "NMax: " << isogValues[7] << endl;
    if (isogValues.size() > 8) simRun->SetX0Max(isogValues[8]);
    cout << "X0Max: " << isogValues[8] << endl;
    if (isogValues.size() > 9) simRun->SetLambda(isogValues[9]);
    cout << "Lambda: " << isogValues[9] << endl;
    if (isogValues.size() > 10) simRun->SetNeutrinoEnergy(isogValues[10]);
    cout << "Neutrino Energy: " << isogValues[10] << endl;
    if (isogValues.size() > 11) simRun->SetDecayLength(isogValues[11]);
    cout << "Decay Length: " << isogValues[11] << endl;
    if (isogValues.size() > 12) simRun->SetShowerStartX(isogValues[12]);
    cout << "Shower Start X: " << isogValues[12] << endl;
    if (isogValues.size() > 13) simRun->SetShowerStartY(isogValues[13]);
    cout << "Shower Start Y: " << isogValues[13] << endl;
    if (isogValues.size() > 14) simRun->SetShowerStartZ(isogValues[14]);
    cout << "Shower Start Z: " << isogValues[14] << endl;
    if (isogValues.size() > 15) simRun->SetTauEmergenceProb(isogValues[15]);
    cout << "Tau Emergence Probability: " << isogValues[15] << endl;
    if (isogValues.size() > 16) simRun->SetEmergencePointX(isogValues[16]);
    cout << "Emergence Point X: " << isogValues[16] << endl;
    if (isogValues.size() > 17) simRun->SetEmergencePointY(isogValues[17]);
    cout << "Emergence Point Y: " << isogValues[17] << endl;
    if (isogValues.size() > 18) simRun->SetEmergencePointZ(isogValues[18]);
    cout << "Emergence Point Z: " << isogValues[18] << endl;
    if (isogValues.size() > 19) simRun->SetGlobeTheta(isogValues[19]);
    cout << "Globe Theta: " << isogValues[19] << endl;
    if (isogValues.size() > 20) simRun->SetGlobePhi(isogValues[20]);
    cout << "Globe Phi: " << isogValues[20] << endl;
    if (isogValues.size() > 21) simRun->SetTauEnergy(isogValues[21]);
    cout << "Tau Energy: " << isogValues[21] << endl;

    return true;
}

std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}


void DrawMUSICBoundaries()
{
        TBox *b = new TBox(-48.5,-48.5,-36.5,-24.5);
        b->SetFillStyle(0);
        b->SetLineColor(kRed);
        for(int i=1; i<32; i++)
        {
                TBox *bn = (TBox*)b->Clone();
                bn->SetX1((i%8)*12-48.5);
                bn->SetX2((i%8)*12-36.5);
                bn->SetY1((i/8)*24-48.5);
                bn->SetY2((i/8)*24-24.5);
                bn->Draw();
        }
}