#include "StageControl.h"
#include <iostream>
#include <fstream>
#include <string.h> // String function definitions
#include <TString.h>
#include <TGraph.h>
#include <vector>
#include <stdio.h> // standard input / output functions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitionss
#include <time.h>   // time calls
#include <math.h>
#include <sys/ioctl.h>

using namespace std;

int main (int argc, char* argv[]){ //argv[] Has Pixel ID
	cout<<"Initializing linear stage"<<endl;
	StageControl sc(7);
	//int coord = 1;

	//Look for PixelID Coordinates from LUT

	//int x_mm = GetXCoord(pixID);
	//int y_mm = GetYCoord(pixID);

	double x_mm = atof(argv[1]);
	double y_mm = atof(argv[2]);
	double z_mm = atof(argv[3]);
	
	cout << "Inputs: " <<endl;
	cout << argv[1] << endl;
	cout << argv[2] <<endl;
	cout << argv[3] <<endl;

	int x_step = x_mm/0.1;
	int y_step = y_mm/0.1;
	int z_step = z_mm/0.1;

	double x_start = sc.getX(); //Object module has Y-axis labeled for what in the setup is X
	double y_start = sc.getY(); //Object module has Z-axis labeled for what in the setup is Y
	double z_start = sc.getZ();
	
	cout<<x_start<<endl;
	cout<<y_start<<endl;
	cout<<z_start<<endl;

	sc.moveAbs(1, x_step); //X Axis											   
	sc.moveAbs(2, y_step); //Y Axis
	sc.moveAbs(3, z_step); //Z Axis?

	double x_end = sc.getX(); //Object module has Y-axis labeled for what in the setup is X
	double y_end = sc.getY(); //Object module has Z-axis labeled for what in the setup is Y
	double z_end = sc.getZ();

	cout<<x_end<<endl;
	cout<<y_end<<endl;
	cout<<z_end<<endl;


	return 0;
}
