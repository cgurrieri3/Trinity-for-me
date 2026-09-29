#!/usr/bin/python3

# Import Libs
import tkinter as tk
import glob
import os
import sys
import time
import argparse
import matplotlib.pyplot as plt
from PIL import ImageTk, Image
import datetime
from astropy.io import ascii
import numpy as np
import pickle
import subprocess
import pexpect
import ast
from tkinter import messagebox
from widgets.cameras import CameraWidget
from widgets.weather import WeatherWidget
from widgets.outlets import OutletWidget
from widgets.door import DoorWidget
from widgets.runlist import RunlistWidget

#######################################################
################## User File paths ####################

# Linux Machine in lab
LHOMEDIR = "/home/mpotts32/"
LCAMDIR = LHOMEDIR + "cams/" # directory for the camera
os.environ['LINCAMDIR'] = LCAMDIR + "IN/" # directory for the inside camera
os.environ['LOUTCAMDIR'] = LCAMDIR + "OUT/" # directory for the outside camera
os.environ['LWXDIR'] = LHOMEDIR + "weather/" # director for the path of the weather data # set this as an environmental variable
LWPDIR = LHOMEDIR + 'weather_plots/' # directory for the weather plots

# Linux Machine anywhere
HOMEDIR   = os.path.abspath(os.curdir)
RESDIR    = HOMEDIR + "/resources/"
WIDDIR    = HOMEDIR + "/widgets/"
os.environ['BINDIR']    = RESDIR + "/bg_scripts/" # set this as an environmental variable
os.environ['CAMDIR']    = RESDIR + "cams/" # set this as an environmental variable
os.environ['WXDIR']     = RESDIR + "weather/"# set this as an environmental variable
os.environ['WPDIR']     = RESDIR + "weather_plots/"
########## End of File paths ##########################
#######################################################


class GUI:
    def __init__(self, root, LCF):
        self.root = root
        self.root.title("Main Application")

        self.LCF = LCF

        logo_path = RESDIR + "/trinity-logo.png"
        global logo
        logo = ImageTk.PhotoImage(Image.open(logo_path).resize((75, 75))) # get the image import into tkinter
        logo_label = tk.Label(root, image = logo)
        logo_label.pack()
        
        main_label = tk.Label(root, text="Trinity GUI")
        main_label.pack(padx=20, pady=20)
        
        page1_button = tk.Button(root, text="Cameras", command=self.open_cameras)
        page1_button.pack(pady=10)
        
        page2_button = tk.Button(root, text="Weather", command=self.open_weather)
        page2_button.pack(pady=10)

        outlets_button = tk.Button(root, text="Outlets", command=self.open_outlets)
        outlets_button.pack(pady=10)

        door_button = tk.Button(root, text="Door", command=self.open_doors)
        door_button.pack(pady=10)

        runlist_button = tk.Button(root, text="Observation Run", command=self.open_runlist)
        runlist_button.pack(pady=10)

        SAM_button = tk.Button(root, text="SAM", command=self.open_SAM)
        SAM_button.pack(pady=10)
        
        quit_button = tk.Button(root, text="Quit", command=self.root.quit)
        quit_button.pack(pady=20)
    
    def open_cameras(self):
        CameraWidget(self.root)
    
    def open_weather(self):
        WeatherWidget(self.root)

    def open_outlets(self):
        OutletWidget(self.root,self.LCF)

    def open_doors(self):
        DoorWidget(self.root,self.LCF)

    def open_runlist(self):
        RunlistWidget(self.root)

    def open_SAM(self):
        SAM = WIDDIR + "/SAM"
        cmd = f"python3 {SAM}/SPB2Obs.py -loc {SAM}/gpsLocation.txt -obj {SAM}/input0.txt -elv 2000"
        print(cmd)
        return subprocess.Popen(cmd.split())
        
        
def main(LCF):
    root = tk.Tk()
    app = GUI(root,LCF)
    root.mainloop()

# This function is used to read an SSH tunnel to the lab computer.
# This way the GUI can be used anywhere provided you have GT creds.
def create_tunnel():
    print(f"Creating SSH Tunnel to {os.environ['PHYS_HOST']} on port {os.environ['PORT']}")
   

    # SSH into the first computer and establish an SSH tunnel
    ssh_cmd = f"ssh -o StrictHostKeyChecking=no {os.environ['GT_USR']}@{os.environ['GT_HOST']} -L {os.environ['PORT']}:{os.environ['PHYS_HOST']}:22"
    print(ssh_cmd)
    try:
        ssh = pexpect.spawn(ssh_cmd)
        # Expect the password prompt and send the password
        ssh.expect('password:')
        ssh.sendline(os.environ['GT_PASS'])
    except:
        print("ssh: connect to host ssh.physics.gatech.edu port 22: Connection refused")
        print("Please try again in a couple minutes.")
        sys.exit(1)
    return ssh

