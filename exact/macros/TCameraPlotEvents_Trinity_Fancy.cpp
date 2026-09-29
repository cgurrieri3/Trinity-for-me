R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TObjString.h>
#include <TGaxis.h>
#include <SEvent.h>

using namespace std;


int iLastPix = -1;
long long eventCounter = 0;
int eventFail = 0;
TLatex *text = 0;
TTree *tree = 0;
// The branch holds either an Event (raw data) or an IEvent (simulations /
// auxilliary data). IEvent derives from Event, so everything below reads the
// event through evBase; only one of evPlain/evSims is ever allocated.
// ROOT keeps the address of the pointer itself, so these must stay in scope.
Event *evPlain = 0;
IEvent *evSims = 0;
Event *ev = 0;
bool isIEvent = false;

// Simulation  lives in its own "Sim" tree (branch "GrOptics", one SEvent
// per event, indexed 1:1 with the Events tree). Real data files have no such
// tree, so its presence is what marks a file as a simulation.
TTree *simTree = 0;
SEvent *sEvent = 0;
bool isSim = false;
TPaveText *simInfoBox = 0;

// Sim run / sim event number of every entry, so the search commands do not have
// to re-read the (photon-vector carrying) SEvents on every query. Built lazily
// on the first search, see BuildSimIndex().
bool simIndexBuilt = false;
// All three are indexed by *Events tree entry*, not by Sim tree entry: see
// BuildSimIndex() for why those two numberings are not the same.
vector<Long64_t> simEntryIndex;   // Events entry -> Sim tree entry
vector<string> simRunIndex;
vector<int> simEvtIndex;
// Every shower in the Sim tree, triggered or not, so a failed search can say
// whether the shower exists but simply never triggered.
vector<string> simRunAll;
vector<int> simEvtAll;
vector<bool> simTrigAll;
// Remembered search, so "n" can step to the next match.
TString lastSearchRun = "";
long long lastSearchEvt = -1;

IPlotTools *plottools;
IUtilities *util;

void BuildSimIndex();   // defined below, needed by ShowSimInfo()

TCanvas *c_disp = 0;
TH1D *hPixelTrace = 0;

int MaxNofChannels = 256;
int LengthofPedestal = 200;
// Sample range Pulse searches for the peak. Raw data pulses land around sample
// 242, but CARE simulations put them near 180, and Pulse's default constructor
// hard-codes 230-250 -- on a sim that window sees only pedestal and every pixel
// comes back with an amplitude of a few counts, i.e. noise. So the window is
// found per event by DetectSignalWindow() unless it has been pinned with "w".
int SignalStart = 230;
int SignalWidth = 20;
bool autoWindow = true;
int curWinStart = 230;
int curWinEnd = 250;

TButton *but1;
bool ADC = true; // simulations are recorded in PE, so a sim file flips this to false

TH2F *hCam;
TH1F *hAmp;

// Merged data files carry their own calibration in a "SiPM" branch, filled per
// event by AddSiPMInfoToEvents. Its "gain" is the ADC per PE of each pixel,
// 22.1 at 25 C corrected for the SiPM temperature recorded with that event, and
// it supersedes the flat number for those files. Everything else -- pre-merge
// raw data and simulations -- uses the flat IUtilities::ADCtoPEratio of 24.1.
ISiPM *sipmInfo = 0;
bool hasSiPMInfo = false;

// ADC counts per PE for one pixel, i.e. what a value has to be divided by.
double GainForPixel(int iPix)
{
    if(hasSiPMInfo && sipmInfo != 0){
        vector<double> gain = sipmInfo->GetGain();
        if(iPix >= 0 && iPix < (int)gain.size() && gain[iPix] > 0){return gain[iPix];}
    }
    return IUtilities::GetADCtoPEratio();
}

// Mean ADC per PE over the camera. Only used to scale the axis of the amplitude
// histogram; the per-pixel gains differ by a fraction of a percent, so any one
// of them would serve, but the mean keeps the axis from following a dead pixel.
double MeanGain()
{
    double sum = 0;
    int n = 0;
    for(int i = 0; i < MaxNofChannels; i++){
        double g = GainForPixel(i);
        if(g > 0){sum += g; n++;}
    }
    return (n > 0) ? sum/n : IUtilities::GetADCtoPEratio();
}

// Every TPad::Update() repaints the whole canvas, so the display is updated once
// per event through here instead of once per pad. Pads mark themselves dirty
// with Modified(); only those are actually repainted.
void RefreshCanvas()
{
    if(c_disp == 0){return;}
    c_disp->Update();
}

// Draw obj into pad only if it is not already in that pad's primitive list.
// TObject::Draw appends unconditionally, so calling it once per event makes the
// list (and therefore every later repaint) grow without bound.
void DrawOnce(TVirtualPad *pad, TObject *obj, const char *opt = "")
{
    if(pad == 0 || obj == 0){return;}
    if(pad->GetListOfPrimitives() != 0 &&
       pad->GetListOfPrimitives()->FindObject(obj) != 0){return;}
    pad->cd();
    obj->Draw(opt);
}


