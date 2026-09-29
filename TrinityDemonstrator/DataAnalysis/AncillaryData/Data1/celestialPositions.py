#!/usr/bin/env python
# coding: utf-8

# **Solar & Lunar Positions**

# In[4]:


import numpy as np
import math
import pandas as pd
from pprint import pprint
import ephem
import argparse

# Initialize parser
parser = argparse.ArgumentParser()

# Adding optional argument
parser.add_argument("-d", "--date", help = "YYYYMMDD")

# Read arguments from command line
args = parser.parse_args()

if args.date:
    print("Displaying Output as: % s" % args.date)

# In[5]:


# STARTING DATE FOR FILE CREATIONS AND FOLLOWING DAYS AFTER 
# start_date = "20240718"
start_date = args.date
num_days = 1500
lat = "38.5183"     # [deg]
long = "-113.2855"  # [deg]
amsl = 3048         # HEIGHT ABOVE MEAN SEA LEVEL [m]
azimuth = 280       #[deg]
altitude = -1.56       #[deg]
#In its normal observing position, the center of the telescope camera points 1.56◦ below the horizon.

# In[6]:


# CREATES DATE RANGE
dates = pd.date_range(start_date, periods = num_days).date

# SET BETWEEN 00:00-13:00 AND 22:00-23:59 
times = list(pd.date_range("00:00","23:59",freq="1min",tz="UTC").time)


# In[7]:


# INITALIZES PYEPHEM LOCATION
trinity = ephem.Observer()
trinity.lat = lat
trinity.lon = long


# In[8]:


# CREATES SUN AND MOON OBJECTS
m = ephem.Moon()
s = ephem.Sun()

zn_sun = []
hc_sun = []
hc_moon = []
zn_moon = []
i_moon = []
ra_camera = []
dec_camera = []


# In[9]:


# GETS POSITIONAL INFO AND CREATES CSV
i = 0
q = 0
while q < len(dates):
    print(dates[q])
    while i < len(times):
        
        # converts time and date to usable type for pyephem
        trinity.date = str(dates[q]) + ' ' + str(times[i])
        # computes values for objects
        m.compute(trinity)
        s.compute(trinity)

        # illumination moon 
        ill_moon = '{:.3f}'.format(m.moon_phase) 
        i_moon.append(ill_moon)

        # azimuth in degrees for moon 
        m_az = str(m.az).split(':')
        m_az = round(int(m_az[0]) + int(m_az[1])/60 + float(m_az[2])/3600,4)
        zn_moon.append('{:.2f}'.format(m_az))
        
        # altitude in degrees for moon     
   
        m_alt = m.alt / math.pi * 180.0
        hc_moon.append('{:.2f}'.format(m_alt))
        
        
        
        # azimuth in degrees for sun 
        s_az = str(s.az).split(':')
        s_az = round(int(s_az[0]) + int(s_az[1])/60 + float(s_az[2])/3600,4)
        zn_sun.append('{:.2f}'.format(s_az))
        
        # altitude in degrees for sun 
        s_alt = s.alt / math.pi * 180.0
        hc_sun.append('{:.2f}'.format(s_alt))

        # get ra and dec of camera center for each time. 
        ra, dec = trinity.radec_of(azimuth, altitude)
        ra_camera.append('{:.2f}'.format(ra))
        dec_camera.append('{:.2f}'.format(dec))
        # print(f"RA: {ra}, Dec: {dec}")
 
        i+=1
    
    #DATA FRAME AND CSV FILE WITH ALL INFO 
    df = pd.DataFrame(
        {
            "date": str(dates[q]).replace('-',''),   # YYYYMMDD
            "time": times,                           # HH:MM:SS
            "latitude": float(lat),                  # deg
            "longitude": float(long),                # deg
            "altitude": amsl,                        # m
            "azimuth": azimuth,                      # deg
            "sunAzimuth": zn_sun,                    # deg
            "sunAltitude": hc_sun,                   # deg
            "moonAzimuth": zn_moon,                  # deg
            "moonAltitude": hc_moon,                 # deg
            "moonIllumination": i_moon,              # %
            "RAinCam": ra_camera,                    # deg
            "DECinCam": dec_camera,                  # deg
        })    
    df.to_csv('celestialPositions{}.csv'.format(dates[q]).replace('-',''),index=False)
    
    # RESETS COUNTERS FOR NEXT DAY
    q+=1
    i=0
    zn_sun = []
    hc_sun = []
    hc_moon = []
    zn_moon = []
    i_moon = []
    ra_camera = []
    dec_camera = []
    


# In[ ]:




