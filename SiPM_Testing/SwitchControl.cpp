#include "SwitchControl.h"

SwitchControl::SwitchControl(int port)						//Port ttyS6
{
   fd = open_port(port);		
   configure_port();
}

SwitchControl::~SwitchControl()
{
   close_port();
}

int SwitchControl::open_port(int num)
{	
	TString portNum = to_string(num).c_str();
	TString port = "/dev/ttyS" + portNum;
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);		
	if(fd == -1) // if open is unsucessful
	{
		printf("open_port: Unable to open port for Switch device.\n");
	}
	else
	{
		fcntl(fd, F_SETFL, 0);		//block until data comes in
		cout<<"Port "<<port<<" is open."<<endl;
		//printf("Port is open for Keithley Switch device communication.\n");
	}
	
	return(fd);

} 

int SwitchControl::close_port()
{
	fd = close(fd);
	
	if(fd == -1) // if close is unsucessful
	{
		printf("close_port: Unable to close port. \n");
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("Port is closed.\n");
	}

	return(fd);
}

int SwitchControl::configure_port()      	
{
	//Note: the Keithley Switch device needs to be set for carrier to be LF or CRLF for proper reading back.
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

int SwitchControl::switch_channel(int i)
{
	//write(fd,"SYST:BEEP 1\n", 12); 		// Quiet the Keithley
	//write(fd,"DISP:TEXT:DATA 'TESTING'\n", 25);
	//write(fd,"DISP:TEXT:STAT ON\n", 18);
	//write(fd,"TRAC:CLE\n", 9);			// Clear the buffer at the beginning
	//write(fd,"*RST\n", 5);				// Will also open all channels
	//write(fd,"INIT:CONT OFF\n", 14);		// Disable continuous scan
	//write(fd,"TRIG:COUN 1\n", 12);			// Set number of scan
	//write(fd,"SAMP:COUN 8\n", 12);			// Determine number of scan channels
	//write(fd,"ROUT:SCAN (@101:108)\n", 21);		// Configure scan
	//write(fd,"ROUT:SCAN:LSEL INT\n", 19);		// Enable scan
	//write(fd,"ROUT:SCAN:LSEL NONE\n", 20);		// Stop scanning
	//write(fd,"CALC1:DATA?\n", 12);		
	//write(fd,"FUNC 'VOLT'\n", 12);
	//write(fd,"FUNC 'CURR'\n", 12);
	//write(fd,"READ?\n", 6);
	//write(fd,"ROUT:OPEN:ALL\n", 14);		// Good practice to open all channels at end
cout<<"switching channel"<<endl;
   if(i == 0)
		write(fd,"ROUT:MULT:CLOS (@101)\n", 22);	// Close a channel
   else if(i == 1)
		write(fd,"ROUT:MULT:CLOS (@102)\n", 22);
   else if(i == 2)
		write(fd,"ROUT:MULT:CLOS (@103)\n", 22);
   else if(i == 3)
		write(fd,"ROUT:MULT:CLOS (@104)\n", 22);
   else if(i == 4)
		write(fd,"ROUT:MULT:CLOS (@105)\n", 22);
   else if(i == 5)
		write(fd,"ROUT:MULT:CLOS (@106)\n", 22);
   else if(i == 6)
		write(fd,"ROUT:MULT:CLOS (@107)\n", 22);
   else if(i == 7)
		write(fd,"ROUT:MULT:CLOS (@108)\n", 22);
   else if(i == 8)
		write(fd,"ROUT:MULT:CLOS (@201)\n", 22);
   else if(i == 9)
		write(fd,"ROUT:MULT:CLOS (@202)\n", 22);
   else if(i == 10)
		write(fd,"ROUT:MULT:CLOS (@203)\n", 22);
   else if(i == 11)	
		write(fd,"ROUT:MULT:CLOS (@204)\n", 22);	
   else if(i == 12)
		write(fd,"ROUT:MULT:CLOS (@205)\n", 22);
   else if(i == 13)
		write(fd,"ROUT:MULT:CLOS (@206)\n", 22);
   else if(i == 14)
		write(fd,"ROUT:MULT:CLOS (@207)\n", 22);
   else if(i == 15)
		write(fd,"ROUT:MULT:CLOS (@208)\n", 22);
   else
		write(fd,"ROUT:OPEN:ALL\n", 14);

cout<<"done switching"<<endl;	
	write(fd,"ROUT:MULT:CLOS?\n", 16);				//Query for closed channels (4 total).

	volatile int bytes = 0;								//Readable data at port
	int maxByte = 20;
	unsigned char reply[maxByte];
	int timer = 0;
	while((timer < 40) && (bytes < maxByte))
	{
		usleep(100000);									//Sleep for 0.1 sec.
	   ioctl(fd, FIONREAD, &bytes);					//Check for readable data at port
	   timer++;
	}
	//cout<<"Number of replied bytes = "<<bytes<<endl;

	if ((bytes > maxByte) || (bytes == 0))
	{
	   cout<<"Output overflow or unable to receive output."<<endl;
	   Getline("Type <return> to go on: ");
	}
	else														//Read data
	{  
	   read(fd, reply, bytes);
		cout<<"Reading: ";
	   int j = 0;
	   while(j<bytes)
	   {
	      printf("%c", (char)reply[j]);
	      j++;
	   }
	      //printf("\n");
	}

	return 0;
}
/*
int main(void)
{
	SwitchControl sc(6);
	sc.switch_channel(16);
	sw.switch_channel(3);

	return(0);
	
}*/
