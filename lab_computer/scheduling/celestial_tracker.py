import ephem
import matplotlib.pyplot as plt
from datetime import datetime, timedelta, date, time, timezone
from matplotlib.dates import HourLocator, DateFormatter
import pandas as pd

latitude = 38.5202  # Latitude of Frisco Peak
longitude = -113.2883  # Longitude Frisco Peak
elevation = 3048  # Elevation of Frisco Peak
start_date = datetime(2024, 4, 2).replace(tzinfo=timezone.utc, hour=0, minute=0, second=0, microsecond=0) # Current UTC time
interval_minutes = 1  # Adjust the interval as needed


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

#Intitalizing Moon
moon_times, moon_altitudes, time_of_max_altitude, time_of_moonrise, time_of_moonset = moon_position_over_time(latitude, longitude, start_date, interval_minutes)


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


# Initializing Sun
sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)


# Creating plot
fig, ax = plt.subplots()

sun_color = (1.0, 0.8, 0.6)
moon_color = (0.7, 0.7, 0.7)




# Plotting Moon and Sun altitude line
ax.plot(moon_times, moon_altitudes, label='Moon Altitude', color=moon_color, marker='o', markersize=3, linewidth=0.8,
        zorder=1)
ax.plot(sun_times, sun_altitudes, label='Sun Altitude', color=sun_color, marker='o', markersize=3, linewidth=0.8,
        zorder=1)

time_list = []
# Highlight points with dangerous light levels
for i in range(len(moon_altitudes) - 1):
    if moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0:
        '''
        ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)
        '''
        time_list.append(sun_times[i])

for i in range(len(sun_altitudes) - 1):
    if sun_altitudes[i] > -15 and not (moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0):
        '''
        ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)
        '''
        time_list.append(sun_times[i])




# Plot more pronounced horizon line
ax.axhline(y=0, color='black', linestyle='--', linewidth=1.5, label='', zorder=5)


# Plot when midnight is
'''
start_date_date = start_date.date()
next_date = start_date_date + timedelta(days=1)
nextday_time = datetime.combine(next_date, time(0, 00))
plt.axvline(x=nextday_time, color='black', linestyle='--')
'''

# Find start and end times (90 minutes after/before sunset/sunrise) **NOT USING ALTITUDE Y=-15**
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



# Improve chart labels
start_date_str = start_date.strftime("%m-%d-%Y")
ax.set_xlabel('Time (UTC)', fontsize=10)
ax.set_ylabel('Elevation (degrees)', fontsize=10)
ax.set_title('Celestial positions over the next 24 hours (starting ' + start_date_str + ')', fontsize=12)
ax.xaxis.set_major_locator(HourLocator(interval=3))
ax.xaxis.set_major_formatter(DateFormatter('%H:%M'))
ax.xaxis.set_minor_locator(HourLocator(interval=1))
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(True)


# Adding sources of interest

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

    for hour in range(27):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            ngc.compute(observer)
            ngc_altitude = float(ngc.alt) * 180 / ephem.pi
            ngc_altitudes.append(ngc_altitude)
            ngc_times.append(current_time_with_minute)

    return ngc_times, ngc_altitudes

ngc_times, ngc_altitudes = ngc_position_over_time(latitude, longitude, start_date, interval_minutes)

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

    for hour in range(27):
        current_time = start_date + timedelta(hours=hour)

        for minute in range(0, 60, interval_minutes):
            current_time_with_minute = current_time + timedelta(minutes=minute)
            observer.date = current_time_with_minute
            txs.compute(observer)
            txs_altitude = float(txs.alt) * 180 / ephem.pi
            txs_altitudes.append(txs_altitude)
            txs_times.append(current_time_with_minute)

    return txs_times, txs_altitudes

txs_times, txs_altitudes = txs_position_over_time(latitude, longitude, start_date, interval_minutes)


ngc_observing = []
for i in range(len(ngc_times) - 1):
    if ((0 > ngc_altitudes[i] > -10) or (0 > ngc_altitudes[i+1] > -10)) and (time_of_sunset < ngc_times[i] < time_of_sunrise):
        ngc_observing.append(ngc_times[i])
for i in range(len(ngc_observing) - 1):
    ax.axvspan(ngc_observing[i], ngc_observing[i + 1], color='blue', alpha=0.05)
