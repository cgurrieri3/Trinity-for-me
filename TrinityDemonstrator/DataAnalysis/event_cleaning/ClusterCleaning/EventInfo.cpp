#include "EventInfo.h"

using namespace std;

ClassImp(EventInfo);
EventInfo::EventInfo(int size){
    date = 0;
    file = "";
    event = 0;
    hpanel1 = nullptr;
    // eventBranch = 1 test
    // eventBranch = 9 HLED
    eventBranch = 0;

    // flag = 0 default flag
    // 1, pre cleaned before first cut
    // 2, cleaned after  first cuts (panel2)
    // 3, cleaned after second  cuts(panel 3)
    // 4, cleaned after 3 set of  cut (panel 4)
    // 5 Suviving Event from the  event cleaning
    // 9, HLED event

    flag = 0; // default null flag 
    hpanel4 = nullptr;
    
    w = 0;
    l = 0;
    WLratio = 0;
    Angle = 0;
    r1 = 0;
    r2 = 0;
    SurvivngPixels =0; 
    Size =  0;
    Conc  = 0;
    TriggeredPixelID = 0;
    TriggeredMUSICID = 0;
    RMS_AvgAmp = 0;
    RMS = 0;
    AvgAmp = 0;
    Area = 0;
    SaturatedPixelIDs.clear(); // starts empty, filled with the IDs of the saturated pixels
    COGx = 0;
    COGy = 0;
    CoreRatio = 0;
    NumberOfCores = 0;
    M3Long = 0;
    M3LongPow3 = 0;
    UpDownSym = 0;

    Distance2MajorAxis = 0;
    RMSMajorAxis = 0;
    WeightedRMSMajorAxis = 0;
    PixelsonMajorAxis = 0;
    PixelsoffMajorAxis = 0;
    RatioPixelsMajorAxis = 0;
}

EventInfo::~EventInfo(){

}

void EventInfo::Clear() {
    date = 0;
    file = "";
    event = 0;
    hpanel1 = nullptr;
    eventBranch = 0;
    // flag = 0 default flag
    // 1, pre cleaned before first cut
    // 2, cleaned after  first cuts (panel2)
    // 3, cleaned after second  cuts(panel 3)
    // 4, cleaned after 3 set of  cut (panel 4)
    // 5 Suviving Event from the  event cleaning
    // 9, HLED event

    flag = 0; // default null flag 
    hpanel4 = nullptr;
    
    w = 0;
    l = 0;
    WLratio = 0;
    Angle = 0;
    r1 = 0;
    r2 = 0;
    SurvivngPixels =0; 
    Size =  0;
    Conc  = 0;
    TriggeredPixelID = 0;
    TriggeredMUSICID = 0;
    RMS_AvgAmp = 0;
    RMS = 0;
    AvgAmp = 0;
    Area = 0;
    SaturatedPixelIDs.clear(); // starts empty, filled with the IDs of the saturated pixels
    COGx = 0;
    COGy = 0;
    CoreRatio = 0;
    NumberOfCores = 0;
    M3Long = 0;
    M3LongPow3 = 0;
    UpDownSym = 0;

    Distance2MajorAxis = 0;
    RMSMajorAxis = 0;
    WeightedRMSMajorAxis = 0;
    PixelsonMajorAxis = 0;
    PixelsoffMajorAxis = 0;
    RatioPixelsMajorAxis = 0;
}

// Setters
void EventInfo::SetDate(Int_t v) { date = v; }
void EventInfo::SetFile(string v) { file = v; }
void EventInfo::SetEvent(Int_t v) { event = v; }
void EventInfo::SetHPanel1(TH2F* ptr) { hpanel1 = ptr; }
void EventInfo::SetEventFlag(Int_t v) {flag = v; }
void EventInfo::SetEventBranch(Int_t v) {eventBranch = v; }
void EventInfo::SetHPanel4(TH2F* ptr) { hpanel4 = ptr; }
void EventInfo::SetW(Float_t v) { w = v; }
void EventInfo::SetL(Float_t v) { l = v; }
void EventInfo::SetWLRatio(Float_t v) { WLratio = v; }
void EventInfo::SetAngle(Float_t v) { Angle = v; }
void EventInfo::Setr1(Float_t v) { r1 = v; }
void EventInfo::Setr2(Float_t v) { r2 = v; }
void EventInfo::SetSurvivngPixels(Int_t v) { SurvivngPixels = v; }
void EventInfo::SetSize(Float_t v) { Size = v; }
void EventInfo::SetConc(Float_t v) { Conc = v; }
void EventInfo::SetTriggeredPixelID(Int_t v) { TriggeredPixelID = v; }
void EventInfo::SetTriggeredMUSICID(Int_t v) { TriggeredMUSICID = v; }
void EventInfo::SetRMS_AvgAmp(Float_t v) { RMS_AvgAmp = v; }
void EventInfo::SetAvgAmp(Float_t v) {  AvgAmp = v; }
void EventInfo::SetRMS(Float_t v)  { RMS = v; }
void EventInfo::SetArea(Float_t v) { Area = v; }
void EventInfo::SetSaturatedPixels(std::vector<Int_t> v) {SaturatedPixelIDs = v;}
void EventInfo::SetCOGx(Float_t v) {COGx = v; }
void EventInfo::SetCOGy(Float_t v) {COGy = v; }
void EventInfo::SetCoreRatio(Float_t v) {CoreRatio = v; }
void EventInfo::SetNumberOfCores(Float_t v) {NumberOfCores = v; }
void EventInfo::SetM3Long(Float_t v) {M3Long = v; }
void EventInfo::SetM3LongPow3(Float_t v) {M3LongPow3 = v; }
void EventInfo::SetUpDownSym(Float_t v) {UpDownSym = v; }

