from influxdb import InfluxDBClient
import pandas as pd
import argparse
import os
import subprocess
import sys
import time

# takes file type
def sql2csv(ft,d):
    print(f"Converting {ft} from sql file to csv file.")
    if ft == "WX":  
        command = r"sed 's/^.\{163\}//; s/..$//; s/'\''//g' " + f"Weather_{d}.sql > Weather_{d}.csv" # weather to csv
    elif ft == "RX":
        command = r"sed 's/^.\{61\}//; s/..$//; s/'\''//g' "  + f"Ranking_{d}.sql > Ranking_{d}.csv"
    elif ft == "SM":
        command = r"sed 's/^.\{304\}//; s/..$//; s/'\''//g' " + f"DataInfo_{d}.sql > Data_{d}.csv"
    elif ft == "CX":
        command = r"sed 's/^.\{133\}//; s/..$//; s/'\''//g' " + f"Celestial_{d}.sql > Celestial_{d}.csv"
    subprocess.run(command, shell=True, check=True)

# takes folder path and date
def loaddf(fp,d):
    # convert .sql file to csv
    sql2csv("WX",d)
    # load in the csv
    header = ["Filename", "Humidity", "OutsideTempature", "Dewpoint", "WindSpeed", "WindDirection", "clouds", "cloudFrequency", "snowGround", "rainSnow", "comments"]
    df_wx=pd.read_csv(f'{fp}/Weather_{str(d)}.csv', names = header,na_values='')
    values={"Filename": '', "Humidity": -1000, "OutsideTempature": -1000, "Dewpoint": -1000, "WindSpeed": -1000, "WindDirection": -1000, "clouds": '', "cloudFrequency": '', "snowGround": '', "rainSnow": '', "comments": ''}
    df_wx = df_wx.fillna(value=values)
    
    sql2csv("RX",d)
    header = ["Filename", "ranking", "comments"]
    df_rx=pd.read_csv(f'{fp}/Ranking_{str(d)}.csv', names = header,na_values='')
    values = {"Filename": '', "ranking": '', "comments": ''}
    df_rx = df_rx.fillna(value=values)
    
    sql2csv("SM",d)
    header =["Filename", "AmpDist", "HLEDmean", "HLEDnormmean", "Pedestalmean", "PedestalRMSmean", "Chargemean", "Peaktimemean", "badPixels", "thresholdtime", "threshold", "avgEvents", "sigma", "hvValues1", "hvValues2", "hvValues3", "hvValues4", "hvcurrents1", "hvcurrents2", "hvcurrents3", "hvcurrents4", "musicpower1", "comments"]
    df_sm=pd.read_csv(f'{fp}/Data_{str(d)}.csv', names = header,na_values='')
    values ={"Filename": '', "AmpDist": -1000, "HLEDmean": -1000, "HLEDnormmean": -1000, "Pedestalmean": -1000, "PedestalRMSmean": -1000, "Chargemean": -1000, "Peaktimemean": -1000, "badPixels": '', "thresholdtime": '', "threshold": '', "avgEvents": -1000, "sigma": -1000, "hvValues1": -1000, "hvValues2": -1000, "hvValues3": -1000, "hvValues4": -1000, "hvcurrents1": -1000, "hvcurrents2": -1000, "hvcurrents3": -1000, "hvcurrents4": -1000, "musicpower1": -1000, "comments": ''}
    df_sm = df_sm.fillna(value=values)

    sql2csv("CX",d)
    header =["Filename", "sunAzimuth", "sunAltitude", "moonAzimuth", "moonAltitude", "moonIllumination", "RAinCam", "DECinCam"]
    df_cx=pd.read_csv(f'{fp}/Celestial_{str(d)}.csv', names = header,na_values='')
    values ={"Filename": '', "sunAzimuth": -1000, "sunAltitude": -1000, "moonAzimuth": -1000, "moonAltitude": -1000, "moonIllumination": -1000}
    df_cx = df_cx.fillna(value=values)
    
    # print(df_wx)
    return df_wx, df_rx, df_sm, df_cx

def prepWXdf(df_wx,d):
    data_points = []
    

    for index, row in df_wx.iterrows():
        if str(row['Filename'][0]) == 'C':
            filetime=pd.to_datetime(str(row['Filename'][:-10]), format="CoBo0_AsAd0_%Y-%m-%dT%H:%M:%S.%f")
        else:
            filetime=pd.to_datetime(str(row['Filename']), format="%Y%m%dT%H:%M:%S.%f_FolderEmpty")
        data_point = [{
            'measurement': 'WeatherInfo', 
            'tags': {'Date': str(d)},
            'time': filetime,
            'fields': {
                'Filename': str(row['Filename']),
                'Humidity': float(row['Humidity']),
                'OutsideTempature': float(row['OutsideTempature']),
                'Dewpoint': float(row['Dewpoint']),
                'WindSpeed': float(row['WindSpeed']),
                'WindDirection': float(row['WindDirection']),
                'clouds': str(row['clouds']),
                'cloudFrequency': str(row['cloudFrequency']),
                'snowGround': str(row['snowGround']),
                'rainSnow': str(row['rainSnow']),
                'comments': str(row['comments'])
            }
        }]   
        data_points.append(data_point)
    # print(data_points)
    return data_points

def prepRXdf(df_rx,d):
    data_points = []

    for index, row in df_rx.iterrows():
        if str(row['Filename'][0]) == 'C':
            filetime=pd.to_datetime(str(row['Filename'][:-10]), format="CoBo0_AsAd0_%Y-%m-%dT%H:%M:%S.%f")
        else:
            filetime=pd.to_datetime(str(row['Filename']), format="%Y%m%dT%H:%M:%S.%f_FolderEmpty")
        data_point = [{
            'measurement': 'RankingInfo', 
            'tags': {'Date': str(d)},
            'time': filetime,
            'fields': {
                'Filename': str(row['Filename']),
                'ranking': str(row['ranking']),
                'comments': str(row['comments'])

            }
        }]   
        data_points.append(data_point)
    # print(data_points)
    return data_points

