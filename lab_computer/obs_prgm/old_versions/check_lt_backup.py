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
from datetime import datetime, timedelta
import pandas as pd
import time
import communicate as lets

latitude = 38.5202  # Latitude of Frisco Peak
longitude = -113.2883  # Longitude Frisco Peak
elevation = 3048  # Elevation of Frisco Peak
start_date = datetime.utcnow()  # Current UTC time

interval_minutes = 2  # Adjust the interval as needed


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
	
	# Find the time at which the maximum altitude is reached
	max_altitude_index = moon_altitudes.index(max(moon_altitudes))
	time_of_max_altitude = moon_times[max_altitude_index]
	

	# Find the time of Moonrise (positive slope crossing y=0)
	moonrise_index = next(
	    (i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] > 0 and moon_altitudes[i - 1] <= 0), None)
	time_of_moonrise = moon_times[moonrise_index] if moonrise_index is not None else None

	# Find the time of Moonset (negative slope crossing y=0)
	moonset_index = next(
	    (i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] < 0 and moon_altitudes[i - 1] >= 0), None)
	
	time_of_moonset = moon_times[moonset_index] if moonset_index is not None else None

	return moon_times, moon_altitudes, time_of_max_altitude, time_of_moonrise, time_of_moonset

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

	# Find the time of sunrise (positive slope crossing y=0)
	sunrise_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > 0 and sun_altitudes[i - 1] <= 0), None)
	time_of_sunrise = sun_times[sunrise_index] if sunrise_index is not None else None

	# Finding the time of critical sunrise time (positive slope crossing y=-15) **WORKING BUT NOT USED**
	sunrise_crit_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > -15 and sun_altitudes[i - 1] <= 0), None)
	time_of_sunrise_crit = sun_times[sunrise_crit_index] if sunrise_crit_index is not None else None

	# Find the time of sunset (negative slope crossing y=0)
	sunset_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < 0 and sun_altitudes[i - 1] >= 0), None)
	time_of_sunset = sun_times[sunset_index] if sunset_index is not None else None

	# Finding the time of critical sunset time (negative slope crossing y=-15) **CURRENTLY NOT WORKING**
	sunset_crit_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < -15 and sun_altitudes[i - 1] >= 0), None)
	time_of_sunset_crit = sun_times[sunset_crit_index] if sunset_crit_index is not None else None

	return sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit



