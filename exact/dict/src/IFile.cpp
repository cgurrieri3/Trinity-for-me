#include "IFile.h"

IFile::IFile(std::string filename,std::string mode){
	ITFile = new TFile(filename.c_str(),mode.c_str());
	GetTrees();
}

void IFile::GetTrees(){
	//gets all trees in root file
	treeHLED = (TTree*)ITFile->Get("HLED");
	treeBiFocal = (TTree*)ITFile->Get("BiFocal");
	treeForced = (TTree*)ITFile->Get("Forced");
	treeTest = (TTree*)ITFile->Get("Test");
}

int IFile::GetTreeID(std::string treeName){
	//matches treeName to known trees and assigns it an ID
	std::transform(treeName.begin(),treeName.end(),treeName.begin(),::toupper);
	int treeID = 0;

	if(treeName.compare("HLED")==0){
		treeID = 0;
	}else if(treeName.compare("BIFOCAL")==0){
		treeID = 1;
	}else if(treeName.compare("TEST")==0){
		treeID = 2;
	}else if(treeName.compare("FORCED")==0){
		treeID = 3;
	}else{
		std::cout<<"Tree Name not found!"<<std::endl;
	}

	return treeID;
	
}

IFile::~IFile(){
	ITFile->Close();
}
