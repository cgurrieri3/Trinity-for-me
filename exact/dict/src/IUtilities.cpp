#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "IUtilities.h"

IUtilities::IUtilities(){

}
IUtilities::~IUtilities(){

}

uint64_t IUtilities::ConvertUTCtoUnix(const std::string& utcTimeStr) {
    // Convert UTC time string to tm structure
    std::tm utcTime = {};
    std::istringstream ss(utcTimeStr);
    ss >> std::get_time(&utcTime, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        std::cerr << "Invalid UTC time format" << std::endl;
        return 0;
    }

    // Convert tm structure to Unix timestamp
    std::time_t unixTime = std::mktime(&utcTime);

    // Extract microseconds and convert to Unix timestamp format
    int microsec;
    ss >> microsec;

    // Calculate Unix timestamp with microseconds
    uint64_t unixTimestamp = static_cast<uint64_t>(unixTime) * 1000000ULL + microsec;

    return unixTimestamp;
}

std::string IUtilities::ConvertUnixToUTCfromFile(long long unixTimestampInNanoseconds) {

    // Add an extra "0" at the end
    unixTimestampInNanoseconds *= 10;

    std::chrono::system_clock::time_point tp = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::nanoseconds(unixTimestampInNanoseconds));

    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::chrono::system_clock::duration sinceEpoch = tp.time_since_epoch();
    std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(sinceEpoch);

    // Calculate the hours, minutes, and seconds
    // int hours = ns.count() / 3600000000000;
    // int minutes = (ns.count() % 3600000000000) / 60000000000;
    // int seconds = (ns.count() % 60000000000) / 1000000000;
    int nanoseconds = ns.count() % 1000000000;


	std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(9) << std::setfill('0') << nanoseconds;

    return oss.str();
}

float IUtilities::convertADC2PE(float ADC){
    float PE = (ADC*1.0)/ADCtoPEratio;
    return PE;
}

std::vector<float> IUtilities::convertADC2PE(std::vector<float> ADC){
	int vectorEnd = ADC.size();
	std::vector<float> PE(vectorEnd);
    for(int i= 0; i<vectorEnd;i++){
		PE[i] = (ADC[i]*1.0)/ADCtoPEratio;
	}
	return PE;
}

int IUtilities::GetMUSICPosition(int musicID){
	musicID = MUSICs[musicID];
	return musicID;
}

int IUtilities::GetMUSICMate(int musicID, bool c){
	if (!c){
		musicID = MUSICs[musicID];
	} 

	if (musicID % 2  == 0){
		musicID = musicID + 1;
	} else {
		musicID = musicID - 1;
	}
	return musicID;
	
}

int IUtilities::GetMaximumPixelID(std::vector<float>amplitude,int musicID){
	musicID = MUSICs[musicID];
	int iStartPixel, iEndPixel;
	//looks for pixels in specified music ID if none start from pixel#0
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}

	int max = 0;
	int maxID = 0;
	//looks for the pixel with highest amplitude
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]>max){
			max = amplitude[i];
			maxID = i;
		}
	}

	return maxID;
}

//SPB2 Only Trinity Demononstartor does not have ROI Pixels
// int IUtilities::GetMaximumPixelID(std::vector<float>amplitude,std::vector<int> roiPixels){
// 	int iStartPixel, iEndPixel;


// 	int max = 0;
// 	int maxID = 0;
// 	//If no region of interest pixels specified look for highest amplitude out of pixels starting from pixel#0 -> amplitude.size()
// 	if(roiPixels.size() == 0){
// 		iStartPixel = 0;
// 		iEndPixel = amplitude.size();

// 		for(int i = iStartPixel; i<iEndPixel; i++){
// 			if(amplitude[i]>max){
// 				max = amplitude[i];
// 				maxID = i;
// 			}
// 		}
// 	//if roi pixels specified looks for pixel with highest amplitude
// 	}else{
// 		for(int i = 0; i<roiPixels.size(); i++){
// 			if(amplitude[roiPixels[i]]>max){
// 				max = amplitude[roiPixels[i]];
// 				maxID = roiPixels[i];
// 			}
// 		}
// 	}
// 	return maxID;
// }

