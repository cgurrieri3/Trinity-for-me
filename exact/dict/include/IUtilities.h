#ifndef IUTILITIES_H
#define IUTILITIES_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <dirent.h>
#include <chrono>
#include <TFile.h>
#include <fstream>
#include <sys/stat.h>


class IUtilities{
	public:
		/**
		 * \class IUtilities
		 * \brief A class containing methods used in aiding analysis
		 * 
		 * */

		/**
		 * Public constructor
		 * */
		IUtilities();
		/**
		 * Public destructor
		 * */
		~IUtilities();

		/**
		 * Public method to convert UTC time to Unix epoch
		 * 
		 * @param utcTimeStr A time string containing the date and time to be converted
		 * 
		 * @return An unsigned 64 bit number containing the UNIX epoch
		 * */
		static uint64_t ConvertUTCtoUnix(const std::string& utcTimeStr);

		/**
		*  Public method to convert Unix time fro TBtime to UTC time
		*  the input is a 18 digit number pulled from the file time. 
		*   Output is a string of date and time in the format: 
		*									"2023-09-13 14:30:00.00000".
		**/
		static std::string ConvertUnixToUTCfromFile(long long unixTimestampInNanoseconds);

		/**
		* Public method to convert ADC (Analog Digital Converter) values to PE (PhotoElectron) which takes the analog signal converted to digital from the 
		* pixels to the number of photoelections hit the SiPM Pixels. The conversion rate is listed below in private.
		* 
		* @param ADC A float for an ADC value that will be converted to PE
		* 
		* @return A float of the PE value
		* */ 

		static float convertADC2PE(float ADC);
		/**
		* Public method to convert ADC (Analog Digital Converter) values to PE (PhotoElectron), which takes the analog signals converted to digital 
		* from the pixels and converts them to the number of photoelectrons that hit the SiPM pixels. The conversion rate is listed below in private.
		* 
		* @param ADC A vector of floats representing ADC values that will be converted to PE
		* 
		* @return A vector of floats representing the corresponding PE values
		*/
		static std::vector<float> convertADC2PE(std::vector<float> ADC);

		/**
		* Public method to get the plotting postion for the MUSICs from the ROIMUSICID
		* 
		* @param musicID An integer for the ROIMUSICID
		* 
		* @return An integer representing the MUSICs position for plotting
		*/
		static int GetMUSICPosition(int musicID=-1);

		static int GetMUSICMate(int musicID=-1, bool c=true);

		/**
		 * Public method to find the Pixel ID (assuming the amplitude vector is sorted in ascending pixelID number) of the maximum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return int With the pixel ID
		 * */