text = ax.text(0.02, 0.97, 'NGC is within observing window', color='blue', fontsize=7, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
text2 = ax.text(0.02, 0.9094, 'TXS is within observing window', color='purple', fontsize=7, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
txs_observing = []
for i in range(len(txs_times) - 1):
    if ((0 > txs_altitudes[i] > -10) or (0 > txs_altitudes[i+1] > -10)) and (time_of_sunset < txs_times[i] < time_of_sunrise):
        txs_observing.append(txs_times[i])
for i in range(len(txs_observing) - 1):
    ax.axvspan(txs_observing[i], txs_observing[i + 1], color='purple', alpha=0.05)


# Print the times of interest

print(f"Moonrise Time: {time_of_moonrise.strftime('%m-%d %H:%M')}")
print(f"Moonset Time: {time_of_moonset.strftime('%m-%d %H:%M')}")
print(f"Sunrise Time: {time_of_sunrise.strftime('%m-%d %H:%M')}")
print(f"Sunset Time: {time_of_sunset.strftime('%m-%d %H:%M')}")

print(f"\033[1mStart Time: {start_time.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold
print(f"\033[1mEnd Time: {end_time.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold

if not (start_time_2 == None and end_time_2 == None):
    print(f"\033[1mStart Time 2: {start_time_2.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold
    print(f"\033[1mEnd Time 2: {end_time_2.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold


# Write to text file for trinity.py
with open("eon_times.txt", "w") as file:
    file.write(f"Moonrise Time: {time_of_moonrise}\n")
    file.write(f"Moonset Time: {time_of_moonset}\n")
    file.write(f"Sunrise Time: {time_of_sunrise}\n")
    file.write(f"Sunset Time: {time_of_sunset}\n")
    if not (start_time_2 == None and end_time_2 == None):
        if start_date < end_time:
            file.write(f"Start Time: {start_time.strftime}\n")
            file.write(f"End Time: {end_time}\n")
        elif start_time_2 < (time_of_sunrise - timedelta(minutes=90)) and start_time_2 > (time_of_sunset + timedelta(minutes=90)):
            file.write(f"Start Time 2: {start_time_2}\n")
            file.write(f"End Time 2: {end_time_2}\n")
    else:
        file.write(f"Start Time: {start_time.strftime}\n")
        file.write(f"End Time: {end_time}\n")

print("Times have been written to eon_times.txt")



legend = ax.legend(fontsize=8, loc='upper right')
legend.get_frame().set_facecolor('white')
legend.get_frame().set_alpha(1)
plt.xlim(min(sun_times), min(sun_times) + timedelta(hours=24))




def moon_illumination(latitude, longitude, start_date):
    observer = ephem.Observer()
    observer.lat = str(latitude)
    observer.lon = str(longitude)
    observer.elevation = elevation
    observer.date = start_date
    moon = ephem.Moon()
    moon.compute(observer)
    illumination = moon.moon_phase
    return round((illumination * 100))

illumination = moon_illumination(latitude, longitude, start_date)



if illumination >= 90:
    text3 = ax.text(0.78, 0.06, 'Moon Phase = ' + str(illumination) + '%', color='red', fontsize=8, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
    '''
    for i in range(len(moon_altitudes) - 1):
        if not (sun_times[i] in time_list):
            ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)
    '''
else:
    text3 = ax.text(0.784, 0.06, 'Moon Phase = ' + str(illumination) + '%', color='black', fontsize=8, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
    text4 = ax.text(0.784, 0.1795, 'Trinity Start Time', color='green', fontsize=8, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
    text5 = ax.text(0.784, 0.12, 'Trinity End Time', color='Red', fontsize=8, ha='left', va='top', transform=ax.transAxes, bbox=dict(facecolor='white', alpha=1, edgecolor='grey'))
    plt.axvline(x=start_time - timedelta(minutes=10), color='green', linestyle='--', linewidth=2, alpha=0.5)
    plt.axvline(x=end_time + timedelta(minutes=10), color='red', linestyle='--', linewidth=2, alpha=0.5)
    
    if not (start_time_2 == None and end_time_2 == None):
        plt.axvline(x=start_time_2 - timedelta(minutes=10), color='green', linestyle='--', linewidth=2, alpha=0.5)
        plt.axvline(x=end_time_2, color='red', linestyle='--', linewidth=2, alpha=0.5)

    for i in range(len(moon_altitudes) - 1):
        if not (sun_times[i] in time_list):
            ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)

plt.show()