// Pulses are negative going, so in PE the trace sits on a pedestal near full
// scale and the signal runs downwards, which reads backwards. The trace itself
// is left alone; instead the y axis is renumbered to run the other way, 0 at
// the top and full scale at the bottom, so the baseline reads as a few PE and
// the pulse reads as a large one. TH1 has no reversed-axis draw option in ROOT
// 6.30 (only TGraph does, and that mirrors the curve as well), so the
// histogram's own labels are hidden and a descending TGaxis takes their place.
// The title stays on the histogram axis, where ROOT centres it as usual.
TGaxis *peAxis = 0;

void UpdatePEAxis(TVirtualPad *pad)
{
    if(pad == 0 || hPixelTrace == 0){return;}
    TAxis *yax = hPixelTrace->GetYaxis();

    if(ADC == true){   // ADC counts are shown the normal way up
        yax->SetLabelOffset(0.005);   // ROOT's defaults
        yax->SetTickLength(0.03);
        if(peAxis != 0){
            if(pad->GetListOfPrimitives() != 0){pad->GetListOfPrimitives()->Remove(peAxis);}
            delete peAxis;
            peAxis = 0;
        }
        return;
    }

    yax->SetLabelOffset(999);   // pushes the ascending labels off the pad
    yax->SetTickLength(0);

    // The frame range is only final once the pad has been painted.
    double lo = pad->GetUymin();
    double hi = pad->GetUymax();
    double x0 = pad->GetUxmin();

    // Running from the top of the frame down makes the values ascend downwards.
    if(peAxis == 0){
        peAxis = new TGaxis(x0, hi, x0, lo, lo, hi, 510, "+");
        peAxis->SetLabelFont(yax->GetLabelFont());
        peAxis->SetLabelSize(yax->GetLabelSize());
        pad->cd();
        peAxis->Draw();
    } else {
        peAxis->SetX1(x0);  peAxis->SetX2(x0);
        peAxis->SetY1(hi);  peAxis->SetY2(lo);
        peAxis->SetWmin(lo);
        peAxis->SetWmax(hi);
    }
}


void PlotTrace(int iPix)
{
    if(iPix < 0 || iPix >= MaxNofChannels){return;}

    if(hPixelTrace == 0)
    {
        hPixelTrace = new TH1D("hPixelTrace","Pixel Trace",500,-0.5,499.5);
        hPixelTrace->SetStats(0);
        hPixelTrace->GetXaxis()->SetTitle("ADC sample");
        hPixelTrace->GetYaxis()->SetTitle("ADC counts");
    }

    hPixelTrace->Reset();
    if(ADC == true){
        hPixelTrace->GetYaxis()->SetTitle("ADC counts");
        hPixelTrace->SetMinimum();    // back to ROOT's auto scaling
    } else {
        hPixelTrace->GetYaxis()->SetTitle("PE");
        // Pinning the bottom of the range at 0 is what puts 0 at the *top* of
        // the reversed axis. Left to auto scaling the range would start at the
        // depth of this particular pulse, so the top label would move from
        // pixel to pixel instead of staying at 0.
        hPixelTrace->SetMinimum(0);
    }

    TString title;
    title.Form("Trace of Pixel %i", iPix);
    hPixelTrace->SetTitle(title);

    vector<Int_t> trace = ev->GetSignalValue(iPix);
    int nSamples = TMath::Min((int)trace.size(), 500);
    double gain = GainForPixel(iPix);
    for(int k = 0; k<nSamples; k++){
        if(ADC==true){
            hPixelTrace->SetBinContent(k+1,trace[k]);
        } else {
            hPixelTrace->SetBinContent(k+1,trace[k]/gain);
        }


    }

    // Called from a pad exec, so the caller's gPad has to survive.
    TVirtualPad *save = gPad;
    TVirtualPad *pad = c_disp->cd(3);
    DrawOnce(pad, hPixelTrace);
    // Painted before the axis is placed: UpdatePEAxis needs the frame range,
    // and that is only settled once the histogram has been drawn.
    pad->Modified();
    pad->Update();
    UpdatePEAxis(pad);
    pad->Modified();
    RefreshCanvas();
    if(save){save->cd();}
}

void ShowInfoAtCursor(int x, int y)
{

    TString statusline;
    statusline.Form("Pixel: %i, MUSIC: %i, SIAB: %i", plottools->FindPixel(x,y), plottools->FindMUSIC(x,y), plottools->FindSIAB(x,y));
    // Reuse the label instead of deleting and re-creating it on every mouse
    // move: only pad 1 ends up dirty, so the repaint stays local to the camera.
    if(text == 0)
    {
        TLatex T1;
        text = T1.DrawLatexNDC(0.25,0.95,statusline.Data());
    }
    else
    {
        text->SetTitle(statusline.Data());
    }
    gPad->Modified();
    RefreshCanvas();
}


