#!/usr/bin/env python3

import sys
import numpy as np
import pickle
import datetime
import os
import math
import time
import matplotlib.pyplot as plt
from PIL import ImageTk, Image
from astropy.io import ascii

class weather_plots:
    def __init__(self,WXDIR,WPDIR):
        # data string format of the wx station
        # Relative here refers to the notch that is in the wx station that has been position
        # so that it faces North
        self.array_titles = (
            "Node", "RelativeWindDirection", "RelativeWindSpeed",
            "CorrectedWindDirection","AverageRelativeWindDirection","AverageRelativeWindSpeed",
            "RelativeGustDirection", "RelativeGustSpeed","AverageCorrectedWindDirection",
            "WindSensorStatus", "Pressure", "Pressure at Sea level",
            "Pressure at Station", "Relative Humidity","Temperature",
            "Dewpoint","Absolute Humidity", "compassHeading",
            "WindChill", "HeatIndex", "AirDensity", "WetBulbTempature",
            "SunRiseTime", "SolarNoonTime", "SunsetTime",
            "Position of the Sun", "Twilight (Civil)","Twilight (Nautical)",
            "Twilight (Astronomical)", "X-Tilt", "Y-Tilt",
            "Z-Orientation", "User Information Field","System Date and Time",
            "Supply Voltage", "Status", "Checksum")

        # Define Directories
        self.WXDIR = WXDIR
        self.WPDIR = WPDIR

        # iterate over the files in WXDIR to parse out all data
        self.filelist = os.popen(f'ls -rt {WXDIR} | tail -n2').read().split('\n')[:2]
        for filename in self.filelist:
            globals()[f'{filename}_data'] = self.weather_data(f'{self.WXDIR}/'+filename)
            
            # init the data arrays and fill them
            globals()[f'{filename}_size'] = len(globals()[f'{filename}_data'])
            globals()[f'{filename}_wind_direction'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_wind_gust_direction'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_wind_gust_speed'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_date_n_time'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_date'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_tempature'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_wind_speed'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_humidity'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_dew_point'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_pressure'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_sunrise'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_sunset'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_civil_twi'] = [0] * globals()[f'{filename}_size']
            globals()[f'{filename}_astro_twi'] = [0] * globals()[f'{filename}_size']
            self.fill_arrays(filename,globals()[f'{filename}_data'])

        # Init figure that will make all the weather data plots
        self.fig = plt.figure(1)
        self.all_wx_plot()
        plt.close(self.fig)

        self.insta_wx_plot()
        self.fig3, self.ax = plt.subplots(figsize=(7, 6))
        plt.axis('equal')  # do not remove this makes it work??? .... magic
        self.wind_speed_plot(globals()[f'{self.filelist[-1]}_wind_speed'][-1],globals()[f'{self.filelist[-1]}_wind_direction'][-1],globals()[f'{self.filelist[-1]}_wind_gust_speed'][-1],globals()[f'{self.filelist[-1]}_wind_gust_direction'][-1])

    def weather_data(self, file):
        try:
            weather_data_array = ascii.read(file, format='no_header', data_start=0, delimiter=',', names=self.array_titles)
            log_out = ascii.read(file, delimiter=',', names=self.array_titles)

            #print('\n')
            #print(log_out[-1]) # print wx to terminal


            # converts the data to an numpy array to be able to use standard array access
            output_array = np.array(weather_data_array)

            for i in range(len(output_array)):
                for j in range(len(output_array[0])):
                    output_array[i][j] = ''.join(str(output_array[i][j]).split(','))
                    return output_array
        except InconsistentTableError:
            print('Inconsistant Table Error: Check WX file! Logging of the WX station may have an issue!')

    def fill_arrays(self, filename, all_data):
        # arrays are loaded with data
        for i in range(globals()[f'{filename}_size']):
            globals()[f'{filename}_tempature'][i] = float(all_data[i][14])
            globals()[f'{filename}_wind_speed'][i] = float(all_data[i][2])
            globals()[f'{filename}_wind_gust_speed'][i] = float(all_data[i][7])
            globals()[f'{filename}_humidity'][i] = float(all_data[i][13])
            globals()[f'{filename}_dew_point'][i] = float(all_data[i][15])
            globals()[f'{filename}_pressure'][i] = float(all_data[i][12])
            globals()[f'{filename}_wind_direction'][i] = int(all_data[i][1])
            globals()[f'{filename}_wind_gust_direction'][i] = int(all_data[i][6])
            globals()[f'{filename}_date_n_time'][i] = str(all_data[i][33])
            globals()[f'{filename}_date'][i] = datetime.datetime.strptime(str(all_data[i][33])[:19], "%Y-%m-%dT%H:%M:%S")
            globals()[f'{filename}_sunrise'][i] = str(all_data[i][22])
            globals()[f'{filename}_sunset'][i] = str(all_data[i][24])
            globals()[f'{filename}_civil_twi'][i] = str(all_data[i][26])
            globals()[f'{filename}_astro_twi'][i] = str(all_data[i][28])

    def all_wx_plot(self):
        def plot_format(x, y, xlabel, ylabel, title, color, fig, thick=1):

                    plt.plot(x, y, c=color, linewidth=thick)
                    plt.title(title)
                    plt.xlabel(xlabel)
                    plt.ylabel(ylabel)
                    try:
                        plt.xticks(rotation = 25)
                    except:

                    #plt.xticks(x.to_pydatetime(),rotation=25)
                        plt.xticks(x[::50000], rotation=25)

                    #plt.locator_params(axis='x', nbins=3)

                    fig.canvas.draw()  # create the canvas
                    plt.savefig(f"{self.WPDIR}/{title.replace(' ','_')}.png")  # save the image as a png
                    pickle.dump(fig, open(f"{self.WPDIR}/{title.replace(' ','_')}",'wb'))  # save the image as a pickle so that it can be view in python again
                    fig.clear()  # clear the plot
                    fig.clf()  # clear the plot ----- This is import so that only one plot is used and reducind the ram needed to run the program


        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_wind_direction']+globals()[f'{self.filelist[1]}_wind_direction'], 'Date', 'Wind Direction (Degrees)', 'Wind Direction','green', self.fig, thick=0.1)
        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_tempature']+globals()[f'{self.filelist[1]}_tempature'], 'Date', 'Tempature (Degrees C)', 'Temperature', 'red', self.fig)
        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_humidity']+globals()[f'{self.filelist[1]}_humidity'], 'Date', 'Humidity (Percent)', 'Humidity', 'blue', self.fig)
        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_wind_speed']+globals()[f'{self.filelist[1]}_wind_speed'], 'Date', 'Wind Speed (Knots)', 'Wind Speed', 'purple', self.fig,thick=0.1)
        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_dew_point']+globals()[f'{self.filelist[1]}_dew_point'], 'Date', 'Dew Point (Degrees C)', 'Dewpoint', 'orange', self.fig)
        plot_format(globals()[f'{self.filelist[0]}_date']+globals()[f'{self.filelist[1]}_date'], globals()[f'{self.filelist[0]}_pressure']+globals()[f'{self.filelist[1]}_pressure'], 'Date', 'Pressure (hPa)', 'Pressure', 'pink', self.fig)

    def insta_wx_plot(self):
        tempature = float(globals()[f'{self.filelist[1]}_tempature'][-1])
        wind_speed = float(globals()[f'{self.filelist[1]}_wind_speed'][-1])
        wind_direction = int(globals()[f'{self.filelist[1]}_wind_direction'][-1])
        gust_speed = float(globals()[f'{self.filelist[1]}_wind_gust_speed'][-1])
        gust_direction = int(globals()[f'{self.filelist[1]}_wind_gust_direction'][-1])
        humidity = float(globals()[f'{self.filelist[1]}_humidity'][-1])
        dew_point = float(globals()[f'{self.filelist[1]}_dew_point'][-1])
        pressure = float(globals()[f'{self.filelist[1]}_pressure'][-1])
        date_n_time = globals()[f'{self.filelist[1]}_date_n_time'][-1]
        sun_rise = str(globals()[f'{self.filelist[1]}_sunrise'][-1])
        sun_set = str(globals()[f'{self.filelist[1]}_sunset'][-1])
        #civil_twi = str(globals()[f'{self.filelist[1]}_civil_twi'][-1])
        #astro_twi = str(globals()[f'{self.filelist[1]}_astro_twi'][-1])

        plt.figure(figsize=(7, 6))
        x = np.arange(-10, 10, 0.01)
        y = x ** 2
        
        weather_label_text = [f"Temperature:      {tempature}{chr(176)}C",
                              f"Wind Speed:       {wind_speed} kts",
                              f"Wind Direction:   {wind_direction}{chr(176)}",
                              f"Gust Speed:       {gust_speed} kts",
                              f"Gust Direction:   {gust_direction}{chr(176)}",
                              f"Humidity:         {humidity}%",
                              f"Pressure:         {pressure} hPa",
                              f"Dew Point         {dew_point}{chr(176)}C",
                              f"Sunrise:          {sun_rise}Z",
                              f"Sunset:           {sun_set}Z"] 
        # adding text inside the plot
        plt.text(-10, 0, '\n'.join(weather_label_text) , fontsize=24 , fontfamily='monospace',horizontalalignment='left')
        
        
        plt.plot(x, y, c='g', alpha=0)
        plt.axis('off')
        plt.savefig(f"{self.WPDIR}/wind_label_readout.png")
        plt.close()

    
