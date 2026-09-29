#!/usr/bin/env python3
import pandas as pd
import math
import time
import argparse

# Create ArgumentParser object
parser = argparse.ArgumentParser(description='Example of command-line arguments with flags')

# Add arguments with flags
parser.add_argument('-p', '--pixel', type=int, help='Pixel number 0-256')
parser.add_argument('-th', '--threshold', type=str, help='Threshold value you want to be set 1-249')

# Parse the command-line arguments
args = parser.parse_args()

# Access the values of arguments using the flags
pixel = args.pixel
threshold = args.threshold

def get_r(x):
    x = x%1
    if x >= 0.5:
        x -= 0.5
    result=x/(0.0625)
    return result
  
def get_v(x):
    # uses the method in the excel sheet to get the -v value
    # https://gtvault-my.sharepoint.com/personal/aotte6_gatech_edu/Documents/SPB2/Software%20Group/Music%20Register%20Tables/Music_Register_Table.xlsx
    # its based off of binary
    shift = x << 3
    result = shift + 61440 +7
    return result
    

def get_values(p, th):
    # get the hot pixel chart
    
    pixel= int(p)
    # fill in the values to be used on the commands of the hot pixels
    r= pixel/16
    if r % 1 >= 0.5:
        m = 'top'
    else:
        m= 'bottom'

    s = int(r)
    r = get_r(r)
    r = int(r)
    v = get_v(int(th))
    
    
    return print(f'SIAB MusicRegWrite -m {m} -r {r} -v {v} -s {s}-{s} ')
    
    
if __name__ == '__main__':
    get_values(pixel,threshold)
    
    
   
    


    
