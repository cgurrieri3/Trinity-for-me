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

def removeDates(f):
    #print(f)
    removeList = ['20240104',"20240105","20240109","20240110","20240111","20240122","20240130","20240131","20240201","20240202","20240203",'20240206', '20240207',
    '20240208', '20240209', '20240210', '20240211', '20240212', '20240213', '20240214', '20240215', '20240216', '20240217', '20240218', '20240219', '20240220', '20240221',
    '20240222', '20240223', '20240224', '20240225', '20240226', '20240227', '20240228', '20240229', '20240301', '20240302', '20240303', '20240304', '20240305', '20240306',
    '20240307', '20240308', '20240309', '20240313', '20240314', '20240315', '20240316', '20240317', '20240320', '20240321', '20240322', '20240323', '20240324', '20240325',
    '20240326', '20240327', '20240328', '20240329', '20240330', '20240331', '20240401', '20240402', '20240403', '20240404', '20240405', '20240406', '20240407', '20240408',
    '20240412', '20240418', '20240419', '20240420', '20240421', '20240422', '20240423', '20240424', '20240425', '20240426', '20240427', '20240504', '20240505', '20240506', '20240508', '20240511'
    '20240514', '20240515', '20240517', '20240519', '20240520', '20240521', '20240522', '20240523', '20240524', '20240525', '20240526', '20240527', '20240528', '20240529',
    '20240530', '20240601', '20240602', '20240603', '20240604', '20240605', '20240606', '20240607', '20240608', '20240612', '20240613', '20240614', '20240615', '20240616',  
    '20240618', '20240619', '20240620', '20240621', '20240622', '20240623', '20240624', '20240625', '20240626', '20240627', '20240628', '20240629', '20240630', '20240701',   
    '20240702', '20240703', '20240704', '20240705', '20240706', '20240707', '20240708', '20240709', '20240711', '20240712', '20240713', '20240714', '20240715', '20240716',   
    '20240717', '20240719', '20240720', '20240721', '20240722', '20240723', '20240724', '20240725', '20240726', '20240727', '20240728', '20240803', '20240810', '20240811',   
    '20240812', '20240813', '20240817', '20240818', '20240819', '20240820', '20240821', '20240822', '20240823', '20240824', '20240905', '20240910', '20240914', '20240915',
    '20240916', '20240917', '20240918', '20240919', '20240920', '20240921', '20240922', '20240923', '20240928', '20241002', '20241003', '20241004', '20241005', '20241006', 
    '20241007', '20241008', '20241009', '20241010', '20241011', '20241012', '20241013', '20241014', '20241015', '20241016', '20241017', '20241018', '20241019', '20241020',
    '20241021', '20241022', '20241023', '20241024', '20241025', '20241026', '20241027', '20241028', '20241029', '20241030', '20241031', '20241101', '20241102', '20241103',
    '20241104', '20241105', '20241106', '20241110', '20241111', '20241112', '20241113', '20241114', '20241115', '20241116', '20241117', '20241118', '20241126', '20241127', 
    '20241128', '20241204', '20241213', '20241214', '20241215', '20241216', '20241217', '20241218', '20241223', '20241225', '20241226', '20250108', '20250109', '20250110',
    '20250111', '20250112', '20250113', '20250114', '20250115', '20250116', '20250117', '20250119', '20250120', '20250121', '20250123', '20250124', '20250127', '20250207',
    '20250208', '20250209', '20250210', '20250211', '20250212', '20250213', '20250214', '20250215', '20250216', '20250217', '20250218', '20250304', '20250306', '20250307',
    '20250308', '20250309', '20250310',]

    for rm in removeList:

        if f'{rm}' in f:
            f.remove(f'{rm}')
        # else:
        #     print(f'Item "2024{rm}" not found in list.')

    #print(f)
    return f

def main():
    datapath = '/storage/hive/project/phy-otte/shared/Trinity/Data'
    runtime = 2 # Time for each run in minutes
    os.chdir(datapath)
    folderArray=removeDates(os.listdir())
    
    print(folderArray)
    totalMins = 0
    ngcMins = 0
    txsMins = 0

    folderArray = [item for item in folderArray if "_" not in item]
    folderArray = [item for item in folderArray if "-" not in item]
    folderArray = [item for item in folderArray if "s" not in item]
    folderArray = [item for item in folderArray if "2023" not in item]
    folderArray = [item for item in folderArray if "202401" not in item]
    folderArray = [item for item in folderArray if "202402" not in item]
    folderArray = [item for item in folderArray if "202403" not in item]
    folderArray = [item for item in folderArray if "202404" not in item]
    folderArray = [item for item in folderArray if "202405" not in item]
    folderArray = [item for item in folderArray if "202406" not in item]
    folderArray = [item for item in folderArray if "202407" not in item]
    folderArray = [item for item in folderArray if "202408" not in item]
    folderArray = [item for item in folderArray if "202409" not in item]
    # folderArray = [item for item in folderArray if "202410" not in item]
    # folderArray = [item for item in folderArray if "202411" not in item]
    # folderArray = [item for item in folderArray if "202412" not in item]
    print(folderArray)
    for folder in folderArray:
        if (os.path.isdir(folder)) & (folder[0] == '2'): #first few folders are only logs folders and aren't numbered according to YYYYMMDD
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

    
if __name__ == '__main__':
    totalTime, ngcTime, txsTime = main()
    print(f"Total Observing Time (mins): {totalTime}")
    print(f"Total NGC Observed Time (mins): {ngcTime:.2f}")
    print(f"Total TXS Observed Time (mins): {txsTime:.2f}") # The .xf rounds to x digits

