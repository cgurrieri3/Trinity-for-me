#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/smbus.h>

#define	BMX160_MAGN_IF_0_ADDR	0x4C
#define	BMX160_MAGN_IF_1_ADDR	0x4D
#define BMX160_MAGN_IF_2_ADDR	0x4E
#define	BMX160_MAGN_IF_3_ADDR	0x4F
#define	BMX160_MAGN_CONFIG_ADDR	0x44

using namespace std;

void Print_BMX_Data(__u8 data[])
{
	double magn[3] = {0};
	double gyro[3] = {0};
	double accl[3] = {0};

	// Sorting the Magnetometer data
	if (data[1] & 0x80){
		magn[0] = - 0x10000 + ((data[1] << 8) | (data[0]));
	}else{
		magn[0] =  (data[1] << 8) | (data[0]);
	}

	if (data[3] & 0x80){
		magn[1] = - 0x10000 + ((data[3] << 8) | (data[2]));
	}else{
		magn[1] =  (data[3] << 8) | (data[2]);
	}

	if (data[5] & 0x80){
		magn[2] = - 0x10000 + ((data[5] << 8) | (data[4]));
	}else{
		magn[2] =  (data[5] << 8) | (data[4]);
	}

	// Sorting the Gyroscope data
	if (data[9] & 0x80){
		gyro[0] = - 0x10000 + ((data[9] << 8) | (data[8]));
	}else{
		gyro[0] =  (data[9] << 8) | (data[8]);
	}

	if (data[11] & 0x80){
		gyro[1] = - 0x10000 + ((data[11] << 8) | (data[10]));
	}else{
		gyro[1] =  (data[11] << 8) | (data[10]);
	}

	if (data[13] & 0x80){
		gyro[2] = - 0x10000 + ((data[13] << 8) | (data[12]));
	}else{
		gyro[2] =  (data[13] << 8) | (data[12]);
	}

	// Sorting the Accelerometer data
	if (data[15] & 0x80){
		accl[0] = - 0x10000 + ((data[15] << 8) | (data[14]));
	}else{
		accl[0] =  (data[15] << 8) | (data[14]);
	}

	if (data[17] & 0x80){
		accl[1] = - 0x10000 + ((data[17] << 8) | (data[16]));
	}else{
		accl[1] =  (data[17] << 8) | (data[16]);
	}

	if (data[19] & 0x80){
		accl[2] = - 0x10000 + ((data[19] << 8) | (data[18]));
	}else{
		accl[2] =  (data[19] << 8) | (data[18]);
	}

    // This converts the raw data and prints them
    magn[0] *= 0.3;
    magn[1] *= 0.3;
    magn[2] *= 0.3;
    gyro[0] *= 0.0038110;
    gyro[1] *= 0.0038110;
    gyro[2] *= 0.0038110;
    accl[0] *= 0.000061035 * 9.8;
    accl[1] *= 0.000061035 * 9.8;
    accl[2] *= 0.000061035 * 9.8;

	std::cout << magn[0] << "\t" << magn[1] << "\t" << magn[2] <<  "\t" << gyro[0] << "\t" << gyro[1] << "\t" << gyro[2] << "\t" << accl[0] << "\t" << accl[1] << "\t" << accl[2] << "\t";
}

int main (int argc, char *argv[])
{
	int file = 0;
	int TOP_BMX_ADDR = 0x68;
	int BOT_BMX_ADDR = 0x69;
	__u8 BMX_DATA_SIZE = 20;
	__u8 BMX_DATA_ADDR = 0x04;
	__u8 bot_data[20] = {0};
	__u8 top_data[20] = {0};
	__u8 tbytes_read, bbytes_read = 0;

	file = open("/dev/i2c-0", O_RDWR);
	if (file < 0) {
		std::cout << "Error in opening file ..." << std::endl;
		exit(1);
	}

	time_t sample_time = time (NULL);
	std::cout << sample_time << "\t";

	if (ioctl(file, I2C_SLAVE, BOT_BMX_ADDR) < 0)
	{
		std::cout << "Error in setting up Bot BMX chip." << std::endl;
		exit(1);
	}
	bbytes_read = i2c_smbus_read_i2c_block_data(file, BMX_DATA_ADDR, BMX_DATA_SIZE, bot_data);
	if(bbytes_read != BMX_DATA_SIZE)
	{
		std::cout << "Number of bytes read from bottom chip mismatch" << std::endl;
	}
	else{
		Print_BMX_Data(bot_data);
	}

	if (ioctl(file, I2C_SLAVE, TOP_BMX_ADDR) < 0)
	{
		std::cout << "Error in setting up Top BMX chip." << std::endl;
		exit(1);
	}
	tbytes_read = i2c_smbus_read_i2c_block_data(file, BMX_DATA_ADDR, BMX_DATA_SIZE, top_data);
	if(tbytes_read != BMX_DATA_SIZE)
	{
		std::cout << "Number of bytes read from top chip mismatch" << std::endl;
	}
	else{
		Print_BMX_Data(top_data);
	}
	std::cout << std::endl;

	return 0;
}
