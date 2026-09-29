#ifndef ICALIBRATION_H
#define ICALIBRATION_H

#include "IUtilities.h"

#include <TROOT.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

const double BrVperDeg = 0.03844;/**< Breakdown Voltage change per degree C, 0.001 times avg breakdown voltage at 25 deg C where we did gain calibration; avg breakdown voltage is 38.44 V at 25 deg C*/
const double BrVol[16] = {38.44, 38.44, 38.44, 38.44, 38.44, 38.44, 38.44, 38.44,
                          38.44, 38.44, 38.44, 38.44, 38.44, 38.44, 38.44, 38.44}; /**< use average breakdown voltages for each of the matrices in Trinity reported by Hamamatsu at 25C, because min and max deviation from the average value across the camera is less than 1%*/

/*const double VOffsetPx[256] = {
    636.8, 598.4, 672.0, 598.4, 604.8, 595.2, 585.6, 544.0, 784.0, 819.2, 844.8, 867.2, 1040.0, 723.2, 768.0, 787.2,
    899.2, 828.8, 777.6, 780.8, 992.0, 934.4, 851.2, 931.2, 726.4, 627.2, 633.6, 716.8, 761.6, 729.6, 704.0,
    694.4, 732.8, 768.0, 867.2, 838.4, 748.8, 771.2, 819.2, 812.8, 924.8, 966.4, 963.2, 1094.4, 816.0, 870.4,
    880.0, 950.4, 950.4, 886.4, 816.0, 784.0, 1017.6, 1033.6, 963.2, 969.6, 761.6, 739.2, 752.0, 681.6, 777.6, 784.0,
    803.2, 755.2, 835.2, 1120.0, 1081.6, 982.4, 819.2, 835.2, 1222.4, 1187.2, 860.8, 835.2, 1219.2, 1171.2,
    931.2, 905.6, 1331.2, 1302.4, 822.4, 860.8, 832.0, 784.0, 739.2, 825.6, 700.8, 761.6, 681.6, 592.0, 598.4,
    713.6, 595.2, 588.8, 624.0, 668.8, 604.8, 553.6, 588.8, 576.0, 576.0, 624.0, 652.8, 649.6, 729.6, 662.4, 729.6,
    729.6, 752.0, 672.0, 742.4, 793.6, 880.0, 809.6, 912.0, 963.2, 742.4, 780.8, 844.8, 908.8, 681.6, 681.6, 752.0,
    796.8, 608.0, 630.4, 713.6, 732.8, 1062.4, 1049.6, 1059.2, 1097.6, 979.2, 1017.6, 1011.2, 1043.2, 1228.8,
    1225.6, 1308.8, 1363.2, 1152.0, 1161.6, 1184.0, 1180.8, 742.4, 704.0, 684.8, 624.0, 819.2, 742.4, 768.0, 713.6,
    627.2, 598.4, 636.8, 620.8, 556.8, 659.2, 643.2, 688.0, 822.4, 796.8, 809.6, 803.2, 796.8, 790.4, 825.6,
    771.2, 902.4, 940.8, 998.4, 995.2, 819.2, 876.8, 896.0, 918.4, 857.6, 825.6, 844.8, 777.6, 953.6, 966.4,
    947.2, 921.6, 768.0, 732.8, 720.0, 713.6, 723.2, 780.8, 784.0, 800.0, 1033.6, 1036.8, 985.6, 969.6, 1059.2,
    1059.2, 1075.2, 1091.2, 1241.6, 1148.8, 1132.8, 1174.4, 1260.8, 1264.0, 1251.2, 1324.8, 710.4, 713.6,
    694.4, 784.0, 601.6, 624.0, 646.4, 672.0, 595.2, 579.2, 585.6, 630.4, 502.4, 528.0, 502.4, 544.0, 736.0, 732.8,
    726.4, 723.2, 838.4, 838.4, 800.0, 803.2, 969.6, 937.6, 841.6, 915.2, 956.8, 985.6, 947.2, 982.4, 1273.6,
    1292.8, 1248.0, 1360.0, 1116.8, 1120.0, 1209.6, 1296.0, 1036.8, 1068.8, 1084.8, 1104.0, 966.4, 992.0, 1017.6, 1040.0};
< Offset adjustments for Trinity Demonstrator camera flatfielding in mV, from 2025/03/10 */


//to use with average breakdown voltage for all matrices
const double VOffsetPx[256] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

class ICalibration {
	/**
	 * \class ICalibration
	 * \brief Class used to calculate correction factors for each of the SiPMs
	 * 
	 * During the data acquisition 2 things affect the response of the SiPMs, temperature and operating voltage. The calculations to obtain the correct gain and temperature corrections are performed by this class
	 * */
	public:
		/**
		 * Public constructor. Uses a file with the ADC to PE conversion factors and errors for each of the SiPMs as measured in the lab
		 * 
		 * @param filename A string with the file that contains the measurements
		 * */
    	ICalibration(const std::string& filename);
    	/**
    	 * A destructor
    	 * */
    	~ICalibration();

