#include "StageControl.h"

StageControl::StageControl(int port)			//Port ttyS7
{
   fd = open_port(port);		
   configure_port();
}

StageControl::~StageControl()
{
   close_port();
}

int StageControl::open_port(int num)
{	
	TString portNum = to_string(num).c_str();
	TString port = "/dev/ttyS" + portNum;
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);		
	if(fd == -1) // if open is unsucessful
	{
		printf("open_port: Unable to open port for linear stage.\n");
	}
	else
	{
		fcntl(fd, F_SETFL, 0);		//block until data comes in
		cout<<"Port "<<port<<" is open."<<endl;
		//printf("Port is open for linear stage communication.\n");
	}
	
	return(fd);
} 

int StageControl::close_port()
{
	fd = close(fd);
	if(fd == -1) // if close is unsucessful
		printf("close_port: Unable to close port.\n");
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("Port is closed.\n");
	}
	
	return(fd);
} 

int StageControl::configure_port()      	
{
	struct termios port_settings;           				//structure to store the port settings in
	
	//tcgetattr(fd, &port_settings);							//Fetch the current port settings
	tcflush(fd, TCIOFLUSH);									//Flush the port's buffers (in and out) before using it

	cfsetispeed(&port_settings, B9600);     				//set baud rates
	cfsetospeed(&port_settings, B9600);

	port_settings.c_cflag |= (CLOCAL | CREAD);			//Enable the receiver and set local mode
	port_settings.c_cflag &= ~PARENB;       				//set no parity
	port_settings.c_cflag &= ~CSTOPB;						//one stop bit
	port_settings.c_cflag &= ~CSIZE;							//clear frame size info
	port_settings.c_cflag |= CS8;								//8 bit frames
	port_settings.c_cflag &= ~CRTSCTS;      				//Disable hardware flow control

	port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 	//Raw input data
	port_settings.c_iflag &= ~(IXON | IXOFF | IXANY); 				//Turn off software flow control
	port_settings.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP);		
	port_settings.c_oflag &= ~OPOST;							//Raw output data
	//port_settings.c_cc[VMIN] = 0;							//Specify minimum number of characters being read
	//port_settings.c_cc[VTIME] = 10;						//Specify time to read each character
	
	tcsetattr(fd, TCSANOW, &port_settings);    			//apply the settings to the port
	tcflush(fd,TCIOFLUSH);										//Flush the buffer one more time.
	return(fd);
}

int StageControl::query_modem(char* cmd)
{	
	//Send data
	write(fd, cmd, 6);
	//printf("Wrote the bytes. %x %x %x %x %x %x \n", (char)cmd[0], (char)cmd[1], (char)cmd[2], (char)cmd[3], (char)cmd[4], (char)cmd[5]);
	
	//Receive data
	int maxByte = 6;				
	if (cmd[0] == 0)
		maxByte = 18;
	volatile int bytes = 0;			//Readable data at port
	int timer = 0;
	unsigned char reply[maxByte];

	while ((bytes < maxByte) && (timer < 100))
	{
	   usleep(100000);
	   ioctl(fd, FIONREAD, &bytes);			//Check for readable data at port
	   timer++;
	}	
	//cout<<bytes<<endl;
	
	if (bytes > 0)
	{
		read(fd, reply, bytes);					//Read data
		//fcntl(fd, F_SETFL, FNDELAY);	
	   if ((cmd[1] == 0x3c) || (cmd[1] == 0x14))
	   {
	      position = (reply[2] + reply[3]*256 + reply[4]*256*256 + reply[5]*256*256*256) * stepSize / 1000;				//Return the current position in unit of mm.
	      if (reply[5] > 127)				//If position is negative
	      	position = position - pow(256,4) * stepSize / 1000;
	      printf("Current Position: %.2fmm \n", position);
  	   }
	   /*else
	   {
	      printf("Reading: \n");
	      int i = 0;
	      while(i < bytes)
	      {
	         printf("%x ", (char)reply[i]);
	         if ((i+1) % 6 == 0)
		   		printf("\n");
	      	i++;
	      }
	   }*/
	}

	return 0;	
}

Data StageControl::convert(long long int num)					//Convert number (in unit of 0.1mm) to microsteps.
{
   Data data;
   num = (long long int)(num * 100 / stepSize);
   if (num < 0)
		num += 256ll*256*256*256;
   data.byte6 = (int)(num/(256*256*256));
   num -= data.byte6*256*256*256;
   data.byte5 = (int)(num/(256*256));
   num -= data.byte5*256*256;
   data.byte4 = (int)(num/256);
   data.byte3 = (int)(num - data.byte4*256);
   return(data);
}

void StageControl::init()
{
   char cmd[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
   query_modem(cmd);
   cout<<"Returned to home position."<<endl;
}

void StageControl::moveAbs(int device, int pos)			//pos is in unit of 0.1mm.
{
   Data data = convert(pos);
   char cmd[] = {(char)device, 0x14, data.byte3, data.byte4, data.byte5, data.byte6};
   query_modem(cmd);
}

void StageControl::moveRel(int device, int dist)
{
  cout<<"moving to next pixel"<<endl;
   Data data = convert(dist);
   char cmd[] = {(char)device, 0x15, data.byte3, data.byte4, data.byte5, data.byte6};
   query_modem(cmd);
  cout<<"done moving"<<endl;
}

double StageControl::getX()
{
   char cmd[] = {0x01, 0x3c, 0, 0, 0, 0};
   query_modem(cmd);
   return position;
}

double StageControl::getY()
{
   char cmd[] = {0x02, 0x3c, 0, 0, 0, 0};
   query_modem(cmd);
   return position;
}

double StageControl::getZ()
{
   char cmd[] = {0x03, 0x3c, 0, 0, 0, 0};
   query_modem(cmd);
   return position;
}

void StageControl::Restore_Setting()
{
   char cmd[] = {0x00, 0x24, 0x00, 0x00, 0x00, 0x00};
   query_modem(cmd);
}

void StageControl::Renumber()
{
   char cmd[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
   query_modem(cmd);
}

void StageControl::Set_Hold_Current()
{
   char cmd[] = {0x00, 0x27, 0, 0, 0, 0};
   query_modem(cmd);
}
