# reads all data
import sys
import time
import smbus
from operator import add
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

bmx_bot.set_gyro_range(bmx_bot.GyroRange_125DPS)
time.sleep(0.2)
bmx_top.set_gyro_range(bmx_top.GyroRange_125DPS)
time.sleep(0.2)

def main():
    bot_avg = [ 0.009673, -0.02523,  0.02633]
    top_avg = [-0.029740, -0.034300,-0.01013]
    #bot_avg = [ 0.0128811, -0.0291541,  0.0270199]
    #top_avg = [-0.0245428, -0.0385292, -0.0110519]
    #bot_avg = get_average(bmx_bot, 100)
    #top_avg = get_average(bmx_top, 100)
    #print(bot_avg)
    #print(top_avg)

    #pos = [0.0] * 6
    while True:
        #deg = [0.0] * 6
        #nofsamples = 10
        #for i in range(nofsamples):
        data_bot= bmx_bot.get_all_data()
        #deg[0] = data_bot[3]
        #deg[1] = data_bot[4]
        #deg[2] = data_bot[5]
        #deg[0] = data_bot[3]-bot_avg[0]
        #deg[1] = data_bot[4]-bot_avg[1]
        #deg[2] = data_bot[5]-bot_avg[2]
        #deg[0] += (data_bot[3]-bot_avg[0])
        #deg[1] += (data_bot[4]-bot_avg[1])
        #deg[2] += (data_bot[5]-bot_avg[2])

        #deg[0] = data_bot[6]
        #deg[1] = data_bot[7]
        #deg[2] = data_bot[8]

        #print("Bot mag (uT):    x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(data_bot[0],data_bot[1],data_bot[2]))
        #print("Bot gyr (g):     x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format((data_bot[3]-bot_avg[0]),(data_bot[4]-bot_avg[1]),(data_bot[5]-bot_avg[2])))
        #print("Bot acc (m/s^2): x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(data_bot[6],data_bot[7],data_bot[8]))
        #print("")
        #time.sleep(0.05)

        data_top= bmx_top.get_all_data()
        #deg[3] = data_top[3]
        #deg[4] = data_top[4]
        #deg[5] = data_top[5]
        #deg[3] = data_top[3]-top_avg[0]
        #deg[4] = data_top[4]-top_avg[1]
        #deg[5] = data_top[5]-top_avg[2]
        #deg[3] += (data_top[3]-top_avg[0])*0.1
        #deg[4] += (data_top[4]-top_avg[1])*0.1
        #deg[5] += (data_top[5]-top_avg[2])*0.1

        #deg[3] = data_top[6]
        #deg[4] = data_top[7]
        #deg[5] = data_top[8]
        #print("Top mag (uT):    x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(data_top[0],data_top[1],data_top[2]))
        #print("Top gyr (g):     x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format((data_top[3]-top_avg[0]),(data_top[4]-top_avg[1]),(data_top[5]-top_avg[2])))
        #print("Top acc (m/s^2): x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(data_top[6],data_top[7],data_top[8]))
        #print("----------------------------------------")
        #pos = list(map(add, pos, deg))
        #print("{0:+.3f} {1:+.3f} {2:+.3f} {3:+.3f} {4:+.3f} {5:+.3f}".format(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]))
        print("{0:+.3f} {1:+.3f} {2:+.3f} {3:+.3f} {4:+.3f} {5:+.3f} {6:+.3f} {7:+.3f} {8:+.3f} {9:+.3f} {10:+.3f} {11:+.3f} {12:+.3f} {13:+.3f} {14:+.3f} {15:+.3f} {16:+.3f} {17:+.3f}".format(data_bot[0], data_bot[1], data_bot[2], data_bot[3], data_bot[4], data_bot[5], data_bot[6], data_bot[7], data_bot[8], data_top[0], data_top[1], data_top[2], data_top[3], data_top[4], data_top[5], data_top[6], data_top[7], data_top[8]))
        #print("{0:+.3f} {1:+.3f} {2:+.3f} {3:+.3f} {4:+.3f} {5:+.3f} {6:+.3f} {7:+.3f} {8:+.3f}".format(data_top[0], data_top[1], data_top[2], data_top[3], data_top[4], data_top[5], data_top[6], data_top[7], data_top[8]))
        #print("{0:+.3f} {1:+.3f} {2:+.3f} {3:+.3f} {4:+.3f} {5:+.3f}".format(deg[0], deg[1], deg[2], deg[3], deg[4], deg[5]))
        #print("Bot backplane: x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(deg[0], deg[1], deg[2]))
        #print("Top backplane: x={0:+.3f}, y={1:+.3f}, z={2:+.3f}".format(deg[3], deg[4], deg[5]))
        #print("-------------------------------------------")
        time.sleep(1)

def get_average(bmx, n):
    gyro = []
    gx = 0.0
    gy = 0.0
    gz = 0.0
    for i in range(n):
        data= bmx.get_all_data()
        time.sleep(0.1)
        gx = gx + data[3]
        gy = gy + data[4]
        gz = gz + data[5]
    gyro.append(gx/n)
    gyro.append(gy/n)
    gyro.append(gz/n)
    return gyro

if __name__ == "__main__":
    main()
