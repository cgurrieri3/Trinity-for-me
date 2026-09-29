import os
import csv
import numpy as np
from datetime import datetime, timedelta
import pandas as pd
import time
import pytz
# TO DO: 
# make it work for files names and times
# get weather and moon to work 
# get weather and moon to update daily

class color:
   PURPLE = '\033[95m'
   CYAN = '\033[96m'
   DARKCYAN = '\033[36m'
   BLUE = '\033[94m'
   GREEN = '\033[92m'
   YELLOW = '\033[93m'
   RED = '\033[91m'
   BOLD = '\033[1m'
   UNDERLINE = '\033[4m'
   END = '\033[0m'

# checks user input of the data
def checkDatainput():
    while True: 
        date=input(color.RED + "Date (YYYYMMDD):" + color.END)
        try: 
            val=int(date)
            if len(date) == 8 and int(date) > 20231003:
                return date
                break
            else:
                print("Please enter a value date.") 
        except:
            print("Please enter a value date.") 

def getPath():
    # user input path
    # /data/TrinityLabComputer/TrinityDemonstrator: /mnt
    path=input(color.RED + "Source Path:" + color.END)
    print(path)
    return path

#############################################################################################
def loadDataFiles(d, p):
    # /mnt 
    # folderPath = f"{p}/Data/{d}/RawDataMerged/"
    listFileName = f"{p}/DataAnalysis/data_lists/file_list_{d}.txt"
    with open(listFileName, 'r') as listFile:
        dataList = [line.strip() for line in listFile]
    # dataList = os.listdir(folderPath)
    dataList = np.sort(np.array(dataList))
    if len(dataList) == 0:
        dataList = [f"{d}T00:00:00.00_FolderEmpty"]
    print(dataList)
    return dataList
#############################################################################################

def timeFromFile(file,n):
    file = file[:-8]
    try: 
        ftime = datetime.strptime(file, 'CoBo0_AsAd0_%Y-%m-%dT%H:%M:%S.%f_0')
    except ValueError: 
        #print("here")
        ftime = datetime.strptime(file, '%Y%m%dT%H:%M:%S.%f_Fol')
    if n == "w":
        ftime = ftime.strftime("%Y-%m-%d %H:%M:%S")
    else:
        ftime = ftime.strftime("%Y-%m-%d %H:%M")


    return ftime
##-------------------------Get Definitions-------------------------##

def getDataInfo():
    print(color.UNDERLINE + "\nData Information Table please provide: " + color.END)
    badPixels=input(color.BLUE + "Bad Pixels (ex. 23,56,129):" + color.END)
    # hvValues=input (color.BLUE + "\nDid the HV values change after the DAQ begun? (ex. 44 or 'yes')" + color.END)    
    commentDataInfo=input(color.BLUE + "\nComment: "+ color.END)
    return badPixels, commentDataInfo


def getRankingInfo():
    print(color.UNDERLINE + "\nRanking Table please provide: " + color.END)
    ranking=input(color.RED + "Ranking (see wiki):"+ color.END)
    rankingComment=input(color.BLUE + "\nComment: "+ color.END)
    return ranking, rankingComment

def getWeatherInfo():
    print(color.UNDERLINE + "\nWeather Table please provide: " + color.END)
    clouds=input(color.RED + "Clouds (see wiki): "+ color.END)
    cloudsFreq=input(color.RED + "\nClouds Frequency (see wiki): "+ color.END)
    snowGround = input(color.RED + "\nSnow on the Ground?: " + color.END)
    rain=input(color.RED + "\nRaining/Snowing (see wiki): "+ color.END)
    comments=input(color.BLUE + "\nComment: "+ color.END)
    return clouds, cloudsFreq, snowGround, rain, comments
##-------------------------Write Definitions-------------------------##