float IUtilities::GetMaximum(std::vector<float>amplitude,int musicID){
	musicID = MUSICs[musicID];
	int iStartPixel, iEndPixel;
	//if music ID is none  start from music #0
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}
	//find max amplitude out of vector
	float max = 0;
	// int maxID = 0;
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]>max){
			max = amplitude[i];
			// maxID = i;
		}
	}

	return max;
}

//SPB2 Only Trinity Demononstartor does not have ROI Pixels
// float IUtilities::GetMaximum(std::vector<float>amplitude,std::vector<int> roiPixels){
// 	int iStartPixel, iEndPixel;


// 	float max = 0;
// 	int maxID = 0;
// 	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitde.size()
// 	//returns max amplitude
// 	if(roiPixels.size() == 0){
// 		iStartPixel = 0;
// 		iEndPixel = amplitude.size();

// 		for(int i = iStartPixel; i<iEndPixel; i++){
// 			if(amplitude[i]>max){
// 				max = amplitude[i];
// 				maxID = i;
// 			}
// 		}
// 	}else{
// 		for(int i = 0; i<roiPixels.size(); i++){
// 			if(amplitude[roiPixels[i]]>max){
// 				max = amplitude[roiPixels[i]];
// 				maxID = roiPixels[i];
// 			}
// 		}
// 	}
// 	return max;
// }

int IUtilities::GetMinimumPixelID(std::vector<float>amplitude,int musicID){
	musicID = MUSICs[musicID];
	int iStartPixel, iEndPixel;
	
	//looks for pixels in specified music ID
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}
	//returns min amplitude ID out of pixels in music
	float min = 4096;
	int minID = 0;
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]<min){
			min = amplitude[i];
			minID = i;
		}
	}

	return minID;
}

//SPB2 Only Trinity Demononstartor does not have ROI Pixels
// int IUtilities::GetMinimumPixelID(std::vector<float>amplitude,std::vector<int> roiPixels){
// 	int iStartPixel, iEndPixel;

// 	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitude.size()

// 	//returns min amplitude ID out of pixels in roiPixels vector
// 	float min = 4096;
// 	int minID = 0;

// 	if(roiPixels.size() == 0){
// 		iStartPixel = 0;
// 		iEndPixel = amplitude.size();

// 		for(int i = iStartPixel; i<iEndPixel; i++){
// 			if(amplitude[i]<min){
// 				min = amplitude[i];
// 				minID = i;
// 			}
// 		}
// 	}else{
// 		for(int i = 0; i<roiPixels.size(); i++){
// 			if(amplitude[roiPixels[i]]<min){
// 				min = amplitude[roiPixels[i]];
// 				minID = roiPixels[i];
// 			}
// 		}
// 	}
// 	return minID;
// }

float IUtilities::GetMinimum(std::vector<float>amplitude,int musicID){
	musicID = MUSICs[musicID];
	int iStartPixel, iEndPixel;

	//looks for pixels in specified music ID if none start from pixel 0	
	if(musicID != -2){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}

	float min = 4096;
	// int minID = 0;
	//returns min amplitude out of pixels in music
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]<min){
			min = amplitude[i];
			// minID = i;
		}
	}

	return min;
}

//SPB2 Only Trinity Demononstartor does not have ROI Pixels
// float IUtilities::GetMinimum(std::vector<float>amplitude,std::vector<int> roiPixels){
// 	int iStartPixel, iEndPixel;

// 	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitde.size()
// 	//returns min amplitude out of pixels in roiPixels vector

// 	float min = 4096;
// 	int minID = 0;

// 	if(roiPixels.size() == 0){
// 		iStartPixel = 0;
// 		iEndPixel = amplitude.size();

// 		for(int i = iStartPixel; i<iEndPixel; i++){
// 			if(amplitude[i]<min){
// 				min = amplitude[i];
// 				minID = i;
// 			}
// 		}
// 	}else{
// 		for(int i = 0; i<roiPixels.size(); i++){
// 			if(amplitude[roiPixels[i]]<min){
// 				min = amplitude[roiPixels[i]];
// 				minID = roiPixels[i];
// 			}
// 		}
// 	}
// 	return min;
// }

