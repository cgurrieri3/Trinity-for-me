import datetime
from influxdb import InfluxDBClient

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.dates as mdates
import matplotlib.colorbar as colorbar
from matplotlib import colormaps
import matplotlib.cm as cm
import pandas as pd
import json
import numpy as np
import datetime as datetime
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import argparse
from scipy import stats
import csv

# Local host lines for access
host = 'localhost'
port = 8086
username = 'admin'
database = 'TDFiles'
password = 'Ttys@210'

# Initialize the InfluxDB client and write the points in batches
client = InfluxDBClient(host = host, port=port, username=username, password=password)

# Switch to the database
client.switch_database(database)

result = client.query(f'SELECT "Filename","ranking","OpMode" FROM "RankingInfo"')

Filename = []
Ranking = []
OpMode = []

for point in result.get_points():
    Filename.append(point['Filename'])
    Ranking.append(point['ranking'])
    OpMode.append(point['OpMode'])

data = np.array([Filename, Ranking, OpMode]).T

# Create a DataFrame from the array
dfRanking = pd.DataFrame(data, columns=['Filename', 'Ranking', 'OpMode'])

result = client.query(f'SELECT "hvValues1", "hvcurrents1", "Filename" FROM "DataInfo"')


hvValues1 = []
hvcurrents1 = []
Filename = []

for point in result.get_points():
    hvValues1.append(point['hvValues1'])
    hvcurrents1.append(point['hvcurrents1'])
    Filename.append(point['Filename'])

data = np.array([hvValues1, hvcurrents1, Filename]).T
# Create a DataFrame from the array
dfData = pd.DataFrame(data, columns=['hvValues1', 'hvcurrents1', 'Filename'])
result = client.query(f'SELECT "sunAltitude", "moonAltitude", "Filename" FROM "CelestialInfo"')

sunAltitude = []
moonAltitude = []
Filename = []
for point in result.get_points():
    sunAltitude.append(point['sunAltitude'])
    moonAltitude.append(point['moonAltitude'])
    Filename.append(point['Filename'])

data = np.array([sunAltitude, moonAltitude, Filename]).T
# Create a DataFrame from the array
dfCelestial = pd.DataFrame(data, columns=['sunAltitude', 'moonAltitude', 'Filename'])

# Merge the DataFrames on 'Filename'
dfMerged = pd.merge(dfRanking, dfData, on='Filename')
dfMerged = pd.merge(dfMerged, dfCelestial, on='Filename')

dfMerged['Ranking'] = pd.to_numeric(dfMerged['Ranking'], errors='coerce')
dfMerged['Ranking'] = dfMerged['Ranking'].fillna(0).astype(int) 
dfMerged['OpMode'] = pd.to_numeric(dfMerged['OpMode'], errors='coerce')
dfMerged['OpMode'] = dfMerged['OpMode'].fillna(0).astype(int) 
dfMerged['hvValues1'] = dfMerged['hvValues1'].astype(float)
dfMerged['hvcurrents1'] = dfMerged['hvcurrents1'].astype(float)
dfMerged['sunAltitude'] = dfMerged['sunAltitude'].astype(float)
dfMerged['moonAltitude'] = dfMerged['moonAltitude'].astype(float)

dfFiltered = dfMerged[(dfMerged['Ranking'] == 8)] # & (dfMerged['hvValues1'] >= 43.95) & (dfMerged['hvcurrents1'] >= dict_doorClosed[44.0]) & (dfMerged['hvcurrents1'] <= 12.0)]

# create a new column with the data from the filename column so that it is yyyy-mm-dd
dfFiltered['Date'] = (pd.to_datetime(dfFiltered['Filename'].str[12:22])).astype(str)
dfFiltered['Date'] = (dfFiltered['Date'].str.replace('-','')).astype(int)

dfFiltered = dfFiltered[dfFiltered['Date'] >= 20250101]
dfFiltered = dfFiltered[dfFiltered['Date'] <= 20251130]

dfFiltered['OpMode'] = dfMerged['OpMode']
opmode = dfFiltered['OpMode']

closed_hours = 0
other_hours = 0

for i in opmode:
    if i ==3:
        total_closed_data = dfFiltered[dfFiltered['OpMode'] == i]
        if len(total_closed_data) != 0:          
            closed_files = len(total_closed_data)
            closed_hours = (closed_files * 97) / 3600
        else:
            closed_hours = 0
    else:
        other_data = dfFiltered[dfFiltered['OpMode'] == i]
        if len(other_data) != 0:          
            other_files = len(other_data)
            other_hours = (other_files * 97) / 3600
        else:
            other_hours = 0

# create a txt file
with open('rank8DCFiles.txt', mode='w', newline='') as file:
    # write the header
    file.write("Trinity Demonstrator Database Output - Rank 8 Door Closed Files \n")
    file.write("")
    file.write(f"Period of time : {dfFiltered['Date'].min()} to {dfFiltered['Date'].max()} \n")
    file.write(f"Total Days: {len(dfFiltered['Date'].unique())} days \n")
    file.write(f"Total Files: {len(dfFiltered['Filename'].unique())} files \n")
    file.write(f"Total Hours Data: {(len(dfFiltered['Filename'].unique()) * 97) / 3600:.1f} hours \n")
    file.write(f"Door Closed Hours Data: {closed_hours:.1f} hours \n")
    file.write("\n")
    file.write("Date, Night Files, Hours Data \n")

    unique_dates = dfFiltered['Date'].unique()
    total_days = len(unique_dates)

    hours_data = (total_files * 97) / 3600

    for date in unique_dates:
        day_data = dfFiltered[dfFiltered['Date'] == date]
        night_files = len(day_data)
        night_hours_data = (night_files * 97) / 3600

        file.write(f"{date}, {night_files}, {night_hours_data:.2f}, {door_position} \n")

print("Rank 8 Door Closed Files Data written to rank8DCFiles.txt")

