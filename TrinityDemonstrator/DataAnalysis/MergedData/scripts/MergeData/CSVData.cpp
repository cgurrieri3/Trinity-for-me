#include "CSVData.h"

CSVData::CSVData(std::string SMfpath, std::string WXfpath, std::string CPfpath) :
    StatemessageFilePath(SMfpath), WeatherFilePath(WXfpath), CelestialFilePath(CPfpath) {}

void CSVData::SetCSV() {
    cout << "Setting Weather Data..." <<endl;
    SetWeather();
    cout << "Setting StateMessage Data..." << endl;
    SetStatemessages();
    cout << "Setting Celestial Postions Data..." << endl;
    SetCelestial();
}
     
void CSVData::SetWeather() {
    WeatherData = readCSV(WeatherFilePath);

    bool firstRow = true; 
    // Process the 2D vector for Weather(for example, print it)
    for (const auto& row : WeatherData) {
          if (firstRow) {
            firstRow = false;  // Skip the first row
            continue;
        }
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                std::string firstValue = row[33];

                //cout << firstValue << endl;
                std::time_t weatherunixTime =convertToUnixTime(firstValue);
                //cout << weatherunixTime << endl;
				double timeInDouble = static_cast<double>(weatherunixTime);
                WeatherTime.push_back(timeInDouble);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: " << row[33] << std::endl;
                WeatherTime.push_back(0);
            } catch (const std::out_of_range& e) {
            	WeatherTime.push_back(0);   
			} 


            std::vector<float> columns;
            processRow(row, columns, 14, 14);
            WeatherTempature.push_back(columns);
            columns.clear();

            processRow(row, columns, 13, 13);
            WeatherHumidity.push_back(columns);
            columns.clear();
        }
    }	
}

void CSVData::SetCelestial() {
    SunmoonData = readCSV(CelestialFilePath);
    //Process the 2D vector for SunMoon
	bool firstRowSun = true;
  
    for (const auto& row : SunmoonData) {
          if (firstRowSun) {
            firstRowSun = false;  // Skip the first row		
            continue;
        }
		
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                std::string firstValue = formatDateTime(row[0], row[1]);
                std::time_t sunmoonunixTime =convertToUnixTime(firstValue);
                
		double timeInDouble = static_cast<double>(sunmoonunixTime);
                SunmoonTime.push_back(timeInDouble);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
                std::cerr << "Invalid number in the first column: "  << std::endl;
                SunmoonTime.push_back(0);
            } catch (const std::out_of_range& e) {
                // Handle out-of-range number cases
                std::cerr << "Number out of range in the first column: " << std::endl;
                SunmoonTime.push_back(0);
            }

            std::vector<float> columns;
            processRow(row, columns,6, 6);
            
            SunAzimuth.push_back(columns);
            columns.clear();

            processRow(row, columns, 7, 7);
            SunElevation.push_back(columns);
            columns.clear();
	
            processRow(row, columns, 8, 8);
           	MoonAzimuth.push_back(columns);
            columns.clear();

            processRow(row, columns, 9, 9);
            MoonElevation.push_back(columns);
            //cout << columns[1] << endl;
            columns.clear();
            
            processRow(row, columns, 10, 10);
            MoonIllumination.push_back(columns);
            //cout << columns[1] << endl;
            columns.clear();

            processRow(row, columns, 11, 11);
            CameraRA.push_back(columns);
            //cout << columns[1] << endl;
            columns.clear();

            processRow(row, columns, 12, 12);
            CameraDEC.push_back(columns);
            //cout << columns[1] << endl;
            columns.clear();
        }
    }
}

