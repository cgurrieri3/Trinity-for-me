#!/usr/bin/python3

#import libs
import os
import argparse
from multiprocessing import Pool


HOME = '/data/TrinityLabComputer/cams'

def make_vids(date,cam):
    # Run bash commands to do everything
    # turns out it is much more efficient than OpenCV
    print("{0} {1}".format(date,cam))
    os.system("mkdir -p {0}/{1}/{2} && cd {0}/{1}/{2} && /bin/mv {0}/{1}/{2}*.jpg {0}/{1}/{2} && ffmpeg -framerate 30 -pattern_type glob -i '{2}*.jpg'   -c:v libx264 -pix_fmt yuv420p {1}_{2}.mp4 && /bin/cp {1}_{2}.mp4 {0}/VIDS && /bin/tar cfz {0}/{1}/{2}.tar.gz -C {0}/{1}/{2} . --remove-files".format(HOME,cam,date))
    
def make_vids_horizon(date,cam):
    # Run bash commands to do everything
    # turns out it is much more efficient than OpenCV
    print("{0} {1}".format(date,cam))
    os.system("mkdir -p {0}/{1}/jpg/{2} && cd {0}/{1}/jpg/{2} && /bin/mv {0}/{1}/jpg/{2}*.jpg {0}/{1}/jpg/{2} && ffmpeg -framerate 15 -pattern_type glob -i '{2}*.jpg'   -c:v libx264 -pix_fmt yuv420p {1}_{2}.mp4 && /bin/cp {1}_{2}.mp4 {0}/VIDS && /bin/tar cfz {0}/{1}/jpg/{2}.tar.gz -C {0}/{1}/jpg/{2} . --remove-files".format(HOME,cam,date))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='A script to create videos of the photos taken by the cameras on the demonstrator site')
    parser.add_argument('-d',metavar='DATE',type=int,help='Enter the date in the format YYYYMMDD',required=True)
    args = parser.parse_args()
    date = args.d
    inputs = [(date,"IN"),(date,"OUT")]
    print(date,inputs)
    with Pool() as pool:
        pool.starmap(make_vids, inputs)
        
    make_vids_horizon(date,'Horizon')
