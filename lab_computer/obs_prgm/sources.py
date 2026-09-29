#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Dec 11 18:35:54 2023

@author: srikar
"""

import numpy as np
import ephem as ep
import pandas as pd
import datetime as dt
#from targetMethods import *

#Creating list of days of interest:
#dayList = []

# startday = dt.date(2024, 1, 1)
# endday = dt.date(2024, 3, 31)
# numDays = (endday-startday).days
# for i in range(numDays):
#     dayList.append(startday+dt.timedelta(days = i))


def get_source_times():
    ngc = ep.FixedBody()
    ngc.name = 'NGC 1068'
    ngc._ra = '02:42:40.7091669408' #'40.6696278886761'
    ngc._dec = '-00:00:47.859690204'#'-0.0132960435544'
    ngc._epoch = '2000'
    # Right Ascension and Declination takedn from simbad

    txs = ep.FixedBody()
    txs.name = 'TXS 0506'
    txs._ra = '05:09:25.9644373872' #'077.3581918833887'
    txs._dec = '05:41:35.333820420' #'05.6931434428979'
    txs._epoch = '2000'
    # Right Ascension and Declination taken from simbad

    trinity = ep.Observer()
    trinity.elevation = 3048
    trinity.lat = '38.5183333'
    trinity.lon = '-113.285556'
    # Coordinate values taken from Windy: (N 38º 31' 6", W 113º 17' 8")

    ngc_strict = pd.DataFrame(data = {'Start Date': [], 'Start Time': [], 'End Date': [], 'End Time': [], 'Total Time (min)': []}) #unpadded chart goes from an altitude of 0 to -10 degrees strictly
    #ngc_strict = pd.DataFrame({('Target Window', 'Start Date'): [], ('Target Window', 'Start Time'): [], ('Target Window', 'End Date'): [], ('Target Window', 'End Time'): [], ('Target Window', 'Total Mins'): []})
    txs_strict = pd.DataFrame(data = {'Start Date': [], 'Start Time': [], 'End Date': [], 'End Time': [], 'Total Time (min)': []}) #unpadded chart goes from an altitude of 0 to -10 degrees strictly
    hor_strict = ['00:00:00', '-10:00:00'] #This value is IMPORTANT - defines the horizon for setting/rising

    # get current day 
    current_date = dt.datetime.utcnow().date()

    #for date in dayList:
    date = current_date
    ngc_targetWindow = []
    txs_targetWindow = []
    ngc_arr_strict = [date]
    txs_arr_strict = [date]
    for hors in hor_strict:
        trinity.horizon = hors
        ngc_setting = str(trinity.next_setting(ngc, start = date))
        ngc_targetWindow.append(trinity.next_setting(ngc, start = date))
        ngc_arr_strict.append(ngc_setting.split()[1])
        txs_setting = str(trinity.next_setting(txs, start = date))
        txs_targetWindow.append(trinity.next_setting(txs, start = date))
        txs_arr_strict.append(txs_setting.split()[1])

    ngc_starttime = dt.datetime.strptime(ngc_arr_strict[1], '%H:%M:%S')
    ngc_endtime = dt.datetime.strptime(ngc_arr_strict[2], '%H:%M:%S')
    ngc_arr_strict.append(float((ngc_endtime-ngc_starttime).total_seconds()/60))
    ngc_arr_strict.insert(2, date)
    ngc_strict.loc[len(ngc_strict.index)] = ngc_arr_strict

    txs_starttime = dt.datetime.strptime(txs_arr_strict[1], '%H:%M:%S')
    txs_endtime = dt.datetime.strptime(txs_arr_strict[2], '%H:%M:%S')
    txs_arr_strict.append(float((txs_endtime-txs_starttime).total_seconds()/60))
    txs_arr_strict.insert(2, date)
    txs_strict.loc[len(txs_strict.index)] = txs_arr_strict
    
    # print('--------------')
    # print(ngc_strict)
    # print('---------------')
    # print(txs_strict)
    # print('---------------')
    return ngc_strict, txs_strict
##Calculate total minutes
#ngc_cum = ngc_strict['Total Mins'].sum()
#ngc_cum = 0
#txs_cum = txs_strict[('Target Window', 'Total Mins')].sum()
#txs_cum = 0

##Allow User to Select Their Timezone
# options = {'UTC: ': '1', 'EST: ':'2', 'MST: ': '3'}
# print(options)
#tz = int(input('Enter the corresponding number for the desired timezone: '))

# if tz == 2:
#     deltaH = 5
# elif tz == 3:
#     deltaH = 7
#if tz != 1:
# for i in range(ngc_strict.shape[0]):
#     ngc_dateOne = dt.datetime.strptime(str(ngc_strict.iat[i, 0]) + ' ' + ngc_strict.iat[i, 1], '%Y-%m-%d %H:%M:%S')
#     ngc_dateTwo = dt.datetime.strptime(str(ngc_strict.iat[i, 0]) + ' ' + ngc_strict.iat[i, 3], '%Y-%m-%d %H:%M:%S')
#     ngc_dateOneNew = ngc_dateOne-dt.timedelta(hours = deltaH)
#     ngc_dateTwoNew = ngc_dateTwo-dt.timedelta(hours = deltaH)
#     ngc_strict.iat[i, 0] = ngc_dateOneNew.date()
#     ngc_strict.iat[i, 1] = ngc_dateOneNew.time()
#     ngc_strict.iat[i, 2] = ngc_dateTwoNew.date()
#     ngc_strict.iat[i, 3] = ngc_dateTwoNew.time()
    
#     txs_dateOne = dt.datetime.strptime(str(txs_strict.iat[i, 0]) + ' ' + txs_strict.iat[i, 1], '%Y-%m-%d %H:%M:%S')
#     txs_dateTwo = dt.datetime.strptime(str(txs_strict.iat[i, 0]) + ' ' + txs_strict.iat[i, 3], '%Y-%m-%d %H:%M:%S')
#     txs_dateOneNew = txs_dateOne-dt.timedelta(hours = deltaH)
#     txs_dateTwoNew = txs_dateTwo-dt.timedelta(hours = deltaH)
#     txs_strict.iat[i, 0] = txs_dateOneNew.date()
#     txs_strict.iat[i, 1] = txs_dateOneNew.time()
#     txs_strict.iat[i, 2] = txs_dateTwoNew.date()
#     txs_strict.iat[i, 3] = txs_dateTwoNew.time()
    
#print('Timezone successfully converted!')


##Outputting dataframes into files
# ngc_strict.to_csv('NGC 1068 Schedule.csv')
# txs_strict.to_csv('TXS 0506 Schedule.csv')

"""fh_ngc = open("NGC 1068 Schedule.txt", 'w')
ngc_asString = ngc_strict.to_string()
fh_ngc.write(ngc_asString)
fh_ngc.write('\n\nTotal Observing Minutes: ' + str(ngc_cum))
fh_ngc.close()

fh_txs = open("TXS 0506 Schedule.txt", 'w')
txs_asString = txs_strict.to_string()
fh_txs.write(txs_asString)
fh_txs.write('\n\nTotal Observing Minutes: ' + str(txs_cum))
fh_txs.close()"""