void PixelClicked()
{
    int event = gPad->GetEvent();
    TObject *o = gPad->GetSelected();
    if (!o){return;}
    if (!(o->InheritsFrom("TH2"))){return;}
    TH2F *h = (TH2F*)o;
    int px = gPad->GetEventX();
    int py = gPad->GetEventY();
    Float_t xx = gPad->AbsPixeltoX(px);
    Float_t yy = gPad->AbsPixeltoY(py);
    Float_t x = 0.5+gPad->PadtoX(xx);
    Float_t y = 0.5+gPad->PadtoY(yy);
    int pix = plottools->FindPixel((int)x,(int)y);
    if(pix!=iLastPix)
    {
        iLastPix = pix;
        ShowInfoAtCursor((int) x, (int) y);
    }
    if (event == 11)
    {
        PlotTrace(pix);
    }
}


// hAmp is filled in whichever unit is currently selected, so its range and title
// have to follow the ADC/PE toggle. SetBins also clears the contents, so it
// doubles as the reset when the unit has just changed; otherwise a plain Reset
// is enough and avoids reallocating the bin array on every event.
void ConfigureAmpHist()
{
    static int lastUnit = -1;
    if(hAmp == 0){return;}
    int unit = ADC ? 1 : 0;
    if(unit == lastUnit){
        hAmp->Reset();
        return;
    }
    lastUnit = unit;
    if(ADC == true){
        hAmp->SetBins(410, 0, 4096);
        hAmp->GetXaxis()->SetTitle("Signal Max Amplitude [ADC]");
    } else {
        hAmp->SetBins(410, 0, 4096.0/MeanGain());
        hAmp->GetXaxis()->SetTitle("Signal Max Amplitude [PE]");
    }
}

// Find the sample range holding this event's pulse. Summing every pixel makes
// the shower stand out from the per-pixel noise; the deepest dip below the
// pedestal in that sum is the peak. Pulses are negative going, hence "dip".
void DetectSignalWindow(int *tStart, int *tEnd)
{
    if(!autoWindow){
        *tStart = SignalStart;
        *tEnd   = SignalStart + SignalWidth;
        return;
    }

    vector<double> sum(512, 0.0);
    int nSamples = 0;
    for(int i = 0; i < MaxNofChannels; i++){
        vector<Int_t> tr = ev->GetSignalValue(i);
        if((int)tr.size() > nSamples){nSamples = TMath::Min((int)tr.size(), 512);}
        for(int k = 0; k < (int)tr.size() && k < 512; k++){sum[k] += tr[k];}
    }
    if(nSamples < 150){   // too short to reason about, keep the default
        *tStart = SignalStart;
        *tEnd   = SignalStart + SignalWidth;
        return;
    }

    // Pulse always takes its pedestal from the first 100 samples, so the window
    // must stay clear of them or the pulse would bias its own baseline.
    const int pedEnd = 100;
    double ped = 0;
    for(int k = 0; k < pedEnd; k++){ped += sum[k];}
    ped /= pedEnd;

    int peak = -1;
    double best = 0;
    for(int k = pedEnd + 5; k < nSamples - 5; k++){
        double d = ped - sum[k];
        if(d > best){best = d; peak = k;}
    }
    if(peak < 0){   // flat event, nothing to centre on
        *tStart = SignalStart;
        *tEnd   = SignalStart + SignalWidth;
        return;
    }

    int half = TMath::Max(SignalWidth/2, 5);
    *tStart = TMath::Max(peak - half, pedEnd + 5);
    *tEnd   = TMath::Min(peak + half, nSamples - 2);
    if(*tEnd <= *tStart){*tEnd = TMath::Min(*tStart + 2, nSamples - 1);}
}

// Fill the camera map and the amplitude distribution for the event currently
// loaded in ev, in the selected unit. Called on every event and again whenever
// the ADC/PE button is pushed, so both histograms always agree with the button.
void FillHistograms()
{
    if(ev == 0 || hCam == 0 || hAmp == 0){return;}

    hCam->Reset();
    ConfigureAmpHist();

    DetectSignalWindow(&curWinStart, &curWinEnd);

    for(int i = 0; i < MaxNofChannels; i++){
            // Stack allocated, and given an explicit search window: the default
            // Pulse constructor would hard-code 230-250 and miss sim pulses.
            Pulse pulse(ev->GetSignalValue(i), curWinStart, curWinEnd, 512, true);
            int nx, ny;
            plottools->FindBin(i,&nx,&ny);
            float amplitude = pulse.GetAmplitude();
            if(ADC == false){amplitude /= GainForPixel(i);}
            hCam->SetBinContent(nx+1,ny+1,amplitude);
            hAmp->Fill(amplitude);
    }
}

