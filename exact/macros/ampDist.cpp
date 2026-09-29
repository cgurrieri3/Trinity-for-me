//////////////////////
//ANGELINA TESTING!!//
//////////////////////

//tells root where to find ExACT libraries. Only needed if using as a macro for root. To compile, you remove this line and include ExACT libraries directly
R__LOAD_LIBRARY(libExACT.so)
//includes libraries used in script
#include <TH1.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <cmath>
#include <TImage.h>



//initialize or declare global variables, which are allocated static memory and are available in every scope
//TTree *tree = 0;
//Event *ev;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;

//declare functions to be defined later
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();
std::vector<Double_t> fileData(std::string dirStr, std::string treeStr);
Double_t Median(vector<Double_t> v);

void ampDist(std::string dStr, std::string treeString)
{



	std::vector<Double_t> dvAvg = fileData(dStr,treeString);

	//c_disp = new TCanvas("Display","CameraPlot",2500,1000);
        // Create and divide the canvas
        TCanvas *c_disp = new TCanvas("Display", "CameraPlot", 1250, 1000);
	c_disp->Divide(2,2); // Angelina change - (2,1) to (2,2) to have a 2x2 grid

	for (int i = 1; i <= 4; i++) {
		c_disp->cd(i);
		gPad->SetRightMargin(0.15);
		gPad->SetLeftMargin(0.12);
		gPad->SetBottomMargin(0.12);
		gPad->SetTopMargin(0.08);
	}

	TH2F *aCam = new TH2F("aCam","Normalized amplitude offset from camera median",16,-0.5,15.5,16,-0.5,15.5);
	TH1 *aDist = new TH1F("aDist","Amplitudes normalized to camera median",100,0,2);

	Double_t dMedian = Median(dvAvg);
	
	for(int i = 0; i < MaxNofChannels; i++){
		int nx, ny;
		FindBin(i,&nx,&ny);
		aCam->SetBinContent(nx+1,ny+1,(dvAvg[i]/dMedian)-1);
		aDist->Fill(dvAvg[i]/dMedian);
	}
	aCam->SetStats(0);
	//aDist->SetStats(0);
	c_disp->cd(1);

	gPad->SetRightMargin(0.20);
	// gPad->SetFixedAspectRatio(false); 
	// c_disp->cd(1)->SetRightMargin(0.15);
	aCam->Draw("colz");
	DrawMUSICBoundaries();



	c_disp->cd(2);
	aDist->GetXaxis()->SetTitle("Normalized amplitude");
	aDist->Draw("hist");


	// Angelina changes below
	
	// compute std dev of normalized amplitudes
	// (Angelina: previously using median as mean? Confused why it's displaying mean)
	Double_t mean_norm = 0;
	for (size_t i = 0; i < dvAvg.size(); i++) {
		mean_norm += (dvAvg[i] / dMedian);
	}
	mean_norm /= dvAvg.size();

	Double_t stdDev = 0;
	for (size_t i = 0; i < dvAvg.size(); i++) {
		stdDev += pow((dvAvg[i] / dMedian) - mean_norm, 2);
	}
	stdDev = sqrt(stdDev / dvAvg.size());

	// red stats box
	TLatex *stats = new TLatex();
	stats->SetTextColor(kRed);
	stats->SetTextSize(0.04);
	stats->SetNDC();  // normalized device coords

	// vertical line for the MEAN
	TLine *meanLine = new TLine(mean_norm, 0, mean_norm, aDist->GetMaximum());
	meanLine->SetLineColor(kRed);
	meanLine->SetLineWidth(3);
	meanLine->Draw("same");

	// show mean and std
	stats->DrawLatex(0.65, 0.85, Form("Mean = %.3f", mean_norm));
	stats->DrawLatex(0.65, 0.80, Form("Std = %.3f (~<10%%)", stdDev));

	// bottom left panel
	c_disp->cd(3);
	TImage *refImg1 = TImage::Open("/home/trinity/Programs/exact/macros/exampleHeatMap.png");
	if (!refImg1) { //if the heat map doesn't exist or load
		std::cout << "WARNING: Could not load exampleHeatMap.png" << std::endl;
	} else {
		refImg1->Draw(); 
	}
	gPad->Modified();

	// bottom right panel
	c_disp->cd(4);
	TImage *refImg2 = TImage::Open("/home/trinity/Programs/exact/macros/exampleHistogram.png");
	if (!refImg2) { // if histogram doesn't exist or load
		std::cout << "WARNING: Could not load exampleHistogram.png" << std::endl;
	} else {
		refImg2->Draw();
	}
	gPad->Modified();


	// Angelina changes above

	// Manually remove the ".root" extension from the root file name
	const char* cFilename = dStr.c_str();
    	const char* cRootFilename = gSystem->BaseName(cFilename);
    	TString rootFileName = cRootFilename;
    	size_t extensionPos = rootFileName.Index(".root");
   	 if (extensionPos != kNPOS) {
        	rootFileName.Remove(extensionPos);
    	}



	// c_disp->SaveAs(Form("/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataQualityPlots/avgDist_%s.png",rootFileName.Data()));
	c_disp->SaveAs(Form("/home/trinity/Programs/exact/macros/avgDist_%s.png",rootFileName.Data())); // testing 


    	delete c_disp;


}

