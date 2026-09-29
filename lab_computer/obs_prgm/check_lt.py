# generate the file
# auto set time - from SAM
# Maunel set time is the sam as sam unless changed by the user

# check to make sure the time makes sense
# get UTC time from lab computer
# get UTC time of SAM/Manuel time
# Sam will add buffer
import ephem
import pytz
import requests
from datetime import datetime, timezone, timedelta, time as time_obj
import pandas as pd
import time
import math
import numpy as np
import communicate as lets

latitude = 38.5202  # Latitude of Frisco Peak
longitude = -113.2883  # Longitude Frisco Peak
elevation = 3048  # Elevation of Frisco Peak
interval_minutes = 1  # Adjust the interval as needed

# datetime_today = datetime(2024, 9, 15) #datetime.utcnow() # - timedelta(hours=6)
datetime_today = datetime.now(timezone.utc)
day_today = datetime_today.date()
time_today = datetime_today.time()

if time_today > time_obj(15, 15):
    day_today += timedelta(days=1)
else:
    pass

start_date = datetime.combine(day_today, time_obj(0, 0))

# Getting relevant moon times

def moon_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    moon_times = []
    moon_altitudes = []

    for hour in range(25):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            moon = ephem.Moon(observer)
            m_altitude = float(moon.alt) * 180 / ephem.pi  # Convert altitude to degrees
            moon_times.append(current_time_with_minute)
            moon_altitudes.append(m_altitude)

    moonrise_index = next((i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] > 0 and moon_altitudes[i - 1] <= 0), None)
    time_of_moonrise = moon_times[moonrise_index] if moonrise_index is not None else None

    moonset_index = next((i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] < -3 and moon_altitudes[i - 1] >= -3), None)
    time_of_moonset = moon_times[moonset_index] if moonset_index is not None else None

    max_altitude_index = moon_altitudes.index(max(moon_altitudes))
    time_of_max_altitude = moon_times[max_altitude_index]

    return moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude





# Getting relevant sun times

def sun_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    sun_times = []
    sun_altitudes = []

    for hour in range(25):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            sun = ephem.Sun(observer)
            s_altitude = float(sun.alt) * 180 / ephem.pi
            sun_altitudes.append(s_altitude)
            sun_times.append(current_time_with_minute)

    sunrise_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > -15 and sun_altitudes[i - 1] <= -15), None)
    time_of_sunrise_crit = sun_times[sunrise_index] if sunrise_index is not None else None

    sunset_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < -18 and sun_altitudes[i - 1] >= -18), None)
    time_of_sunset_crit = sun_times[sunset_index] if sunset_index is not None else None

    sunrise_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > 0 and sun_altitudes[i - 1] <= 0), None)
    time_of_sunrise = sun_times[sunrise_index] if sunrise_index is not None else None

    sunset_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < 0 and sun_altitudes[i - 1] >= 0), None)
    time_of_sunset = sun_times[sunset_index] if sunset_index is not None else None



    return sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit





# Creating list of unsafe times

def get_times(sun_altitudes, sun_times, moon_altitudes, moon_times):
    times_list = []
    date_format = "%Y-%m-%d %H:%M:%S"
    for i in range(len(moon_altitudes) - 1):
        if moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0:
            times_list.append(sun_times[i].strftime(date_format))
    for i in range(len(sun_altitudes) - 1):
        if sun_altitudes[i] > -15 and not (moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0):
            times_list.append(sun_times[i].strftime(date_format))
    return times_list






# Get endtime