// Read the SEvent  for this event and show it on the (otherwise unused)
// fourth pad. Distance and emergence angle are not stored on SEvent; they are
// derived from the azimuth angle and the telescope position.
void ShowSimInfo(long long iEvent)
{
    if(!isSim){return;}
    BuildSimIndex();
    // The Events index is NOT a Sim index; translate it.
    if(iEvent < 0 || iEvent >= (long long)simEntryIndex.size()){return;}
    Long64_t simEntry = simEntryIndex[iEvent];

    simTree->GetEntry(simEntry);

    double emergenceAngle    = IUtilities::GetEmergenceAngle(sEvent->GetAzimuthAngle());
    double emergenceDistance = IUtilities::GetEmergenceDistance(sEvent->GetTelescope_Xpos(),
                                                                sEvent->GetTelescope_Ypos(),
                                                                sEvent->GetTelescope_Zpos());
    // GetPosX() hands back a copy of the whole photon list, so ask once.
    size_t nPhotons = (sEvent->GetPosX()).size();

    cout << "Sim Run: " << sEvent->GetSumRun()
         << ", Sim tree entry: " << simEntry
         << ", Date: " << sEvent->GetDate()
         << ", Sim Event: " << sEvent->GetSimEventNumber() << endl;
    cout << "Neutrino Energy: " << sEvent->GetNeutrinoEnergy() << " GeV"
         << ", Emergence Distance: " << emergenceDistance << " m"
         << ", Emergence Angle: " << emergenceAngle << " deg"
         << ", Photons that Trigger event: "<< nPhotons << endl
         << ", All photons: "<< sEvent->GetNMax() << endl;

    if(simInfoBox == 0){
        simInfoBox = new TPaveText(0.05,0.05,0.95,0.95,"NDC");
        simInfoBox->SetFillColor(0);
        simInfoBox->SetTextAlign(12);
    }
    simInfoBox->Clear();
    simInfoBox->AddText("Simulation (SEvent)");
    simInfoBox->AddText(Form("Entry: %lld  (Sim entry %lld)", iEvent, simEntry));
    simInfoBox->AddText(Form("Sim Run: %s", sEvent->GetSumRun().c_str()));
    simInfoBox->AddText(Form("Date: %i", sEvent->GetDate()));
    simInfoBox->AddText(Form("Sim Event: %i", sEvent->GetSimEventNumber()));
    simInfoBox->AddText(Form("Neutrino Energy: %.3g GeV", sEvent->GetNeutrinoEnergy()));
    simInfoBox->AddText(Form("Emergence Distance: %.3g m", emergenceDistance));
    simInfoBox->AddText(Form("Emergence Angle: %.3g deg", emergenceAngle));
    simInfoBox->AddText(Form("All Photons: %.3g", sEvent->GetNMax()));
    simInfoBox->AddText(Form("Trigger Photons?: %zu", nPhotons));
    // Draw only the first time round; afterwards Clear()/AddText() have already
    // replaced the contents of the box that is on the pad.
    DrawOnce(c_disp->cd(4), simInfoBox);
    c_disp->cd(4)->Modified();
}


// ---------------------------------------------------------------------------
// Sim run / sim event lookup
// ---------------------------------------------------------------------------

// Build the Events-entry -> Sim-entry mapping and cache each event's run name
// and sim event number.
//
// The two trees are NOT parallel. The Sim tree holds every thrown shower while
// the Events tree holds only the ones that triggered the DAQ (81144 vs 745 in
// the 20260827 CARE file), so indexing the Sim tree with an Events entry number
// reads an unrelated shower. The k-th shower with triggered==true is the one
// that produced Events entry k.
//
// Only simrun / simEvent / triggered are read when the "Sim" tree is split, so
// the per-shower photon vectors are never touched: 81144 entries scan in 0.03 s.
void BuildSimIndex()
{
    if(!isSim || simIndexBuilt){return;}

    Long64_t nSim = simTree->GetEntries();
    Long64_t nEvt = tree->GetEntries();

    bool haveTrig = (simTree->FindBranch("triggered") != 0);
    bool split    = (simTree->FindBranch("simrun") != 0) && (simTree->FindBranch("simEvent") != 0);
    if(split){
        simTree->SetBranchStatus("*",0);
        simTree->SetBranchStatus("simrun",1);
        simTree->SetBranchStatus("simEvent",1);
        if(haveTrig){simTree->SetBranchStatus("triggered",1);}
    }

    cout << "Indexing " << nSim << " simulation entries..." << flush;
    vector<Long64_t> trigEntries;
    simRunAll.clear(); simEvtAll.clear(); simTrigAll.clear();
    simRunAll.reserve(nSim);
    simEvtAll.reserve(nSim);
    simTrigAll.reserve(nSim);
    for(Long64_t i = 0; i < nSim; i++){
        simTree->GetEntry(i);
        bool trig = haveTrig && sEvent->GetTriggered();
        simRunAll.push_back(sEvent->GetSumRun());
        simEvtAll.push_back(sEvent->GetSimEventNumber());
        simTrigAll.push_back(trig);
        if(trig){trigEntries.push_back(i);}
    }
    if(split){simTree->SetBranchStatus("*",1);}
    cout << " done." << endl;

    // Establish which Sim entry belongs to which Events entry. Preferred case:
    // the number of triggered showers equals the number of recorded events, so
    // the k-th triggered shower is the one that produced Events entry k.
    simEntryIndex.clear();
    if(haveTrig && (Long64_t)trigEntries.size() == nEvt){
        simEntryIndex = trigEntries;
        cout << "Matched " << nEvt << " events to the " << nEvt
             << " triggered showers out of " << nSim << " thrown." << endl;
    }
    else if(nSim == nEvt){
        simEntryIndex.resize(nEvt);
        for(Long64_t i = 0; i < nEvt; i++){simEntryIndex[i] = i;}
        cout << "Sim and Events trees are the same length, using a 1:1 mapping." << endl;
    }
    else{
        cout << "Warning: cannot match the Sim tree (" << nSim << " entries";
        if(haveTrig){cout << ", " << trigEntries.size() << " triggered";}
        cout << ") to the Events tree (" << nEvt << " entries)." << endl;
        cout << "         Falling back to a positional mapping; the simulation info"
             << " shown may not belong to the displayed event." << endl;
        Long64_t m = TMath::Min(nSim, nEvt);
        simEntryIndex.resize(m);
        for(Long64_t i = 0; i < m; i++){simEntryIndex[i] = i;}
    }

    simRunIndex.resize(simEntryIndex.size());
    simEvtIndex.resize(simEntryIndex.size());
    for(size_t k = 0; k < simEntryIndex.size(); k++){
        simRunIndex[k] = simRunAll[simEntryIndex[k]];
        simEvtIndex[k] = simEvtAll[simEntryIndex[k]];
    }

    simIndexBuilt = true;
}

