#include "DataSummary.h"

#include "rootils.h"
#include "constants.h"

#include <TCanvas.h>
#include <TH2.h>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv){
    if(argc != 3){
        cout << "Usage: ./FolderDataSum YYYYMMDD y/n" << endl;
    }

    std::string mount = argv[2];
    
    if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
        std::cout << "using mounted directory path" << std::endl;
        mnt="/mnt/";
    }
    else if(mount != "n"){
        std::cout << "using specific directory path" << std::endl;
        mnt=mount.c_str();
    }

    dataDir = Form("%sDataAnalysis/",mnt.c_str());
    
    string outStr = Form("%s%s",outDir.c_str(),argv[1]);
    DataSummary ds(argv[1]);
    if(ds.hasData()){
        ds.t_disp->Print((outStr+".pdf[").c_str());

        ds.PlotTrig();
        ds.t_disp->Print((outStr+".pdf").c_str());

        ds.PlotROIMusic();
        ds.t_disp->Print((outStr+".pdf").c_str());

        if(ds.hasData44()){

            ds.PlotFF44();
            ds.t_disp->Print((outStr+".pdf").c_str());

            ds.PlotHLED44();
            ds.t_disp->Print((outStr+".pdf").c_str());  

            ds.PlotHLEDNorm44();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotPedestal44();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotPedestalRMS44();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotAmplitude44();
            ds.t_disp->Print((outStr+".pdf").c_str());
            
            ds.PlotCharge44();
            ds.t_disp->Print((outStr+".pdf").c_str());

            ds.PlotTimePeak44();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotPSF44();
            ds.t_disp->Print((outStr+".pdf").c_str());
        }

        if(ds.hasData415()){
            if(ds.hasHLEDData415()){
                ds.PlotFF415();
                ds.t_disp->Print((outStr+".pdf").c_str());

                ds.PlotHLED415();
                ds.t_disp->Print((outStr+".pdf").c_str());  

                ds.PlotHLEDNorm415();
                ds.t_disp->Print((outStr+".pdf").c_str());
            }

            ds.PlotPedestal415();
            ds.t_disp->Print((outStr+".pdf").c_str()); 
            
            ds.PlotPedestalRMS415();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotAmplitude415();
            ds.t_disp->Print((outStr+".pdf").c_str());   

            ds.PlotCharge415();
            ds.t_disp->Print((outStr+".pdf").c_str()); 

            ds.PlotTimePeak415();
            ds.t_disp->Print((outStr+".pdf").c_str());  

            ds.PlotPSF415();
            ds.t_disp->Print((outStr+".pdf").c_str());
        }

        ds.t_disp->Print((outStr+".pdf]").c_str());

        ofstream csvOutput;
        csvOutput.open((outStr+".csv"));

        if(ds.hasData44()){
            csvOutput << ds.GetAvgEv44() << "," << ds.GetAmpDist44() << "," << ds.GetHLEDMean44() << "," << ds.GetHLEDNMean44() << "," << ds.GetPedMean44() << "," << ds.GetPedRMSMean44() << "," << ds.GetqMean44() << "," << ds.GetPTMean44() << "," << ds.GetPSFSigma44() ;
        }
        else{
            csvOutput << "-1,-1,-1,-1,-1,-1,-1,-1,-1" ;
        }
        
        if(ds.hasData415()){
            csvOutput << "," << ds.GetAvgEv415() << "," << ds.GetAmpDist415() << "," << ds.GetHLEDMean415() << "," << ds.GetHLEDNMean415() << "," << ds.GetPedMean415() << "," << ds.GetPedRMSMean415() << "," << ds.GetqMean415() << "," << ds.GetPTMean415() << "," << ds.GetPSFSigma415();
        }
        
        for(vector<int>& vec : ds.GetTrTh()){
            csvOutput << ",(" << vec[0] << "," << vec[1] << ")";
        }
        csvOutput.close();
        
        system(Form("chmod 660 %s",(outStr+".pdf").c_str()));
        system(Form("chmod 660 %s",(outStr+".csv").c_str()));
    }
    else{
        cout << "No data saved to Test branch on date " << argv[1] << endl;
        ofstream csvOutput;
        csvOutput.open((outStr+".csv"));
        csvOutput << "-1,-1,-1,-1,-1,-1,-1,-1,-1";
        csvOutput.close();
        system(Form("chmod 660 %s",(outStr+".csv").c_str()));
    }
}