std::vector<Double_t> fileData(std::string fileName, std::string treeStr){
	std::vector<Double_t> out(MaxNofChannels,0.0);
	cout << "Loading file: " << fileName << endl;
	TFile *f0 = TFile::Open(fileName.c_str());
	TTree *tree = (TTree*)f0->Get(treeStr.c_str());
	Event *ev = new Event();
	tree->SetBranchAddress("Events", &ev);
	int nEntries = tree->GetEntries();
	cout << "TEST: Total Number of Events: " << nEntries << endl;
	for(int countEvent = 0; countEvent < nEntries; countEvent++){
		std::vector<Double_t> outRun(MaxNofChannels,0.0);
		tree->GetEntry(countEvent);
		Pulse *pulse;
		for(int i = 0; i < MaxNofChannels; i++){
			pulse = new Pulse(ev->GetSignalValue(i));
			int ampVal = pulse->GetAmplitude();
			/*if(ampVal == 0){
				nEntries -= 1;
				goto evskip;
			}
			outRun[i] = ampVal;*/
      out[i] += ampVal;
			delete pulse;
		}
		/*for(int i = 0; i < MaxNofChannels; i++){   
			out[i] += outRun[i];      
		}
		evskip:;*/
	}
	delete f0;
	//delete tree;
	delete ev;
	for(int i = 0; i < MaxNofChannels; i++){
		out[i] /= nEntries;
	}
	return out;
}

//Calculates the 2D bin coordinates associated with a 1D vector/number, i.e. pixel number, based on Trinity camera layout
void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;	
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
}

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
}

//Function for calculating median 
Double_t Median(vector<Double_t> v){
	//Size of vector
	int n = v.size();
	//Make temp copy of the vector to leave original in the same order
	std::vector<Double_t> tempV(v);
    //Sort the vector 
    sort(tempV.begin(), tempV.end()); 
    //Check if the number of elements is odd 
    if(n%2!=0){
        return(Double_t)tempV[n/2];
	}
    //If the number of elements is even, return the average of the two middle elements 
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0; 
}

//returns HVCh id -1 (because c++ indexes starting from 0)
int HVCh(int camPix){
	if((camPix <= 31) || (camPix >= 64 && camPix <= 95)){
		return(int)0;
	}
	if((camPix >= 32 && camPix <= 63) || (camPix >= 96 && camPix <= 127)){
		return(int)2;
	}
	if((camPix >= 128 && camPix <= 159) || (camPix >= 192 && camPix <= 224)){
		return(int)1;
	}
	if((camPix >= 160 && camPix <= 191) || (camPix >= 224)){
		return(int)3;
	}
	cout << "error: pixel ID did not match any HV channels" << endl;
	return(int)0;
}