// Entries matching run (a case insensitive substring, empty matches anything)
// and sim event number (negative matches anything). The scan starts just after
// "from" and wraps, so repeating a search steps through the matches.
long long FindSimEntry(const TString &run, long long evtNo, long long from)
{
    BuildSimIndex();
    long long n = (long long)simRunIndex.size();
    if(n == 0){return -1;}

    TString runLower = run; runLower.ToLower();
    for(long long k = 1; k <= n; k++){
        long long i = (from + k) % n;
        if(evtNo >= 0 && (long long)simEvtIndex[i] != evtNo){continue;}
        if(runLower.Length() > 0){
            TString have = simRunIndex[i].c_str(); have.ToLower();
            if(!have.Contains(runLower)){continue;}
        }
        return i;
    }
    return -1;
}

void ListSimMatches(const TString &run, long long evtNo, int maxLines = 40)
{
    BuildSimIndex();
    TString runLower = run; runLower.ToLower();
    int shown = 0;
    long long total = 0;
    for(size_t i = 0; i < simRunIndex.size(); i++){
        if(evtNo >= 0 && (long long)simEvtIndex[i] != evtNo){continue;}
        if(runLower.Length() > 0){
            TString have = simRunIndex[i].c_str(); have.ToLower();
            if(!have.Contains(runLower)){continue;}
        }
        total++;
        if(shown < maxLines){
            cout << "  entry " << i << "  run " << simRunIndex[i]
                 << "  sim event " << simEvtIndex[i] << endl;
            shown++;
        }
    }
    cout << total << " match(es)";
    if(total > shown){cout << " (" << shown << " listed)";}
    cout << endl;
}

// A search can fail because the shower was never thrown, or because it was
// thrown but did not trigger and so has no recorded event. Those mean very
// different things to the user, so say which.
void ReportMissing(const TString &run, long long evtNo)
{
    BuildSimIndex();
    TString runLower = run; runLower.ToLower();
    long long thrown = 0, firstEntry = -1;
    for(size_t i = 0; i < simRunAll.size(); i++){
        if(evtNo >= 0 && (long long)simEvtAll[i] != evtNo){continue;}
        if(runLower.Length() > 0){
            TString have = simRunAll[i].c_str(); have.ToLower();
            if(!have.Contains(runLower)){continue;}
        }
        thrown++;
        if(firstEntry < 0){firstEntry = (long long)i;}
    }

    cout << "No recorded event with";
    if(run.Length() > 0){cout << " sim run '" << run << "'";}
    if(evtNo >= 0){cout << " sim event " << evtNo;}
    cout << "." << endl;

    if(thrown > 0){
        cout << "  That shower IS in the Sim tree (" << thrown << " match(es), first at Sim entry "
             << firstEntry << ") but it did not trigger," << endl;
        cout << "  so there is no trace in the Events tree to display." << endl;
    } else {
        cout << "  No such shower was thrown either - check the run name and event number." << endl;
    }
}

