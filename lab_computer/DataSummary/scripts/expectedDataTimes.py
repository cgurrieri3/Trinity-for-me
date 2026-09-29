import ephem
from datetime import datetime, timedelta, date, time, timezone
import pandas as pd
import argparse

def parse_arguments():
    """
    Parse command-line arguments.

    Returns:
        argparse.Namespace: An object containing the parsed arguments.
    """
    parser = argparse.ArgumentParser(description="makes plots of databases WX and SM")

    # Add arguments
    parser.add_argument('-y', '--year', type=int, help='Date in YYYY format')
    parser.add_argument('-m', '--month', type=int, help='Date in MM format')
    parser.add_argument('-d', '--day', type=int, help='Date in DD format')

    # Parse the command-line arguments
    args = parser.parse_args()

    return args

# Parse command-line arguments
args = parse_arguments()

latitude = 38.5202  # Latitude of Frisco Peak
longitude = -113.2883  # Longitude Frisco Peak
elevation = 3048  # Elevation of Frisco Peak
start_date = datetime(args.year, args.month, args.day).replace(tzinfo=timezone.utc, hour=0, minute=0, second=0, microsecond=0) # Current UTC time
interval_minutes = 1  # Adjust the interval as needed

# Moon calculations
def moon_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    moon_times = []
    moon_altitudes = []

    for hour in range(27):
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

#Sun calculations
def sun_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    sun_times = []
    sun_altitudes = []

    for hour in range(27):
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

# NGC calculations
def ngc_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    ngc = ephem.FixedBody()
    ngc.name = 'NGC 1068'
    ngc._ra = '02:42:40.7091669408'
    ngc._dec = '-00:00:47.859690204'
    ngc._epoch = '2000'

    ngc_times = []
    ngc_altitudes = []

    for hour in range(24):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            ngc.compute(observer)
            ngc_altitude = float(ngc.alt) * 180 / ephem.pi
            ngc_altitudes.append(ngc_altitude)
            ngc_times.append(current_time_with_minute)

    return ngc_times, ngc_altitudes

# TXS calculations
def txs_position_over_time(latitude, longitude, start_date, interval_minutes):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation

    txs = ephem.FixedBody()
    txs.name = 'TXS 0506'
    txs._ra = '05:09:25.9644373872'
    txs._dec = '05:41:35.333820420'
    txs._epoch = '2000'

    txs_times = []
    txs_altitudes = []

    for hour in range(24):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            txs.compute(observer)
            txs_altitude = float(txs.alt) * 180 / ephem.pi
            txs_altitudes.append(txs_altitude)
            txs_times.append(current_time_with_minute)

    return txs_times, txs_altitudes


# Initializing objects
moon_times, moon_altitudes, time_of_max_altitude, time_of_moonrise, time_of_moonset = moon_position_over_time(latitude, longitude, start_date, interval_minutes)
sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
ngc_times, ngc_altitudes = ngc_position_over_time(latitude, longitude, start_date, interval_minutes)
txs_times, txs_altitudes = txs_position_over_time(latitude, longitude, start_date, interval_minutes)


# Expected observation time calculations
start_time = None
end_time = None
start_time_2 = None
end_time_2 = None

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
data = {'Moonrise Time': time_of_moonrise, 'Moonset Time': time_of_moonset, 'Moonmax Time': time_of_max_altitude, 'Sunrise Crit Time': (time_of_sunrise - timedelta(minutes=90)), 'Sunset Crit Time': (time_of_sunset + timedelta(minutes=90))}
index = ['Times']
df = pd.DataFrame(data, index=index)
df_sorted = df.apply(lambda x: pd.to_datetime(x).sort_values(), axis=1)

if not ((time_of_sunset + timedelta(minutes=90)) < time_of_moonset < (time_of_sunrise - timedelta(minutes=90))):
    start_time = (time_of_sunset + timedelta(minutes=90))
else:
    start_time = time_of_moonset

if not ((time_of_sunset + timedelta(minutes=90)) < time_of_max_altitude < (time_of_sunrise - timedelta(minutes=90))):
    end_time = (time_of_sunrise - timedelta(minutes=90))
else:
    end_time = time_of_max_altitude

