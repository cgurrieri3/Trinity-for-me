R__LOAD_LIBRARY(libExACT.so)

#include <TTree.h>
#include <TFile.h>
//#include <Event.h>
#include <algorithm>

void readDir(string dirname,vector<string> *names) {

	string file;
	void *dir = gSystem->OpenDirectory(gSystem->ExpandPathName(dirname.c_str()));
    const char* strptr;
    if (dir) {
        while ((strptr = gSystem->GetDirEntry(dir))) {
          file = strptr;
          cout<<file<<endl;
          //if (file=="." || file==".." || file=="log") continue;
          //names->push_back(file);
          size_t found;
          found=file.find(".root");
          if (found==string::npos) continue;
          names->push_back(file);
        }
        gSystem->FreeDirectory(dir);
    }
	std::sort(names->begin(),names->end());
     // for(size_t i(0); i<names->size(); i++)
     //   Printf("%s",(*names)[i].c_str());
}

unsigned long long CorrectTime(Event *ev,unsigned long long startTime,unsigned long long timeT){
    unsigned long long timeInFile = ev->GetTBTime();
    unsigned long long timeClkTicks = timeInFile - startTime;

    return (timeT + timeClkTicks);
}   

bool TimeVariableReset(Event *ev, unsigned long long prevTime){
    bool isReset = false;

    if(ev->GetTBTime()<prevTime){
        isReset = true;
    }

    return isReset;
}