def writeDataInfoSQL(d, p, fileList, badPixels, commentDataInfo):
    output_sql = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/Output/DataInfo_{d}.sql'  # Output SQL file
    dataSMFile = f"{p}/DataAnalysis/AncillaryData/Data2/statemessages{d}.csv"
    header = ["Timestamp","TriggerRate","HV1","HV2","HV3","HV4","HVCur1","HVCur2","HVCur3","HVCur4","UCTemp#1","UCTemp#10","UCTemp11","UCTemp#12","UCTemp#13","UCTemp#14","UCTemp#15","UCTemp#16","UCTemp#2","UCTemp#3","UCTemp#4","UCTemp#5","UCTemp#6","UCTemp#7","UCTemp#8","UCTemp#9","MUSICMPWR1","MUSICPWR10","MUSICPWR11","MUSICPWR12","MUSICPWR13","MUSICPWR14","MUSICPWR15","MUSICPWR16","MUSICPWR2","MUSICPWR3","MUSICPWR4","MUSICPWR5","MUSICPWR6","MUSICPWR7","MUSICPWR8","MUSICPWR9","HVSW1","HVSW10","HVSW11","HVSW12","HVSW13","HVSW14","HVSW15","HVSW16","HVSW2","HVSW3","HVSW4","HVSW5","HVSW6","HVSW7","HVSW8","HVSW9","SiPMTemp#1","SiPMTemp#10","SiPMTemp#11","SiPMTemp#12","SiPMTemp#13","SiPMTemp#14","SiPMTemp#15","SiPMTemp#16","SiPMTemp#2","SiPMTemp#3","SiPMTemp#4","SiPMTemp#5","SiPMTemp#6","SiPMTemp#7","SiPMTemp#8","SiPMTemp#9","ASADCurrent","SIABcurr#1","SIABcurr#10","SIABcurr#11","SIABcurr#12","SIABcurr#13","SIABcurr#14","SIABcurr#15","SIABcurr#16","SIABcurr#2","SIABcurr#3","SIABcurr#4","SIABcurr#5","SIABcurr#6","SIABcurr#7","SIABcurr#8","SIABcurr#9","TBCurr","UnixTime"]

    # load and datetime the weather csv
    df = pd.read_csv(dataSMFile, delimiter=',', header=None, names=header)
    #print(df)
    df['Timestamp'] = (pd.to_datetime(df['Timestamp'],unit='ns',errors='coerce'))
    # Define ET and UTC timezones
    et_tz = pytz.timezone('US/Eastern')
    utc_tz = pytz.utc

    # Convert ET to UTC
    #df['Timestamp'] = df['Timestamp'].dt.tz_localize(et_tz, ambiguous='NaT',nonexistent='shift_forward').dt.tz_convert(utc_tz)

    df['Timestamp'] = df['Timestamp'].dt.strftime("%Y-%m-%d %H:%M")



    dataSumFile = f"{p}/DataAnalysis/DataSummary/Output/{d}.csv"
    columns = ["AvgNumEvents","AmpDist", "HLEDmean", "HLEDnormmean", "Pedestalmean","PedestalRMSmean", "Chargemean", "Peaktimemean", "Sigma", "Threshold","ThresholdValue"] 

    with open(dataSumFile, mode='r') as file:
        #reader = csv.DictReader(file)  # Using DictReader to handle CSV columns by name
        reader = csv.reader(file)
        # Open the input CSV file for reading
        with open(output_sql, mode='w') as sql_file:
                # Loop over each row in the CSV file
            for row in reader:
                row = dict(zip(columns, row))
                for f in fileList:
                    #print(df['Timestamp'])
                    #print(timeFromFile(f,"n"))
                    

                    SMInfo=df[df['Timestamp'] == timeFromFile(f,"n")]
                    #print(SMInfo)
                    if SMInfo.empty:
                        for i in range(1,15):
                            #print(f"Checking the next minute  + {i}")
                            updateTime = datetime.strptime(timeFromFile(f,"n"),'%Y-%m-%d %H:%M') + timedelta(minutes=i)
                            updateTime = updateTime.strftime("%Y-%m-%d %H:%M")
                            #print(updateTime)
                            SMInfo=df[df['Timestamp'] == updateTime]
                            if not SMInfo.empty:
                                #print(SMInfo)
                                break
                    if SMInfo.empty:
                        for i in range(1,15):
                            #print(f"Checking the next minute - {i}")
                            updateTime = datetime.strptime(timeFromFile(f,"n"),'%Y-%m-%d %H:%M') - timedelta(minutes=i)
                            updateTime = updateTime.strftime("%Y-%m-%d %H:%M")
                            #print(updateTime)
                            SMInfo=df[df['Timestamp'] == updateTime]
                            if not SMInfo.empty:
                                #print(SMInfo)
                                break
                    #print(SMInfo)    
                    #print(row)    
                    # Generate the INSERT statement for each row
                    try: 
                        if "Threshold" in row:
                            sql = (
                                f"INSERT INTO DataInfo(FileName, AmpDist, HLEDmean, HLEDnormmean, Pedestalmean, PedestalRMSmean, Chargemean, Peaktimemean, badPixels, thresholdtime, threshold, avgEvents, sigma, hvValues1, hvValues2, hvValues3, hvValues4, hvcurrents1, hvcurrents2, hvcurrents3, hvcurrents4, musicpower1, comments) "
                                f"VALUES ('{f}','{row['AmpDist']}','{row['HLEDmean']}','{row['HLEDnormmean']}','{row['Pedestalmean']}','{row['PedestalRMSmean']}','{row['Chargemean']}','{row['Peaktimemean']}','{badPixels}','{row['Threshold']}','{row['ThresholdValue']}','{row['AvgNumEvents']}','{row['Sigma']}','{SMInfo['HV1'].tolist()[0]}','{SMInfo['HV2'].tolist()[0]}','{SMInfo['HV3'].tolist()[0]}','{SMInfo['HV4'].tolist()[0]}','{SMInfo['HVCur1'].tolist()[0]}','{SMInfo['HVCur2'].tolist()[0]}','{SMInfo['HVCur3'].tolist()[0]}','{SMInfo['HVCur4'].tolist()[0]}','{SMInfo['MUSICMPWR1'].tolist()[0]}','{commentDataInfo}');\n"
                            )
                        else:
                            sql = (
                                f"INSERT INTO DataInfo(FileName, AmpDist, HLEDmean, HLEDnormmean, Pedestalmean, PedestalRMSmean, Chargemean, Peaktimemean, badPixels, thresholdtime, threshold, avgEvents, sigma, hvValues1, hvValues2, hvValues3, hvValues4, hvcurrents1, hvcurrents2, hvcurrents3, hvcurrents4, musicpower1, comments) "
                                f"VALUES ('{f}','{row['AmpDist']}','{row['HLEDmean']}','{row['HLEDnormmean']}','{row['Pedestalmean']}','{row['PedestalRMSmean']}','{row['Chargemean']}','{row['Peaktimemean']}','{badPixels}','-1','-1','{row['AvgNumEvents']}','{row['Sigma']}','{SMInfo['HV1'].tolist()[0]}','{SMInfo['HV2'].tolist()[0]}','{SMInfo['HV3'].tolist()[0]}','{SMInfo['HV4'].tolist()[0]}','{SMInfo['HVCur1'].tolist()[0]}','{SMInfo['HVCur2'].tolist()[0]}','{SMInfo['HVCur3'].tolist()[0]}','{SMInfo['HVCur4'].tolist()[0]}','{SMInfo['MUSICMPWR1'].tolist()[0]}','{commentDataInfo}');\n"
                            )
                        # Write the generated SQL statement to the output file
                        sql_file.write(sql)
                    except: 
                        print(f"SM incomplete such as empty night or some error please confirm file {f}")
                        sql = (
                            f"INSERT INTO DataInfo(FileName, AmpDist, HLEDmean, HLEDnormmean, Pedestalmean, PedestalRMSmean, Chargemean, Peaktimemean, badPixels, thresholdtime, threshold, avgEvents, sigma, hvValues1, hvValues2, hvValues3, hvValues4, hvcurrents1, hvcurrents2, hvcurrents3, hvcurrents4, musicpower1, comments) "
                            f"VALUES ('{f}','{row['AmpDist']}','{row['HLEDmean']}','{row['HLEDnormmean']}','{row['Pedestalmean']}','{row['PedestalRMSmean']}','{row['Chargemean']}','{row['Peaktimemean']}','{badPixels}','-1','-1','{row['AvgNumEvents']}','{row['Sigma']}', '-1', '-1','-1','-1', '-1', '-1', '-1', '-1', '-1','{commentDataInfo}');\n"
                        )
                        # Write the generated SQL statement to the output file
                        sql_file.write(sql)


    os.chmod(output_sql, 0o774)
    return print(f"DataInfo SQL queries have been written to {output_sql}")
    
