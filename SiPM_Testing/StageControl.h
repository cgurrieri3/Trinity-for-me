#include <iostream>
#include <string.h> // String function definitions
#include <TString.h>
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

struct Data {
   char byte3;
   char byte4;
   char byte5;
   char byte6;
};

class StageControl
{
   private:
   
   int open_port(int);
	int close_port();
	int configure_port();
	int query_modem(char* cmd);
	int fd;

	Data convert(long long int);
	double position;
	double stepSize = 0.047625;							//Step resolution; unit in micron.

   public:
	StageControl(int);
	~StageControl();
	void init();												//Reset the linear stage to initial position.
	void moveAbs(int device, int pos);					//Move device to a position
	void moveRel(int device, int dist);					//Move device a distance from its current position.
	double getX();												//Return current position.
	double getY();
	double getZ();
	void Restore_Setting();									//Restore all device to factory setting.
	void Renumber();											//Reset device numbering.
	void Set_Hold_Current();								//Turn hold current off to reduce noise.
};