void PrintHelp()
{
    cout << "  <return>       next event            b            previous event" << endl;
    cout << "  <N>            go to entry N         q            quit" << endl;
    cout << "  w              signal window         w a b        pin window / w auto" << endl;
    if(isSim){
        cout << "  e <N>          go to sim event N     r <run>      go to sim run <run>" << endl;
        cout << "  r <run> <N>    sim run and event     <run>:<N>    same, short form" << endl;
        cout << "  n              next match            l [...]      list matches" << endl;
    }
}

// Parse "run:event", "e <n>", "r <run> [<n>]". Returns false if cmd is not a
// search command.
bool ParseSearch(TString cmd, TString &run, long long &evtNo)
{
    run = "";
    evtNo = -1;

    if(cmd.Contains(":")){
        int pos = cmd.Last(':');
        run = cmd(0,pos);
        TString ev = cmd(pos+1, cmd.Length()-pos-1);
        run = run.Strip(TString::kBoth);
        ev  = ev.Strip(TString::kBoth);
        if(ev.Length() > 0){
            if(!ev.IsDigit()){return false;}
            evtNo = ev.Atoll();
        }
        return true;
    }

    if(cmd.BeginsWith("e ") || cmd.BeginsWith("ev ")){
        TString ev = cmd(cmd.First(' ')+1, cmd.Length());
        ev = ev.Strip(TString::kBoth);
        if(!ev.IsDigit()){return false;}
        evtNo = ev.Atoll();
        return true;
    }

    if(cmd.BeginsWith("r ") || cmd.BeginsWith("run ")){
        TString rest = cmd(cmd.First(' ')+1, cmd.Length());
        rest = rest.Strip(TString::kBoth);

        // Split into words so that "r <run> e <N>", "r <run> e<N>" and
        // "r <run> <N>" all work. A trailing number is the sim event; an "e" or
        // "ev" keyword just in front of it is a separator, not part of the run.
        vector<TString> tok;
        TObjArray *parts = rest.Tokenize(" ");
        for(int i = 0; i < parts->GetEntries(); i++){
            TString t = ((TObjString*)parts->At(i))->GetString();
            t = t.Strip(TString::kBoth);
            if(t.Length() > 0){tok.push_back(t);}
        }
        delete parts;
        if(tok.empty()){return false;}

        // "e123" / "ev123" glued together counts as the event token too.
        if(tok.size() > 1){
            TString last = tok.back();
            TString digits = last;
            if(digits.BeginsWith("ev")){digits = digits(2, digits.Length()-2);}
            else if(digits.BeginsWith("e")){digits = digits(1, digits.Length()-1);}
            if(last.IsDigit() || (digits.Length() > 0 && digits.IsDigit())){
                evtNo = last.IsDigit() ? last.Atoll() : digits.Atoll();
                tok.pop_back();
                // Drop a bare "e"/"ev" keyword sitting in front of the number.
                if(!tok.empty() && (tok.back() == "e" || tok.back() == "ev")){tok.pop_back();}
            }
        }
        if(tok.empty()){return false;}   // "r e 104" has no run pattern left

        run = tok[0];
        for(size_t i = 1; i < tok.size(); i++){run += " "; run += tok[i];}
        return true;
    }

    return false;
}

