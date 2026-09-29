import datetime
from influxdb import InfluxDBClient

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd
import json
import numpy as np
import datetime as datetime
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import argparse

def WXquery(client,date):

    # Create a query to fetch all data for the specific day
    result = client.query(f'SELECT "Temperature" FROM "{date}" ORDER BY time DESC') 
    #print(result)
    times = []
    temps = []
    
    for point in result.get_points():
        times.append(point['time'])
        temps.append(point['Temperature'])
    
    # Create a NumPy array from the collected data
    data = np.array([times, temps]).T  # Transpose to match (n, 2) shape
    
    # Create a DataFrame from the array
    df = pd.DataFrame(data, columns=['time', 'temp'])
    # Convert the 'time' column to datetime
    df['time'] = pd.to_datetime(df['time'])
    
    # Remove the timezone information
    #df['time'] = df['time'].dt.tz_localize(None)
    #print(df)
    return df
    
def convert_date_format(date):
    dateV2 = f"{date[:4]}-{date[4:6]}-{date[6:]}"
    # Convert the date string to a datetime object
    date_obj = datetime.strptime(dateV2, "%Y-%m-%d")

    # Subtract one day
    new_date_obj = date_obj - timedelta(days=1)

    # Convert the new date object back to a string in "YYYY-MM-DD" format
    dateBack = new_date_obj.strftime("%Y-%m-%d")
    return dateV2, dateBack


def plot_hv_currents(client,date):
    client.switch_database("dbSM")

    
    dateV2, dateBack = convert_date_format(date)
    #print(dateV2,dateBack)
    result = client.query(f"SELECT * FROM \"HV_currents\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T09:00:00Z\'")

    #print(result)
    times = []
    hv1 = []
    hv2 = []
    hv3 = []
    hv4 = []
    
    for point in result.get_points():
        times.append(point['time'])
        hv1.append(point['1'])
        hv2.append(point['2'])
        hv3.append(point['3'])
        hv4.append(point['4'])
    
    # Create a NumPy array from the collected data
    data = np.array([times, hv1,hv2,hv3,hv4]).T  # Transpose to match (n, 2) shape
    #print(data)
    
    # Create a DataFrame from the array
    df = pd.DataFrame(data, columns=['time', 'hv1', 'hv2','hv3','hv4'])
    # Convert the 'time' column to datetime
    df['time'] = pd.to_datetime(df['time'])
    
    # Remove the timezone information
    #df['time'] = df['time'].dt.tz_localize(None)
    #print(df)
    return df

def plot_sipmTemps(client,date):
    client.switch_database("dbSM")
    dateV2, dateBack = convert_date_format(date)
    result = client.query(f"SELECT * FROM \"SiPM_temps\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T09:00:00Z\'")

    #print(result)
    times = []
    sc1 = []
    sc2 = []
    sc3 = []
    sc4 = []
    sc5 = []
    sc6 = []
    sc7 = []
    sc8 = []
    sc9 = []
    sc10 = []
    sc11 = []
    sc12 = []
    sc13 = []
    sc14= []
    sc15 = []
    sc16 = []

    for point in result.get_points():
        times.append(point['time'])
        
        sc1.append(point['1'])
        sc2.append(point['2'])
        sc3.append(point['3'])
        sc4.append(point['4'])
        sc5.append(point['5'])
        sc6.append(point['6'])
        sc7.append(point['7'])
        sc8.append(point['8'])
        sc9.append(point['9'])
        sc10.append(point['10'])
        sc11.append(point['11'])
        sc12.append(point['12'])
        sc13.append(point['13'])
        sc14.append(point['14'])
        sc15.append(point['15'])
        sc16.append(point['16'])
    
    # Create a NumPy array from the collected data
    data = np.array([times, sc1, sc2, sc3, sc4, sc5, sc6, sc7, sc8, sc9, sc10, sc11, sc12, sc13, sc14, sc15, sc16]).T  # Transpose to match (n, 2) shape
    #print(data)
    
    # Create a DataFrame from the array
    df = pd.DataFrame(data, columns=['time', 'sc1', 'sc2', 'sc3', 'sc4', 'sc5', 'sc6', 'sc7', 'sc8', 'sc9', 'sc10', 'sc11', 'sc12', 'sc13', 'sc14', 'sc15', 'sc16'])
    # Convert the 'time' column to datetime
    df['time'] = pd.to_datetime(df['time'])
    
    # Remove the timezone information
    #df['time'] = df['time'].dt.tz_localize(None)
    #print(df)
    return df