    	/**
    	 * A method used to obtain the conversion factor in ADC to PE for the amplitude of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the conversion from ADC to photo-electrons for amplitude.
    	 * */
	    double GetAmplitude(int pixelID) const;
	    /**
    	 * A method used to obtain the error on the measurement of the conversion factor in ADC to PE for the amplitude of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the error on the measurement of the conversion from ADC to photo-electrons for amplitude.
    	 * */
	    double GetErrAmplitude(int pixelID) const;
	    /**
    	 * A method used to obtain the conversion factor in ADC to PE for the cahrge of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the conversion from ADC to photo-electrons for charge.
    	 * */
	    //double GetCharge(int pixelID) const;
	    /**
    	 * A method used to obtain the error on the measurement of the conversion factor in ADC to PE for the charge of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the error on the measurement of the conversion from ADC to photo-electrons for charge.
    	 * */
	    //double GetErrCharge(int pixelID) const;
	    /**
	     * A method used to obtain the voltage offset set for the pixel in the camera
	     * 
	     * @param pixelID An int with the pixel ID of which the voltage offset is to be retrieved
	     * 
	     * @return A double with the offset setting in mV
	     * */
	    double GetVOffsetPx(int pixelID);
	     /**
	     * A method used to obtain the voltage offset set for all pixels in the camera
	     * 
	     * @return A std::vector<double> with the offset setting in mV
	     * */
	    std::vector<double> GetVOffsetPx();
	    /**
	     * A method to calculate the relative over voltage compared to the nominal voltage used for the lab measurements
	     * 
	     * The relative over voltage is defined as 
	     * 
	     * R.O.V. = (operatingVoltage - breakdownVoltage)/(nominalVoltage - breakdownVoltage)
	     * 
	     * @param pixelID An int with the pixel ID of interes
	     * @param operatingVol The voltage at which the SiPM was biased in V
	     * @param nominalVol The voltage used in the lab measurements in V
	     * 
	     * @return A double with the R.O.V.
	     * */
	    static double GetRelativeOverVoltage(int pixelID, double operatingVol, double nominalVol);
	    /**
	     * A method to calculate the over voltage of the SiPM
	     * 
	     * The over voltage is defined as 
	     * 
	     * O.V. = (operatingVoltage - breakdownVoltage)/(breakdownVoltage)
	     * 
	     * @param pixelID An int with the pixel ID of interes
	     * @param operatingVol The voltage at which the SiPM was biased in V
	     * 
	     * @return A double with the O.V.
	     * */
	    static double GetOverVoltage(int pixelID, double operatingVol);
	    /**
	     * A method to calculate the relative gain of the SiPM when compared to room temperature at which the lab measurements where performed.
	     * 
	     * The relative gain is defined as follows
	     * 
	     * G(T_Lab)/G(T) = 1/(1-BrVperDeg*((T-T_Lab)/(OperatingVoltage-BreakdownVoltage)))
	     * 
	     * @param pixelID An int with the pixelID of interest
	     * @param sipmTemp A double with the temperature of the SiPM in C
	     * @param OperatingVol A double with the operating voltage in V
	     * 
	     * @return A double with the relativeGain
	     * */
	    static double GetRelativeGain(int pixelID, double sipmTemp, double OperatingVol);
	    /**
	     * 
	     * A method that obtains on which line of a text file the data related to a measurement is present
	     * 
	     * @param filename A string with the file with the measurements for the SiPM
	     * @param measurement A string describing which measurement to extract
	     * 
	     * @return An int with the line number where the measurement appears
	     * */
	   	static int GetMeasurementLine(std::string filename, std::string measurement);
	   	/**
	   	 * A method to obtain a measurement from the file containing all of the measurements for the SiPM.
	   	 * 
	   	 * @param pixelID An int with the pixel ID of interest
	   	 * @param sipmTemp A double with the temperature of the SiPM in C
	   	 * @param OperatingVol A double with the operating voltage in V
	   	 * @param filename A std::string with the name of the file that contains the measurements
	   	 * @param measurement A std::string with the measurement name in the file to extract
	   	 * 
	   	 * Optional parameters:
	   	 * @param biasVoltageRef A std::string describing if the provided OperatingVoltage is the over voltage or absolute ("REL","ABS").\ Default is "ABS"
	   	 * 
	   	 * @return A double with the value of the measurement at that voltage and temperature
	   	 * */
	    static double GetMeasurementFromFile(int pixelID, double sipmTemp, double OperatingVol, std::string filename, std::string measurement, std::string biasVoltageRef="ABS");


	private:
	    std::vector<double> amplitudes;/**< ADC to photo-electron conversion for amplitude*/
	    std::vector<double> errAmplitudes;/**< Error in the ADC to photo-electron conversion for amplitude*/
	    std::vector<double> charges;/**< ADC to photo-electron conversion for charge*/
	    std::vector<double> errCharges;/**< Error in the ADC to photo-electron conversion for charge*/

		/**
		 * A private method to find the closest temperature line in the measurement file to that of the SiPM
		 * 
		 * @param sipmTemp A double with the temperature of the SiPM in C
		 * @param filename A std::string with the name of the measurement file
		 * @param measurement A std::string with the name of the measurement of interest
		 * 
		 * @return An int with the line where the closest temperature is located
		 * */
	    static int GetClosestTemperatureLine(double sipmTemp, std::string filename, std::string measurement);

	   	
	    //double relativeGain;
};

#endif // ICALIBRATION_H