void CSVData::SetStatemessages() {
    StateMessageData = readCSV(StatemessageFilePath);
    for (const auto& row : StateMessageData) {
          if (&row == &StateMessageData.back()) {
            //firstRow = false;  // Skip the first row
            continue;
        }
        if (!row.empty()) {

            // get a vector for the time of the statemessages
            try {
                // Convert the first value to double, divide by 1,000,000,000
                double firstValue = std::stod(row[0]) / 1000000000.0;
                StateMessageTime.push_back(firstValue);
            } catch (const std::invalid_argument& e) {
                // Handle invalid number cases
		std::cerr << "Invalid number in the first column: " << row[0] << std::endl;
                StateMessageTime.push_back(0);
            } catch (const std::out_of_range& e) {
                // Handle out-of-range number cases
                std::cerr << "Number out of range in the first column: " << row[0] << std::endl;
                StateMessageTime.push_back(0);
            }


            std::vector<float> columns;
            processRow(row, columns, 2, 5);
            StateMessageHV.push_back(columns);
            columns.clear();

            processRow(row, columns, 6, 9);
            StateMessageHVCurrent.push_back(columns);
            columns.clear();

            processRow(row,columns,10,25);
            StateMessageUCTempatures.push_back(columns);
            columns.clear();


            processRow(row,columns,26,41);
            StateMessageMUSICPower.push_back(columns);
            columns.clear();
            
            processRow(row,columns,42,57);
            StateMessageHVSwitch.push_back(columns);
            columns.clear();

            processRow(row, columns, 58, 73);
            StateMessageSiPMTempatures.push_back(columns);
            columns.clear();

            processRow(row, columns, 74, 74);
            StateMessageASADCurrent.push_back(columns);
            columns.clear();

            processRow(row, columns, 75, 90);
            StateMessageSIABCurrents.push_back(columns);
            columns.clear();

            processRow(row, columns, 91, 91);
            StateMessageTBCurrent.push_back(columns);
            columns.clear(); 
        }
    }
}

// Function to read a CSV file and return the data as a 2D vector
std::vector<std::vector<std::string>> CSVData::readCSV(const std::string& filePath) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> fields = split(line, ',');
        data.push_back(fields);
    }
    file.close();
    return data;
}

std::time_t CSVData::convertToUnixTime(const std::string& datetime) {
    struct tm tm{};
    
    // Parse the main date-time part "2024-07-19T00:49:26"
    strptime(datetime.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
    // Convert struct tm to UTC time_t
    std::time_t time = timegm(&tm);

    // Handling fractional seconds if present
    std::size_t dotPos = datetime.find('.');
    if (dotPos != std::string::npos) {
        std::string fractionalPart = datetime.substr(dotPos + 1);
        int milliseconds = std::stoi(fractionalPart);
        time += milliseconds / 1000.0;
    }

    return time;
}

void CSVData::processRow(const std::vector<std::string>& row, std::vector<float>& destination, size_t start, size_t end) {
    for (size_t i = start; i <= end; ++i) {
        try {
            destination.push_back(std::stod(row[i]));
        } catch (const std::invalid_argument& e) {
           // std::cerr << "Invalid number in column " << (i + 1) << ": " << row[i] << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Number out of range in column " << (i + 1) << ": " << row[i] << std::endl;
        }
    }
}

std::string CSVData::formatDateTime(const std::string& date, const std::string& time) {
    // Extract year, month, and day from the date
    std::string year = date.substr(0, 4);
    std::string month = date.substr(4, 2);
    std::string day = date.substr(6, 2);

    // Remove spaces from the time and ensure it has the correct format
    std::stringstream ss(time);
    int hour, minute, second;
    char colon1, colon2;
    ss >> hour >> colon1 >> minute >> colon2 >> second;

    // Format the date and time as ISO 8601
    std::ostringstream formatted;
    formatted << year << "-" << month << "-" << day << "T"
              << std::setw(2) << std::setfill('0') << hour << ":"
              << std::setw(2) << std::setfill('0') << minute << ":"
              << std::setw(2) << std::setfill('0') << second;

    return formatted.str();
}

// Function to split a string into a vector based on a delimiter
std::vector<std::string> CSVData::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}