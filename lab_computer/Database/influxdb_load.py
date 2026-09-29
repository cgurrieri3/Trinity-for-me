from influxdb import InfluxDBClient
import pandas as pd
import numpy as np
import glob
import os
import time
import datetime
import csv

# Local host lines for access
host = 'localhost'
port = 8086
username = 'mpotts32'
password = 'Ttys@210'
database = 'Trinity'

wd_path = '/home/mpotts32/weather/'

# Get the last file in the directory
#last_file = max(glob.glob(wd_path + '//*'), key=os.path.getctime)
#print('file', last_file)
#measurement = last_file[-8:]

# gets a single file
file_ = f'{wd_path}weather_20231108'
measurement = file_[-8:]
print('measurement',measurement)

def time_epoch_ms(df):
    print(df['DateTime'])
    result = df.index[~df['DateTime'].str.contains('T', na=False)].tolist()
    df = df.drop(result)
    print(df['DateTime'])

    print(result)
    df['DateTime'] = pd.to_datetime(df['DateTime'], format='%Y-%m-%dT%H:%M:%S.%f')
    #df['DateTime'] = df['DateTime'].astype('int64')
    #print(df['DateTime'])
    


# Read the file into a pandas DataFrame
header = [
    "Node", "RelativeWindDirection", "RelativeWindSpeed", "CorrectedWindDirection",
    "AverageRelativeWindDirection",
    "AverageRelativeWindSpeed", "RelativeGustDirection", "RelativeGustSpeed",
    "AverageCorrectedWindDirection",
    "WindSensorStatus", "Pressure", "Pressure_at_Sea_level", "Pressure_at_Station", "Relative_Humidity",
    "Temperature",
    "Dewpoint",
    "Absolute_Humidity", "compassHeading", "WindChill", "HeatIndex", "AirDensity", "WetBulbTempature",
    "SunRiseTime", "SolarNoonTime", "SunsetTime",
    "Position of the Sun", "Twilight(Civil)",
    "Twilight(Nautical)",
    "Twilight(Astronomical)", "X_Tilt", "Y_Tilt", "Z_Orientation", "User_Information_Field",
    "DateTime",
    "Supply_Voltage", "Status", "Checksum"]


# Counts the number of commas in a line. Some lines contian null values this removes them. 
# Paramters: the file and the number of col there should be
# Returns: All the lines that have the correct number of columns
def comma_count(file,num_col):
    valid_lines= []

    while True:
        try:
            row = next(file)
            # checks each line for the amount of commas and if its not correct does not append it
            if str(row).count(',') == num_col-1:
                    
                valid_lines.append(row)
        except csv.Error:
            continue
        except StopIteration:
            break

    return valid_lines


# this def removes all of the lines that are not the correct column length based on the correct number of commas
# takes: file path and the number of columns
# Returns: nothing (but it creates a file for the pandas data from to upload)
def remove_incomplete_lines(f_path, num_col):
    # opens the file
    with open(f_path, 'r') as file: 
        reader=csv.reader(file)
        
        #print(reader)
        
        valid_lines=comma_count(reader,num_col)
        

# puts them all to a new corrected file
    with open(f'{file_}c','w',newline='') as output_file:
        writer = csv.writer(output_file)
        writer.writerows(valid_lines)

       

# loads the data
#df = pd.read_csv(file_, names=header,low_memory=False)


    

#print(df)
# trys to get the data switched to nano seconds if not then the code will see if there are incomplete lines and delete them 
#try:
#    time_epoch_ms(df)

#except: 

# gets the ccompleted columns only
remove_incomplete_lines(file_, 37)
print('removing incomplete lines')
df = pd.read_csv(f'{file_}c', names=header,low_memory=False)
print(df['DateTime'])
os.remove(f'{file_}c')
time_epoch_ms(df[60550:60570])
print(df)
# this is needed so that each column can get the correct dtype
df['Node'].fillna(str(0),inplace= True)
df['RelativeWindDirection'].fillna(int(0),inplace= True)
df['RelativeWindSpeed'].fillna(float(0),inplace= True)
df['CorrectedWindDirection'].fillna(int(0),inplace= True)

df['AverageRelativeWindDirection'].fillna(int(0),inplace= True)

df['AverageRelativeWindSpeed'].fillna(int(0),inplace= True)
df['RelativeGustDirection'].fillna(int(0),inplace= True)
df['RelativeGustSpeed'].fillna(float(0),inplace= True)

df['AverageCorrectedWindDirection'].fillna(float(0),inplace= True)

df['WindSensorStatus'].fillna(int(0),inplace= True)
df['Pressure'].fillna(float(0),inplace= True)
df['Pressure_at_Sea_level'].fillna(float(0),inplace= True)
df['Pressure_at_Station'].fillna(float(0),inplace= True)
df['Relative_Humidity'].fillna(int(0),inplace= True)

df['Temperature'].fillna(float(0),inplace= True)
df['Dewpoint'].fillna(float(0),inplace= True)

df['Absolute_Humidity'].fillna(float(0),inplace= True)
df['compassHeading'].fillna(int(0),inplace= True)
df['WindChill'].fillna(float(0),inplace= True)
df['HeatIndex'].fillna(float(0),inplace= True)
df['AirDensity'].fillna(float(0),inplace= True)
df['WetBulbTempature'].fillna(float(0),inplace= True)

df['SunRiseTime'].fillna(str(0),inplace= True)
df['SolarNoonTime'].fillna(str(0),inplace= True)
df['SunsetTime'].fillna(str(0),inplace= True)

df['Position of the Sun'].fillna(str(0),inplace= True)
df['Twilight(Civil)'].fillna(str(0),inplace= True)


