#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>

#include <TString.h>
#include <Getline.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TAxis.h>
#include <TTimer.h>
#include "SignalAnalysis.h"
#include "StageControl.h"
#include "SwitchControl.h"

using namespace std;

int main (int argc, char* argv[]){
  SwitchControl sw(5);
  double channel_id = atof(argv[1]);
  sw.switch_channel(channel_id);
}