def create_file(user_input='nope'):


	# Initializing Sun
	sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
	#Intitalizing Moon
	moon_times, moon_altitudes, time_of_max_altitude, time_of_moonrise, time_of_moonset = moon_position_over_time(latitude, longitude, start_date, interval_minutes)

	# Find start and end times (90 minutes after/before sunset/sunrise) **NOT USING ALTITUDE Y=-15**
	start_time = None
	end_time = None
	start_time_2 = None
	end_time_2 = None
	case = 0

	df_cutoff = {'Name' : ['time_of_moonset', 'time_of_max_altitude','time_of_sunrise'], 
		'DT' : [time_of_moonset, time_of_max_altitude,time_of_sunrise]}
	df_cutoff = pd.DataFrame(df_cutoff)

	df_start = {'Name' : ['time_of_moonset', 'time_of_sunset','time_of_sunrise'], 
		'DT' : [time_of_moonset, time_of_sunset,time_of_sunrise]}
	df_start = pd.DataFrame(df_start)

	if time_of_max_altitude > time_of_sunset and time_of_moonset > time_of_sunrise:
	    case = 1
	elif time_of_max_altitude < time_of_sunset and time_of_moonset < time_of_sunrise:
	    case = 2
	elif time_of_max_altitude > time_of_sunset and time_of_moonset < time_of_sunrise:
	    case = 3
	elif time_of_sunset > time_of_sunrise:
	    df_sort_cutoff = df_cutoff.sort_values(by='DT')
	    #print(df_sort_cutoff)
	    #df_sort_start = df_start.sort_values(by='DT')
	    #print(df_sort_start)
	    time_of_sunset = time_of_sunset - timedelta(days=1)
	    case = 4
		
	#print(case)

	if case == 1:
	    start_time = time_of_sunset + timedelta(minutes=90)
	    end_time = time_of_max_altitude
	elif case == 2:
	    start_time = time_of_moonset
	    end_time = time_of_sunrise - timedelta(minutes=90)
	elif case == 3:
	    start_time = time_of_sunset + timedelta(minutes=90)
	    end_time = time_of_max_altitude
	    start_time_2 = time_of_moonset
	    end_time_2 = time_of_sunrise - timedelta(minutes=90)
	elif case ==4: 
	    start_time = time_of_sunset
	    end_time = df_sort_cutoff.loc[0,'DT']
	    
	#print(start_time)

	# Print the times of interest
	lets.communicate(f"Sunset Time:   {time_of_sunset.strftime('%Y-%m-%d %H:%M:%S')}")
	lets.communicate(f"Moonrise Time: {time_of_moonrise.strftime('%Y-%m-%d %H:%M:%S')}")
	lets.communicate(f"Moonmax Time:  {time_of_max_altitude.strftime('%Y-%m-%d %H:%M:%S')}")
	lets.communicate(f"Moonset Time:  {time_of_moonset.strftime('%Y-%m-%d %H:%M:%S')}")
	lets.communicate(f"Sunrise Time:  {time_of_sunrise.strftime('%Y-%m-%d %H:%M:%S')}")
	lets.communicate(f"Start Time:    {start_time.strftime('%Y-%m-%d %H:%M:%S')}")   # "\033[1m" and "\033[0m" make the text bold
	lets.communicate(f"End Time:      {end_time.strftime('%Y-%m-%d %H:%M:%S')}")   # "\033[1m" and "\033[0m" make the text bold
	if case == 3:
	    lets.communicate(f"Start Time 2:    {start_time_2.strftime('%Y-%m-%d %H:%M:%S')}")   # "\033[1m" and "\033[0m" make the text bold
	    lets.communicate(f"End Time 2:      {end_time_2.strftime('%Y-%m-%d %H:%M:%S')}")   # "\033[1m" and "\033[0m" make the text bold

	# add back in user input
	if user_input != 'nope':
		if start_time_2 == None:
			if (start_time < user_input and user_input < end_time):
				end_time = user_input
				lets.communicate(f'User value {end_time} ACCEPTED')
			else:
				lets.communicate('User input unsafe')
		else:
			if (start_time_2 < user_input and user_input < end_time_2):
				end_time_2 = user_input
				lets.communicate(f'User value {end_time} ACCEPTED')
			else:
				lets.communicate('User input unsafe')

	# Write to text file for trinity.py
	with open("/home/mpotts32/obs_prgm/EON_time.txt", "w") as file:
	    file.write(f"Moonrise: {time_of_moonrise}\n")
	    file.write(f"Moonset: {time_of_moonset}\n")
	    file.write(f"Sunrise: {time_of_sunrise}\n")
	    file.write(f"Sunset: {time_of_sunset}\n")
	    if case ==1 or case == 2 or case ==4:
	    	file.write(f"Start: {start_time}\n")
	    	file.write(f"Cutoff: {end_time}\n")
	    elif case == 3:
	        if start_date < end_time:
	            file.write(f"Start: {start_time}\n")
	            file.write(f"Cutoff: {end_time}\n")
	        else:
	            file.write(f"Start2: {start_time_2}\n")
	            file.write(f"Cutoff: {end_time_2}\n")
	print("EON_times.txt: Complete")
	print('---')

def check_current_time():


	df = pd.read_csv('/home/mpotts32/obs_prgm/EON_time.txt',delimiter = ' ',header=None)
	#print(df)
	df.columns = ['Name','UTC DATE', 'UTC time']
	row_titles = ['Moonrise', 'Moonset', 'Sunrise','Sunset','Start','Cutoff']
	df.index = row_titles
	# Combine "Date" and "Time" into a single column "DateTime"
	df['DateTime'] = df['UTC DATE'] + 'T' + df['UTC time']

	# Convert the "DateTime" column to datetime objects
	df['DateTime'] = pd.to_datetime(df['DateTime'])
	

	df = df.sort_values(by='DateTime')  # Sort by time and reset the index
	
	
	cutoff_row = df.loc['Cutoff']
	

	cutoff_time = cutoff_row['DateTime']
	#print(cutoff_time)


	current_utc_time = datetime.utcnow()

	#print(current_utc_time)

	sunset_time = df.loc['Sunset']
	sunset_time = sunset_time['DateTime']
	#print(sunset_time)
	def curr_est_offset():
	    tz_est = pytz.timezone('US/Eastern')
	    offset = tz_est.utcoffset(datetime.utcnow())
	    offset_seconds = (offset.days * 86400) + offset.seconds
	    offset_hours = offset_seconds // 3600
	    #print(abs(offset_hours))
	    return abs(offset_hours) # -4 or -5
	#curr_est_offset()

	if current_utc_time < cutoff_time: #and current_utc_time > sunset_time:
		# utc_timezone = pytz.timezone('UTC')
		# cutoff_time_utc = utc_timezone.localize(cutoff_time)
		# print(cutoff_time_utc)
		# et_timezone = pytz.timezone('US/Eastern')
		# cutoff_time_et = cutoff_time_utc.astimezone(et_timezone)
		#print(cutoff_time)
		final_time = cutoff_time-timedelta(hours =curr_est_offset())
		final_time = final_time.strftime('%Y-%m-%d %H:%M:%S')
		lets.communicate(f'TIME: TIME - The time is safe cutoff is {final_time} in ET')
		
		return 1
	else: 
		lets.communicate('TIME: Current time UNSAFE')
		return 0

	
create_file()

check_current_time()