################### Wind direction arrows #################
            # def to make the angle and all the other information on the plot
    def wind_speed_plot(self,wind_speed, angle,gust_speed,gust_angle):
        self.ax.set_xlim(-50, 50)
        self.ax.set_ylim(-50, 50)
        plt.axis('off')
        # plots the compass pieces
        plt.text(-1.7, 36, 'N', dict(size=20))
        plt.text(-1.7, -38, 'S', dict(size=20))
        plt.text(-39, -2, 'W', dict(size=20))
        plt.text(35, -2, 'E', dict(size=20))
        
        # adds the windspeed labels for the concentric circles
        plt.text(-4.3, -8, '10 m/s', dict(size=10))
        plt.text(-4.3, -18, '20 m/s', dict(size=10))
        plt.text(-4.3, -28, '30 m/s', dict(size=10))
        
        # takes the angle from degrees to radians
        cartesianAngleRadians = (450 - angle) * math.pi / 180.0
        gcartesianAngleRadians = (450 - gust_angle) * math.pi / 180.0
        # creates the length of change of the arrows based on the size of windspeed
        terminus_x = wind_speed * math.cos(cartesianAngleRadians)
        terminus_y = wind_speed * math.sin(cartesianAngleRadians)
        gterminus_x = gust_speed * math.cos(gcartesianAngleRadians)
        gterminus_y = gust_speed * math.sin(gcartesianAngleRadians)
        
        # plots the circles for wind speed
        low_speed = plt.Circle((0, 0), 10, fill=False, color='black')
        meduim_speed = plt.Circle((0, 0), 20, fill=False, color='black')
        high_speed = plt.Circle((0, 0), 30, fill=False, color='black')
        
        # plotting the n*pi/2  tick marks
        plt.arrow(0, 30, 0, 2.4, head_width=0, width=0.25, color='black')
        plt.arrow(30, 0, 2.4, 0, head_width=0, width=0.25, color='black')
        plt.arrow(-30, 0, -2.4, 0, head_width=0, width=0.25, color='black')
        plt.arrow(0, -30, 0, -2.4, head_width=0, width=0.25, color='black')
        
        # plotting the n*pi/4  tick marks
        plt.arrow(21.2, 21.2, 2.4, 2.4, head_width=0, width=0.25, color='black')
        plt.arrow(-21.2, -21.2, -2.4, -2.4, head_width=0, width=0.25, color='black')
        plt.arrow(-21.2, 21.2, -2.4, 2.4, head_width=0, width=0.25, color='black')
        plt.arrow(21.2, -21.2, 2.4, -2.4, head_width=0, width=0.25, color='black')
        
        # based on different wind speeds differnet arrows are plotted
        if wind_speed <= 10:  # low speeds will be blue
            plt.arrow(0, 0, terminus_x, terminus_y, head_width=1, width=0.5, color='blue')
            plt.arrow(0,0,gterminus_x,gterminus_y,head_width=1, width=0.5,color='blue',alpha=0.5)
        elif 10 < wind_speed < 25:  # meduim speeds will be black
            plt.arrow(0, 0, terminus_x, terminus_y, head_width=2, width=0.75, color='black')
            plt.arrow(0,0,gterminus_x,gterminus_y,head_width=2, width=0.75,color='black',alpha=0.5)
        elif wind_speed >= 25:
        # if the wind speed is greater than 35 m/s then the arrow will stop getting bigger but it will turn red to indicate
            # high wind speed
            #terminus_x = 35 * math.cos(cartesianAngleRadians)
            #terminus_y = 35 * math.sin(cartesianAngleRadians)
            plt.arrow(0, 0, terminus_x, terminus_y, head_width=3, width=1, color='red')
            plt.arrow(0,0,gterminus_x,gterminus_y,head_width=3, width=1,color='red',alpha=0.5)

        # puts the speed cicles on the plot
        self.ax.add_patch(low_speed)
        self.ax.add_patch(meduim_speed)
        self.ax.add_patch(high_speed)
        
        plt.savefig(f'{self.WPDIR}/wind_direction.png')
        self.fig3.clear()  # clear the plot
        self.fig3.clf()
    
if __name__ == '__main__':
    while True:
        weather_plots(sys.argv[1],sys.argv[2])
        time.sleep(60)
        
    