void EventInfo::SetDistance2MajorAxis(Float_t v) {Distance2MajorAxis = v; }
void EventInfo::SetRMSMajorAxis(Float_t v) {RMSMajorAxis = v; }
void EventInfo::SetWeightedRMSMajorAxis(Float_t v) {WeightedRMSMajorAxis = v; }
void EventInfo::SetPixelsonMajorAxis(Int_t v) {PixelsonMajorAxis = v; }
void EventInfo::SetPixelsoffMajorAxis(Int_t v) {PixelsoffMajorAxis = v; }
void EventInfo::SetRatioPixelsMajorAxis(Float_t v) {RatioPixelsMajorAxis = v; }


// Getters
Int_t EventInfo::GetDate() const { return date; }
string EventInfo::GetFile() const { return file; }
Int_t EventInfo::GetEvent() const { return event; }
TH2F* EventInfo::GetHPanel1() const { return hpanel1; }
Int_t EventInfo::GetEventFlag() const { return flag;}
Int_t EventInfo::GetEventBranch() const {return eventBranch; }
TH2F* EventInfo::GetHPanel4() const { return hpanel4; }
Float_t EventInfo::GetW() const { return w; }
Float_t EventInfo::GetL() const { return l; }
Float_t EventInfo::GetWLRatio() const { return WLratio; }
Float_t EventInfo::GetAngle() const { return Angle; }
Float_t EventInfo::Getr1() const { return r1; }
Float_t EventInfo::Getr2() const { return r2; }
Int_t EventInfo::GetSurvivngPixels() const { return SurvivngPixels; }
Float_t EventInfo::GetSize() const { return Size; }
Float_t EventInfo::GetConc() const { return Conc; }
Int_t EventInfo::GetTriggeredPixelID() const { return TriggeredPixelID; }
Int_t EventInfo::GetTriggeredMUSICID() const { return TriggeredMUSICID; }
Float_t EventInfo::GetRMS_AvgAmp() const { return RMS_AvgAmp; }
Float_t EventInfo::GetAvgAmp() const { return AvgAmp; }
Float_t EventInfo::GetRMS() const { return RMS; }
Float_t EventInfo::GetArea() const { return Area; }
std::vector<Int_t> EventInfo::GetSaturatedPixels() const { return SaturatedPixelIDs; }
Float_t EventInfo::GetCOGx() const { return COGx; }
Float_t EventInfo::GetCOGy() const { return COGy; }
Float_t EventInfo::GetCoreRatio() const { return CoreRatio; }
Float_t EventInfo::GetNumberOfCores() const { return NumberOfCores; }
Float_t EventInfo::GetM3Long() const { return M3Long; }
Float_t EventInfo::GetM3LongPow3() const { return M3LongPow3; }
Float_t EventInfo::GetUpDownSym() const { return UpDownSym; }

Float_t EventInfo::GetDistance2MajorAxis() const { return Distance2MajorAxis; }
Float_t EventInfo::GetRMSMajorAxis() const { return RMSMajorAxis; }
Float_t EventInfo::GetWeightedRMSMajorAxis() const { return WeightedRMSMajorAxis; }
Int_t EventInfo::GetPixelsonMajorAxis() const { return PixelsonMajorAxis; }
Int_t EventInfo::GetPixelsoffMajorAxis() const { return PixelsoffMajorAxis; }
Float_t EventInfo::GetRatioPixelsMajorAxis() const { return RatioPixelsMajorAxis; }