float IUtilities::GetPedestalAverage(std::vector<int> trace, int tWindow,int pad, bool isUnCorrelated){
	//create window for which to view trace samples
	std::vector<int> traceWindow = std::vector<int>(0);
	std::vector<int>::iterator itr;
	float average;
	float sum=0;
	int counter = 0;
	//if isUnCorrelated is true, then following pad samples are skipped
	//calculates sum of all trace values in vector
	if(isUnCorrelated){
		for(std::size_t i = 0; i+tWindow<trace.size(); i+=(tWindow+pad)){
			traceWindow = std::vector<int>(trace.begin()+i,trace.begin()+i+tWindow);
			sum += std::accumulate(traceWindow.begin(),traceWindow.end(),0);
			counter++;
			traceWindow.clear();
		}
	//same as above expect pad samples not skipped
	}else{
		sum = std::accumulate(trace.begin(),trace.end(),0);
		counter = 1;
		tWindow = trace.size();
	}
	//returns average trace value
	average = sum/(float)(counter*tWindow);
	return average;
}

float IUtilities::GetEventAverageAmplitude(std::vector<float> amplitude){
	int iStartPixel = 0;
	int iEndPixel = amplitude.size();
	float average;
	float sum = 0;
	int counter = 0;
	//looks for the pixel with highest amplitude
	for(int i = iStartPixel; i<iEndPixel; i++){
		sum += amplitude[i];
		counter++;
	}
	average = sum/counter;
	return average;
}

float IUtilities::GetEventAmplitudeSum(std::vector<float> amplitude){
	int iStartPixel = 0;
	int iEndPixel = amplitude.size();
	float sum = 0;
	//looks for the pixel with highest amplitude
	for(int i = iStartPixel; i<iEndPixel; i++){
		sum += amplitude[i];
	}
	return sum;
}

std::vector<int> IUtilities::GetSaturatedPixels(std::vector<std::vector<int>> trace, int signalStart, int signalEnd){
	std::vector<int> saturation;
	int counter = 0;
	for(std::size_t i = 0; i<trace.size(); i++){
		
		// The window is caller supplied, so keep it inside this trace rather than running off
		// either end. Callers scanning a simulated pulse pass a window of their own.
		int start = std::max(0, signalStart);
		int end   = std::min(static_cast<int>(trace[i].size()), signalEnd);
		counter = 0;
		for(int j = start; j<end; j++){
			if(trace[i][j] <= 1){
				counter++;
			}
		}

		if(counter>0) {
			saturation.push_back(1);
		} else {
			saturation.push_back(0);
		}
	}
	return saturation;
}

std::vector<std::string> IUtilities::GetFilesInDirectory(std::string dirname, std::string extension){
	//looks through all files in directory with given extension
	std::vector<std::string> files;
	std::string fileName;
	DIR *dir = opendir(dirname.c_str());

	if(dir){
		dirent *element;
		while ((element = readdir(dir))){
			fileName = element->d_name;
			if(fileName.length()>=extension.length()){
				if(fileName.compare(fileName.length() - extension.length(),extension.length(),extension) == 0){
					files.push_back(fileName);
				}
			}
		}

	}
	std::sort(files.begin(), files.end());

	// Remove elements matching the pattern _0001.root as these are overflow files and as of right now we dont process them 
    files.erase(
        std::remove_if(
            files.begin(),
            files.end(),
            [](const std::string& name) {
                return name.find("_0001.root") != std::string::npos;
            }
        ),
        files.end()
    );
	return files;
}

// Function to check if a branch exists in a ROOT file
bool IUtilities::isBranchPresentInFile(const std::string fileName, const std::string treeName) {
    // Open the ROOT file
    TFile* file = TFile::Open(fileName.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open the .root file: " << fileName << std::endl;
        delete file;
        return false;
    }

    // Check if the tree exists
    bool treeExists = (file->Get(treeName.c_str()) != nullptr);
    if (!treeExists) {
        std::cout << "Tree '" << treeName << "' is not present in the file. Skipping..." << std::endl;
    }

    // Clean up
    file->Close();
    delete file;

    return treeExists; // Return whether the tree exists
}

