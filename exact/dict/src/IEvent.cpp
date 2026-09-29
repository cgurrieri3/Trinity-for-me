#include "IEvent.h"



IEvent::IEvent(){
	//Iazimuth = 0;
	//Ialtitude = 0;
	IcorrectionFactor = 0;

/*    timestampIndex = -1;
    timestamp2Index = -1;

    closestTimestamp1 = 0;
    closestTimestamp2 = 0;*/


    sunAzimuth = 0.0f;
    sunElevation = 0.0f;
    moonAzimuth = 0.0f;
    moonElevation = 0.0f;
    moonIllumination = 0.0f;
    CameraRA = 0.0f;
    CameraDEC = 0.0f;

    TrigEvent = 0;
    ASADcurr =0.0f;
    TBcurr=0.0f;
    hv.resize(4, 0.0f);
    hvc.resize(4, 0.0f);
    ucTemp.resize(16, 0.0f);
    siabMPWR.resize(16, 0);
    hvSW.resize(16, 0);
    sipmTemp.resize(16, 0.0f);
    siabCurr.resize(16,0.0f);
    outsideTempature = 0.0f;
    humidity=0.0f;


}

// void IEvent::LoadFromFile(const std::string& filename1, const std::string& filename2) {
//     std::ifstream file1(filename1);
//     if (!file1.is_open()) {
//         throw std::runtime_error("Failed to open file: " + filename1);
//     }

//     std::string line;
//     if (std::getline(file1, line)) {
//         std::stringstream ss(line);
//         std::string header;
//         while (ss >> header) {
//             headers.push_back(header);
//         }
//     }

//     if (headers.empty() || headers.size() != 9) {
//         throw std::runtime_error("Invalid file format: " + filename1);
//     }

//     timestampIndex = 0;
//     for (int i = 0; i < headers.size(); ++i) {
//         if (headers[i] == "timestamp") {
//             timestampIndex = i;
//             break;
//         }
//     }

//     while (std::getline(file1, line)) {
//         std::stringstream ss(line);
//         std::string cell;
//         std::vector<std::string> row;

//         while (ss >> cell) {
//             row.push_back(cell);
//         }

//         if (row.size() != headers.size()) {
//             throw std::runtime_error("Invalid file format: " + filename1);
//         }

//         data.push_back(row);
//     }

//     file1.close();

//     std::ifstream file2(filename2);
//     if (!file2.is_open()) {
//         throw std::runtime_error("Failed to open file: " + filename2);
//     }

//     if (std::getline(file2, line)) {
//         std::stringstream ss(line);
//         std::string header;
//         while (ss >> header) {
//             headers2.push_back(header);
//         }
//     }

//     if (headers2.empty() || headers2.size() != 124) {
//         throw std::runtime_error("Invalid file format: " + filename2);
//     }

//     timestamp2Index = 0;
//     for (int i = 0; i < headers2.size(); ++i) {
//         if (headers2[i] == "timestamp2") {
//             timestamp2Index = i;
//             break;
//         }
//     }

//     while (std::getline(file2, line)) {
//         std::stringstream ss(line);
//         std::string cell;
//         std::vector<std::string> row;

//         while (ss >> cell) {
//             row.push_back(cell);
//         }

//         if (row.size() != headers2.size()) {
//             throw std::runtime_error("Invalid file format: " + filename2);
//         }

//         data2.push_back(row);
//     }

//     file2.close();
// }

