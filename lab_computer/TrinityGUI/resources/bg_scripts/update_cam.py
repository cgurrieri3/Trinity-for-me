#!/usr/bin/env python3
# Import Libs
import subprocess
import sys
import pexpect
import time
import os

# Which image?
IMG = sys.argv[1]

# Path to the program resources directory
PATH = sys.argv[2]

while True: 
    # Image from web
    cmd = f'wget -q https://trinity.physics.gatech.edu/wp-content/uploads/demonstrator-uploads/{IMG}.jpg -O {PATH}/{IMG}.jpg'
    subprocess.Popen(cmd.split())
    time.sleep(60)
