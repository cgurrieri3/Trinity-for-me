//tells root where to find ExACT libraries. Only needed if using as a macro for root. To compile, you remove this line and include ExACT libraries directly
R__LOAD_LIBRARY(libExACT.so)
//includes libraries used in script
#include <TTree.h>
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

//set "base" directories
//dataDir is where the input data directories are saved
//outDir is the directory where output will be saved
std::string dataDir = "/home/trinity/Documents/exact_output/flatfielding/";
std::string outDir = "/home/trinity/Documents/exact_output/flatfielding/";
std::string dvOffsetsPixelMappingFile = "/home/trinity/Programs/exact/macros/dvOffsetsPixelMapping.csv";

//initialize or declare global variables, which are allocated static memory and are available in every scope
int MaxNofChannels = 256;

//declare functions to be defined later
std::vector<Double_t> dirData(std::string dirStr, std::string treeStr);
std::vector<int> getDVOffsetsMapping(std::string dvOffsetsPixelMappingFile);
Double_t Median(vector<Double_t> v);
int HVCh(int camPix);

void FF_Updated(){

	std::vector<int> dvmapping = getDVOffsetsMapping(dvOffsetsPixelMappingFile);
	cout << "DV Offset Mapping loaded with " << dvmapping.size() << " entries." << endl;

	std::string hiDir = Form("%s100/",dataDir.c_str());
	std::string miDir = Form("%s255/",dataDir.c_str());
	std::string loDir = Form("%s400/",dataDir.c_str());
	std::string treeString = "HLED";

	std::vector<Double_t> hiVPAvg = dirData(hiDir,treeString);
	std::vector<Double_t> miVPAvg = dirData(miDir,treeString);
	std::vector<Double_t> loVPAvg = dirData(loDir,treeString);
	std::vector<Double_t> pixGain(MaxNofChannels, 0.0);
	std::vector<Double_t> ampOffset(MaxNofChannels, 0.0);
	std::vector<int> bias(MaxNofChannels, 0);
	std::vector<int> REG(MaxNofChannels, 0.0);

	std::ofstream CSVFile(Form("%sPixelHVBiases.csv",outDir.c_str()));
	std::ofstream UpdatedDVFile(Form("%sUpdatedDVoffsetsOnly.csv",outDir.c_str()));
	CSVFile << "DV Offset file order, Pixel ID, Offset from median at Voffset = 255 DAC (amp DAC), Gain (Voffset DAC/amp DAC), Bias value (Voffset DAC), #REG_Value (DEC)" << endl;
	UpdatedDVFile << " #REG_Value (DEC)" << endl;
	Double_t miVMedian = Median(miVPAvg);
	Int_t DVLoc;

	
	
	for(int i = 0; i < MaxNofChannels; i++){
		DVLoc = dvmapping[i];	
		//calculation: amplitude of each pixel should be close as possible to the median value, miVMedian. amount needed to increase the amplitude of i by is given by the
		//difference between miVMedian and miVPAvg[i] is ampOffset[i]. an amplitude difference between Voffset = 100 (HIGHER voltage at SiPM) and Voffset = 400 (LOWER voltage at SiPM)
		//is the difference between loVPAvg[i] and hiVPAvg[i], and the ratio of this difference in Voffset and measured amplitude is pixGain[i].
		//so, to increase the amplitude of the signal by ampOffset[i], we must subtract ampOffset[i]*pixGain[i] (which is bias[i]) from the value used, 255.
		//REG[i] is then the value of the register to have the required Voffset and also set the two bits to enable the channel & SiPMs, which should be 10 (bin) in the 10th and 11th bits of the register
		pixGain[DVLoc] = 300/(loVPAvg[DVLoc] - hiVPAvg[DVLoc]); //gain per pixel, calculated by dividing difference in Voffset by difference in average measured amplitudes of the pixel
		cout << "hiVPAvg[" << DVLoc << "] = " << hiVPAvg[DVLoc] << ", loVPAvg[" << DVLoc << "] = " << loVPAvg[DVLoc] << ", pixGain[" << DVLoc << "] = " << pixGain[DVLoc] << endl;
		ampOffset[DVLoc] = miVMedian-miVPAvg[DVLoc];
		bias[DVLoc] = ampOffset[DVLoc]*pixGain[DVLoc];
		REG[DVLoc] = (2 << 9 | 255 + bias[DVLoc]);
		CSVFile << i << "," << DVLoc << "," << ampOffset[DVLoc] << "," << pixGain[DVLoc] << "," << bias[DVLoc] << "," << REG[DVLoc] << endl;
		UpdatedDVFile << REG[DVLoc] << endl;
	}
	CSVFile.close();
	UpdatedDVFile.close();
}

std::vector<Double_t> dirData(std::string dirStr, std::string treeStr){
	int tEntries = 0;
	std::vector<Double_t> out(MaxNofChannels,0.0);
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(dirStr.c_str())) != NULL){
		while((ent = readdir(dir)) != NULL){
			string fileName = Form("%s%s",dirStr.c_str(),ent->d_name);
			if(fileName.substr(fileName.find_last_of(".")+1) == "root"){
				cout << "Loading file: " << fileName << endl;
				TFile *f0 = TFile::Open(fileName.c_str());
				TTree *tree = (TTree*)f0->Get(treeStr.c_str());
				Event *ev = new Event();
				tree->SetBranchAddress("Events", &ev);
				int nEntries = tree->GetEntries();
				tEntries += nEntries;
				cout << "Total Number of Events: " << nEntries << endl;
				for(int countEvent = 0; countEvent < nEntries; countEvent++){
					std::vector<Double_t> outRun(MaxNofChannels,0.0);
					tree->GetEntry(countEvent);
					Pulse *pulse;
					for(int i = 0; i < MaxNofChannels; i++){
						pulse = new Pulse(ev->GetSignalValue(i));
						int ampVal = pulse->GetAmplitude();
						if(ampVal == 0){
							tEntries -= 1;
							goto evskip;
						}
						outRun[i] = ampVal;
						delete pulse;
					}
					for(int i = 0; i < MaxNofChannels; i++){
						out[i] += outRun[i];
					}
					evskip:;
				}
				delete f0;
				//delete tree;
				delete ev;
			}
		}
	}
	cout << tEntries << endl;
	for(int i = 0; i < MaxNofChannels; i++){
		out[i] /= tEntries;
	}
	return out;
}

std::vector<int> getDVOffsetsMapping(std::string dvOffsetsPixelMappingFile){
	std::ifstream file(dvOffsetsPixelMappingFile);

	std::vector<int> dvmapping;
	std::string str;

	// Read the next line from File until it reaches the
	// end.
	while (file >> str) {
		// Now keep reading next line
		// and push it in vector function until end of file
		dvmapping.push_back(stoi(str));
	}
	return dvmapping;
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