def writeRankingSQL(d, p, fileList,ranking, rankingComment):
    output_sql = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/Output/Ranking_{d}.sql'  # Output SQL file
    OpModeFile = f'{p}/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/Output/AllFiles.txt'
    # OpModeFile = f'{p}/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/Output/{d}_AllFiles.txt'

    df = pd.read_csv(OpModeFile, delimiter=',')
    csv_path = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/OpModeFile.csv'
    # csv_path = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/Output/OpModeFile_{d}.csv'
    print(f"Opmode file name {csv_path}")
    df.to_csv(csv_path, index = None)
    print(df)

   # makes fileList name and File Name from csv in same format 
    df['File Name'] = df['File Name'].str.replace('Merged_', '') 
    for x in df['File Name']:
        print(x)

    with open(output_sql, mode='w') as sql_file:

        for f in fileList:
            #looking for csv data for entry with same file name as f in fileList
            opModeInfo = df[df['File Name'] == f]

            try:
            # Generate the INSERT statement for each row
                sql = (
                    f"INSERT INTO RankingInfo(Filename, ranking, comments, OpMode, Test Events, HLED Events, Forced Events) "
                    f"VALUES ('{f}','{ranking}','{rankingComment}','{opModeInfo[' OpMode'].tolist()[0]}','{opModeInfo[' Test Events'].tolist()[0]}','{opModeInfo[' HLED Events'].tolist()[0]}','{opModeInfo[' Forced Events'].tolist()[0]}');\n"
                )
                # Write the generated SQL statement to the output file
                sql_file.write(sql)

            except:
                sql = (
                    f"INSERT INTO RankingInfo(Filename, ranking, comments, OpMode, Test Events, HLED Events, Forced Events) "
                    f"VALUES ('{f}','{ranking}','{rankingComment}', '6');\n"
                )
                # Write the generated SQL statement to the output file
                sql_file.write(sql)
                print(f" no matching file in csv, check file {f}")

    os.chmod(output_sql, 0o774)
    return print(f"Ranking SQL queries have been written to {output_sql}")