		static int GetMaximumPixelID(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the Pixel ID for the maximum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return int With the pixel ID
		 * */

		// static int GetMaximumPixelID(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the maximum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return float containing the maximum value
		 * */

		static float GetMaximum(std::vector<float> amplitude, int musicID=-1);/**
		 * Public method to find the maximum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return float containing the maximum value
		 * */

		// static float GetMaximum(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the Pixel ID of the minimum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return int With the pixel ID
		 * */

		static int GetMinimumPixelID(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the Pixel ID for the minimum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return int With the pixel ID
		 * */

		// static int GetMinimumPixelID(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the minimum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return float containing the minimum value
		 * */

		static float GetMinimum(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the minimum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return float containing the minimum value
		 * */

		// static float GetMinimum(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to calculate the pedestal based on the algorithm developed for uncorrelated samples in a trace
		 * The average of all samples in the specified time window is calculated. If isUnCorrelated is true, then the following pad samples are skipped. The process is repeated for the length of the trace std::vector.\ The std::vector provided should only include pedestal samples and not signal.
		 * 
		 * @param trace A std::vector containing the trace of the pixel to be analyzed
		 * 
		 * Optional parametes:
		 * @param tWindow The size of the window in 10s of ns to use. Default is 20 samples.
		 * @param pad The number of samples to skipa after each time window
		 * @param isUnCorrelated A bool specifying whether to skip the pad number of samples or not
		 * 
		 * @return A float with the average of the different window samples
		 * */

		static float GetPedestalAverage(std::vector<int> trace, int tWindow=20,int pad=10, bool isUnCorrelated=true);
		
		/**
		* Public method to get the average event amplitude from all the pixels 
		* 
		* 
		* @param amplitude A vector of floats representing the max amplitude for each pixel
		* 
		* @return A floats of the events average amplitude
		*/

		static float GetEventAverageAmplitude(std::vector<float> amplitude);
		/**
		* Public method to get the sum of the event amplitude from all the pixels
		* 
		* @param amplitude A vector of floats representing the max amplitude for each pixel
		* 
		* @return A float representing the sum of the event's amplitude
		*/

		static float GetEventAmplitudeSum(std::vector<float> amplitude);
		/** Default sample window scanned for a railed sample. Brackets the expected peak
		 * signal region of the Trinity Demonstrator (TimeBinAll). Named here so callers that
		 * need to move the window derive it from these rather than restating the bounds. */
		static constexpr int kSatWindowStart = 230;
		static constexpr int kSatWindowEnd   = 250;

		/**
		* Public method to identify saturated pixels based on their trace data.
		* 
		* @param trace A vector of vectors of floats, where each inner vector represents the signal trace for a pixel.
		* 
		* @param signalStart First sample of the window to scan. Defaults to the Trinity Demonstrator's
		*        peak signal region.
		* @param signalEnd One past the last sample of the window to scan. Defaults as above.
		* 
		* Note: simulated traces do not put the pulse where the demonstrator's readout does, so they
		* pass their own window instead of being resampled to match. The window is clamped to the
		* length of each trace.
		* 
		* @return A vector of integers, where each value corresponds to a pixel: 
		*         1 indicates saturation, and 0 indicates no saturation.
		*/

		std::vector<int> GetSaturatedPixels(std::vector<std::vector<int>> trace,
		                                    int signalStart = kSatWindowStart,
		                                    int signalEnd   = kSatWindowEnd);

		/**
		 * Public method to obtain all the files with a certain extension in a directory.
		 * 
		 * @param dirname A string with the path to the directory
		 * 
		 * Optional parameters:
		 * @param extension A string with the extension of files to accept. Default is ".root"
		 * 
		 * @return A std::vector of std::strings for each of the files in the directory
		 * */
		static std::vector<std::string> GetFilesInDirectory(std::string dirname, std::string extension=".root");
		
		/**
		* Public method to check if a specific branch (tree) exists in a given ROOT file.
		* 
		* @param fileName A string with the path to the .root file.
		* @param treeName A string with the name of the tree (branch) to check for.
		* 
		* @return A boolean value: true if the tree exists in the file, false otherwise.
		* */
		static bool isBranchPresentInFile(const std::string fileName, const std::string treeName);

		/**
		* Public method to set the permissions of a given file to `rwxrwx---` (octal 0770). for standardization
		*
		* This function uses `chmod` to change the permissions of the specified file,
		* granting read, write, and execute permissions to the owner and group, and no
		* permissions to others.
		*
		* @param filename A string containing the path to the file whose permissions will be changed.
		* 
		* @note This function prints a message to `stdout` or `stderr` based on the success or failure of the operation.
		*/
		static void setFilePermissions(const std::string& filename);

		/**
		* Public method to remove duplicate values from a vector of doubles.
		* 
		* @param arr A reference to a vector of doubles from which duplicate values will be removed.
		*            The vector is modified in place.
		* */
		static void removeDuplicates(std::vector<double>& arr);
		
		/**
		* Public method to read a file containing numerical values and store them in a vector.
		* 
		* @param filename A string with the path to the input file. The file is expected to contain
		*                 whitespace-separated numeric values.
		* 
		* @return A vector of doubles containing the values read from the file. Returns an empty vector
		*         if the file cannot be opened.
		*/
		static std::vector<double> readFileToVector(const std::string filename);
		
		/**
		* Public method to read a file containing textual values and store them in a vector.
		* 
		* @param filename A string with the path to the input file. The file is expected to contain
		*                 whitespace-separated string values.
		* 
		* @return A vector of strings containing the values read from the file. Returns an empty vector
		*         if the file cannot be opened.
		*/
		std::vector<std::string> readFileToVectorString(const std::string& filename);

		/**
		* Public method to write a vector of doubles to a specified file.
		* 
		* Each element in the vector is written to the file separated by spaces.
		* If an element is NaN, it is replaced with `1.0` before writing.
		* The function also sets the file permissions to `rwxrwx---` (0770) upon successful write.
		* 
		* @param vec A `std::vector<double>` containing the values to write to the file.
		* @param filename A string with the path to the output file.
		* 
		* @note Prints a message to `stdout` on success or `stderr` on failure.
		*/
		static void writeVectorToFile(const std::vector<double>& vec, const std::string& filename);

		/**
		 * Public method to obtain the High Voltage channel to which the pixel ID is connected
		 * 
		 * @param pixelID An int with the pixel ID
		 * 
		 * @return An int with the HV channel the pixel is connected to
		 * */
		static int GetHVChannel(int pixelID); 

		/**
		 * Public method to linearly interpolate between 2 data points.
		 * The linear interpolation is done the following way
		 * 
		 * f(x) = ((y2-y1)/(x2-x1))*x + (f(x1)-((y2-y1)/(x2-x1))*x1)
		 * 
		 * @param x1 A double of the x coordinate of the first point
		 * @param y1 A double of the y coordinate of the first point
		 * @param x2 A double of the x coordinate of the second point
		 * @param y2 A double of the y coordinate of the second point
		 * @param xNew A double of the x coordinate of the point to find, between x1 and x2. (There is no strict need for xNew to be within the range, but the result will not be an interpolation but an extrapolation.)
		 * 
		 * @return A double with the y coordinate corresponding to xNew
		 * */
		static double Interpolate(double x1, double y1, double x2, double y2, double xNew);

		/**
		* Public method to compute the median of a vector of doubles.
		*
		* The function creates a temporary copy of the input vector to avoid modifying the original data.
		* It then sorts the temporary vector and returns the median value:
		* - If the number of elements is odd, returns the middle element.
		* - If the number of elements is even, returns the average of the two middle elements.
		*
		* @param v A `std::vector<double>` containing the data to compute the median from.
		* 
		* @return The median value as a `Double_t`.
		*/
		static Double_t Median(std::vector<double> v);

		/**
		* Public method to compute the tau emergence angle from the simulated azimuth angle.
		*
		* The azimuth angle is wrapped to the range [-180, 180] degrees so that it is
		* measured from the telescope axis: any value greater than 180 has 360 subtracted
		* from it.
		*
		* @param azimuthAngle A double with the azimuth angle in degrees (e.g. from SEvent::GetAzimuthAngle()).
		*
		* @return A double with the emergence angle in degrees, wrapped to [-180, 180].
		*/
		static double GetEmergenceAngle(double azimuthAngle);

		/**
		* Public method to compute the distance to the emergence point from the telescope position vector.
		*
		* The distance is the magnitude of the telescope position vector,
		* sqrt(x^2 + y^2 + z^2), in the units of the telescope coordinates.
		*
		* @param telescopeX A double with the telescope x position.
		* @param telescopeY A double with the telescope y position.
		* @param telescopeZ A double with the telescope z position.
		*
		* @return A double with the distance to the emergence point in the telescope coordinate units.
		*/
		static double GetEmergenceDistance(double telescopeX, double telescopeY, double telescopeZ);

		static float GetADCtoPEratio(){return ADCtoPEratio;}

	private:
		// Music mapping from the ROIMUSIC as the index and elements are the postion on the camera for determining pixels
		inline static std::vector<int> MUSICs = {3,2,11,10,19,18,27,26,7,6,15,14,23,22,31,30,1,0,9,8,17,16,25,24,5,4,13,12,21,20,29,28};
		// ADC to PE ratio found from testing
		inline static float ADCtoPEratio = 24.1;
};

#endif
