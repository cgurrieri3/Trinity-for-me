#!/usr/bin/python3
import subprocess
import os
import time



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


   
def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/root/FailSAFE/reboot.log", "a") as file:
        file.write(message + str(current_time) + '\n')


log_file("Computer Rebooted ")

run_commands('shut_door')
     