def get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude):
    start_time = None
    end_time = None
    start_time_2 = None
    end_time_2 = None
    if not ((time_of_sunset_crit) < time_of_moonset < (time_of_sunrise_crit)): #if sun doesn't set before moon
        start_time = (time_of_sunset_crit) #start time is sunset + 1.5hr
        #print(1)
    else:
        start_time = time_of_moonset #otherwise, start at moonset, all good here.
        #print(2)
        #print(start_time)

    if not ((time_of_sunset_crit) < time_of_max_altitude < (time_of_sunrise_crit)): #if moon peak not between sunrise and sunset
        end_time = (time_of_sunrise_crit) #end at sunrise - 1.5hr
        #print(3)
    else:
            if start_time > time_of_max_altitude: #if ending at sunrise - 1.5hr
                end_time = time_of_sunrise_crit
            else:
                end_time = time_of_max_altitude #otherwise, end at moon peak
            #print(4)
            #print(end_time)
    if start_time == (time_of_sunset_crit) and end_time == time_of_max_altitude: #start = sunset + 1.5hr, end = moon peak: this is the block we should fix
        #print(5)
        if (time_of_sunset_crit) < time_of_moonset < (time_of_sunrise_crit): #if moon sets between sunset and  sunrise
            start_time_2 = time_of_moonset
            end_time_2 = (time_of_sunrise_crit) #return these instead?
            if (end_time_2 - start_time_2) < timedelta(minutes=95):
                print("Observation window too short.")
            return start_time_2, end_time_2 #NEW
    #print(start_time)
    #print(end_time)
    return start_time, end_time

    #invalid logic here
    '''
    if start_time > end_time == time_of_max_altitude: #if start is after end and end is moon peak
        start_time_2 = start_time #no idea what start time is set to at this point
        start_time = time_of_sunset_crit #this is creating the extra first block I think that we don't want
        end_time_2 = time_of_sunrise_crit #second block ends at sunrise - 1.5hr
    '''
    '''
    if not (start_time_2 == None and end_time_2 == None): #dealing with invalid periods of time
        if start_time_2 > end_time_2: #start2 after end2
            start_time_2 = None
            end_time_2 = None

    if not (start_time_2 == None and end_time_2 == None):
        if (end_time_2 - start_time_2) < timedelta(minutes=95): #if start2 before end2 but there's less than 95 min of observation in the second block
            end_time_2 = None
            start_time_2 = None
        elif (end_time - start_time) < timedelta(minutes=95): #if the first block is less than 95 min of obs
            start_time = start_time_2
            end_time = end_time_2
            end_time_2 = None
            start_time_2 = None
    
    return start_time, end_time
    '''




# Create file

def create_file(user_input='nope'):
    try:
        date_format = "%Y-%m-%d %H:%M:%S"
        user_input = datetime.strptime(user_input, date_format)

    except ValueError:
        user_input = 0

    moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude = moon_position_over_time(latitude, longitude, start_date, interval_minutes)
    sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
    sun_moon_list = [time_of_sunrise.strftime(date_format), time_of_sunset.strftime(date_format), time_of_moonrise.strftime(date_format), time_of_moonset.strftime(date_format)]
    start_time, endtime = get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude) #changed from entime, start_time
    #print(start_time, endtime)
    # endtime = time_of_sunrise_crit

    sun_moon_list.append(start_time)

    if user_input !=  0:
        if user_input < time_of_sunrise_crit and user_input > time_of_sunset_crit:
                        # and user_input < moonrise:
             sun_moon_list.append(user_input)
        else:
                        lets.communicate('User input not safe')

    else:
        sun_moon_list.append(endtime.strftime(date_format))


    with open("/data/TrinityLabComputer/obs_prgm/EON_time.txt", "w") as file: #/data/TrinityLabComputer/obs_prgm/
            # Loop through the list and write each element to the file
        for item in sun_moon_list:
            file.write(str(item) + '\n')
        file.close()


    print("Moonrise time (UTC):", time_of_moonrise.strftime("%Y-%m-%d %H:%M"))
    print("Moonset time (UTC):", time_of_moonset.strftime("%Y-%m-%d %H:%M"))
    print("Sunrise time (UTC):", time_of_sunrise.strftime("%Y-%m-%d %H:%M"))
    print("Sunset time (UTC):", time_of_sunset.strftime("%Y-%m-%d %H:%M"))
    print("---")
    start_timetoprint=(start_time - timedelta(minutes=32)).strftime("%Y-%m-%d %H:%M")
    print("\033[1mStart extrigs time (UTC):\033[1m", start_timetoprint)
    print("\033[1mDoor Open time (UTC):\033[1m", start_time.strftime("%Y-%m-%d %H:%M"))
    print("\033[1mDoor Closed Cutoff time (UTC):\033[1m", endtime.strftime("%Y-%m-%d %H:%M"))
    print("\033[1mStop extrigs time (UTC):\033[1m", time_of_sunrise_crit.strftime("%Y-%m-%d %H:%M"))
    print("---")
    lets.log_file('EON_time.txt created')





# Check current time

