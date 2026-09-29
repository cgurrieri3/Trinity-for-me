#ifndef DataSummary_H
#define DataSummary_H

#include "DtStruct.h"

#include <TROOT.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TLegend.h>
#include <TPaveText.h>

// #include <Event.h>
#include <IEvent.h>

#include <vector>
#include <cmath>

using namespace std;

class DataSummary {
	private:
		// double avgEv;
		double avgEv44;
		double avgEv415;
		// double ampDist;
		double ampDist44;
		double ampDist415;
		// double hledMean;
		double hledMean44;
		double hledMean415;
		// double hledNMean;
		double hledNMean44;
		double hledNMean415;
		// double pedMean;
		double pedMean44;
		double pedMean415;
		// double pedRMSMean;
		double pedRMSMean44;
		double pedRMSMean415;
		// double ampMean;
		double ampMean44;
		double ampMean415;
		// double qMean;
		double qMean44;
		double qMean415;
		// double ptMean;
		double ptMean44;
		double ptMean415;
		// double psfSigma;
		double psfSigma44;
		double psfSigma415;

		vector<vector<int>> trTh;
		// vector<DtStruct> testEv;
		vector<DtStruct> testEv44;
		vector<DtStruct> testEv415;
		// vector<DtStruct> hledEv;
		vector<DtStruct> hledEv44;
		vector<DtStruct> hledEv415;
		vector<vector<Double_t>> pixMeans;
		// vector<Double_t> meanPedRMS;
		vector<Double_t> meanPedRMS44;
		vector<Double_t> meanPedRMS415;
		TF1 *fConvolutedFit;
		TH2F *camera;
		TH2F *ddt;
		TH2F *addt;
		TH1F *trig;
		TH1F *misc1;
		TGraph *misc2;
		TLine *lin;
		TLegend *leg;
		TPaveText *pt;
		bool isData;
		//
		bool isData44;
		bool isData415;
		bool isHLEDData415;
		//
		void ReadEv(string readStr);
		// bool isHLED(Event *&ev);
		bool isHLED44(IEvent *&ev);
		bool isHLED415(IEvent *&ev);
		// void AddTestEv(Event *&ev);
		void AddTestEv44(IEvent *&ev);
		void AddTestEv415(IEvent *&ev);
		// void AddHLEDEv(Event *&ev);
		void AddHLEDEv44(IEvent *&ev);
		void AddHLEDEv415(IEvent *&ev);
		void ReadTrThresholds(string readStr);
		void FillCamera(int dp);
		void FillDt(int dp);
		void PlotAverages(int dp);
		void FillTrig();
	public:
		TCanvas *t_disp;
		DataSummary(char* dateStr);
		bool hasData();
		//
		bool hasData44();
		bool hasData415();
		bool hasHLEDData415();
		//
		void PlotTrig();
		void PlotROIMusic();
		// void PlotFF();
		void PlotFF44();
		void PlotFF415();
		// void PlotHLED();
		void PlotHLED44();
		void PlotHLED415();
		// void PlotHLEDNorm();
		void PlotHLEDNorm44();
		void PlotHLEDNorm415();
		// void PlotPedestal();
		void PlotPedestal44();
		void PlotPedestal415();
		// void PlotPedestalRMS();
		void PlotPedestalRMS44();
		void PlotPedestalRMS415();
		// void PlotAmplitude();
		void PlotAmplitude44();
		void PlotAmplitude415();
		// void PlotCharge();
		void PlotCharge44();
		void PlotCharge415();
		// void PlotTimePeak();
		void PlotTimePeak44();
		void PlotTimePeak415();
		// void PlotPSF();
		void PlotPSF44();
		void PlotPSF415();
		vector<vector<int>> GetTrTh();
		// double GetAvgEv();
		double GetAvgEv44();
		double GetAvgEv415();
		// double GetAmpDist();
		double GetAmpDist44();
		double GetAmpDist415();
		// double GetHLEDMean();
		double GetHLEDMean44();
		double GetHLEDMean415();
		// double GetHLEDNMean();
		double GetHLEDNMean44();
		double GetHLEDNMean415();
		// double GetPedMean();
		double GetPedMean44();
		double GetPedMean415();
		// double GetPedRMSMean();
		double GetPedRMSMean44();
		double GetPedRMSMean415();
		// double GetqMean();
		double GetqMean44();
		double GetqMean415();
		// double GetPTMean();
		double GetPTMean44();
		double GetPTMean415();
		// double GetPSFSigma();
		double GetPSFSigma44();
		double GetPSFSigma415();
};

#endif