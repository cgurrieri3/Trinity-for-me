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

def get_moon_sun_set():
	# Milford, Utah coordinates
	lat = 38.3760
	lon = -113.0132

	# Create an observer object for Milford, Utah
	obs = ephem.Observer()
	obs.lat = str(lat)
	obs.long = str(lon)

	# Set the time to the current date and time
	obs.date = datetime.utcnow()

	# Compute the moonrise, moonset, and sunset times
	moonrise_time = obs.next_rising(ephem.Moon())
	moonset_time = obs.next_setting(ephem.Moon())
	sunrise_time = obs.next_rising(ephem.Sun())
	sunset_time = obs.next_setting(ephem.Sun())	

	# Convert times to UTC
	moonrise_time_utc = moonrise_time.datetime()
	moonset_time_utc = moonset_time.datetime() + timedelta(hours=-6)
	sunrise_time_utc = sunrise_time.datetime() + timedelta(hours=-1.5)
	sunset_time_utc = sunset_time.datetime()

	print("Moonrise time (UTC):", moonrise_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
	print("Moonset time -6 hours (UTC):", moonset_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
	print("Sunrise time -1:30 hours (UTC):", sunrise_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
	print("Sunset time (UTC):", sunset_time_utc.strftime("%Y-%m-%d %H:%M:%S %Z"))
	return moonset_time_utc,moonrise_time_utc,sunrise_time_utc,sunset_time_utc

def create_file(user_input='nope'):
	try:
		date_format = "%Y-%m-%dT%H:%M:%S"
		user_input = datetime.strptime(user_input, date_format)
		#print(user_input)
	except ValueError:
		user_input = 0
	# Define the timezone of the input string (if known)
	# input_timezone = pytz.timezone('UTC')
	# # Assuming the input string is in the input timezone, localize it
	# user_input = input_timezone.localize(user_input)
	moonset,moonrise, sunrise,sunset = get_moon_sun_set()

	sun_moon_array = [moonrise,moonset,sunrise,sunset]
	no_sunset=sorted(sun_moon_array[0:3])
	if user_input != 0:
		if user_input < sunrise and user_input > sunset: 
			# and user_input < moonrise:
			sun_moon_array.append(user_input)
		else:
			lets.communicate('User input not safe ')


	else:
		sun_moon_array.append(no_sunset[0])

	with open("/data/TrinityLabComputer/obs_prgm/EON_time.txt", "w") as file:
	    # Loop through the array and write each element to the file
		for item in sun_moon_array:
			file.write(str(item) + '\n')	
		file.close()
	lets.log_file('EON_time.txt created ')

#create_file('2023-11-04T03:45:00')
#create_file()

def check_current_time():


	df = pd.read_csv('/data/TrinityLabComputer/obs_prgm/EON_time.txt',delimiter = ' ',header=None)
	#print(df)
	df.columns = ['UTC DATE', 'UTC time']
	row_titles = ['Moonrise', 'Moonset', 'Sunrise','Sunset','Cutoff']
	df.index = row_titles
	# Combine "Date" and "Time" into a single column "DateTime"
	df['DateTime'] = df['UTC DATE'] + 'T' + df['UTC time']

	# Convert the "DateTime" column to datetime objects
	df['DateTime'] = pd.to_datetime(df['DateTime'])
	

	df = df.sort_values(by='DateTime')  # Sort by time and reset the index
	#print(df)
	# Find a specific row by its index (e.g., index 1)
	
	
	# sorted_times=list(df.index)
	# print(sorted_times)
	# index = sorted_times.index('Sunset')
	# #print(index)
	# try:
	# 	next_time = sorted_times[index+1]
	# except IndexError:
	# 	next_time = sorted_times[0]
	
	
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
	    #print(tz_est)
	    offset = tz_est.utcoffset(datetime.utcnow())
	    #offset = tz_est.utcoffset(datetime(2024, 3, 8, 12, 0, 0))
	    #print(offset)
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
		lets.communicate(f'TIME: TIME - The time is safe cutoff is {cutoff_time-timedelta(hours =curr_est_offset())} in ET')
		
		return 1
	else: 
		lets.communicate('TIME: Current time UNSAFE')
		return 0

#get_moon_sun_set()	
create_file()
check_current_time()


# # compare the times and make sure that current time is before cut off time
# # Produce either that the time is safe of not safe
# from datetime import datetime,timedelta




# # Calculate the time difference
# time_difference = EON -current_utc_time
# print(time_difference)

# # Define a time duration of 1 hour
# EON = timedelta(hours=1)
# print(time_difference)
# # weather stations is updating
# if time_difference <= EON:
# 	print('TIME: The time is safe')
# 	return 1
# else: 
# 	print('TIME: The time is not safe')
# 	return 0