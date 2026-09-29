#!/usr/bin/env python3
#import matplotlib
import os
import subprocess
import time
import sys

# import all the libraries
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw
import matplotlib.pyplot as plt
import numpy as np
import cv2

#print(matplotlib.get_backend())
#matplotlib.use('TkAgg')
#print(matplotlib.get_backend())

#matplotlib.use('Qt4Agg')
#print(matplotlib.get_backend())

#matplotlib.use('MPLBACKEND')
#print(matplotlib.get_backend())

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/mpotts32/cams/horizon_snap.log", "a") as file:
        file.write(message + str(current_time) + '\n')


def lab_directory():
	# directory with all expect scripts are located
	directory = os.path.expanduser("~/control_comp")
	os.chdir(directory)


def run_ssh(command):
	# Run the command and capture its output
	result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# Remove ANSI escape codes
	#result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
	# Check for errors
	if result.returncode == 0:
	    # Command was successful
	    # print(f"{command} output:")
	    # print(result.stdout)
	    print(f"{command} output: {result.stdout} ")
	    log_file(f"{command} output: {result.stdout} ")
	    
	else:
	    # Command encountered an error
	    log_file(f"{command} output error: {result.stderr} ")
	    
	#return result.stdout


def horizon_snap(dur, unit):
	lab_directory()
	# The command you want to run
	command = f'./horizon_snap.exp {dur} {unit}'  # Replace with your desired command
	run_ssh(command)
	
def sync_folder():
	
	directory = os.path.expanduser("~/cams")
	os.chdir(directory)
	
	command = f'./update_horizon_only.exp'  # Replace with your desired command
	run_ssh(command)

def website_upload():
	
	directory = os.path.expanduser("~/cams")
	os.chdir(directory)
	
	command = f'./update_site_horizon_only.exp'  # Replace with your desired command
	run_ssh(command)


def get_file_size():
	file_path=get_most_recent_file('/home/mpotts32/cams/Horizon/jpg/')
	size = os.path.getsize(file_path)
	return size/1000

def analyze_image(image_path):
    # Read the image
    image = cv2.imread(image_path)
    
    # Convert image to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    
    # Calculate average pixel intensity
    avg_intensity = np.mean(gray)
    
    return avg_intensity

def adjust_exposure(avg_intensity, target_intensity):
    file_path_exposure = "/home/mpotts32/cams/exposure.txt"
    with open(file_path_exposure, "r") as file:
      # Read the entire contents of the file
      current_exposure = file.read()
    #print(f'Prior exp: {current_exposure}')
    # Adjust exposure time based on difference in intensities
    exposure_adjustment_factor = target_intensity / avg_intensity
    new_exposure = float(current_exposure) * float(exposure_adjustment_factor)
    
    # Limit the new exposure time within certain bounds
    min_exposure = 1  # Minimum exposure time (adjust as needed)
    max_exposure = 180000000  # Maximum exposure time (adjust as needed)
    new_exposure = max(min(new_exposure, max_exposure), min_exposure)
    #print(f'float new exposure: {new_exposure}')
    with open(file_path_exposure, "w") as file:
      # Read the entire contents of the file
      file.write(f'{new_exposure}')
    return int(new_exposure)
 
 
def get_most_recent_file(folder_path):
    # Get a list of all files in the folder
    files = [os.path.join(folder_path, f) for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
    files.remove(f'{folder_path}horizon_snap.jpg')
    files = [file for file in files if not file.endswith(".tar.gz")]

    
    # If there are no files in the folder, return None
    if not files:
        return None

    # Get the most recent file based on modification time
    most_recent_file = max(files, key=os.path.getmtime)
    
    return most_recent_file

def convert_format_date(td):
	tdmod = td.replace("-", ":")
	tdmod = tdmod.replace("_", " ")
	tdmod = tdmod + ' UTC'
	return tdmod
 
def set_units(exp):
  units = ''
  if exp > 5746032:
    new_exp = int(exp / 10**6)
    units = 's'
    
    log_file(f'Convert exposure {new_exp} ')
    log_file(f'Convert units {units}')
  return new_exp, units

def add_time_stamp():
	# image opening
	latest_file = get_most_recent_file('/home/mpotts32/cams/Horizon/jpg/')
	
	#latest_file = '/home/mpotts32/cams/Horizon/jpg/05-02-2024_21-21-57.jpg'
	image = Image.open(latest_file)
	# this open the photo viewer
	# Image.composite(image, Image.new('RGB', image.size, 'white'), image).show() 
	# base.paste(image, box, image)
	#image.show() 
	#plt.imshow(image)
	
	# text Watermark
	watermark_image = image.copy()
	 
	draw = ImageDraw.Draw(watermark_image)
	# ("font type",font size)
	w, h = image.size
	x, y = int(w / 9), int(h / 30)
	
	# Define the coordinates for the box
	box_left = 17
	box_top = 17
	box_right = box_left + 250  # Adjust this value according to the desired width of the box
	box_bottom = box_top + 21  # Adjust this value according to the desired height of the box

	# Define the color of the box (light grey)
	box_color = (220,220,220)  # You can adjust the RGB values to get the desired shade of grey

	   
	fnt = ImageFont.truetype("~Pillow/Tests/fonts/FreeMono.ttf", 17)
	timestamp = convert_format_date(latest_file[32:-4])
	
	draw.rectangle([box_left, box_top, box_right, box_bottom], fill=box_color)
	draw.text((x, y), timestamp , fill=(0,0,0), anchor='ms',font=fnt)

	# Define the coordinates for the box
	box_left = 750
	box_top = 500
	box_right = box_left + 100  # Adjust this value according to the desired width of the box
	box_bottom = box_top + 100  # Adjust this value according to the desired height of the box

	# Define the color of the box (light grey)
	outline_color = (225, 0, 0)  # You can adjust the RGB values to get the desired shade of grey
	draw.rectangle([box_left, box_top, box_right, box_bottom], outline=outline_color)
	plt.subplot(1, 2, 2)
	plt.title("white text")
	#plt.imshow(watermark_image)
	
	# add image water mark 
	# to open the image
	Logo_image = Image.open("/home/mpotts32/cams/TrinityLogo.png")
	# this open the photo viewer
	#Logo_image.show()
	#plt.imshow(Logo_image)
	 
	# add watermark
	size = (150, 300)
	Logo_image.thumbnail(size)
	Logo_image = Logo_image.copy()
	# base image
	watermark_image.paste(Logo_image, (1100, 885))
	
	#watermark_image.show() 
	watermark_image.save("/home/mpotts32/cams/Horizon/jpg/horizon_snap.jpg")


# check the exposure
avg_intensity=analyze_image("/home/mpotts32/cams/Horizon/jpg/horizon_snap.jpg")
log_file(f'avg_int: {avg_intensity}')
new_exp=adjust_exposure(avg_intensity, 80.0)
log_file(f'new exposure: {new_exp}')

log_file('Starting process ')
#new_exp =  111111111
new_exp, units=set_units(new_exp)
#new_exp = 111
#units = 's'
horizon_snap(new_exp,units)
log_file('Image Created ')
sync_folder()
log_file('Folder sync')
add_time_stamp()
log_file('timestamp added ')

website_upload()
log_file('uploaded to website ')
sys.exit(0)






# get the time and the moon 

# set the exposure 

# take the picture

# add time stamp and trinity logo

# resave the image
