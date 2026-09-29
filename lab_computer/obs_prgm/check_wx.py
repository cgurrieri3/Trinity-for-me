from influxdb import InfluxDBClient
from datetime import datetime,timedelta
import time
import math
import communicate as lets
import parseMetar as pm
import numpy as np
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver import FirefoxOptions 
from selenium.webdriver import FirefoxService
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException
from selenium.webdriver import FirefoxOptions
from selenium.webdriver.common.by import By
from PIL import Image, ImageChops
import cv2



def get_screenshot(link, output_png):
	# lets.communicate('Taking screenshot...')
	# # Create a Firefox webdriver with the specified options
	# #browser = webdriver.Firefox(options=firefox_options)
	# service = FirefoxService(executable_path='/snap/bin/geckodriver')   
	# options = FirefoxOptions()
	# options.add_argument('--headless')  # Run Firefox in headless mode
	# browser = webdriver.Firefox(service=service, options=options) 
	# service = Service(executable_path="/snap/bin/geckodriver")
	# browser = webdriver.Firefox(service=service)
	# # Set up Firefox options for headless mode
	# firefox_options = Options()
	#firefox_options = Options()
	#opts = FirefoxOptions()
	#opts.add_argument("--headless")
	#browser = webdriver.Firefox(options=opts)
	#firefox_options.add_argument('--headless')  # Run Firefox in headless mode
	# Create a Firefox webdriver with the specified options
	#browser = webdriver.Firefox(service=service,options=firefox_options)

	print('Taking screenshot...')

    # Configure Firefox WebDriver for headless mode
	service = Service(executable_path='/snap/bin/geckodriver')  # Path to geckodriver
	options = FirefoxOptions()
	options.add_argument('--headless')  # Run in headless mode

    # Start Firefox browser
	browser = webdriver.Firefox(service=service, options=options)

	browser.get(link)


	#myElem = WebDriverWait(browser, 15).until(EC.presence_of_element_located((By.ID, 'Nearest weather stations')))
	screenshot = browser.save_screenshot(f'Weather_radar/{output_png}')
	browser.quit()
         


def take_screenshot():
	#get_screenshot('https://www.windy.com/-Clouds-clouds?clouds,38.574,-115.032,7','clouds.png') #regular clouds
	#get_screenshot('https://www.windy.com/-Low-clouds-lclouds?lclouds,38.574,-115.032,7', 'clouds.png') #low clouds
	get_screenshot('https://www.windy.com/-Weather-radar-radar?radar,38.565,-115.021,7','radar.png')

def get_difference(png1):
	# Load the two images
	image = cv2.imread(f'/data/TrinityLabComputer/obs_prgm/Weather_radar/{png1}.png')
	image_base = cv2.imread(f'/data/TrinityLabComputer/obs_prgm/Weather_radar/{png1}_baseline.png')
	
	#crop the image
	x,y,width, height = 550,250,400,400
	image1 = image[y:y+height, x:x+width]
	image2 = image_base[y:y+height, x:x+width]

	#black out spots
	x,y,width, height = 740,300,150,150
	image3 = image[y:y+height, x:x+width]
	image4 = image_base[y:y+height, x:x+width]

	
	# Ensure the images have the same dimensions
	if image1.shape != image2.shape:
		raise ValueError("Images must have the same dimensions")
	if image3.shape != image4.shape:
		raise ValueError("Images must have the same dimensions")

	# Compute the absolute difference between the two images
	difference = cv2.absdiff(image1, image2)
	difference_small = cv2.absdiff(image3, image4)

	# Convert the difference image to grayscale
	difference_gray = cv2.cvtColor(difference, cv2.COLOR_BGR2GRAY)
	difference_gray_small = cv2.cvtColor(difference_small, cv2.COLOR_BGR2GRAY)

	# Threshold the difference image to identify regions of significant change
	# threshold = 15  # You may need to adjust this threshold based on your specific use case
	# _, thresholded_diff = cv2.threshold(difference_gray, threshold, 255, cv2.THRESH_BINARY)

	# Compute the Mean Squared Error (MSE)
	mse = np.sum((image1 - image2) ** 2) / float(image1.size)
	mse_small = np.sum((image3 - image4) ** 2) / float(image3.size)
	# Calculate the percentage difference
	percentage_difference = mse * 100
	percentage_difference_small = mse_small * 100

	# log_file(f"{png1} Mean Squared Error (MSE): {mse} ")
	lets.log_file(f"{png1} image difference: {percentage_difference}% ")
	lets.log_file(f"{png1} image difference small region: {percentage_difference_small}% ")
	# Display the original images and the difference image
	# cv2.imshow('Image 1', image1)
	# cv2.imshow('Image 2', image2)
	# cv2.imshow('Difference', difference)
	# cv2.imshow('Image 3', image3)
	# cv2.imshow('Image 4', image4)
	# cv2.imshow('Difference', difference_small)
	# #cv2.imshow('Thresholded Difference', thresholded_diff)

	# cv2.waitKey(0)
	# cv2.destroyAllWindows()

	# image1 =Image.open(f'/data/TrinityLabComputer//obs_prgm/Weather_radar/{png1}.jpg')
	# image2 =Image.open(f'/data/TrinityLabComputer//obs_prgm/Weather_radar/{png1}_baseline.jpg')
	# difference = ImageChops.difference(image2,image1)
	# difference.show()
	if percentage_difference_small > 4000:
		
		lets.communicate(f'{png1} activity seems high')
		return 0
	elif percentage_difference > 5000:
		
		lets.communicate(f'{png1} activity in the 200widex250tall range around Frisco peak skewed west')
		return 1
	else:
		
		lets.communicate(f'{png1} seem clear')
		return 1

