#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 29 02:12:36 2024

@author: srikar
"""
import datetime as dt
import numpy as np
from targetTracker import *
import os
import argparse
import pandas


def parse_arguments():
    # Create ArgumentParser object
    parser = argparse.ArgumentParser(description='Get data recorded for a single night')

    # Add arguments
    parser.add_argument('-d', '--date', help='Date you request(YYYYMMDD)', required=True)

    # Parse arguments from the command line
    args = parser.parse_args()
    return args

def get_data_time(folder):
    datapath = '/mnt/Data'
    runtime = 2 # Time for each run in minutes
    os.chdir(datapath)
    
    totalMins = 0
    ngcMins = 0
    txsMins = 0
    
    os.chdir(f"{datapath}/{folder}/RawDataMerged")
    folder = folder[0:8] # The [0:8] is only there because I used fake in the folder name originally, adjust if fake data folders have dif names
    date = dt.datetime.strptime(folder, '%Y%m%d') 
    ngcWindow, txsWindow = trackTarget(date, date + dt.timedelta(days = 1))
    #print(f"{date} {ngcWindow} {txsWindow}") # Debugging to see if windows are calculated correctly


    
    for file in os.listdir():
        stats = os.stat(f"{file}") #Gets file information
        vals = (file.split('.')[0]+file.split('.')[1]).split('_') #Filters out the .root part, then separates the datetime and counter
        # Time for total minutes observed
        if (vals[-1] == '0000') & (stats.st_size/(1024*1024) >= 1): #Ensures that the file isn't corrupted (ending in 000x) and it has meaningful data (size >1 MB)
            totalMins += runtime
        
            # Time for sources observing time
            fileWindow = [dt.datetime.strptime(file[12:31], '%Y-%m-%dT%H:%M:%S'), dt.datetime.strptime(file[12:31], '%Y-%m-%dT%H:%M:%S')+dt.timedelta(minutes = 2)]
            #print(fileWindow) # Debugging to see if windows are calculated correctly from file name
            ngcOppWindow = intersectDT([ngcWindow[0], ngcWindow[1], fileWindow[0], fileWindow[1]])
            txsOppWindow = intersectDT([txsWindow[0], txsWindow[1], fileWindow[0], fileWindow[1]])
            ngcMins += ((ngcOppWindow[1]-ngcOppWindow[0]).total_seconds())/60
            txsMins += ((txsOppWindow[1]-txsOppWindow[0]).total_seconds())/60
    os.chdir(datapath)
    return totalMins, ngcMins, txsMins

def main():
    args = parse_arguments()
    date = args.date
    totalTime, ngcTime, txsTime = get_data_time(date)
    print(f"Total Observing Time (mins): {totalTime}")
    print(f"Total NGC Observed Time (mins): {ngcTime:.2f}")
    print(f"Total TXS Observed Time (mins): {txsTime:.2f}") # The .xf rounds to x digits
    
    dict = {'name': ["Total","NGC", "TXS"],'minutes': [totalTime, ngcTime,txsTime]} 
    df = pd.DataFrame(dict)
    # saving the dataframe
    df.to_csv(f'/mnt/DataAnalysis/DataTotals/outputs/dataTotal_{date}.csv')
 
    print(df)
if __name__ == '__main__':
    main()
    

