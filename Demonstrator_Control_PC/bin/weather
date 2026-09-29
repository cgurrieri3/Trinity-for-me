#!/usr/bin/python3

# Import libs
import serial
import datetime as dt

class weatherlogger:
    def __init__(self):
        # COM port constants
        self.COM      = '/dev/ttymxc1' # COM PORT 2
        self.BAUDRATE = 19200
        self.TIMEOUT  = 1
        self.main()

    def rotate_file(self):
        # Generate the rotating file name
        now = dt.datetime.now()
        logname = '/root/weather_data/weather_' + now.strftime('%Y%m%d')
        #print(logname)
        return logname

    def main(self):
        while True:
            try:
                ser = serial.Serial(self.COM,self.BAUDRATE,timeout=self.TIMEOUT)
                line = ser.readline().decode('ascii')
                f = open(self.rotate_file(), "a")
                print(line,file=f)
                f.close()
            except:
                print("Some error...but keep looking at weather data")

if __name__=='__main__':
    weatherlogger()
    