def cloud_base_caluation(temp, dewpoint):
	try:
		cloud_base = ((temp - dewpoint)/10) *1247 + 9616 # C, C, m
	except:
		cloud_base = 1234567
	return cloud_base

def airport_report():
	try:
		report=pm.main()
	except:
		report='No airport data available'
	print('---')
	print('Airports Weather Information:')
	lets.communicate(report)
	print('---')


def query_last_wx():
	current_time = datetime.now()
	if current_time.minute % 2 == 0:
		try:
			take_screenshot()
		except: 
			lets.communicate("Screenshot failing moving on...")
	#SIAB_current, HVcurrent, SIMP_Temp, UC_Temp,MUSICpower,HVswitch,hv_Current, hv, asad, tb_current,trigger
	# Local host lines for access
	host = 'localhost'
	port = 8086
	#username = 'mpotts32'
	username = 'admin'
	password = 'Ttys@210'
	database = 'dbWX' # Database for independent measurements for each day, need to change the measurent line in cre_df_list()
    #database = 'Trinity1'

    # Initialize the InfluxDB client and write the points in batches
	client = InfluxDBClient(host = host, port=port, username=username, password=password,database=database)

    # Create a new database if it does not already exist
    # client.create_database(database)
	result = client.query('SHOW MEASUREMENTS')
	measurements = [measurement['name'] for measurement in result.get_points()]
	#print(measurements[-1])

	
	#ORDER BY time DESC LIMIT 1
	query =	f'SELECT * FROM \"{str(measurements[-1])}\" ORDER BY time DESC LIMIT 1'
	#print(query)
	result = client.query(query)
	#print(result)
	points = list(result.get_points())

	#print(points[0])

	all_good = 0

	#weather we care about- humidity, wind DO we want direction?,
	# check time is within last 5 minutes
	wx_influx = points[0]['time']
	try:
		wx_influx = datetime.strptime(wx_influx, "%Y-%m-%dT%H:%M:%SZ")
	except:
		wx_influx = datetime.strptime(wx_influx, "%Y-%m-%dT%H:%M:%S.%fZ")
#	lets.log_file(wx_influx)
	current_utc_time = datetime.utcnow()

	#print(current_utc_time)

	# Calculate the time difference
	time_difference = abs(current_utc_time - wx_influx)
	lets.log_file(f'{time_difference}')

	# Define a time duration of 1 hour
	one_hour = timedelta(hours=1)
	#lets.log_file(time_difference)
	# weather stations is updating
	if time_difference <= one_hour:
		all_good = 1 + all_good
		#print('WX: time good')
	else:
		lets.communicate('WX: Weather station not updating')

	Rhumidity = points[0]['Relative_Humidity']
	TempC = points[0]['Temperature']
	wind_value = points[0]['AverageRelativeWindSpeed']
	wind_direction = points[0]['AverageCorrectedWindDirection']
	dewpoint = points[0]['Dewpoint']
	# print(TempC)
	# print(dewpoint)
	# print(cloud_base_caluation(TempC, dewpoint))
	try:
		cloud_base=cloud_base_caluation(TempC,dewpoint)
		lets.communicate(f'Cloud base: {cloud_base:.0f} ft a.s.l.')
	except:
		cloud_base = 1234567
		lets.communicate('Cloud base: not available')

	try:
		radar=get_difference('radar')
	except:
		lets.communicate('Radar: not available')
		radar = 0	
	#clouds=get_difference('clouds')
	# add another spot that will check a smaller region around milford
	if radar == 0: # clouds == 0 #cloud_base < 11000 or
		lets.log_file('Testing: Weather seems bad ')
		all_good = 1 + all_good 
	else:
		all_good = 1 + all_good 

	if not math.isnan(TempC) and not math.isnan(Rhumidity):
		if TempC <= 0:
			all_good = 1 + all_good

		elif Rhumidity < 90 and TempC > 0: # add cloud base?
			all_good = 1 + all_good
			#print('WX: hum good')
		else:
			lets.communicate(f'WX: Humidity to high = {Rhumidity:.0f} ')

	if not math.isnan(wind_value) and not math.isnan(wind_direction):	
		# wind speeds can be higher when they do not impact the door
		if wind_value < 20 and wind_direction < 315 and wind_direction > 225:
			all_good = 1 + all_good
			#print('WX: Wind good')
		elif wind_value < 15.5:
			all_good = 1 + all_good
		else:
			lets.communicate(f'WX: Wind to high = {wind_value:.0f} ')
		

	#print(all_good)
	if all_good == 4:
		
		return 1
	else:
		
		return 0

#get_screenshot('https://www.windy.com/-Weather-radar-radar?radar,38.565,-115.021,7','radar.png')
query_last_wx() # if the weather is not good this will alert the user on bootup of trinity.py
airport_report()