def update_images():
    print("Camera Images BG Process Start...")
    CAMS = ['IN','OUT']
    proc_ls = []
    for cam in CAMS:
        cmd = f'python3 {os.environ["BINDIR"]}/update_cam.py {cam}CAM {os.environ["CAMDIR"]}'
        #print(cmd)
        proc_ls.append(subprocess.Popen(cmd.split()))
    return proc_ls

def update_wx(LCF):
    print("Weather Data BG Process Start...")
    cmd = f"python3 {os.environ['BINDIR']}/update_wx.py {os.environ['PORT']} {os.environ['PHYS_USR']} {os.environ['PHYS_PASS']} {os.environ['LWXDIR']} {os.environ['WXDIR']} {LCF}"
    #print(cmd)
    return subprocess.Popen(cmd.split())

def update_pdu_status(LCF):
    print("PDU Status BG Process Start...")
    cmd = f"python3 {os.environ['BINDIR']}/update_pdu_status.py {os.environ['PORT']} {os.environ['PHYS_USR']} {os.environ['PHYS_PASS']} {LCF}"
    #print(cmd)
    return subprocess.Popen(cmd.split())

def update_wx_plots():
    print("Weather Plots BG Process Start...")
    cmd = f"python3 {os.environ['BINDIR']}/weather_plots.py {os.environ['WXDIR']} {os.environ['WPDIR']}"
    #print(cmd)
    return subprocess.Popen(cmd.split())

def parse_args():
    parser = argparse.ArgumentParser(description = 'Trinity Demonstrator Control GUI. It starts several background processes that facilitate widgets that allow you to control, view, and run the Trinity Demonstrator.')
    parser.add_argument('-lcf', action='store_true', default=False, help='Use this flag only if you are on the GT Lab computer PHYS43199.')
    return parser.parse_args()

def login_info(LCF):
    if not LCF:
        ########## Login Information ##########################
        #######################################################
        if os.path.isfile(RESDIR + "/tmp.txt"):
            with open(RESDIR + "/tmp.txt") as f:
                lines = f.readlines()
                os.environ['PORT']      = lines[0].split()[0]    
                os.environ['GT_HOST']   = lines[1].split()[0]
                os.environ['GT_USR']   = lines[1].split()[1]
                os.environ['GT_PASS']   = lines[1].split()[2]
                os.environ['PHYS_HOST'] = lines[2].split()[0]
                os.environ['PHYS_USR']  = lines[2].split()[1]
                os.environ['PHYS_PASS'] = lines[2].split()[2]
                os.environ['CTRL_PASS'] = lines[3].split()[0]
        else:
            os.environ['GT_HOST']   = 'ssh.physics.gatech.edu'
            os.environ['GT_USR']    = input("GT Username: ")
            os.environ['GT_PASS']   = input("GT Password: ")
            os.environ['PHYS_HOST'] = 'phys43199.physics.gatech.edu'
            os.environ['PHYS_USR']  = input(f"{PHYS_HOST} Username: ")
            os.environ['PHYS_PASS'] = input(f"{PHYS_HOST} Password: ")
            os.environ['PORT']      = input("Input Tunnel Port: ")
            os.environ['CTRL_PASS'] = input("Control PC Password: ")
            print(f"{os.environ['PORT']}\n{os.environ['GT_HOST']} {os.environ['GT_USR']} {os.environ['GT_PASS']}\n{os.environ['PHYS_HOST']} {os.environ['PHYS_USR']} {os.environ['PHYS_PASS']}\n{os.environ['CTRL_PASS']}",file=open(RESDIR + '/tmp.txt','w'))
            ########## End of Login Information ###################
    else:
        os.environ['GT_HOST']   = 'tmp'
        os.environ['GT_USR']    = 'tmp'
        os.environ['GT_PASS']   = 'tmp'
        os.environ['PHYS_HOST'] = 'tmp'
        os.environ['PHYS_USR']  = 'tmp'
        os.environ['PHYS_PASS'] = 'tmp'
        os.environ['PORT']      = 'tmp'
        os.environ['CTRL_PASS'] = 'tmp'

if __name__ == "__main__":
    args = parse_args()
    LCF = args.lcf

    login_info(LCF)
    
    if not LCF: # if Remote computer open tunnel
        tunnel = create_tunnel()

        # Allow time for tunnel to open
        print("Allowing the tunnel time to establish... Sleeping 5 seconds")
        time.sleep(5)

    # Enable the background scripts that update the weather and cameras
    cam_procs = update_images()
    print(cam_procs)
    wx_proc = update_wx(LCF)
    print(wx_proc)
    pdu_proc = update_pdu_status(LCF)
    print(pdu_proc)
    wx2_proc = update_wx_plots()
    print(wx2_proc)

    # Allow time for bg_scripts to execute fully
    print("Starting up background scripts... Sleeping 20 seconds")
    time.sleep(20)

    # Start GUI
    main(LCF)

    # End background scripts
    for proc in cam_procs:
        print(proc)
        proc.kill()
    print(wx_proc)
    wx_proc.kill()
    print(pdu_proc)
    pdu_proc.kill()
    print(wx2_proc)
    wx2_proc.kill()

    if not LCF: # if Remote computer close tunnel
        tunnel.close()