def check_current_time():
    df = pd.read_csv('/data/TrinityLabComputer/obs_prgm/EON_time.txt',header=None) 
    df.columns = ['UTC DateTime']
    df['UTC DateTime'] = pd.to_datetime(df['UTC DateTime'])
    row_titles = ['Sunrise', 'Sunset','Moonrise', 'Moonset', 'Start Extrigs', 'Cutoff']
    df.index = row_titles
    cutoff_row = df.loc['Cutoff']
    cutoff_time = cutoff_row['UTC DateTime']
    #current_utc_time = datetime.utcnow() #change to the time I want
    #current_utc_time = something like DateTime.date and test times

    moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude = moon_position_over_time(latitude, longitude, start_date, interval_minutes)
    sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
    times_list = get_times(sun_altitudes, sun_times, moon_altitudes, moon_times)
    start_time, end_time = get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude) # switched from endtime, start_time

    
    #print(start_time)
    #print(sunset_time)
    def curr_est_offset():
        tz_est = pytz.timezone('US/Eastern')
        offset = tz_est.utcoffset(datetime.utcnow())
        offset_seconds = (offset.days * 86400) + offset.seconds
        offset_hours = offset_seconds // 3600
        return abs(offset_hours) # -4 or -5
    
    # current_date = datetime_today.date() 
    # time_today = datetime_today.time()
    time_today_dt = datetime.utcnow()
    lets.log_file(f'start time: {time_of_sunset_crit}, end time: {time_of_sunrise_crit}, current time: {time_today_dt} ')
    #print(start_time)
    
    # region when it is safe for door open data colletion (any voltage level)
    if (start_time < time_today_dt and time_today_dt < cutoff_time): # and not (current_utc_time.strftime("%Y-%m-%d %H:%M") in times_list):   #and current_utc_time > sunset_time:
        lets.communicate(f'TIME: TIME - Regular observation period - Door open data collection safe till {cutoff_time-timedelta(hours=curr_est_offset())} in ET')
        
        return 2
    # Region when it is safe for HV to be turned on (between sunrise and sunset)
    elif (time_of_sunset_crit - timedelta(minutes=32) < time_today_dt and time_today_dt < time_of_sunrise_crit): # and not (current_utc_time.strftime("%Y-%m-%d %H:%M") in times_list):   #and current_utc_time > sunset_time:

        lets.communicate(f'TIME: TIME - The time is safe cutoff is {time_of_sunrise_crit-timedelta(hours =curr_est_offset())} in ET')

        return 1
    else:
        lets.communicate('TIME: Current time UNSAFE')
        return 0
        
def get_hv_value():
    df = pd.read_csv('/data/TrinityLabComputer/obs_prgm/HVvalue_Angle.txt', header=0)
    
    #print(df)


    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation
    observer.date = datetime.now(timezone.utc) +timedelta(hours=7)
    # print(f"Observer date (UTC): {observer.date}")
    moon = ephem.Moon()
    moon.compute(observer)
    #print( f"{np.rad2deg(float(repr(moon.alt)))}, {np.rad2deg(float(repr(moon.az)))}")
    #print(f"moon phase: {moon.phase}")

    # convert polar to cartesian
    # start with telescope at -0.5 deg altitude, -80 deg azimuth
    # r, phi, theta
    tp_polar = [1,-80.0, 90.5]
    tp_cart= [tp_polar[0]*math.sin(math.radians(tp_polar[2]))*math.cos(math.radians(tp_polar[1])),tp_polar[0]*math.sin(math.radians(tp_polar[2]))*math.sin(math.radians(tp_polar[1])),tp_polar[0]*math.cos(math.radians(tp_polar[2]))]
    #print(tp_cart)
    mp_polar = [1, float(np.rad2deg(float(repr(moon.az)))),float(np.rad2deg(float(repr(moon.alt))))]
    if mp_polar[2] < 0:
        mp_polar[2] = 90 + abs(mp_polar[2])
    mp_cart= [mp_polar[0]*math.sin(math.radians(mp_polar[2]))*math.cos(math.radians(mp_polar[1])),mp_polar[0]*math.sin(math.radians(mp_polar[2]))*math.sin(math.radians(mp_polar[1])),mp_polar[0]*math.cos(math.radians(mp_polar[2]))]
    # print(mp_polar)
    #print(mp_cart)
    

    angle = (tp_cart[0] * mp_cart[0]) + (tp_cart[1] * mp_cart[1]) + (tp_cart[2] * mp_cart[2])
    angle = math.degrees(math.acos(angle))
    print(f"Angle between telescope pointing and moon: {np.round(angle)} degrees")


    if float(np.rad2deg(float(repr(moon.alt)))) > 0.0:
        print("Moon is above the horizon - Getting starting HV value from table")
        # round to nearst 0.10
        moon_phase_rounded = int(round(moon.phase,-1))
        angle_rounded = int(round(angle/10,0))
        print(moon_phase_rounded, angle_rounded)
        #print(df[str(moon_phase_rounded)][angle_rounded])
        return df[str(moon_phase_rounded)][angle_rounded]

    else:
        print("Moon is below the horizon - Setting HV to 44.0")
        return 44.0
    # print((telescope_position[0] * moon_position[0]))
    # print((telescope_position[1] * moon_position[1]))
    # while angle > 360.0:
    #     angle -= 360.0

    # while angle < 0.0:
    #     angle += 360.0
    # print(180 -angle)



# print(f"Output of HV: {get_hv_value()}")
create_file()
check_current_time()
