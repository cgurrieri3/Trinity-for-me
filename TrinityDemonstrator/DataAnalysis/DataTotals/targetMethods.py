#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jan 10 13:21:14 2024

@author: srikar
"""
import numpy as np
import datetime as dt
import ephem as ep

def getObservationWindow(date):
    window = calcObservationWindow(date)
    if window[0] < window[1] and (window[1]-window[0]).seconds/3600 < 13:
        return window
    else:
        return calcObservationWindowOld(date)

def calcObservationWindow(date):
    # Milford, Urah coordinates
    lat = '38.3760'
    lon = '-113.0132'
    
    # Trinity observer
    obs = ep.Observer()
    obs.lat = lat
    obs.long = lon
    obs.date = date
    
    moonset_time = (obs.next_setting(ep.Moon())).datetime()
    obs.horizon = '-12'
    astronomical_time = (obs.next_setting(ep.Sun())).datetime()
    obs.horizon = '0'
    moonrise_time = (obs.next_rising(ep.Moon())).datetime()
    obs.horizon = '-15'
    sunrise_time = (obs.next_rising(ep.Sun())).datetime()
    
    return [max([moonset_time, astronomical_time]), min([moonrise_time, sunrise_time])]

def calcObservationWindowOld(date):
    
    # Milford, Utah coordinates
    lat = '38.3760'
    lon = '-113.0132'
    
    # Create an observer object for Milford, Utah
    obs = ep.Observer()
    obs.lat = str(lat)
    obs.long = str(lon)
    
    # Set the time to the current date and time
    obs.date = date
    
    # Astronomical Twilight
    obs.horizon = '-12:00:00'
    astronomical_time = obs.next_setting(ep.Sun())

    # Compute the moonrise, moonset, and sunset times
    obs.horizon = '00:00:00'
    moonrise_time = obs.next_rising(ep.Moon())
    moonset_time = obs.next_setting(ep.Moon())
    sunrise_time = obs.next_rising(ep.Sun())
    sunset_time = obs.next_setting(ep.Sun())
    
    # Convert times to UTC
    date = date + dt.timedelta(hours = 0)
    moonrise_time_utc = moonrise_time.datetime()
    moonset_time_utc = moonset_time.datetime() + dt.timedelta(hours=-6)
    sunrise_time_utc = sunrise_time.datetime() + dt.timedelta(hours=-1.5)
    sunset_time_utc = sunset_time.datetime()
    astronomical_time_utc = astronomical_time.datetime()
    
    # Getting Cutoff time
    sun_moon_array = [moonrise_time_utc,moonset_time_utc,sunrise_time_utc,sunset_time_utc]
    no_sunset=sorted(sun_moon_array[0:3])
    cutoff_time = no_sunset[0]
    
    #print("Moonrise time (UTC):", moonrise_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
    #print("Moonset time -6 hours (UTC):", moonset_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
    #print("Sunrise time -1:30 hours (UTC):", sunrise_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
    #print("Sunset time (UTC):", sunset_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
    return [astronomical_time_utc, cutoff_time]

def intersectDT(args): #args will be in list format [start1, end1, start2, end2] such that the target window range first, then observation window second
    #Convert all ephem Dates to datetimes
    for i, val in enumerate(args):
        if str(type(val)) == "<class 'ephem.Date'>":
            args[i] = val.datetime()
            
    window1 = args[0:2]
    window2 = args[2:4]
    if window1[1]<window2[0] or window1[0]>window2[1]:
        intersection = [dt.datetime(1882, 3, 23, 0, 0, 0), dt.datetime(1882, 3, 23, 0, 0, 0)]
    else:
        intersection = [max(window1[0], window2[0]), min(window1[1], window2[1])]
    return intersection

def degToDec(angle):
    vals = angle.split(':')
    return float(vals[0]) + float(vals[1])/60 + float(vals[2])/3600

def main():
    calcObservationWindow(dt.date.today())

if __name__ == '__main__':
    main()