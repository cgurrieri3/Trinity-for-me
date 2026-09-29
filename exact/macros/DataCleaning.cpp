R__LOAD_LIBRARY(libExACT.so)
#include <iostream>
#include <TH2.h>
#include <TCanvas.h>
#include <TProfile2D.h>

int DataCleaning(std::string fileName){
	IFile *file = new IFile(fileName,"UPDATE");
	bool isBadEvent;
	bool isOverTriggered;
	int badcount = 0;
	int overTrigCount = 0;
	IEvent *ev;
	std::vector<float> ampl;
	TBranch *badBranch = file->treeBiFocal->Branch("isBadEvent",&isBadEvent,"isBadBranch/O");
	TBranch *overTriggeredBranch = file->treeBiFocal->Branch("isOverTriggered",&isOverTriggered,"isOverTriggered/O");
	float firstPeak, secondPeak;
	int firstID, secondID;
	for(int i = 0; i <file->treeBiFocal->GetEntries(); i++){
		isOverTriggered = false;
		ev = new IEvent();
		isBadEvent = IHealthTools::IsBadEvent(file,i,"bifocal",0,150,20,10);
		ampl = IHealthTools::CameraAmplitude(file, i, "bifocal",230,250);
//		
		
		file->treeBiFocal->SetBranchAddress("Events",&ev);
		file->treeBiFocal->GetEntry(i);
		if((ev->GetROIMusicID()[0]==5 || ev->GetROIMusicID()[0]==6) && !isBadEvent){
			//cout<<"Here"<<endl;
			firstPeak = IUtilities::GetMinimum(ampl, ev->GetROIMusicID()[0]);
			firstID = IUtilities::GetMinimumPixelID(ampl,ev->GetROIMusicID()[0]);
			//cout<<firstPeak<<endl;
			secondPeak = IUtilities::GetMinimum(ampl, ev->GetROIMusicID()[0]+1);
			secondID = IUtilities::GetMinimum(ampl,ev->GetROIMusicID()[0]+1);

			if(!(TMath::Abs(secondID - firstID - 8) <= 1)||TMath::Abs(firstPeak - secondPeak)/firstPeak > 0.05){
				overTrigCount++;
				isOverTriggered = true;
			}
		}
		if (isBadEvent){
			badcount++;
		}
		if(i%100==0){
			cout<<"Processed Event: "<<i<<endl;
		}
		badBranch->Fill();
		overTriggeredBranch->Fill();
		delete ev;
	}
	cout<<"Total Bad Events: "<<badcount<<endl;
	cout<<"Total OverTriggered Events: "<<overTrigCount<<endl;
	file->ITFile->Write();
	file->ITFile->Close();

	cout<<"Added Branch isBadEvent to BiFocal tree"<<endl;
	cout<<"Added Branch isOverTriggered to BiFocal tree"<<endl;
	return 0;
}