df['Twilight(Nautical)'].fillna(str(0),inplace= True)

df['Twilight(Astronomical)'].fillna(str(0),inplace= True)
df['X_Tilt'].fillna(int(0),inplace= True)
df['Y_Tilt'].fillna(int(0),inplace= True)
df['Z_Orientation'].fillna(int(0),inplace= True)
df['User_Information_Field'].fillna(float(0),inplace= True)


df['Supply_Voltage'].fillna(float(0),inplace= True)
df['Status'].fillna(int(0),inplace= True)
df['Checksum'].fillna(str(0),inplace= True)




#df.info() shows what the columns should be named



df.fillna(0.0,inplace=True)
#print(df['HeatIndex'])



#print(df['HeatIndex'])
# Create a list of data points from the DataFrame
data_points = []

for index, row in df.iterrows():
    data_point = {
        'measurement': measurement,
        'tags': {'location': 'Utah'},
        'time': row['DateTime'],
        'fields': {
            'Node': row['Node'],
            'RelativeWindDirection': row['RelativeWindDirection'],
            'RelativeWindSpeed': row['RelativeWindSpeed'],
            'CorrectedWindDirection': row['CorrectedWindDirection'],
            'AverageRelativeWindDirection': row['AverageRelativeWindDirection'],
            'AverageRelativeWindSpeed': row['AverageRelativeWindSpeed'],
            'RelativeGustDirection': row['RelativeGustDirection'],
            'RelativeGustSpeed': row['RelativeGustSpeed'],
            'AverageCorrectedWindDirection': row['AverageCorrectedWindDirection'],
            'WindSensorStatus': row['WindSensorStatus'],
            'Pressure': row['Pressure'],
            'Pressure_at_Sea_level': row['Pressure_at_Sea_level'],
            'Pressure_at_Station': row['Pressure_at_Station'],
            'Relative_Humidity': row['Relative_Humidity'],
            'Temperature': row['Temperature'],
            'Dewpoint': row['Dewpoint'],
            'Absolute_Humidity': row['Absolute_Humidity'],
            'WindChill': row['WindChill'], # these lines have no data and it messes everything up
            'HeatIndex': row['HeatIndex'],
            'compassHeading': row['compassHeading'],
            'AirDensity': row['AirDensity'],
            'WetBulbTempature': row['WetBulbTempature'],
            'SunRiseTime': row['SunRiseTime'],
            'SolarNoonTime': row['SolarNoonTime'],
            'SunsetTime': row['SunsetTime'],
            'Position of the Sun': row['Position of the Sun'],
            'Twilight(Civil)': row['Twilight(Civil)'],
            'Twilight(Nautical)': row['Twilight(Nautical)'],
            'Twilight(Astronomical)': row['Twilight(Astronomical)'],
            'X_Tilt': row['X_Tilt'],
            'Y_Tilt': row['Y_Tilt'],
            'Z_Orientation': row['Z_Orientation'],
            'User_Information_Field': row['User_Information_Field'],
            'Supply_Voltage': row['Supply_Voltage'],
            'Status': row['Status'],
            'Checksum': row['Checksum']
       }
    }
    data_points.append(data_point)


# Initialize the InfluxDB client and write the points in batches
client = InfluxDBClient(host = host, port=port, username=username, password=password)



# Create a new database if it does not already exist
# client.create_database(database)

# Switch to the newly created database
client.switch_database(database)
query = f'DROP MEASUREMENT "{measurement}"'
client.query(query)

batch_size = 5000
t0 = time.time()

for i in range(0, len(data_points), batch_size):
    
    batch = data_points[i:i+batch_size]
    #print(batch)
    client.write_points(points=batch) # writes points to the database
    print(f'Time to upload {time.time()-t0}')

    
print(f"Uploaded {len(df)} points from {file_}. Complete. Return to the InfluxDB UI.")





















'''








'Temperature': row['Temperature'],
            'Dewpoint': row['Dewpoint'],
            'Absolute_Humidity': row['Absolute_Humidity'],
            'compassHeading': row['compassHeading'],
            'WindChill': row['WindChill'],
            'HeatIndex': row['HeatIndex'],
            'AirDensity': row['AirDensity'],
            'WetBulbTempature': row['WetBulbTempature'],
            'SunRiseTime': row['SunRiseTime'],
            'SolarNoonTime': row['SolarNoonTime'],
            'SunsetTime': row['SunsetTime'],
            'Position of the Sun': row['Position of the Sun'],
            'Twilight(Civil)': row['Twilight(Civil)']




import pandas as pd
from influxdb import InfluxDBClient

# Load CSV file into a pandas dataframe

df = pd.read_csv('/home/mpotts32/weather/weather_20230313')

print(df)

# Convert the DataFrame to a dictionary
data = df.to_dict(orient='records')

# Convert the dictionary to InfluxDB line protocol
lines = []
for item in data:
    tags = ','.join([f'{k}={v}' for k, v in item.items() if k != 'DateTime'])
    fields = ','.join([f'{k}={v}' for k, v in item.items() if k != 'DateTime' and not pd.isna(v)])
    line = f'{item["measurement"]},{tags} {fields} {int(item["DateTime"])}'
    lines.append(line)

# Connect to InfluxDB
client = InfluxDBClient(host='localhost', port=8086)

# Create a new database if it does not already exist
client.create_database('my_database')

# Switch to the newly created database
client.switch_database('my_database')

# Write data to InfluxDB
client.write('\n'.join(lines),measurement_name='20230313')

print('completed points write')

# Query the database for all data
result = client.query('SELECT * FROM "measurement_name"')

# Print the query result
print(result)
'''