void IEvent::SetParametersFromTimestamp(std::string path_to_exact, std::vector<float> hv_arg, std::vector<float> hvc_arg,std::vector<float> sipmTemp_arg,std::vector<float> UCtemps_arg,std::vector<float> MUSICpower_arg,std::vector<float> HVswitch_arg,std::vector<float> ASADcurr_arg,std::vector<float> siabCurr_arg,std::vector<float> TBCurr_arg,std::vector<float> Humid_arg,std::vector<float> OutTemp_arg,std::vector<float> SunAzi_arg, std::vector<float> SunEle_arg, std::vector<float> MoonAzi_arg, std::vector<float> MoonEle_arg, std::vector<float> MoonIll_arg,std::vector<float> CamRA_arg,std::vector<float> CamDEC_arg ) {

//creating vector with m and b values for SIPM Temp
	std::string SIPMfilen  = Form("%s/exact/data/SiPMTempatureCorrections.csv",path_to_exact.c_str());
	std::ifstream SIPMfile(SIPMfilen);
	std::string line1;
    std::vector<std::vector<float>> SIPMdata;
    
    // Skip the first row
    std::getline(SIPMfile, line1);
    
    while (std::getline(SIPMfile, line1)) {
        std::stringstream ss(line1);
        std::string cell;
        std::vector<float> row;
        
        // Skip the first column
        std::getline(ss, cell, ',');

        // Read only the next 2 columns
        for (int i = 0; i < 2; ++i) {
            if (std::getline(ss, cell, ',')) {
                row.push_back(stof(cell));
            }
        }
        
        SIPMdata.push_back(row);
    }	 
    
    //creating vector with m and b values for UC Temp
    std::string UCfilen  = Form("%s/exact/data/UCTempatureCorrections.csv",path_to_exact.c_str());
    std::ifstream UCfile(UCfilen);
    std::string line2;
    std::vector<std::vector<float>> UCdata;

   // Skip the first row
    std::getline(UCfile, line2);

    while (std::getline(UCfile, line2)) {
        std::stringstream ss(line2);
        std::string cell;
        std::vector<float> row;

        // Skip the first column
        std::getline(ss, cell, ',');

        // Read only the next 2 columns
        for (int i = 0; i < 2; ++i) {
            if (std::getline(ss, cell, ',')) {
                row.push_back(stof(cell));
            }
        }

        UCdata.push_back(row);
    }

	// Just set the Parmeters from the arguments of the function

    for (std::size_t j = 0; j < hv_arg.size(); ++j) {
        hv[j] = hv_arg[j];
    }
    for (std::size_t j = 0; j < hvc_arg.size(); ++j) {
        hvc[j] = hvc_arg[j];
    }
    for (std::size_t j = 0; j < UCtemps_arg.size(); ++j) { 
	float UCy = UCdata[j][0] * UCtemps_arg[j] +  UCdata[j][1];
	std::string str_UCy = std::to_string(UCy);	
	 ucTemp[j] = UCy;
	//ucTemp[j] = UCtemps_arg[j];
    }
    for (std::size_t j = 0; j < MUSICpower_arg.size(); ++j) {
        siabMPWR[j] = MUSICpower_arg[j];
    }
    for (std::size_t j = 0; j < HVswitch_arg.size(); ++j) {
        hvSW[j] = HVswitch_arg[j];
    }
    for (std::size_t j = 0; j < sipmTemp_arg.size(); ++j) {	
		float SIPMy = SIPMdata[j][0] * sipmTemp_arg[j] +  SIPMdata[j][1];
		std::string str_SIPMy = std::to_string(SIPMy);	
        sipmTemp[j] = SIPMy;

    }


    ASADcurr = ASADcurr_arg[0];
    

    for (std::size_t j = 0; j < siabCurr_arg.size(); ++j) {
        siabCurr[j] = siabCurr_arg[j];
    }

   TBcurr = TBCurr_arg[0];

    outsideTempature = OutTemp_arg[0]; 
    humidity= Humid_arg[0];

					
    sunAzimuth = SunAzi_arg[0];
    sunElevation = SunEle_arg[0];
    moonAzimuth = MoonAzi_arg[0];
    moonElevation = MoonEle_arg[0];
    moonIllumination = MoonIll_arg[0];
    CameraRA = CamRA_arg[0];
    CameraDEC = CamDEC_arg[0];

    // Find closest timestamp in the first file
    //int closestTimestamp1 = findClosestTimestamp(data, timestamp);
   /* if (closestTimestamp1 != timestamp) {
        std::cout << "Given timestamp not found in the first file. Closest timestamp is: " << closestTimestamp1 << std::endl;
    }*/

    // Find closest timestamp in the second file
    //int closestTimestamp2 = findClosestTimestamp(data2, timestamp);
    /*if (closestTimestamp2 != timestamp) {
        std::cout << "Given timestamp not found in the second file. Closest timestamp is: " << closestTimestamp2 << std::endl;
    }*/

    // Set parameters from the first file
    // for (int i = 0; i < data.size(); ++i) {
    //     if (std::stoi(data[i][0]) == closestTimestamp1) {
    //         latitude = std::stof(data[i][1]);
    //         longitude = std::stof(data[i][2]);
    //         altitude = std::stoi(data[i][3]);
    //         sunAzimuth = std::stof(data[i][4]);
    //         sunElevation = std::stof(data[i][5]);
    //         moonAzimuth = std::stof(data[i][6]);
    //         moonElevation = std::stof(data[i][7]);
    //         horizon = std::stof(data[i][8]);
    //         azimuth = std::stof(data[i][9]);
    //         break;
    //     }
    // }

    // // Set parameters from the second file
    // for (int i = 0; i < data2.size(); ++i) {
    //     if (std::stoi(data2[i][0]) == closestTimestamp2) {
    //         TrigEvent = std::stoi(data2[i][1]);
    //         for (int j = 0; j < 4; ++j) {
    //             hv[j] = std::stof(data2[i][j + 2]);
    //         }
    //         for (int j = 0; j < 4; ++j) {
    //             hvc[j] = std::stof(data2[i][j + 6]);
    //         }
    //         for (int j = 0; j < 16; ++j) {
    //             ucTemp[j] = std::stof(data2[i][j + 10]);
    //         }
    //         for (int j = 0; j < 16; ++j) {
    //             siabMPWR[j] = std::stoi(data2[i][j + 26]);
    //         }
    //         for (int j = 0; j < 16; ++j) {
    //             hvSW[j] = std::stoi(data2[i][j + 42]);
    //         }
    //         for (int j = 0; j < 16; ++j) {
    //             sipmTemp[j] = std::stof(data2[i][j + 58]);
    //         }
    //         break;
    //     }
}