// Reads a command and leaves eventCounter pointing at the entry to display next.
// Returns kFALSE when the user wants to quit.
Bool_t HandleInput()
{
	TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
	long long nEntries = tree->GetEntries();
	while (1)
	{
        	timer.TurnOn();
        	TString input = Getline(isSim
        	    ? "<return> next, b back, N entry, e N / r <run> [N] search, ? help, q quit: "
        	    : "<return> next, b back, N entry, ? help, q quit: ");
        	timer.TurnOff();

        	TString cmd = input;
        	cmd.ReplaceAll("\n","");
        	cmd.ReplaceAll("\r","");
        	cmd = cmd.Strip(TString::kBoth);

        	if(cmd == "q" || cmd == "Q"){return kFALSE;}

        	if(cmd.Length() == 0){
        	    eventCounter++;
        	    if(eventCounter >= nEntries){eventCounter = 0;}
        	    return kTRUE;
        	}

        	if(cmd == "b"){
        	    eventCounter--;
        	    if(eventCounter < 0){eventCounter = nEntries-1;}
        	    return kTRUE;
        	}

        	if(cmd == "?" || cmd == "h" || cmd == "help"){PrintHelp(); continue;}

        	if(cmd == "w" || cmd.BeginsWith("w ")){
        	    TString rest = (cmd.Length() > 1) ? TString(cmd(2, cmd.Length()-2)) : TString("");
        	    rest = rest.Strip(TString::kBoth);
        	    if(rest.Length() == 0){
        	        cout << "Signal window: samples " << curWinStart << " - " << curWinEnd
        	             << (autoWindow ? "  (auto, re-found each event)" : "  (pinned)") << endl;
        	        continue;
        	    }
        	    if(rest == "auto"){
        	        autoWindow = true;
        	        cout << "Signal window: auto." << endl;
        	        return kTRUE;   // redisplay with the new window
        	    }
        	    int a = 0, b = 0;
        	    if(sscanf(rest.Data(), "%d %d", &a, &b) == 2 && b > a && a >= 0 && b < 512){
        	        autoWindow = false;
        	        SignalStart = a;
        	        SignalWidth = b - a;
        	        cout << "Signal window pinned to samples " << a << " - " << b << "." << endl;
        	        return kTRUE;   // redisplay with the new window
        	    }
        	    cout << "Usage: w            show the current window" << endl;
        	    cout << "       w auto       find the pulse automatically each event" << endl;
        	    cout << "       w <a> <b>    pin the peak search to samples a..b" << endl;
        	    continue;
        	}

        	if(cmd.IsDigit()){
        	    long long target = cmd.Atoll();
        	    if(target < 0 || target >= nEntries){
        	        cout << "Entry " << target << " is out of range (0 - " << nEntries-1 << ")." << endl;
        	        continue;
        	    }
        	    eventCounter = target;
        	    return kTRUE;
        	}

        	if(!isSim){
        	    cout << "Not a simulation file: only <return>, b, <N> and q are available." << endl;
        	    continue;
        	}

        	if(cmd == "n"){
        	    if(lastSearchRun.Length() == 0 && lastSearchEvt < 0){
        	        cout << "No previous search." << endl;
        	        continue;
        	    }
        	    long long hit = FindSimEntry(lastSearchRun, lastSearchEvt, eventCounter);
        	    if(hit < 0){cout << "No further match." << endl; continue;}
        	    if(hit >= nEntries){
        	        cout << "Match at entry " << hit << " is outside the Events tree (0 - "
        	             << nEntries-1 << ")." << endl;
        	        continue;
        	    }
        	    eventCounter = hit;
        	    return kTRUE;
        	}

        	if(cmd == "l" || cmd.BeginsWith("l ")){
        	    TString rest = (cmd.Length() > 1) ? TString(cmd(2,cmd.Length()-2)) : TString("");
        	    rest = rest.Strip(TString::kBoth);
        	    TString run; long long evtNo;
        	    if(rest.Length() == 0){run = lastSearchRun; evtNo = lastSearchEvt;}
        	    else if(!ParseSearch(rest, run, evtNo)){run = rest; evtNo = -1;}
        	    ListSimMatches(run, evtNo);
        	    continue;
        	}

        	TString run; long long evtNo;
        	if(ParseSearch(cmd, run, evtNo)){
        	    long long hit = FindSimEntry(run, evtNo, eventCounter);
        	    if(hit < 0){
        	        ReportMissing(run, evtNo);
        	        continue;
        	    }
        	    lastSearchRun = run;
        	    lastSearchEvt = evtNo;
        	    if(hit >= nEntries){
        	        cout << "Match at entry " << hit << " is outside the Events tree (0 - "
        	             << nEntries-1 << ")." << endl;
        	        continue;
        	    }
        	    eventCounter = hit;
        	    return kTRUE;
        	}

        	cout << "Unrecognised command '" << cmd << "'." << endl;
        	PrintHelp();
	}
	return kFALSE;
}

void PlotEvent()
{
    c_disp->cd(1);
    gPad->AddExec("ev","PixelClicked()");

    hCam = new TH2F("hCam","",16,-0.5,15.5,16,-0.5,15.5);
    hCam->SetStats(0);
    hCam->Draw("colz");
    plottools->DrawMUSICBoundaries();
    long long nEntries = tree->GetEntries();
    cout << "Total Number of Events: " << nEntries << endl;

    hAmp = new TH1F("hAmp","Amplitude Distribution",410, 0, 4096);
    hAmp->SetStats(0);
    hAmp->GetYaxis()->SetTitle("No. of Pixels");
    // Drawn once here rather than once per event: TH1::Draw re-appends the
    // histogram to the pad every time it is called.
    c_disp->cd(2);
    hAmp->Draw();

    // These mark the *canvas* modified, which forces every pad to be repainted.
    // They never change, so set them once instead of on every event.
    c_disp->SetTheta(90);
    c_disp->SetPhi(90);

    PrintHelp();

    while(1){
        // A failed read leaves the previous event in ev, which would silently
        // redisplay the old camera image, so say so rather than showing a lie.
        if(tree->GetEntry(eventCounter) <= 0){
            cout << "Could not read entry " << eventCounter << " of tree "
                 << tree->GetName() << "." << endl;
            eventCounter = 0;
            if(tree->GetEntry(eventCounter) <= 0){
                cout << "Giving up." << endl;
                break;
            }
        }
        // ROOT may replace the object it hands back, so refresh the base pointer
        ev = isIEvent ? (Event*)evSims : evPlain;
        cout << "Event# " << eventCounter << " is displayed." << endl;
        vector<Int_t> musicIDs = ev->GetROIMusicID();
        if(!musicIDs.empty()){
            cout << "The MUSIC which triggered the DAQ for event # " << eventCounter
                 << " is " << musicIDs[0] << endl;
        }

        FillHistograms();
        cout << "Signal window: samples " << curWinStart << " - " << curWinEnd
             << (autoWindow ? " (auto)" : " (pinned)") << endl;
        ShowSimInfo(eventCounter);

        uint64_t convtime = ((uint64_t)ev->GetTBTime() / 100000000ULL);
        //Assuming timeTrig is in the format "seconds since epoch with 10^-7 second resolution"
        time_t epochTime = static_cast<time_t>(convtime); // Convert to seconds
        int microsec = convtime % 1000000;

        // Convert to struct tm in UTC
        tm* utcTime = gmtime(&epochTime);

        // Format and display the UTC time
        cout << "Trigger Time (UTC): ";
        cout << put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(6) << microsec << endl;
        // cout << "time " << ev->GetTBTime() <<" is displayed." << endl;

        hCam->SetMinimum();
        hCam->SetMaximum();
        c_disp->cd(1)->Modified();
        c_disp->cd(2)->Modified();
        // One repaint for the whole display instead of one per pad.
        RefreshCanvas();

        if(!HandleInput()){break;}
    }
}

