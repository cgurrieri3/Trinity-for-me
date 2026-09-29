#include <iostream>
#include <string.h> // String function definitions
#include <TString.h>
#include <vector>
#include <stdio.h> // standard input / output functions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#include <time.h>   // time calls
#include <Getline.h>
#include <math.h>
#include <sys/ioctl.h>

using namespace std;

class SwitchControl
{
   private:
	int open_port(int);
	int close_port();
	int configure_port();

	int fd;

   public:
	SwitchControl(int);
	~SwitchControl();
	int switch_channel(int i);
};