def plot_uc_temps(client,date):
    client.switch_database("dbSM")

    dateV2, dateBack = convert_date_format(date)
    result = client.query(f"SELECT * FROM \"UC_temps\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T09:00:00Z\'")

    #print(result)
    times = []
    sc1 = []
    sc2 = []
    sc3 = []
    sc4 = []
    sc5 = []
    sc6 = []
    sc7 = []
    sc8 = []
    sc9 = []
    sc10 = []
    sc11 = []
    sc12 = []
    sc13 = []
    sc14= []
    sc15 = []
    sc16 = []

    for point in result.get_points():
        times.append(point['time'])
        
        sc1.append(point['1'])
        sc2.append(point['2'])
        sc3.append(point['3'])
        sc4.append(point['4'])
        sc5.append(point['5'])
        sc6.append(point['6'])
        sc7.append(point['7'])
        sc8.append(point['8'])
        sc9.append(point['9'])
        sc10.append(point['10'])
        sc11.append(point['11'])
        sc12.append(point['12'])
        sc13.append(point['13'])
        sc14.append(point['14'])
        sc15.append(point['15'])
        sc16.append(point['16'])
    
    # Create a NumPy array from the collected data
    data = np.array([times, sc1, sc2, sc3, sc4, sc5, sc6, sc7, sc8, sc9, sc10, sc11, sc12, sc13, sc14, sc15, sc16]).T  # Transpose to match (n, 2) shape
    #print(data)
    
    # Create a DataFrame from the array
    df = pd.DataFrame(data, columns=['time', 'sc1', 'sc2', 'sc3', 'sc4', 'sc5', 'sc6', 'sc7', 'sc8', 'sc9', 'sc10', 'sc11', 'sc12', 'sc13', 'sc14', 'sc15', 'sc16'])
    # Convert the 'time' column to datetime
    df['time'] = pd.to_datetime(df['time'])
    
    # Remove the timezone information
    #df['time'] = df['time'].dt.tz_localize(None)
    #print(df)
    return df
    