def prepSMdf(df_sm,d):
    data_points = []    

    for index, row in df_sm.iterrows():
        if str(row['Filename'][0]) == 'C':
            filetime=pd.to_datetime(str(row['Filename'][:-10]), format="CoBo0_AsAd0_%Y-%m-%dT%H:%M:%S.%f")
        else:
            filetime=pd.to_datetime(str(row['Filename']), format="%Y%m%dT%H:%M:%S.%f_FolderEmpty")
        data_point = [{
            'measurement': 'DataInfo', 
            'tags': {'Date': str(d)},
            'time': filetime,
            'fields': {
                'Filename': str(row['Filename']),
                'AmpDist': float(row['AmpDist']),
                'HLEDmean': float(row['HLEDmean']),
                'HLEDnormmean': float(row['HLEDnormmean']),
                'Pedestalmean': float(row['Pedestalmean']),
                'PedestalRMSmean': float(row['PedestalRMSmean']),
                'Chargemean': float(row['Chargemean']),
                'Peaktimemean': float(row['Peaktimemean']),
                'badPixels': str(row['badPixels']),
                'thresholdtime': str(row['thresholdtime']),
                'threshold': str(row['threshold']),
                'avgEvents': float(row['avgEvents']),
                'sigma': float(row['sigma']),
                'hvValues1': float(row['hvValues1']),
                'hvValues2': float(row['hvValues2']),
                'hvValues3': float(row['hvValues3']),
                'hvValues4': float(row['hvValues4']),
                'hvcurrents1': float(row['hvcurrents1']),
                'hvcurrents2': float(row['hvcurrents2']),
                'hvcurrents3': float(row['hvcurrents3']),
                'hvcurrents4': float(row['hvcurrents4']),
                'musicpower1': float(row['musicpower1']),
                'comments': str(row['comments'])
            }
        }]   
        data_points.append(data_point)
    # print(data_points)
    return data_points

def prepCXdf(df_cx,d):
    data_points = []
    for index, row in df_cx.iterrows():
        if str(row['Filename'][0]) == 'C':
            filetime=pd.to_datetime(str(row['Filename'][:-10]), format="CoBo0_AsAd0_%Y-%m-%dT%H:%M:%S.%f")
        else:
            filetime=pd.to_datetime(str(row['Filename']), format="%Y%m%dT%H:%M:%S.%f_FolderEmpty")
        data_point = [{
            'measurement': 'CelestialInfo', 
            'tags': {'Date': str(d)},
            'time': filetime,
            'fields': {
                'Filename': str(row['Filename']),
                'sunAzimuth': float(row['sunAzimuth']),
                'sunAltitude': float(row['sunAltitude']),
                'moonAzimuth': float(row['moonAzimuth']),
                'moonAltitude': float(row['moonAltitude']),
                'moonIllumination': float(row['moonIllumination']),
                'RAinCam': float(row['RAinCam']),
                'DECinCam': float(row['DECinCam'])
            }
        }]   
        data_points.append(data_point)
    # print(data_points)
    return data_points

def deletesql(d):
    os.remove(f"Weather_{d}.sql")
    os.remove(f"Ranking_{d}.sql")
    os.remove(f"DataInfo_{d}.sql")
    os.remove(f"Celestial_{d}.sql")


def main():
    ################# Location of data ################

    folder_path=f'/data/TrinityLabComputer/Database/file_db/hiveData/Output'

    ###########################################################
    

    ################# Database intinitation ###################

    # Local host lines for access
    host = 'localhost'
    port = 8086
    username = 'admin'
    password = 'Ttys@210'
    database = 'TDFiles'

    # Initialize the InfluxDB client and write the points in batches
    client = InfluxDBClient(host = host, port=port, username=username, password=password)

    # Create a new database if it does not already exist
    # client.create_database(database)
    # result=client.query("show databases")
    # print(result)
    # Switch to the newly created database
    client.switch_database(database)

    ################ End of DB Inintiation ####################

    ################ Retreive arguments    ####################
    # Initialize parser
    parser = argparse.ArgumentParser()
    # Adding optional argument
    parser.add_argument("-d", "--Date", help = "Date YYYYMMDD")
    # Read arguments from command line
    args = parser.parse_args()
    date = args.Date
    ################ Complete Retreive arguments ##############

    ################ Run Expect script to get .sql files ##############
    print("Geting files from HIVE:")
    command = f"expect getsqlfiles.exp {date}"
    subprocess.run(command, shell=True, check=True)

    ################ Run Expect script to get .sql files ##############

    # load weather data into the influxdb database in WeatherInfo
    os.chdir('hiveData/Output')
    try:
        df_wx, df_rx, df_sm, df_cx=loaddf(folder_path, date)
    except: 
        print(f"Not all *{date}.sql files are present.")
        print("Exiting...")
        sys.exit()

    list_wx=prepWXdf(df_wx,date)
    list_rx=prepRXdf(df_rx,date)
    list_sm=prepSMdf(df_sm,date)
    list_cx=prepCXdf(df_cx,date)



    for i in list_wx:
        # print(i)
        success = client.write_points(points=i)

    for i in list_rx:
        success = client.write_points(points=i)

    for i in list_sm:
        success = client.write_points(points=i)
    
    for i in list_cx:
        success = client.write_points(points=i)
    
    print("Completed uploading files from:", date)
    deletesql(date)
    print("\nDeleted .sql files", date)

if __name__ == '__main__':
    main()

    

