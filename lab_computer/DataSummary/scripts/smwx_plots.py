import datetime
from influxdb import InfluxDBClient

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.dates as mdates
import matplotlib.colorbar as colorbar
import matplotlib.cm as cm
import pandas as pd
import json
import numpy as np
import datetime as datetime
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import argparse
from scipy import stats

def WXquery(client,date,measurement):
    client.switch_database("dbWX")
    # Create a query to fetch all data for the specific day
    result = client.query(f'SELECT "{measurement}" FROM "{date}" ORDER BY time DESC') 
    print(f'SELECT "{measurement}" FROM "{date}" ORDER BY time DESC')
    #print(result)
    times = []
    temps = []
    
    for point in result.get_points():
        times.append(point['time'])
        temps.append(point[measurement])
    
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
    result = client.query(f"SELECT * FROM \"HV_currents\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T18:00:00Z\'")

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
    
def plot_SIABCurrents(client,date):
    client.switch_database("dbSM")
    dateV2, dateBack = convert_date_format(date)
    result = client.query(f"SELECT * FROM \"SIABcurrent\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T18:00:00Z\'")

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

def plot_sipmTemps(client,date):
    client.switch_database("dbSM")
    dateV2, dateBack = convert_date_format(date)
    result = client.query(f"SELECT * FROM \"SiPM_temps\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T18:00:00Z\'")

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
    result = client.query(f"SELECT * FROM \"UC_temps\" WHERE time >= \'{dateBack}T18:00:00Z\' AND time < \'{dateV2}T18:00:00Z\'")

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

def remove_outliers_zscore(data, threshold=3):
    """
    Remove outliers using the z-score method.
    
    Parameters:
    data (array-like): Input array
    threshold (float): Z-score threshold (default=3)
    
    Returns:
    numpy.ndarray: Array with outliers removed
    numpy.ndarray: Boolean mask of non-outlier values
    """
    data = np.array(data)
    z_scores = np.abs(stats.zscore(data))
    mask = z_scores < threshold
    
    return data[mask], mask

def get_dynamic_limits(data, n_std=5):
    """
    Calculate dynamic plot limits excluding outliers based on standard deviations.
    
    Parameters:
    data (array-like): Input data
    n_std (float): Number of standard deviations to consider for outlier detection
    
    Returns:
    tuple: (min_limit, max_limit)
    """
    data, _ = remove_outliers_zscore(data)
    data = np.array(data)
    mean = np.mean(data)
    std = np.std(data)
    print(mean,std)
    # Calculate limits
    lower_bound = mean - n_std * std
    upper_bound = mean + n_std * std
    
    # Filter data within bounds
    filtered_data = data[(data >= lower_bound) & (data <= upper_bound)]
    
    # Add some padding (10%) to the limits for better visualization
    data_range = np.ptp(filtered_data)
    padding = 0.2 * data_range
    
    return (np.min(filtered_data) - padding, 
            np.max(filtered_data) + padding)
    