def writeWeatherSQL(d, p, fileList, clouds, cloudsFreq, snowGround, rain, comments):
    output_sql = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/Output/Weather_{d}.sql'
    # print(p)
    weatherFile = f'{p}/MiscData/WeatherData/weather/weather_{d}'
    # print(f'{p}/MiscData/WeatherData/weather/weather_{d}')

    header = [
            "Node", "RelativeWindDirection", "RelativeWindSpeed", "CorrectedWindDirection",
            "AverageRelativeWindDirection",
            "AverageRelativeWindSpeed", "RelativeGustDirection", "RelativeGustSpeed",
            "AverageCorrectedWindDirection",
            "WindSensorStatus", "Pressure", "Pressure_at_Sea_level", "Pressure_at_Station", 
            "Relative_Humidity","Temperature", "Dewpoint",
            "Absolute_Humidity", "compassHeading", "WindChill", "HeatIndex", "AirDensity", 
            "WetBulbTempature", "SunRiseTime", "SolarNoonTime", "SunsetTime",
            "Position of the Sun", "Twilight(Civil)","Twilight(Nautical)",
            "Twilight(Astronomical)", "X_Tilt", "Y_Tilt", "Z_Orientation", "User_Information_Field",
            "DateTime","Supply_Voltage", "Status", "Checksum"]

    # load and datetime the weather csv
    df = pd.read_csv(weatherFile, delimiter=',', header=None, names=header)
    #print(df)
    df['DateTime'] = pd.to_datetime(df['DateTime'], errors='coerce', format="%Y-%m-%dT%H:%M:%S.%f")
    df['DateTime'] = df['DateTime'].dt.strftime("%Y-%m-%d %H:%M:%S")

    
    # Get prior days weather file for 00:00-00:48 times
    d_before = datetime.strptime(d, "%Y%m%d") - timedelta(days=1)
    day_before=d_before.strftime("%Y%m%d")

    # load and datetime the weather csv
    weatherFile = f'{p}/MiscData/WeatherData/weather/weather_{day_before}'
    df2 = pd.read_csv(weatherFile, delimiter=',', header=None, names=header)
    #print(df)
    df2['DateTime'] = pd.to_datetime(df2['DateTime'], errors='coerce', format="%Y-%m-%dT%H:%M:%S.%f")
    df2['DateTime'] = df2['DateTime'].dt.strftime("%Y-%m-%d %H:%M:%S")

    try: 
        # get next day too 
        # Get prior days weather file for 00:00-00:48 times
        d_after = datetime.strptime(d, "%Y%m%d") + timedelta(days=1)
        day_after=d_after.strftime("%Y%m%d")
        df = pd.concat([df,df2])

        # load and datetime the weather csv
        weatherFile = f'{p}/MiscData/WeatherData/weather/weather_{day_after}'
        df3 = pd.read_csv(weatherFile, delimiter=',', header=None, names=header)
        df3['DateTime'] = pd.to_datetime(df3['DateTime'], errors='coerce', format="%Y-%m-%dT%H:%M:%S.%f")
        df3['DateTime'] = df3['DateTime'].dt.strftime("%Y-%m-%d %H:%M:%S")
        df = pd.concat([df,df2,df3])
    except: 
        print("Current Day so no next day files are present for weather")

    with open(output_sql, mode='w') as sql_file:

        for f in fileList:
            weatherInfo=df[df['DateTime'] == timeFromFile(f,"w")]
            
            # Generate the INSERT statement for each row
            try: 
                sql = (
                    f"INSERT INTO WeatherInfo(Filename, Humidity, OutsideTempature, Dewpoint, WindSpeed, WindDirection, clouds, cloudFrequency, snowGround, rainSnow, comments) "
                    f"VALUES ('{f}','{weatherInfo['Relative_Humidity'].tolist()[0]}','{weatherInfo['Temperature'].tolist()[0]}','{ weatherInfo['Dewpoint'].tolist()[0]}','{weatherInfo['RelativeWindSpeed'].tolist()[0]}','{weatherInfo['RelativeWindDirection'].tolist()[0]}','{clouds}','{cloudsFreq}','{snowGround}','{rain}','{comments}');\n"
                )
                # Write the generated SQL statement to the output file
                sql_file.write(sql)
            except: 
                print(f"Weather incomplete or some error please check file {f}")

    os.chmod(output_sql, 0o774)
    return print(f"Weather SQL queries have been written to {output_sql}")
        