int MergeFiles (std::string MainDir,std::string fileNameOut){

    string DataDir = MainDir;
    vector<string> * filenames = new vector<string>;
    readDir(DataDir,filenames);
    

    string outFile =  MainDir+fileNameOut;

    TFile *fl_merge = new TFile(outFile.c_str(),"RECREATE");
    TTree *tHLED = new TTree("HLED","HLED Triggers");
    TTree *tBiFocal = new TTree("BiFocal","BiFocal Triggers");
    TTree *tForced = new TTree("Forced","Forced Triggers");
    TTree *tTest = new TTree("Test","Test Triggers");

	Event *evHLEDOut = new Event();
	Event *evBiFocalOut = new Event();
	Event *evForcedOut = new Event();
	Event *evTestOut = new Event();

	tHLED->Branch("Events","Event", &evHLEDOut,64000,0);
	tBiFocal->Branch("Events","Event", &evBiFocalOut,64000,0);
	tForced->Branch("Events","Event", &evForcedOut,64000,0);
	tTest->Branch("Events","Event", &evTestOut,64000,0);


    int runNHLED = 0;
    int runNTest = 0;
    int runNForced = 0;
    int runNBF = 0;

    unsigned long long rTimeHLED= 0;
    unsigned long long rTimeBiFocal= 0;
    unsigned long long rTimeForced= 0;
    unsigned long long rTimeTest= 0;
	
	
	tHLED->Branch("RunNumber", &runNHLED,"RunNumber/I");
    tBiFocal->Branch("RunNumber",&runNBF,"RunNumber/I");
    tForced->Branch("RunNumber", &runNForced,"RunNumber/I");
    tTest->Branch("RunNumber", &runNTest,"RunNumber/I");

    tHLED->Branch("RTimeTB", &rTimeHLED);
    tBiFocal->Branch("RTimeTB",&rTimeBiFocal);
    tForced->Branch("RTimeTB", &rTimeForced);
    tTest->Branch("RTimeTB", &rTimeTest);

    

    TList *lst_Files = new TList();
    int hours;
    int minutes;
    double seconds;
    std::vector<unsigned long long> timeVector;
    size_t pos;
    stringstream tempSS;
    std::string temp;

    unsigned long long finalTime;
    for (int i = 0; i<filenames->size(); i++){

	    stringstream tempSS;
            tempSS<<(*filenames)[i].c_str();
            string fileNameTString(tempSS.str());
	    cout<<fileNameTString<<endl;

            pos = fileNameTString.find('T');
            temp = fileNameTString.substr(pos+1, 2);

            hours = std::stoi(temp);

            temp = fileNameTString.substr(pos+4, 2);

	    minutes = std::stoi(temp);

            temp = fileNameTString.substr(pos+7, 6);
            seconds = std::stod(temp);
	    
	    finalTime = hours*3600*1e8;

	    finalTime += minutes*60*1e8;

	    finalTime += seconds*1e8;
            timeVector.push_back(hours*3600+minutes*60+seconds);

	    std::string fstring = DataDir+"/"+fileNameTString;
            lst_Files->Add(TFile::Open(fstring.c_str()));
    }


    //TString path( (char*)strstr( dir_events->GetPath(), ":" ) );
    //path.Remove( 0, 2 );


//    cout<<"Working Path: "<<path<<endl;


    TFile *source = (TFile*)lst_Files->First();
    source->cd();
    TTree *tHLEDIn= 0;
    TTree *tBiFocalIn = 0;
    TTree *tForcedIn = 0;
    TTree *tTestIn = 0;


//    TTree *t_temp_tSim;
//    TTree *t_temp_T0;

    //fstream fileHLED, fileBiFocal, fileForced, fileTest;
    //std::string fnameHLED, fnameBiFocal, fnameForced,fnameTest;
    //fnameTest= MainDir+"/MergedTest.dat";

//    fileHLED.open(fnameHLED.c_str(),ios::out|ios::binary);
//    fileBiFocal.open(fnameBiFocal.c_str(),ios::out|ios::binary);
//    fileForced.open(fnameForced.c_str(),ios::out|ios::binary);
//    fileTest.open(fnameTest.c_str(),ios::out|ios::binary);
	Event *evHLED = 0;
	Event *evBiFocal = 0;
	Event *evForced = 0;
	Event *evTest = 0;

    unsigned long long startTimeEB;

    unsigned long tbTimeClock;
    unsigned long fileNameTime;

    unsigned int offsetTime = -1;
    unsigned long timeAdjustment = 0;

    unsigned long long eventTime = 0;
    unsigned long long prevTime = 0;
	
    unsigned long long unixEpoch = 1684022400;
    int fileNo = 0;
	
     while(source){
    
     	source->cd();
        tHLEDIn = (TTree*)source->Get("HLED");
        //cout<<tHLEDIn->GetEntries()<<endl;
        tForcedIn = (TTree*)source->Get("Forced");
        tTestIn = (TTree*)source->Get("Test");
        tBiFocalIn = (TTree*)source->Get("BiFocal");
         
    
        tHLEDIn->SetBranchAddress("Events",&evHLED);
        tBiFocalIn->SetBranchAddress("Events",&evBiFocal);
        tTestIn->SetBranchAddress("Events",&evTest);
        tForcedIn->SetBranchAddress("Events",&evForced);
	   
	   for(int i = 0; i<tHLEDIn->GetEntries()-1; i++){
	   	tHLEDIn->GetEntry(i);
	   	evHLEDOut = evHLED;
        if(TimeVariableReset(evHLED, prevTime)){
            timeAdjustment = timeAdjustment + offsetTime;
        }
        if(i == 0){
            startTimeEB = evHLED->GetTBTime()/100000000;
        }
	prevTime = evHLED->GetTBTime();
	if(startTimeEB != 0){
	        rTimeHLED = timeAdjustment+CorrectTime(evHLED,startTimeEB*1e8,timeVector[fileNo]*1e8)+unixEpoch*1e8;
        }
	else{
		rTimeHLED = 0;
	}
//	cout<<rTimeHLED<<endl;
	   	fl_merge->cd();
	   	tHLED->Fill();
	   }

       timeAdjustment = 0;
    
	   for(int i = 0; i<tBiFocalIn->GetEntries(); i++){
	   	tBiFocalIn->GetEntry(i);
	   	evBiFocalOut = evBiFocal;
        if(TimeVariableReset(evBiFocal, prevTime)){
            timeAdjustment = timeAdjustment + offsetTime;
        }
        if(i == 0){
            startTimeEB = evBiFocal->GetTBTime()/100000000;
        }
	prevTime = evBiFocal->GetTBTime();
        if(startTimeEB != 0 ){
		rTimeBiFocal = timeAdjustment+CorrectTime(evBiFocal,startTimeEB*1e8,timeVector[fileNo]*1e8)+unixEpoch*1e8;
        }else{
		rTimeBiFocal = 0;
	}
//	cout<<rTimeBiFocal<<endl;
	   	fl_merge->cd();
	   	tBiFocal->Fill();
	   }
        
        timeAdjustment = 0;

	   for(int i = 0; i<tForcedIn->GetEntries(); i++){
	   	tForcedIn->GetEntry(i);
	   	evForcedOut = evForced;
        if(TimeVariableReset(evForced, prevTime)){
            timeAdjustment = timeAdjustment + offsetTime;
        }
        if(i == 0){
            startTimeEB = evForced->GetTBTime()/100000000;
        }
	prevTime = evForced->GetTBTime();
	if(startTimeEB != 0){
	        rTimeForced = timeAdjustment+CorrectTime(evHLED,startTimeEB*1e8,timeVector[fileNo]*1e8)+unixEpoch*1e8;
        }else{
		rTimeForced = 0;
	}
//	cout<<rTimeForced<<endl;
	   	fl_merge->cd();
	   	tForced->Fill();
	   }
    
        timeAdjustment = 0;

	   for(int i = 0; i<tTestIn->GetEntries(); i++){
	   	tTestIn->GetEntry(i);
	   	evTestOut = evTest;
        if(TimeVariableReset(evForced, prevTime)){
            timeAdjustment = timeAdjustment + offsetTime;
        }
        if(i == 0){
            startTimeEB = evTest->GetTBTime()/100000000;
        }
	prevTime = evTest->GetTBTime();
        if(startTimeEB != 0){
		rTimeTest = timeAdjustment+CorrectTime(evHLED,startTimeEB*1e8,timeVector[fileNo]*1e8)+unixEpoch*1e8;
        }else{
		rTimeTest = 0;
	}
//	cout<<rTimeTest<<endl;
	   	fl_merge->cd();
	   	tTest->Fill();
	   }

       timeAdjustment = 0;
	//fileHLED.write((char*)tHLEDIn,sizeof(tHLEDIn));
	//fileForced.write((char*)tForcedIn,sizeof(tForcedIn));
	//fileTest.write((char*)tTestIn,sizeof(tTestIn));
	//fileBiFocal.write((char*)tBiFocalIn,sizeof(tBiFocalIn));

//        tHLED = tHLEDIn->CloneTree();
//        tBiFocal = tBiFocalIn->CloneTree();
//        tForced = tForcedIn->CloneTree();
//        tTest = tTestIn->CloneTree();

/*        fl_merge->cd();
        tHLED->Fill();
        tBiFocal->Fill();
        tTest->Fill();
        tForced->Fill();
*/
       fileNo++;
        source = (TFile*)lst_Files->After(source);
        runNHLED++;
        runNBF++;
        runNForced++;
	    runNTest++;
    }
	
//	fileHLED.close();
//	fileForced.close();
//	fileTest.close();
//	fileBiFocal.close();
    fl_merge->cd();
    fl_merge->Write();
    fl_merge->Close();


    return 0;


}
