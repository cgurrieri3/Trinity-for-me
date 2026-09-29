import sys
import time
import smbus
from DFRobot_BMX160 import BMX160

# First parameter is smbus number which is related to hardware i2c-0. Run sudo i2cdetect -l for more info.
# Second parameter is the address of the device on the bus. Only 0x68 and 0x69 are available for this device.
bmx_bot = BMX160(0, 0x69)
bmx_top = BMX160(0, 0x68)

# begin return True if succeed, otherwise return False
while not bmx_bot.begin():
    time.sleep(1)

while not bmx_top.begin():
    time.sleep(1)

def main():

    FastOffsetCompensation(bmx_bot)
    time.sleep(0.2)
    EnableOffsetCompensation(bmx_bot)
    time.sleep(0.2)
    data_bot= bmx_bot.get_all_data()
    time.sleep(0.2)
    UpdateNVM(bmx_bot)
    time.sleep(0.2)

    EnableOffsetCompensation(bmx_top)
    time.sleep(0.2)
    FastOffsetCompensation(bmx_top)
    time.sleep(0.2)
    data_top= bmx_top.get_all_data()
    time.sleep(0.2)
    UpdateNVM(bmx_top)
    time.sleep(0.2)

# Fast Offset Compensation of gyroscope and accelerometer
def FastOffsetCompensation(bmx):
    try:
        bmx.write_bmx_reg(bmx._BMX160_FOC_CONF_ADDR, 0x40)      # _BMX160_FOC_CONF_ADDR = 0x69
    except:
        print("BMX160 Error: failed to configure fast offset calibration.")
        exit(1)

    try:
        bmx.write_bmx_reg(bmx._BMX160_COMMAND_REG_ADDR,0x03)    # _BMX160_FOC_CONF_ADDR = 0x7E
    except:
        print("BMX160 Error: failed to start the fast offset compensation.")
        exit(1)

    time.sleep(1) # wait time for FOC to finish

    try:
        stat = bmx.read_bmx_reg(bmx._BMX160_STATUS_ADDR)        # _BMX160_FOC_CONF_ADDR = 0x1B
    except:
        print("BMX160 Error: failed to Read Status Byte after FOC")
        exit(1)

    if (stat[0] & bmx._BMX160_FOC_STATUS_MSK) != 0x08:
        print("BMX160 Error: Couldn't complete FOC")
        exit(1)

# Enable Offset Compensation for gyroscope and accelerometer
def EnableOffsetCompensation(bmx):
    try:
        # Get the OFFSET byte that also holds the 9:8 bits of the gyro offsets, we don't want to disturb those bits
        stat = bmx.read_bmx_reg(bmx._BMX160_OFFSET_CONF_ADDR)
    except:
        return "BMX160 Error: failed to Read Offset config byte."
        exit(1)

    try:
        bmx.write_bmx_reg(bmx._BMX160_OFFSET_CONF_ADDR, stat|0x80)
    except:
        return "BMX160 Error: failed to enable offset."
        exit(1)

    try:
        stat = bmx.read_bmx_reg(bmx._BMX160_OFFSET_CONF_ADDR)
    except:
        return "BMX160 Error: failed to Read Offset config byte."
        exit(1)

# Updating the Non-volatile Memory
def UpdateNVM(bmx):
    try:
        bmx.write_bmx_reg(bmx._BMX160_CONF_ADDR, 0x02)
    except:
        return "BMX160 Error: unlocking NVM failed."
        exit(1)

    try:
        bmx.write_bmx_reg(bmx._BMX160_COMMAND_REG_ADDR,0xA0)    # _BMX160_FOC_CONF_ADDR = 0x7E
    except:
        print("BMX160 Error: failed to trigger the NVM write process.")
        exit(1)

if __name__ == "__main__":
    main()