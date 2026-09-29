#include "SEvent.h"

using namespace std;

ClassImp(SEvent);
SEvent::SEvent(int size){
    simrun = "noname";
    date = 20231003;
    simEvent = 0;
    triggered = false;
    posX = vector<Float_t>(size);
    posY = vector<Float_t>(size);
    polarangle = 0.0;
    azimuthangle = 0.0;
    telposX = 0.0;
    telposY = 0.0;
    telposZ = 0.0;
    telradius = 0.0;
    XMax = 0.0;
    NMax = 0.0;
    X0Max = 0.0;
    lambda = 0.0;
    neutrinoenergy = 0.0;
    decaylength = 0.0;
    showerstartX = 0.0;
    showerstartY = 0.0;
    showerstartZ = 0.0;
    tauemergenceprob = 0.0;
    EmergencepointX = 0.0;
    EmergencepointY = 0.0;
    EmergencepointZ = 0.0;
    globaltheta = 0.0;
    globalphi = 0.0;
    tauenergy = 0.0;
}

SEvent::~SEvent(){

}
// Setters

void SEvent::SetSimRun(std::string temp){
    simrun = temp;
}

void SEvent::SetDate(Int_t temp){
    date = temp;
}
void SEvent::SetSimEventNumber(Int_t temp){
    simEvent = temp;
}

void SEvent::SetTriggered(Bool_t temp){
    triggered = temp;
}

void SEvent::SetPolarAngle(Float_t temp){
    polarangle = temp;
}

void SEvent::SetAzimuthAngle(Float_t temp){
    azimuthangle = temp;
}

void SEvent::SetTelescope_Xpos(Float_t temp){
    telposX = temp;
}

void SEvent::SetTelescope_Ypos(Float_t temp){
    telposY = temp;
}

void SEvent::SetTelescope_Zpos(Float_t temp){
    telposZ = temp;
}

void SEvent::SetTelescope_Radius(Float_t temp){
    telradius = temp;
}

void SEvent::SetXMax(Float_t temp){
    XMax = temp;
}

void SEvent::SetNMax(Float_t temp){
    NMax = temp;
}

void SEvent::SetX0Max(Float_t temp){
    X0Max = temp;
}

void SEvent::SetLambda(Float_t temp){
    lambda = temp;
}

void SEvent::SetNeutrinoEnergy(Float_t temp){
    neutrinoenergy = temp;
}

void SEvent::SetDecayLength(Float_t temp){
    decaylength = temp;
}

void SEvent::SetShowerStartX(Float_t temp){
    showerstartX = temp;
}

void SEvent::SetShowerStartY(Float_t temp){
    showerstartY = temp;
}

void SEvent::SetShowerStartZ(Float_t temp){
    showerstartZ = temp;
}

void SEvent::SetTauEmergenceProb(Float_t temp){
    tauemergenceprob = temp;
}

void SEvent::SetEmergencePointX(Float_t temp){
    EmergencepointX = temp;
}

void SEvent::SetEmergencePointY(Float_t temp){
    EmergencepointY = temp;
}

void SEvent::SetEmergencePointZ(Float_t temp){
    EmergencepointZ = temp;
}

void SEvent::SetGlobeTheta(Float_t temp){
    globaltheta = temp;
}

void SEvent::SetGlobePhi(Float_t temp){
    globalphi = temp;
}

void SEvent::SetTauEnergy(Float_t temp){
    tauenergy = temp;
}

void SEvent::SetPosX(vector<Float_t> temp){
    posX = temp;
}

void SEvent::SetPosY(vector<Float_t> temp){
    posY = temp;
}

void SEvent::SetTilt(Float_t temp){
    tilt = temp;
}

void SEvent::SetAzimuth(Float_t temp){
    azimuth = temp;
}

void SEvent::SetZenith(Float_t temp){
    zenith = temp;
}

// Getters

std::string SEvent::GetSumRun(){
    return simrun;
}

Int_t SEvent::GetDate(){
    return date;
}

Int_t SEvent::GetSimEventNumber(){
    return simEvent;
}

Bool_t SEvent::GetTriggered(){
    return triggered;
}

vector<Float_t> SEvent::GetPosX(){
    return posX;
}

vector<Float_t> SEvent::GetPosY(){
    return posY;
}

Float_t SEvent::GetPolarAngle(){
    return polarangle;
}

Float_t SEvent::GetAzimuthAngle(){
    return azimuthangle;
}

Float_t SEvent::GetTelescope_Xpos(){
    return telposX;
}

Float_t SEvent::GetTelescope_Ypos(){
    return telposY;
}

Float_t SEvent::GetTelescope_Zpos(){
    return telposZ;
}

Float_t SEvent::GetTelescope_Radius(){
    return telradius;
}

Float_t SEvent::GetXMax(){
    return XMax;
}

Float_t SEvent::GetNMax(){
    return NMax;
}

Float_t SEvent::GetX0Max(){
    return X0Max;
}

Float_t SEvent::GetLambda(){
    return lambda;
}

Float_t SEvent::GetNeutrinoEnergy(){
    return neutrinoenergy;
}

Float_t SEvent::GetDecayLength(){
    return decaylength;
}

Float_t SEvent::GetShowerStartX(){
    return showerstartX;
}

Float_t SEvent::GetShowerStartY(){
    return showerstartY;
}

Float_t SEvent::GetShowerStartZ(){
    return showerstartZ;
}

Float_t SEvent::GetTauEmergenceProb(){
    return tauemergenceprob;
}

Float_t SEvent::GetEmergencePointX(){
    return EmergencepointX;
}

Float_t SEvent::GetEmergencePointY(){
    return EmergencepointY;
}

Float_t SEvent::GetEmergencePointZ(){
    return EmergencepointZ;
}

Float_t SEvent::GetGlobeTheta(){
    return globaltheta;
}

Float_t SEvent::GetGlobePhi(){
    return globalphi;
}

Float_t SEvent::GetTauEnergy(){
    return tauenergy;
}

Float_t SEvent::GetTilt(){
    return tilt;
}

Float_t SEvent::GetAzimuth(){
    return azimuth;
}

Float_t SEvent::GetZenith(){
    return zenith;
}