def plot2PDF(client,qdate):
    
    # Set up the PDF file
    pdf_file = f"/data/TrinityLabComputer/DataSummary/Otherfiles/output{qdate}.pdf"
    pp = PdfPages(pdf_file)

    # Plot each measurement against time, 8 per page
    fig, axes = plt.subplots(2, 2, figsize=(11, 8.5))  # 4 rows, 2 columns per page
    axes = axes.flatten()  # Flatten the array of axes
    
    dfWX=WXquery(client,qdate,"Temperature")
    dfWX1=WXquery(client,qdate,"Dewpoint")
    #print(dfWX)
    #print(dfWX1)
    # weather plot
    i = 0
    ax = axes[i % 4]
    ax.plot(dfWX['time'], dfWX["temp"].astype(float),color='black', label = 'Outside Tempature')
    ax.set_title(f'Tempature over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("Degrees C")
    y_min, y_max = get_dynamic_limits(np.array(dfWX["temp"].astype(float)))
    
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    ax.plot(dfWX1['time'], dfWX1["temp"].astype(float),color='blue', label = 'Dewpoint',alpha = 0.7)

    # ax2= ax.twinx()
    # ax2.plot(dfWX1['time'], dfWX1["temp"].astype(float),color='blue', label = 'Dewpoint',alpha = 0.7)
    # ax2.set_ylabel(f'Dewpoint Degrees C', color = 'blue')
    # ax2.tick_params(axis='y', colors='blue')
    y_min1, y_max1 = get_dynamic_limits(np.array(dfWX1["temp"].astype(float)))
    # ax2.set_ylim(y_min1, y_max1)
    if y_min1 < y_min: 
        y_min = y_min1
    if y_max1 > y_max: 
        y_max = y_max1
        
    ax.set_ylim(y_min, y_max)
    ax.legend()
    

    # plot 2
    sc=plot_sipmTemps(client,qdate)
    i = 1
    ax = axes[i % 4]
    for i in range(1, 17):
        column_name = f"sc{i}"
        ax.plot(sc['time'], sc[column_name].astype(float), label=str(i))
    # ax.plot(sc['time'], sc["sc1"].astype(float), label= '1')
    # ax.plot(sc['time'], sc["sc2"].astype(float), label= '2')
    # ax.plot(sc['time'], sc["sc3"].astype(float), label= '3')
    # ax.plot(sc['time'], sc["sc4"].astype(float), label= '4')
    # ax.plot(sc['time'], sc["sc5"].astype(float), label= '5')
    # ax.plot(sc['time'], sc["sc6"].astype(float), label= '6')
    # ax.plot(sc['time'], sc["sc7"].astype(float), label= '7')
    # ax.plot(sc['time'], sc["sc8"].astype(float), label= '8')
    # ax.plot(sc['time'], sc["sc9"].astype(float), label= '9')
    # ax.plot(sc['time'], sc["sc10"].astype(float), label= '10')
    # ax.plot(sc['time'], sc["sc11"].astype(float), label= '11')
    # ax.plot(sc['time'], sc["sc12"].astype(float), label= '12')
    # ax.plot(sc['time'], sc["sc13"].astype(float), label= '13')
    # ax.plot(sc['time'], sc["sc14"].astype(float), label= '14')
    # ax.plot(sc['time'], sc["sc15"].astype(float), label= '15')
    # ax.plot(sc['time'], sc["sc16"].astype(float), label= '16')


    ax.set_title(f'SiPM temps over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("Degrees C")
    # Set the limits for x-axis and y-axis
    y_max = -1000
    y_min = 1000
    
    for i in range(1, 17):
        column_name = f"sc{i}"
        iy_min, iy_max = get_dynamic_limits(np.array(sc[column_name].astype(float)))
        if y_min > iy_min:
            y_min = iy_min
        if y_max < iy_max:
            y_max = iy_max
        
    ax.set_ylim(y_min, y_max)
    
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    # plot 2
    sc=plot_uc_temps(client,qdate)
    i = 2
    ax = axes[i % 4]
    for i in range(1, 17):
        column_name = f"sc{i}"
        ax.plot(sc['time'], sc[column_name].astype(float), label=str(i))


    ax.set_title(f'UC temps over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("Degrees C")
    # Set the limits for x-axis and y-axis
    y_max = -1000
    y_min = 1000
    
    for i in range(1, 17):
        column_name = f"sc{i}"
        iy_min, iy_max = get_dynamic_limits(np.array(sc[column_name].astype(float)))
        if y_min > iy_min:
            y_min = iy_min
        if y_max < iy_max:
            y_max = iy_max
        
    ax.set_ylim(y_min, y_max)
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()
    
    # hv currents plot
    hvc=plot_hv_currents(client,qdate)

    i = 3
    ax = axes[i % 4]
    ax.plot(hvc['time'], hvc["hv1"].astype(float), label= 'hv1')
    ax.plot(hvc['time'], hvc["hv2"].astype(float), label= 'hv2')
    ax.plot(hvc['time'], hvc["hv3"].astype(float), label= 'hv3')
    ax.plot(hvc['time'], hvc["hv4"].astype(float), label= 'hv4')
    ax.set_title(f'SiPM Currents over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("mA")
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    # plot 5
    plt.tight_layout()
    pp.savefig(fig)
    plt.close(fig)


    fig, axes = plt.subplots(2, 2, figsize=(11, 8.5))  # Create a new figure
    axes = axes.flatten()  # Flatten the array of axes
    sc=plot_SIABCurrents(client,qdate)
    i = 4
    ax = axes[i % 4]
    for i in range(1, 17):
        column_name = f"sc{i}"
        ax.plot(sc['time'], sc[column_name].astype(float), label=str(i))


    ax.set_title(f'SIAB Currents over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("mA")
    # Set the limits for x-axis and y-axis
    y_max = -1000
    y_min = 1000
    
    for i in range(1, 17):
        column_name = f"sc{i}"
        iy_min, iy_max = get_dynamic_limits(np.array(sc[column_name].astype(float)))
        if y_min > iy_min:
            y_min = iy_min
        if y_max < iy_max:
            y_max = iy_max
        
    ax.set_ylim(y_min-2, y_max+2)
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50,color = 'k')
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    dfWX=WXquery(client,qdate,"Relative_Humidity")
    print(dfWX)
    # weather plot
    i = 5
    ax = axes[i % 4]
    ax.plot(dfWX['time'], dfWX["temp"].astype(float),color='blue')
    ax.set_title(f'Relative Humidity over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("%")
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50, color = 'k')
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()

    dfWX=WXquery(client,qdate,"AverageRelativeWindSpeed")
    dfWXGust=WXquery(client,qdate,"RelativeGustSpeed")
    print(dfWX)
    # weather plot
    i = 6
    ax = axes[i % 4]
    ax.plot(dfWX['time'], dfWX["temp"].astype(float),color ='black', label='Wind Speed')
    ax.plot(dfWXGust['time'], dfWXGust["temp"].astype(float), color='blue', label='Gust Speed')

    ax.set_title(f'Wind and Gusts over time {qdate}')
    ax.set_xlabel("Time (UTC)")
    ax.set_ylabel("Knots")
    
    ax.set_xticklabels(ax.get_xticks(), rotation = 50, color ='k')
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    ax.legend()


    dfWXd=WXquery(client,qdate,"AverageRelativeWindDirection")
    print(dfWXd)
    # weather plot
    i = 7
    axes[3].remove()

    # Convert the datetime object to a numeric timestamp for color mapping
    time_numeric = mdates.date2num(dfWXd['time'])

    # Convert wind direction to radians (0 at North, clockwise)
    wind_dir_rad = np.deg2rad(270 - dfWXd["temp"].astype(float))

    # Use wind speed as the radius
    wind_speed = dfWX["temp"].astype(float)

    ax = plt.subplot(224,polar=True,frameon=False)
    sc=ax.scatter(wind_dir_rad, wind_speed, c=time_numeric, cmap='viridis', s=5)
    ax.set_theta_zero_location('N')  # Set 0 degrees at North
    ax.set_theta_direction(-1)  # Clockwise rotation

    # Add a color bar with time labels
    cbar = fig.colorbar(sc, ax=ax, orientation='vertical', pad=0.1)
    cbar.set_label('Time')
    even_hours = mdates.HourLocator(interval=2)
    hour_formatter = mdates.DateFormatter('%H:%M')
    cbar.ax.yaxis.set_major_locator(even_hours)
    cbar.ax.yaxis.set_major_formatter(hour_formatter)
    ax.set_title(f'AverageRelativeWindDirection over time {qdate}')
    # Optional: Add directional labels
    directions = ['N', 'NE', 'E', 'SE', 'S', 'SW', 'W', 'NW']
    ax.set_thetagrids(np.linspace(0, 360, 8, endpoint=False), directions)


    # Old wind direction plot
    # ax = axes[i % 4]
    # ax.scatter(dfWXd["time"],dfWX["temp"].astype(float),c ='b', s=5)
    # ax.set_xlabel("Time (UTC)")
    # ax.set_ylabel("Degrees")
    
    # ax.set_xticklabels(ax.get_xticks(), rotation = 50)
    # ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

    # ax.legend()


    # Save any remaining plots in the last page
    #if (i + 1) % 8 != 0:
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