void IUtilities::setFilePermissions(const std::string& filename) {
    // Set permissions to rwxrwx--- (octal 0770)
    mode_t permissions = 0770;

    // Change file permissions using chmod
    if (chmod(filename.c_str(), permissions) == 0) {
        std::cout << "Permissions changed to 770 for: " << filename << std::endl;
    } else {
        std::cerr << "Error changing file permissions" << std::endl;
    }
}

void IUtilities::removeDuplicates(std::vector<double>& arr) {
    // Sort the array
    std::sort(arr.begin(), arr.end());

    // Use std::unique to remove duplicates
    auto last = std::unique(arr.begin(), arr.end());

    // Resize the vector to remove the undefined tail elements
    arr.erase(last, arr.end());
}

std::vector<double> IUtilities::readFileToVector(const std::string filename) {
    std::vector<double> result;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return result; // Return empty vector if file can't be opened
    }

    double value;
    while (file >> value) {
        result.push_back(value);
    }

    file.close();
    return result;
}

std::vector<std::string> IUtilities::readFileToVectorString(const std::string& filename) {
    std::vector<std::string> result;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return result; // Return empty vector if file can't be opened
    }

    std::string value;
    while (file >> value) {
        result.push_back(value);
    }

    file.close();
    return result;
}

// Function to write a vector to a file
void IUtilities::writeVectorToFile(const std::vector<double>& vec, const std::string& filename) {
    std::ofstream outfile(filename);

    // Check if the file was opened successfully
    if (outfile.is_open()) {
        for (const auto& elem : vec) {
            double valueToWrite = std::isnan(elem) ? 1.0 : elem;
            outfile << valueToWrite << " ";  // Write each element followed by a space
        }
        outfile << std::endl;  // Add a newline after the elements
        outfile.close();       // Close the file
        setFilePermissions(filename);
        std::cout << "Data written to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int IUtilities::GetHVChannel(int pixelID){
	// given pixelID calculates hVChannel # bottom left 1, top left 2, bottom right 3, top right 4
	int hVChannel = -1;
	if ((pixelID >= 0 && pixelID <= 31) || (pixelID >= 64 && pixelID <= 95)) {
		hVChannel = 1;
    } else if ((pixelID >= 128 && pixelID <= 159) || (pixelID >= 192 && pixelID <= 223)) {
		hVChannel = 2;
	}else if ((pixelID >= 32 && pixelID <= 63) || (pixelID >= 96 && pixelID <= 127)) {
		hVChannel = 3;
	}else if ((pixelID >= 160 && pixelID <= 191) || (pixelID >= 224 && pixelID <= 255)) {
		hVChannel = 4;
    }
	return hVChannel;
}

double IUtilities::Interpolate(double x1, double y1, double x2, double y2, double xNew){
//uses linear interpolation between data points and returns slopes
	double slope = (y2 - y1)/(x2 - x1);
    double offset = y2 - x2*slope;

   return slope*xNew + offset;
}

Double_t IUtilities::Median(std::vector<double> v)
{
    //Size of vector
    int n = v.size();
    //Make temp copy of the vector to leave original in the same order
    std::vector<double> tempV(v);
    //Sort the vector
    sort(tempV.begin(), tempV.end());
    //Check if the number of elements is odd
    if(n%2!=0){
        return(Double_t)tempV[n/2];
    }
    //If the number of elements is even, return the average of the two middle elements
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0;
}

double IUtilities::GetEmergenceAngle(double azimuthAngle){
    // Wrap azimuth to [-180,180] to measure the emergence angle from the telescope axis
    return (azimuthAngle > 180.0) ? (azimuthAngle - 360.0) : azimuthAngle;
}

double IUtilities::GetEmergenceDistance(double telescopeX, double telescopeY, double telescopeZ){
    // Distance to the emergence point is the magnitude of the telescope position vector
    return std::sqrt(telescopeX*telescopeX + telescopeY*telescopeY + telescopeZ*telescopeZ);
}
