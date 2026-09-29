#!/usr/bin/env python3
import requests
import subprocess
import os
import time

def is_internet_connected():
    try:
        # Attempt to send a request to a known website (e.g., google.com)
        response = requests.get("https://www.google.com", timeout=5)
        return response.status_code == 200
    except requests.ConnectionError:
        return False

def run_shut_down_ct():
    # Specify the path to your Expect script
    expect_script = "/home/trinity/control_software/FailSAFE/ct_exact_scripts/shut_down.exp"

    # Run the Expect script using the 'expect' command
    process = subprocess.Popen(["expect", expect_script], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check the output and any errors
    if process.returncode == 0:
        print("Expect script executed successfully")
        print("Output:\n", stdout)
        log_file(f'Shut down camera Successful ')
    else:
        print("Error running Expect script")
        print("Error Output:\n", stdout)
        log_file(f'Shut down camera failed')

def shut_down_magna():
    # Specify the path to the Python script you want to run
    script_to_run = "/home/trinity/control_software/MagnaPS/magna_off.py"

    # Run the Python script
    process = subprocess.Popen(["python3", script_to_run], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check the output and any errors
    if process.returncode == 0:
        print("Python script executed successfully")
        print("Output:\n", stdout)
    else:
        print("Error running Python script")
        print("Error Output:\n", stderr)

def LastNlines(fname, N):
    # opening file using with() method
    # so that file get closed
    # after completing work
    save_lines = ''
    with open(fname) as file:
         
        # loop to read iterate 
        # last n lines and print it
        
        for line in (file.readlines() [-N:]):
            #print(line, end ='')
            save_lines = save_lines + line
    return save_lines



def check_rc_log():
    try:
        log=LastNlines("/home/trinity/Programs/Trinity/control_software/fcutils/test/LOGS/rc.log",30)
        run=log.find("Starting the run#")
        stopped= log.find("Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt")
        if run == -1 and stopped > -1:
            return 0
        else:
            return 1
    except:
        return 0    

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/trinity/control_software/FailSAFE/internet.log", "a") as file:
        file.write(message + str(current_time) + '\n')

if is_internet_connected() == False:
    log_file("No internet connection ")
    check=check_rc_log()
    if check == 1:
        log_file('Shutting Down Camera ')
        run_shut_down_ct()
        log_file('CTM complete. Powering Down MagnaPS ')
        shut_down_magna()
        log_file('Shutdown Complete ')
        
    else:
        log_file('CT Camera not operating ')

# enable this if you want status checks on wether there is an internet connection
else:
    log_file("Internet connection is available. ")
