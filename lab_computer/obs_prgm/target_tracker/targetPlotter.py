#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Dec 18 15:42:26 2023

@author: srikar
"""

import numpy as np
import pandas as pd
import ephem as ep
import datetime as dt
import matplotlib.pyplot as plt
from targetMethods import *
    
average = True

ngc = ep.FixedBody()
ngc.name = 'NGC 1068'
ngc._ra = '02:42:40.7091669408' 
ngc._dec = '-00:00:47.859690204'
ngc._epoch = '2000'
# Right Ascension and Declination taken from simbad

txs = ep.FixedBody()
txs.name = 'TXS 0506'
txs._ra = '05:09:25.9644373872'
txs._dec = '05:41:35.333820420'
txs._epoch = '2000'
# Right Ascension and Declination taken from simbad

trinity = ep.Observer()
trinity.elevation = 3048
trinity.lat = '38.5183333'
trinity.lon = '-113.285556'
heading = 280

ngc_alts = np.empty(0)
ngc_az = np.empty(0)
txs_alts = np.empty(0)
txs_az = np.empty(0)

#Asking user to input date of interest
date = dt.datetime(2024, 1, 19, 0, 0, 0)

#Calculating values for each minute in date of interest
for i in range(1440):
    dateNew = date + dt.timedelta(minutes = i)
    trinity.date = dateNew
    ngc.compute(trinity)
    txs.compute(trinity)
    ngc_alts = np.append(ngc_alts, degToDec(str(ngc.alt)))
    ngc_az = np.append(ngc_az, degToDec(str(ngc.az)))
    txs_alts = np.append(txs_alts, degToDec(str(txs.alt)))
    txs_az = np.append(txs_az, degToDec(str(txs.az)))
    
if average:
    ngc_avgAlt = []
    txs_avgAlt = []
    for i in range(160):
        start = i*9
        end = (i+1)*9-1
        for x in range(4): ngc_avgAlt.append(None)
        ngc_avgAlt.append(sum(ngc_alts[start:end])/9)
        for x in range(4): ngc_avgAlt.append(None)
        for x in range(4): txs_avgAlt.append(None)
        txs_avgAlt.append(sum(txs_alts[start:end])/9)
        for x in range(4): txs_avgAlt.append(None)
        
    
#Plotting Data
fig, axs = plt.subplots(2, 2, figsize = (20, 10))
fig.suptitle('Targets of Opportunity on ' + str(date.date()))

#Reformatting Azimuth data
ngc_az = np.unwrap(ngc_az, period = 360) - heading
ngc_az = (ngc_az + 180) % (2 * 180) - 180
txs_az = np.unwrap(txs_az, period = 360) - heading
txs_az = (txs_az + 180) % (2 * 180) - 180

#X-axis Ticks
labels = np.linspace(0, 24, num = 1441)
labels = labels[0:1440]

#Vertical Lines for Observation Window
window = getObservationWindow(date)
window[0] = window[0].replace(microsecond = 0)
window[1] = window[1].replace(microsecond = 0)
startLine = window[0].hour + window[0].minute/60
endLine = window[1].hour + window[1].minute/60

#Determine Points for Target Window
startNGC = np.argmin(np.abs(ngc_alts-0))
endNGC = startNGC + 48.3
#startNGCalt = np.argmin(np.abs(ngc_az-startNGC))
#endNGCalt = np.argmin(np.abs(ngc_az-endNGC))
startTXS = np.argmin(np.abs(txs_alts-0))
endTXS = startTXS + 49.2


##Plotting
if average:
    axs[0, 0].plot(labels, ngc_avgAlt, 'k', marker = '.', markersize = 1, linestyle = '')
else:
    axs[0, 0].plot(labels, ngc_alts, 'k', marker = '.', markersize = 0.2, linestyle = '')
axs[0, 0].axhline(y = 0, color = 'yellow')
axs[0, 0].axhline(y = -10, color = 'yellow')
axs[0, 0].axhspan(-10, 0, alpha = 0.4, color = 'yellow')
axs[0, 0].set_title('NGC 1068 Altitude')
axs[0, 0].set_xlabel('Hours from UTC Midnight')
axs[0, 0].set_ylabel('Altitude (º)')

axs[0, 1].plot(labels, ngc_az, 'k', linestyle = '', marker = '.', markersize = 0.2)
#axs[0, 1].axhline(y = ngc_az[int(startNGC)], color = 'yellow')
#axs[0, 1].axhline(y = ngc_az[int(endNGC)], color = 'yellow')
axs[0, 1].set_title('NGC 1068 Azimuth')
axs[0, 1].set_xlabel('Hours from UTC Midnight')
axs[0, 1].set_ylabel('Deviation from Trinity (º)')

if average:
    axs[1, 0].plot(labels, txs_avgAlt, 'k', linestyle = '', marker = '.', markersize = 1)
else:
    axs[1, 0].plot(labels, txs_alts, 'k', linestyle = '', marker = '.', markersize = 0.2)
axs[1, 0].axhline(y = 0, color = 'yellow')
axs[1, 0].axhline(y = -10, color = 'yellow')
axs[1, 0].axhspan(-10, 0, alpha = 0.4, color = 'yellow')
axs[1, 0].set_title('TXS 0506 Altitude')
axs[1, 0].set_xlabel('Hours from UTC Midnight')
axs[1, 0].set_ylabel('Altitude (º)')


axs[1, 1].plot(labels, txs_az, 'k', marker = '.', markersize = 0.2, linestyle = '')
axs[1, 1].set_title('TXS 0506 Azimuth')
#axs[1, 1].axhline(y = txs_az[int(startTXS)], color = 'yellow')
#axs[1, 1].axhline(y = txs_az[int(endTXS)], color = 'yellow')
axs[1, 1].set_xlabel('Hours from UTC Midnight')
axs[1, 1].set_ylabel('Deviation from Trinity (º)')

for ax in axs:
    for axx in ax:
        axx.grid(visible = True, color = 'k', linestyle = '-', alpha = 0.2)
        axx.axvline(startLine, color = 'black')
        axx.axvline(endLine, color = 'black')
        axx.axvspan(startLine, endLine, alpha = 0.4, color = 'gray', label = 'Observing Window')
        axx.text(startLine, axx.get_ylim()[0], str(window[0].time()), rotation = 35, ha = 'right', va = 'top', size = 8)
        axx.text(endLine, axx.get_ylim()[0], str(window[1].time()), rotation = 35, ha = 'right', va = 'top', size = 8)