import pandas as pd

import math
import os
import time
import sys
import subprocess
from datetime import datetime
import logging
import readchar

##################################################################################################################################
##################################################################################################################################
################################################################################################################################## 

def main():
    DAQruns = 5

    y = input('Do you need to turn the telescope on? y/n: ')
    if y == 'y':
        start() # config and HV 

    hvLVPS = [42,44]
    log_print('------- Starting FlatFielding ------') 

    try:
        for hv in hvLVPS:
            run_expect(f'LVPS_nv.exp {hv}', f'Set LVPS {hv}v')
            log_print('Check Graphana to ensure camera is operating.')
            input("Hit \"ENTER\" to continue: ...")

            run_expect(f'SIAB_powerOn.exp 0 15', f'Turning on SIABS')
            log_print('Check Graphana to ensure camera is operating.')
            input("Hit \"ENTER\" to continue: ...")

            take_n_runs(DAQruns)
            
            
            log_print("Hit \"c\" to continue or \"q\" to quit: ...")
            while True: 
                key = readchar.readchar()  # Wait for keypress
                if key == 'c':  # Check if Enter key is pressed
                    break
                elif key == 'q':
                    log_print("\nFlatFielding terminated by user. \n Make sure to shut the camera off\n")
                    sys.exit(0)

                else:
                    log_print("Invalid key. Press \"c\" to continue...")
            
            print('------------------------------------------------------')

    except KeyboardInterrupt:
        print('------------------------------------------------------')
        log_print("\nFlatFielding terminated by user. \n Make sure to shut the camera off\n")
        # Perform any necessary cleanup or exit operations here
    print('------------------------------------------------------')
    log_print("\nFlatFielding terminated by user. \n Make sure to shut the camera off\n")    

##################################################################################################################################
##################################################################################################################################
################################################################################################################################## 

def run_expect(script,message):
    print(script,message)

def run_expect1(script, message):
    directory = os.path.expanduser("/home/trinity/Programs/MAY2024/exc_scripts")
    os.chdir(directory)

    # Run the Expect script using the 'expect' command
    script = "expect " + script
    #process = subprocess.Popen(["expect", script], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    process = subprocess.run(script, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,text=True)

    # Wait for the process to finish
    stdout = process.stdout
    stderr = process.stderr

    # Check the output and any errors
    if process.returncode == 0:
        logging.info(f'Expect {script} executed successfully')
        #logging.info("Output:\n", string(stdout))
        logging.info(f'{message}')
    else:
        logging.info(f'Error running {script} script')
        #logging.info("Error Output:\n", string(stdout))
        logging.info(f'{message}')

def start():

    
    run_expect("PowerOnInit.exp", "Powers on and does Sequence init")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")

    run_expect("conf_hled.exp", "config sequence hled")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")


    


def take_n_runs(n):
    run_expect('startDAQ.exp', f'starting {n} data runs')

    print('Checking the files')
    # nFiles, _ , _ = check_file_info()

    # tFiles = nFiles + n
    # while nFiles < tFiles: 
    #     nFiles, sLFiles, dtFiles = check_file_info()
        
    #     if sLFiles < 2 or dtFiles > 600:
    #       log_print('bad news')
    #       log_print('Turning camera off')
    #       break
    
    run_expect('stopDAQ.exp', 'stoping daq') # turn off statemessages

def trim_log_file(log_file_path, max_lines=1000):
    try:
        # Check if log file exists and is readable
        if os.path.exists(log_file_path) and os.path.isfile(log_file_path):
            # Read the content of the log file
            with open(log_file_path, 'r') as file:
                lines = file.readlines()

            # Keep only the last 'max_lines' lines
            lines_to_keep = lines[-max_lines:]

            # Overwrite the log file with the trimmed content
            with open(log_file_path, 'w') as file:
                file.writelines(lines_to_keep)
    except Exception as e:
        logging.info(f"Error occurred while trimming log file: {e}")


def log_print(mes):
    logging.info(mes)
    print(mes)
  

if __name__ == "__main__":
    # Configure logging
    log_file = 'FlatFielding.log'
    logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    trim_log_file(log_file)
    logging.info('Trimmed Log file')

    main()



   
    


    
