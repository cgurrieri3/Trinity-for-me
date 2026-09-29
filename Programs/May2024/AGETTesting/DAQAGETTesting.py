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
    y = input('Do you need to turn the telescope on? y/n: ')
    if y == 'y':
        start() # config and HV 

    n = int(input('What channel are you currently testing? #: '))

    DAQruns = 5

    log_print('--------------- Starting AGET Testing ----------------') 

    try:
        while True:
            log_print(f'Testing Channel {n} for {DAQruns} DAQ runs.')

            take_n_runs(DAQruns)
            time.sleep(5)
            
            log_print("Hit \"c\" to continue or \"q\" to quit: ...")
            while True: 
                key = readchar.readchar()  # Wait for keypress
                if key == 'c':  # Check if Enter key is pressed
                    break
                elif key == 'q':
                    log_print("\nAGET Testing terminated by user. \n Make sure to shut the camera off\n")
                    sys.exit(0)

                else:
                    log_print("Invalid key. Press \"c\" to continue...")
            
            print('------------------------------------------------------')
            n += 1

    except KeyboardInterrupt:
        print('------------------------------------------------------')
        log_print("\nAGET Testing terminated by user. \n Make sure to shut the camera off\n")
        # Perform any necessary cleanup or exit operations here


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
        log_print(f'Expect {script} executed successfully')
        #logging.info("Output:\n", string(stdout))
        log_print(f'{message}')
    else:
        log_print(f'Error running {script} script')
        #logging.info("Error Output:\n", string(stdout))
        logging.info(f'{message}')


def start():

    run_expect("PowerOnInit.exp", "Powers on and does Sequence init")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")

    run_expect("conf_extrenal_trigger.exp", "set external trigger")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")
  

def check_file_info():
    # Get the current UTC time
    current_utc_time = datetime.utcnow()
    
    # Format the UTC time as YYYYMMDD
    formatted_utc_time = current_utc_time.strftime('%Y%m%d')

    folder_path = f'/home/trinity/Documents/Data/{formatted_utc_time}/RawDataMerged/'
    #print(folder_path)
    #folder_path = '/home/trinity/Documents/Data/20240307/RawDataMerged'
    
    os.chdir(folder_path)
    # Execute the shell command to get the size of the most recently modified file
    size_command = 'stat --printf="%s" "$(ls -t | head -n1)"'
    size_process = subprocess.Popen(size_command, shell=True, stdout=subprocess.PIPE)
    size_output = size_process.communicate()[0].decode().strip()
    
    # Execute the shell command to count the number of files in the current directory
    count_command = 'ls -1 | wc -l'
    count_process = subprocess.Popen(count_command, shell=True, stdout=subprocess.PIPE)
    count_output = count_process.communicate()[0].decode().strip()
    
    # Execute the shell command to get the most recently modified file
    recent_file_command = 'ls -t | head -n1'
    recent_file_process = subprocess.Popen(recent_file_command, shell=True, stdout=subprocess.PIPE)
    recent_file_name = recent_file_process.communicate()[0].decode().strip()
    
    # Get the modification time of the most recently modified file
    recent_file_modification_time = os.path.getmtime(recent_file_name)
    
    # Calculate the time difference
    current_time = datetime.utcnow()
    modification_datetime = datetime.utcfromtimestamp(recent_file_modification_time)
    time_difference = current_time - modification_datetime
    dtFile = int(time_difference.total_seconds())

    
    # Convert the outputs to integers for later use in the Python script
    sLFile = int(size_output)
    num_Files = int(count_output)
    
    log_print("Size of most recently modified file: %s", sLFile)
    log_print("Number of files in the directory: %s", num_Files)
    log_print("Time since last file created: %s", dtFile)

    return num_Files, sLFile, dtFile

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
    log_file = 'DAQAGETTesting.log'
    logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    trim_log_file(log_file)
    logging.info('Trimmed Log file')

    main()





   
    


    