def writeCelestialSQL(d, p, fileList):
    output_sql = f'{p}/DataAnalysis/AncillaryData/file_database/UpdateDB/Output/Celestial_{d}.sql'
    celestialFile=f'{p}/DataAnalysis/AncillaryData/Data1/celestialPositions{d}.csv'

    df = pd.read_csv(celestialFile, delimiter=',')
    df['DateTime'] = df['date'].astype(str)+ "T" + df['time'].astype(str)
    df['DateTime'] = pd.to_datetime(df['DateTime'])
    df['DateTime'] = df['DateTime'].dt.strftime("%Y-%m-%d %H:%M")

    with open(output_sql, mode='w') as sql_file:

        for f in fileList:
            celestialInfo=df[df['DateTime'] == timeFromFile(f,"c")]

            try:
                # Generate the INSERT statement for each row
                sql = (
                    f"INSERT INTO CelestialInfo(Filename, sunAzimuth, sunAltitude, moonAzimuth, moonAltitude, moonIllumination, RAinCam, DECinCam) "
                    f"VALUES ('{f}','{celestialInfo['sunAzimuth'].tolist()[0]}','{celestialInfo['sunAltitude'].tolist()[0]}','{ celestialInfo['moonAzimuth'].tolist()[0]}','{celestialInfo['moonAltitude'].tolist()[0]}','{celestialInfo['moonIllumination'].tolist()[0]}','{celestialInfo['RAinCam'].tolist()[0]}','{celestialInfo['DECinCam'].tolist()[0]}');\n"
                )
                # Write the generated SQL statement to the output file
                sql_file.write(sql)
            except: 
                print(f"Celestial Positions incomplete or some error please check file {f}")
    os.chmod(output_sql, 0o774)
    return print(f"Celestial SQL queries have been written to {output_sql}")
    

def main(): 
    ##------------------------- Start Code ------------------------------------------------------##
    ##-------------------------------------------------------------------------------------------##
    ##-------------------------------------------------------------------------------------------##
    ##------------------------- Load in File Information Table Creatation -----------------------##

    print("Welcome to the database upload. \nPlease provide the night uploading.")

    folderName = checkDatainput()
    #folderName = "20240718"
    path = getPath()
    fileList=loadDataFiles(folderName, path)

    # ##------------------------- Weather Table Creatation------------------------------------------##

    clouds, cloudsFreq, snowGround, rain, comments = getWeatherInfo()
    # print(path)
    writeWeatherSQL(folderName, path, fileList, clouds, cloudsFreq, snowGround, rain, comments)

    ##------------------------- Celstial Table Creatation-----------------------------------------##

    writeCelestialSQL(folderName, path, fileList)

    ##------------------------ Data Info Table Creatation----------------------------------------##

    # User inputs that have to do with the whole night 
    # Bad pixel - User
    # Comment - User
    # HV value - by file
    # Threshold - get from data summary
    # Avg Events - get from data summary
    # PSF - get from data summary
    # Min HV current - get from data summary
    # Max HV current - get from data summary 
    # Amp dist - get from data summary

    badPixels, commentDataInfo = getDataInfo()
    writeDataInfoSQL(folderName, path, fileList, badPixels, commentDataInfo)

    ##------------------------- Ranking Table Creatation------------------------------------------##

    ranking, rankingComment = getRankingInfo()
    writeRankingSQL(folderName, path, fileList, ranking, rankingComment)

if __name__ == "__main__":
    main()





