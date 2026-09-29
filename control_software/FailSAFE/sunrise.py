import ephem
from datetime import datetime, timedelta
import logging

logging.basicConfig(filename='/home/trinity/control_software/FailSAFE/sunriseCheck.log', level=logging.INFO,format='%(asctime)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')

# Define observer location (latitude and longitude)
observer = ephem.Observer()
observer.lat = '38.5202'
observer.lon = '-113.2883'
observer.elevation = 3048
observer.horizon = '-8'
# Compute sunrise time for tomorrow
tomorrow = datetime.utcnow() + timedelta(days=1)
observer.date = tomorrow
sunrise = observer.previous_rising(ephem.Sun())

# Format sunrise time
sunrise_time = ephem.localtime(sunrise).strftime('%Y-%m-%d %H:%M:%S')


# Write sunrise time to file
with open('/home/trinity/control_software/FailSAFE/sunrise.txt', 'w+') as f:
    f.write(sunrise_time)
    f.write('\n')

logging.info(f'Added sunrise_time: {sunrise_time} ')


