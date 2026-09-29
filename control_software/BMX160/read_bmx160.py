import sys
import time
import smbus
from operator import add
from datetime import datetime
from DFRobot_BMX160 import BMX160

# First parameter is smbus number which is related to hardware i2c-0. Run sudo i2cdetect -l for more info.
# Second parameter is the address of the device on the bus. Only 0x68 and 0x69 are available for this device.
bmx_bot = BMX160(0, 0x69)
bmx_top = BMX160(0, 0x68)

while not bmx_bot.begin():
    time.sleep(0.2)

while not bmx_top.begin():
    time.sleep(0.2)

bmx_bot.set_gyro_range(bmx_bot.GyroRange_125DPS)
time.sleep(0.2)
bmx_top.set_gyro_range(bmx_top.GyroRange_125DPS)
time.sleep(0.2)

def main():
    if(len(sys.argv) == 3):
        nofsamples = int(sys.argv[1])
        while True:
            data_bot_avg = [0.0] * 9
            data_top_avg = [0.0] * 9
            for i in range(nofsamples):
                now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                #data_bot = list(range(9))  // For testing
                #data_top = list(range(9))  // For testing
                data_bot= bmx_bot.get_all_data()
                data_top= bmx_top.get_all_data()
                print(str(now)+"\t{0:+.3f} {1:+.3f} {2:+.3f} {3:+.3f} {4:+.3f} {5:+.3f} {6:+.3f} {7:+.3f} {8:+.3f} {9:+.3f} {10:+.3f} {11:+.3f} {12:+.3f} {13:+.3f} {14:+.3f} {15:+.3f} {16:+.3f} {17:+.3f}".format(data_bot[0],data_bot[1], data_bot[2], data_bot[3], data_bot[4], data_bot[5], data_bot[6], data_bot[7], data_bot[8], data_top[0], data_top[1], data_top[2], data_top[3], data_top[4], data_top[5], data_top[6], data_top[7], data_top[8]))
                data_bot_avg = list(map(add, data_bot, data_bot_avg))
                data_top_avg = list(map(add, data_top, data_top_avg))
                time.sleep(1)
            data_bot_avg[:] = [x / nofsamples for x in data_bot_avg]
            data_top_avg[:] = [x / nofsamples for x in data_top_avg]
            f = open(sys.argv[2], 'a')
            f.write(str(int(time.time())))
            for j in data_bot_avg:
                f.write("\t%i" % j)
            for j in data_top_avg:
                f.write("\t%i" % j)
            f.write("\n")
            f.close()
    else:
        print("Incorrect Number of arguments. Please run this script as this:")
        print("sudo python read_bmx160.py <averaging_period_in_seconds, e.g., 60> <filename_to_save_averaged_data, e.g., data.txt>")

if __name__ == "__main__":
    main()