if start_time == (time_of_sunset + timedelta(minutes=90)) and end_time == time_of_max_altitude:
    if (time_of_sunset + timedelta(minutes=90)) < df_sorted['Moonset Time'].iloc[0] < (time_of_sunrise - timedelta(minutes=90)):
        start_time_2 = df_sorted['Moonset Time'].iloc[0]
        end_time_2 = (time_of_sunrise - timedelta(minutes=90))

if start_time > end_time == time_of_max_altitude:
    start_time_2 = start_time
    start_time = time_of_sunset + timedelta(minutes=90)
    end_time_2 = time_of_sunrise - timedelta(minutes=90)

if not (start_time_2 == None and end_time_2 == None):
    if start_time_2 > end_time_2:
        start_time_2 = None
        end_time_2 = None

if not (start_time_2 == None and end_time_2 == None):
    if (end_time_2 - start_time_2) < timedelta(minutes=45):
        end_time_2 = None
        start_time_2 = None
    elif (end_time - start_time) < timedelta(minutes=45):
        start_time = start_time_2
        end_time = end_time_2
        end_time_2 = None
        start_time_2 = None

time_delta_1 = abs(end_time - start_time)
time_delta_hr = round(time_delta_1.total_seconds() / 3600, 2)


# Expected NGC time calculations
ngc_observing = []
for i in range(len(ngc_times) - 1):
    if (0 > ngc_altitudes[i] > -10) and (ngc_altitudes[i] > ngc_altitudes[i + 1]):
        ngc_observing.append(ngc_times[i])

if (start_time < min(ngc_observing) < end_time) and (start_time < max(ngc_observing) < end_time):
    NGC_time_delta_1 = abs((max(ngc_observing)) - (min(ngc_observing)))
    NGC_time_delta_hr = round(NGC_time_delta_1.total_seconds() / 3600, 2)
elif (start_time < min(ngc_observing) < end_time) and not (start_time < max(ngc_observing) < end_time):
    NGC_time_delta_1 = abs(end_time - (min(ngc_observing)))
    NGC_time_delta_hr = round(NGC_time_delta_1.total_seconds() / 3600, 2)
elif (not (start_time < min(ngc_observing) < end_time)) and (start_time < max(ngc_observing) < end_time):
    NGC_time_delta_1 = abs((max(ngc_observing)) - start_time)
    NGC_time_delta_hr = round(NGC_time_delta_1.total_seconds() / 3600, 2)
else:
    NGC_time_delta_hr = 0


txs_observing = []
for i in range(len(txs_times) - 1):
    if (0 > txs_altitudes[i] > -10) and (txs_altitudes[i] > txs_altitudes[i + 1]):
        txs_observing.append(txs_times[i])

if (start_time < min(txs_observing) < end_time) and (start_time < max(txs_observing) < end_time):
    TXS_time_delta_1 = abs((max(txs_observing)) - (min(txs_observing)))
    TXS_time_delta_hr = round(TXS_time_delta_1.total_seconds() / 3600, 2)
elif (start_time < min(txs_observing) < end_time) and not (start_time < max(txs_observing) < end_time):
    TXS_time_delta_1 = abs(end_time - (min(txs_observing)))
    TXS_time_delta_hr = round(TXS_time_delta_1.total_seconds() / 3600, 2)
elif (not (start_time < min(txs_observing) < end_time)) and (start_time < max(txs_observing) < end_time):
    TXS_time_delta_1 = abs((max(txs_observing)) - start_time)
    TXS_time_delta_hr = round(TXS_time_delta_1.total_seconds() / 3600, 2)
else:
    TXS_time_delta_hr = 0



print("Expected observation time: " + str(time_delta_hr) + " hours")
print("Expected NGC time: " + str(NGC_time_delta_hr) + " hours")
print("Expected TXS time: " + str(TXS_time_delta_hr) + " hours")

data = {
    'Name': ["Expected observation time","Expected NGC time","Expected TXS time"],
    'minutes': [time_delta_hr,NGC_time_delta_hr,TXS_time_delta_hr],
    
}

# Convert the dictionary to a DataFrame
df = pd.DataFrame(data)

# Save the DataFrame to a CSV file
df.to_csv('/data/TrinityLabComputer/DataSummary/Otherfiles/ExpectedNightlyTimes.csv', index=False)
