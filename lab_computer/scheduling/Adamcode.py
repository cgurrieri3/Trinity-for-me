import ephem
import matplotlib.pyplot as plt
from datetime import datetime, timedelta, date, time
from matplotlib.dates import HourLocator, DateFormatter

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

    for hour in range(24):
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

    for hour in range(24):
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


# Plotting Moon and Sun altitude line
ax.plot(moon_times, moon_altitudes, label='Moon Altitude', color='grey', marker='o', markersize=5, linewidth=0.8,
        zorder=1)
ax.plot(sun_times, sun_altitudes, label='Sun Altitude', color='orange', marker='o', markersize=5, linewidth=0.8,
        zorder=1)

# Highlight points with dangerous light levels
for i in range(len(moon_altitudes) - 1):
    if moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0:
        ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)
for i in range(len(sun_altitudes) - 1):
    if sun_altitudes[i] > -15 and not (moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0):
        ax.axvspan(sun_times[i], sun_times[i + 1], color='grey', alpha=0.05)


# Plot more pronounced horizon line
ax.axhline(y=0, color='black', linestyle='--', linewidth=1.5, label='', zorder=5)


# Plot when midnight is
start_date_date = start_date.date()
next_date = start_date_date + timedelta(days=1)
nextday_time = datetime.combine(next_date, time(0, 00))
plt.axvline(x=nextday_time, color='black', linestyle='--')


# Find start and end times (90 minutes after/before sunset/sunrise) **NOT USING ALTITUDE Y=-15**
start_time = None
end_time = None
start_time_2 = None
end_time_2 = None
case = 0

if time_of_max_altitude > time_of_sunset and time_of_moonset > time_of_sunrise:
    case = 1
elif time_of_max_altitude < time_of_sunset and time_of_moonset < time_of_sunrise:
    case = 2
elif time_of_max_altitude > time_of_sunset and time_of_moonset < time_of_sunrise:
    case = 3

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


# Improve chart labels
start_date_str = start_date.strftime("%m-%d-%Y")
ax.set_xlabel('Time (UTC)', fontsize=10)
ax.set_ylabel('Altitude (degrees)', fontsize=10)
ax.set_title('Celestial positions over the next 24 hours (starting ' + start_date_str + ')', fontsize=12)
ax.legend(fontsize=8)
ax.xaxis.set_major_locator(HourLocator(interval=3))
ax.xaxis.set_major_formatter(DateFormatter('%H:%M'))
ax.xaxis.set_minor_locator(HourLocator(interval=1))
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(True)
#plt.show()
plt.savefig('schedule.jpg')


# Adding sources of interest **UNFINISHED, NEED TO ADD OBSERVATION WINDOWS**
ngc = ephem.FixedBody()
ngc.name = 'NGC 1068'
ngc._ra = '02:42:40.7091669408'
ngc._dec = '-00:00:47.859690204'
ngc._epoch = '2000'

txs = ephem.FixedBody()
txs.name = 'TXS 0506'
txs._ra = '05:09:25.9644373872'
txs._dec = '05:41:35.333820420'
txs._epoch = '2000'


# Print the times of interest
print(f"Moonrise Time: {time_of_moonrise.strftime('%m-%d %H:%M')}")
print(f"Moonset Time: {time_of_moonset.strftime('%m-%d %H:%M')}")
print(f"Sunrise Time: {time_of_sunrise.strftime('%m-%d %H:%M')}")
print(f"Sunset Time: {time_of_sunset.strftime('%m-%d %H:%M')}")
print(f"\033[1mStart Time: {start_time.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold
print(f"\033[1mEnd Time: {end_time.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold
if case == 3:
    print(f"\033[1mStart Time 2: {start_time_2.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold
    print(f"\033[1mEnd Time 2: {end_time_2.strftime('%m-%d %H:%M')}\033[0m")   # "\033[1m" and "\033[0m" make the text bold


# Write to text file for trinity.py
with open("eon_times.txt", "w") as file:
    file.write(f"Moonrise Time: {time_of_moonrise}\n")
    file.write(f"Moonset Time: {time_of_moonset}\n")
    file.write(f"Sunrise Time: {time_of_sunrise}\n")
    file.write(f"Sunset Time: {time_of_sunset}\n")
    if case == 3:
        if start_date < end_time:
            file.write(f"Start Time: {start_time.strftime}\n")
            file.write(f"End Time: {end_time}\n")
        else:
            file.write(f"Start Time 2: {start_time_2}\n")
            file.write(f"End Time 2: {end_time_2}\n")
print("Times have been written to eon_times.txt")


# Testing altitude-based start/end times
'''
print(time_of_sunrise_crit)
print(time_of_sunset_crit)
'''