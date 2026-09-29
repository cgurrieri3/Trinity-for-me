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

def run_commands(command):
    # Specify the path to your Expect script
    expect_script = f"/root/FailSAFE/expect_scripts/{command}.exp"

    # Run the Expect script using the 'expect' command
    process = subprocess.Popen(["expect", expect_script], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    #Check the output and any errors
    if process.returncode == 0:
        print("Expect script executed successfully")
        print("Output:\n", stdout)
        log_file(f'{command} Successful ')
    else:
        print("Error running Expect script")
        print("Error Output:\n", stdout)
        log_file(f'{command} FAILURE {stdout}')



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

   

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/root/FailSAFE/internet.log", "a") as file:
        file.write(message + str(current_time) + '\n')

if is_internet_connected() == False:
    log_file("No internet connection ")

    log_file('Shutting Down Camera ')
    run_commands('shut_door')
    time.sleep(500) # this is the time needed for the camera to turn off if it also lost internet at the same time
    log_file('Turning off outlets ')
    
    run_commands('outlets_off')
      

# enable this if you want status checks on wether there is an internet connection
else:
   log_file("Internet connection is available. ")
