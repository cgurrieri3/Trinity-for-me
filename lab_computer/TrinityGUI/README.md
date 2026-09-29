# TrinityGUI
The GUI for the Trinity Demonstrator Telescope
##################################################################
################### Trinity GUI ##################################
#####  
##### The goal of this code is to take the weather data from the weeather station at the trinity telescope location
##### and create a user friendly way to view it. This code features images from camera feeds, compass plot of wind data
##### current weather stats, and weather plots. IN DEVELOPMENT: searching weather data which will call the database
##### and opening and closing the doors to the building.
#####
##### Installation to a new computer: ( compatable for windows and linux)
##### Place the TrinityGUI.py code where needed and be sure directories can be made then update the paths for
##### HOMEDIR,CAMDIR,INCAMDIR,OUTCAMDIR,WXDIR,WPDIR. Set the refresh rate. Then the location of the screen shot will need to be adjusted
##### search in the code for "im = ImageGrab.grab(bbox=left_x,top_y,right_x,bottom_y" and edit the 4 numbers in that line to get the correct location
##### for the weather data. Make sure all the packages are install. This should be a complete installation
#####
##### During Run:
#### There are still some bugs:
#### The system refreshes after some time so if the system is unresponsive
#### is is because the data is being reanalyzed. THis was never put to a background refresh process. It is normall with
#### different computers to have different spacing so it is possible buttons can be cut off.
#### Place is used throughout this code and can not be combined with pack or grid in tkinter
####
#### Errors
#### is a max() error comes up it means the directory for cam images/weather is empty check the file path and make sure they are correct and have data.
##################################################################