bool SetBranches()
{
    TBranch *branch = tree->GetBranch("Events");
    if(branch == 0)
    {
        cout << "No branch called 'Events' in tree " << tree->GetName() << endl;
        return false;
    }

    TString className = branch->GetClassName();
    cout << "Branch 'Events' holds objects of class " << className << endl;

    if(className == "IEvent")
    {
        isIEvent = true;
        evSims = new IEvent();
        tree->SetBranchAddress("Events", &evSims);
        ev = evSims;
    }
    else if(className == "Event")
    {
        isIEvent = false;
        evPlain = new Event();
        tree->SetBranchAddress("Events", &evPlain);
        ev = evPlain;
    }
    else
    {
        cout << "Unsupported event class '" << className << "', expected Event or IEvent." << endl;
        return false;
    }

    // Only merged data that has been through AddSiPMInfoToEvents has this; raw
    // files and simulations fall back to the flat conversion.
    if(tree->GetBranch("SiPM") != 0)
    {
        sipmInfo = new ISiPM();
        tree->SetBranchAddress("SiPM", &sipmInfo);
        hasSiPMInfo = true;
        cout << "Found a 'SiPM' branch (merged data): PE uses the per-pixel, "
                "temperature corrected gain stored with each event." << endl;
    }
    else
    {
        cout << "No 'SiPM' branch: PE uses the flat "
             << IUtilities::GetADCtoPEratio() << " ADC/PE conversion." << endl;
    }
    return true;
}

// A simulation file carries a "Sim" tree alongside the events tree. This is
// independent of whether the events are Event or IEvent: IEvent marks auxilliary
// (weather / HV / pointing) data, not simulated data.
bool SetSimBranch()
{
    TFile *file = tree->GetCurrentFile();
    if(file == 0){return false;}

    simTree = (TTree*)file->Get("Sim");
    if(simTree == 0){return false;}

    if(simTree->GetBranch("GrOptics") == 0)
    {
        cout << "Tree 'Sim' has no 'GrOptics' branch, treating file as non-simulation." << endl;
        simTree = 0;
        return false;
    }

    sEvent = new SEvent();
    simTree->SetBranchAddress("GrOptics", &sEvent);
    return true;
}

void LoadEvents(string fileName, string treeString)
{
    cout << "Loading file: " << fileName << endl;
    TFile *fO = new TFile(fileName.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void PushButton(){

    if(ADC == true){
        cout<< "Changing to PE" << endl;
        ADC = false;
        but1->SetTitle("ADC");
    }
    else {
        cout<< "Changing to ADC" << endl;
        ADC = true;
        but1->SetTitle("PE");
    }

    // Refill rather than rescale: hAmp needs a new range, and the open trace has
    // to be redrawn in the new unit too.
    FillHistograms();
    c_disp->cd(1)->Modified();
    c_disp->cd(2)->Modified();
    if(iLastPix >= 0){PlotTrace(iLastPix);}
    RefreshCanvas();
}

void TCameraPlotEvents_Trinity_Fancy(string fileName,string treeString)
{   
    
	c_disp = new TCanvas("Display","CameraPlot",750,750);
	c_disp->Divide(2,2);
	LoadEvents(fileName, treeString);
	if(tree == 0)
	{
		cout << "Could not find tree " << treeString << " in " << fileName << endl;
		return;
	}
	if(!SetBranches()){return;}

    isSim = SetSimBranch();
    if(isSim)
    {
        cout << "Simulation file detected ('Sim' tree found): showing signals in PE." << endl;
        ADC = false; // simulations are always in PE
    }

    // The button is labelled with the unit it switches TO, so it shows the unit
    // that is not currently displayed.
    but1 = new TButton(ADC ? "PE" : "ADC","PushButton()",.01,.95,.07,.99);
    but1->SetFillColor(42);
    but1->Draw();

	PlotEvent();
}
