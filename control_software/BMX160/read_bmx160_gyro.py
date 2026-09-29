# reads gyro data
import sys
import time
import smbus
from DFRobot_BMX160 import BMX160

# First parameter is smbus number which is related to hardware i2c-0. Run sudo i2cdetect -l for more info.
# Second parameter is the address of the device on the bus. Only 0x68 and 0x69 are available for this device.
bmx_bot = BMX160(0, 0x69)
bmx_top = BMX160(0, 0x68)

# Note: change the and to or, once both backplanes are connected
while not bmx_bot.begin():
    time.sleep(0.2)

while not bmx_top.begin():
    time.sleep(0.2)

bmx_bot.set_gyro_range(bmx_bot.GyroRange_500DPS)
time.sleep(0.2)
bmx_top.set_gyro_range(bmx_top.GyroRange_500DPS)
time.sleep(0.2)

def main():
    while True:
        #start_ms = int(time.time() * 1000)
        data_bot= bmx_bot.get_all_data()
        data_top= bmx_top.get_all_data()
        print("Bot Gyroscope:    x: {0:+.2f}, y: {1:+.2f}, z: {2:+.2f}".format(data_bot[3],data_bot[4],data_bot[5]))
        print("Top Gyroscope:    x: {0:+.2f}, y: {1:+.2f}, z: {2:+.2f}".format(data_top[3],data_top[4],data_top[5]))
        #end_ms = int(time.time() * 1000)
        #print "Time difference: ", (end_ms-start_ms)
        print("----------------------------------------------")
        time.sleep(0.99)

if __name__ == "__main__":
    main()