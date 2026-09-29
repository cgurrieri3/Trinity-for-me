#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/smbus.h>

#define BMX160_ACCEL_RANGE_ADDR	0x41
#define BMX160_GYRO_RANGE_ADDR	0x43
#define	BMX160_MAGN_IF_0_ADDR	0x4C
#define	BMX160_MAGN_IF_1_ADDR	0x4D
#define BMX160_MAGN_IF_2_ADDR	0x4E
#define	BMX160_MAGN_IF_3_ADDR	0x4F
#define	BMX160_MAGN_CONFIG_ADDR	0x44
#define BMX160_COMMAND_REG_ADDR	0x7E

using namespace std;

bool BMX160_Config(int file, int Address);

int main (int argc, char *argv[])
{
	int file = 0;
	int TOP_BMX_ADDR = 0x68;
	int BOT_BMX_ADDR = 0x69;

	file = open("/dev/i2c-0", O_RDWR);
	if (file < 0) {
		std::cout << "Error in opening the device ..." << std::endl;
		exit(1);
	}

	if(BMX160_Config(file, BOT_BMX_ADDR))
	{
		std::cout << "Finished configuring the Bottom BMX160 Chip." << std::endl;
	}

	usleep(100000);

	if(BMX160_Config(file, TOP_BMX_ADDR))
	{
		std::cout << "Finished configuring the Top BMX160 Chip." << std::endl;
	}

	return 0;
}

bool BMX160_Config(int file, int Address)
{
	if (ioctl(file, I2C_SLAVE, Address) < 0)
	{
		std::cout << "Error in configuring chip address: " << Address << std::endl;
		return false;
	}

	// Soft Reset
	i2c_smbus_write_byte_data(file, BMX160_COMMAND_REG_ADDR, 0xB6);
	usleep(100000);

	// Set the Power Mode of Accelerometer to normal
	i2c_smbus_write_byte_data(file, BMX160_COMMAND_REG_ADDR, 0x11);
	usleep(100000);
	// Set the Power Mode of Gyroscope to normal
	i2c_smbus_write_byte_data(file, BMX160_COMMAND_REG_ADDR, 0x15);
	usleep(100000);
	// Set the Power Mode of Magnetometer to normal
	i2c_smbus_write_byte_data(file, BMX160_COMMAND_REG_ADDR, 0x19);
	usleep(100000);

	// Setting up the Magnetometer Configuration
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_0_ADDR, 0x80);
	usleep(100000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_3_ADDR, 0x01);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_2_ADDR, 0x4B);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_3_ADDR, 0x04);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_2_ADDR, 0x51);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_3_ADDR, 0x0E);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_2_ADDR, 0x52);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_3_ADDR, 0x02);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_2_ADDR, 0x4C);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_1_ADDR, 0x42);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_CONFIG_ADDR, 0x08);
	usleep(10000);
	i2c_smbus_write_byte_data(file, BMX160_MAGN_IF_0_ADDR, 0x03);
	usleep(100000);

	// Setting the Angular rate measurement range to +/-125 degree/second
	i2c_smbus_write_byte_data(file, BMX160_GYRO_RANGE_ADDR, 0x04);
	usleep(100000);
	// Setting the Accelerometer range to +/-2g
	i2c_smbus_write_byte_data(file, BMX160_ACCEL_RANGE_ADDR, 0x03);

	return true;
}