def plot2PDF(client,qdate):
    
    # Set up the PDF file
    pdf_file = f"/data/TrinityLabComputer/DataSummary/Otherfiles/output{qdate}.pdf"
    pp = PdfPages(pdf_file)

    # Plot each measurement against time, 8 per page
    fig, axes = plt.subplots(2, 2, figsize=(11, 8.5))  # 4 rows, 2 columns per page
    axes = axes.flatten()  # Flatten the array of axes
    
    dfWX=WXquery(client,qdate)
    # weather plot
    i = 0
    ax = axes[i % 4]
    ax.plot(dfWX['time'], dfWX["temp"].astype(float))
    ax.set_title(f'Outside Temp over time {qdate}')
    ax.set_xlabel("Time")
    ax.set_ylabel("degrees")
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    # hv currents plot
    hvc=plot_hv_currents(client,qdate)

    i = 1
    ax = axes[i % 4]
    ax.plot(hvc['time'], hvc["hv1"].astype(float), label= 'hv1')
    ax.plot(hvc['time'], hvc["hv2"].astype(float), label= 'hv2')
    ax.plot(hvc['time'], hvc["hv3"].astype(float), label= 'hv3')
    ax.plot(hvc['time'], hvc["hv4"].astype(float), label= 'hv4')
    ax.set_title(f'Hv currents over time {qdate}')
    ax.set_xlabel("Time")
    ax.set_ylabel("amps")
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    # plot 2
    sc=plot_sipmTemps(client,qdate)
    i = 2
    ax = axes[i % 4]
    ax.plot(sc['time'], sc["sc1"].astype(float), label= '1')
    ax.plot(sc['time'], sc["sc2"].astype(float), label= '2')
    ax.plot(sc['time'], sc["sc3"].astype(float), label= '3')
    ax.plot(sc['time'], sc["sc4"].astype(float), label= '4')
    ax.plot(sc['time'], sc["sc5"].astype(float), label= '5')
    ax.plot(sc['time'], sc["sc6"].astype(float), label= '6')
    ax.plot(sc['time'], sc["sc7"].astype(float), label= '7')
    ax.plot(sc['time'], sc["sc8"].astype(float), label= '8')
    ax.plot(sc['time'], sc["sc9"].astype(float), label= '9')
    ax.plot(sc['time'], sc["sc10"].astype(float), label= '10')
    ax.plot(sc['time'], sc["sc11"].astype(float), label= '11')
    ax.plot(sc['time'], sc["sc12"].astype(float), label= '12')
    ax.plot(sc['time'], sc["sc13"].astype(float), label= '13')
    ax.plot(sc['time'], sc["sc14"].astype(float), label= '14')
    ax.plot(sc['time'], sc["sc15"].astype(float), label= '15')
    ax.plot(sc['time'], sc["sc16"].astype(float), label= '16')


    ax.set_title(f'sipm temps over time {qdate}')
    ax.set_xlabel("Time")
    ax.set_ylabel("degrees")
    # Set the limits for x-axis and y-axis
    ax.set_ylim(-20, 30)  # Limit y-axis from 10 to 25
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    # plot 2
    sc=plot_uc_temps(client,qdate)
    i = 3
    ax = axes[i % 4]
    ax.plot(sc['time'], sc["sc1"].astype(float), label= '1')
    ax.plot(sc['time'], sc["sc2"].astype(float), label= '2')
    ax.plot(sc['time'], sc["sc3"].astype(float), label= '3')
    ax.plot(sc['time'], sc["sc4"].astype(float), label= '4')
    ax.plot(sc['time'], sc["sc5"].astype(float), label= '5')
    ax.plot(sc['time'], sc["sc6"].astype(float), label= '6')
    ax.plot(sc['time'], sc["sc7"].astype(float), label= '7')
    ax.plot(sc['time'], sc["sc8"].astype(float), label= '8')
    ax.plot(sc['time'], sc["sc9"].astype(float), label= '9')
    ax.plot(sc['time'], sc["sc10"].astype(float), label= '10')
    ax.plot(sc['time'], sc["sc11"].astype(float), label= '11')
    ax.plot(sc['time'], sc["sc12"].astype(float), label= '12')
    ax.plot(sc['time'], sc["sc13"].astype(float), label= '13')
    ax.plot(sc['time'], sc["sc14"].astype(float), label= '14')
    ax.plot(sc['time'], sc["sc15"].astype(float), label= '15')
    ax.plot(sc['time'], sc["sc16"].astype(float), label= '16')


    ax.set_title(f'uc temps over time {qdate}')
    ax.set_xlabel("Time")
    ax.set_ylabel("degrees")
    ax.set_ylim(-20, 30)  # Limit y-axis from 10 to 25
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()


    # # If we have 8 plots, save the page and start a new one
    # if (i + 1) % 8 == 0:
    #     plt.tight_layout()
    #     pp.savefig(fig)
    #     plt.close(fig)
    #     fig, axes = plt.subplots(4, 2, figsize=(11, 8.5))  # Create a new figure
    #     axes = axes.flatten()  # Flatten the array of axes

    # Save any remaining plots in the last page
    if (i + 1) % 8 != 0:
        plt.tight_layout()
        pp.savefig(fig)
        plt.close(fig)

    # Close the PDF file
    pp.close()

    # Close the client
    client.close()

    print(f"Plots have been saved to {pdf_file}")
  

def parse_arguments():
    """
    Parse command-line arguments.

    Returns:
        argparse.Namespace: An object containing the parsed arguments.
    """
    parser = argparse.ArgumentParser(description="makes plots of databases WX and SM")

    # Add arguments
    parser.add_argument('-d', '--date', type=str, help='Date in YYYYMMDD format')

    # Parse the command-line arguments
    args = parser.parse_args()

    return args
  
# Local host lines for access
host = 'localhost'
port = 8086
username = 'admin'
database = 'dbWX'
password = 'Ttys@210'

# Initialize the InfluxDB client and write the points in batches
client = InfluxDBClient(host = host, port=port, username=username, password=password)

# Switch to the database
client.switch_database(database)

# Parse command-line arguments
args = parse_arguments()

# Define the day you want to query (e.g., 2023-06-10)
query_date = args.date

plot2PDF(client,query_date)