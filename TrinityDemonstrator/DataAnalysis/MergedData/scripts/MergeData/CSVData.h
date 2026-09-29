#ifndef CSVData_H
#define CSVData_H


#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

class CSVData {
    public:
        CSVData(std::string SMfpath, std::string WXfpath, std::string CPfpath);

        // Set Functions
        void SetCSV();

        // Get functions
        // Weather Variables
        std::vector<double>& GetWeatherTime() { return WeatherTime; }
        std::vector<std::vector<float>>& GetWeatherTempature() { return WeatherTempature; }
        std::vector<std::vector<float>>& GetWeatherHumidity() { return WeatherHumidity; }
        
        // Celestial Position variables
        std::vector<double>& GetSunmoonTime() { return SunmoonTime; }
        std::vector<std::vector<float>>& GetSunAzimuth() { return SunAzimuth; }
        std::vector<std::vector<float>>& GetSunElevation() { return SunElevation; }
        std::vector<std::vector<float>>& GetMoonAzimuth() { return MoonAzimuth; }
        std::vector<std::vector<float>>& GetMoonElevation() { return MoonElevation; }
        std::vector<std::vector<float>>& GetMoonIllumination() { return MoonIllumination; }
        std::vector<std::vector<float>>& GetCameraRA() { return CameraRA; }
        std::vector<std::vector<float>>& GetCameraDEC() { return CameraDEC; }
    

        //Statemessages Variables
        std::vector<double>& GetStateMessageTime() { return StateMessageTime; }
        std::vector<std::vector<float>>& GetStateMessageHV() { return StateMessageHV; }
        std::vector<std::vector<float>>& GetStateMessageHVCurrent() { return StateMessageHVCurrent; }
        std::vector<std::vector<float>>& GetStateMessageSiPMTempatures() { return StateMessageSiPMTempatures; }
        std::vector<std::vector<float>>& GetStateMessageUCTempatures() { return StateMessageUCTempatures; }
        std::vector<std::vector<float>>& GetStateMessageHVSwitch() { return StateMessageHVSwitch; }
        std::vector<std::vector<float>>& GetStateMessageMUSICPower() { return StateMessageMUSICPower; }
        std::vector<std::vector<float>>& GetStateMessageASADCurrent() { return StateMessageASADCurrent; }
        std::vector<std::vector<float>>& GetStateMessageSIABCurrents() { return StateMessageSIABCurrents; }
        std::vector<std::vector<float>>& GetStateMessageTBCurrent() { return StateMessageTBCurrent; }
        
    private:
        std::string StatemessageFilePath;
        std::string WeatherFilePath;
        std::string CelestialFilePath;

        void SetWeather();
        void SetCelestial();
        void SetStatemessages();

        std::vector<std::vector<std::string>> readCSV(const std::string& filePath);
        std::time_t convertToUnixTime(const std::string& datetime);
        void processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end);
        std::string formatDateTime(const std::string& date, const std::string& time);    
        std::vector<std::string> split(const std::string& str, char delimiter);

        // Weather Variables
        std::vector<std::vector<std::string>> WeatherData;
        std::vector<double> WeatherTime;
        std::vector<std::vector<float>> WeatherTempature;
        std::vector<std::vector<float>> WeatherHumidity;
        
        // Celestial Position variables
        std::vector<std::vector<std::string>> StateMessageData;
        std::vector<std::vector<std::string>> SunmoonData;
        std::vector<double> SunmoonTime;
        std::vector<std::vector<float>> SunAzimuth;
        std::vector<std::vector<float>> SunElevation;
        std::vector<std::vector<float>> MoonAzimuth;
        std::vector<std::vector<float>> MoonElevation;
        std::vector<std::vector<float>> MoonIllumination;
        std::vector<std::vector<float>> CameraRA;
        std::vector<std::vector<float>> CameraDEC;
    

        //Statemessages Variables
        std::vector<double> StateMessageTime;
        std::vector<std::vector<float>> StateMessageHV;
        std::vector<std::vector<float>> StateMessageHVCurrent;

        std::vector<std::vector<float>> StateMessageSiPMTempatures;
        std::vector<std::vector<float>> StateMessageUCTempatures;
        std::vector<std::vector<float>> StateMessageHVSwitch;
        std::vector<std::vector<float>> StateMessageMUSICPower;

        std::vector<std::vector<float>> StateMessageASADCurrent;
        std::vector<std::vector<float>> StateMessageSIABCurrents;
        std::vector<std::vector<float>> StateMessageTBCurrent;
};
#endif