int IEvent::GetclosestTimestamp1() const {
    return 0;
}

int IEvent::GetclosestTimestamp2() const {
    return 0;
}


float IEvent::GetSunAzimuth() const {
    return sunAzimuth;
}

float IEvent::GetSunElevation() const {
    return sunElevation;
}

float IEvent::GetMoonAzimuth() const {
    return moonAzimuth;
}

float IEvent::GetMoonElevation() const {
    return moonElevation;
}

float IEvent::GetMoonIlluminaiton() const {
    return moonIllumination;
}


int IEvent::GetTrigEvent() const {
    return TrigEvent;
}


const std::vector<float>& IEvent::Gethv() const {
    return hv;
}

const std::vector<float>& IEvent::Gethvc() const {
    return hvc;
}

//////////////////////////////////////////////////////////////////////////////////////
bool IEvent::GetDoorPosition(const std::vector<float>& hv, const std::vector<float>& hvc){
    isOpen = false;
    std::vector<float> Current = hvc;
    std::vector<float>  BiasVoltage = hv;

    auto maxCurrent = std::max_element(Current.begin(), Current.end());
    float EvCurrent = std::round(10 * *maxCurrent) / 10;

    float sumV = 0.0f;

    for (float i : BiasVoltage){
        sumV += i;
    }
    // float sumV = std::accumulate(BiasVoltage.begin(), BiasVoltage.end(), 0.0);
    float BVAvg = sumV / BiasVoltage.size();
    float EvBVAvg = std::round(10 * BVAvg) / 10;

    if((EvCurrent < 3.5) && ((EvBVAvg == 44.0) || (EvBVAvg == 41.5))){
        isOpen = false;
    }
    else if ((EvCurrent >= 3.5) && ((EvBVAvg == 44.0) || (EvBVAvg == 41.5))){
        isOpen = true;
    }
    else{
        std::cerr << "HLED or weird event. Event BV: " << EvBVAvg << " Event Current: " << EvCurrent << std::endl;
    }

    return isOpen;
}
//////////////////////////////////////////////////////////////////////////////////////

const std::vector<float>& IEvent::GetUCTemp() const {
    return ucTemp;
}


const std::vector<int>& IEvent::GetsiabMPWR() const {
    return siabMPWR;
}

const std::vector<int>& IEvent::GethvSW() const {
    return hvSW;
}

const std::vector<float>& IEvent::GetSiPMTemp() const {
    return sipmTemp;
}

const std::vector<float>& IEvent::GetSIABCurrent() const {
    return siabCurr;
}

float IEvent::GetASADCurrent() const{
    return ASADcurr;
}

float IEvent::GetTBCurrent() const{
    return TBcurr;
}

float IEvent::GetOutsideTempature() const{
    return outsideTempature;
}

float IEvent::GetHumidity() const{
    return humidity;
}




int IEvent::findClosestTimestamp(const std::vector<std::vector<std::string>>& data, int timestamp) const {
    int closestTimestamp = std::stoi(data[0][0]);
    int minDifference = std::abs(timestamp - closestTimestamp);

    for (std::size_t i = 1; i < data.size(); ++i) {
        int currentTimestamp = std::stoi(data[i][0]);
        int difference = std::abs(timestamp - currentTimestamp);

        if (difference < minDifference) {
            minDifference = difference;
            closestTimestamp = currentTimestamp;
        }
    }

    return closestTimestamp;
}

int IEvent::FindClosestTimestampIndex(std::vector<unsigned long long> timeArray, unsigned long long eventTime){
    unsigned long long closestTimestamp = timeArray[0];
    unsigned long long minDifference = std::abs((long long int)(eventTime - closestTimestamp));

    unsigned long long currentTimestamp;
    unsigned long long difference;

    int index = 0;
    for (std::size_t i = 1; i < timeArray.size(); ++i) {
        currentTimestamp = timeArray[i];
        
        difference = std::abs((long long int)(eventTime - currentTimestamp));

        if (difference < minDifference) {
            minDifference = difference;
            closestTimestamp = currentTimestamp;
            index = i;
        }
    }

    return index;
}

IEvent::~IEvent(){

}


