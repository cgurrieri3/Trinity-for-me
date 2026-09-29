import time
#!/usr/bin/env python3
import requests
import os
import glob
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import communicate as lets

# Import the warnings module
import warnings
#import shutup 
from collections import deque
#shutup.please()

warnings.filterwarnings("ignore", message="MatplotlibDeprecationWarning:") 
warnings.filterwarnings("ignore")

folder_path = "/data/TrinityLabComputer/obs_prgm/rcLogs/"


# file to check the last line 
def rclog_directory():
    # directory with all expect scripts are located
    directory = os.path.expanduser(folder_path)
    os.chdir(directory)


def run_ssh(command):
    # Run the command and capture its output
    rclog_directory()
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Remove ANSI escape codes
    #result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
    # Check for errors

    if result.returncode == 0:
        # Command was successful
        lets.log_file(f"{command} successful output: {result.stdout} ")
    else:
        # Command encountered an error
        lets.communicate(f"{command} output error: {result.stderr} ")

    return result.returncode

def get_most_recent_file(folder_path):
    # Use glob to list all files in the folder sorted by modification time
    files = glob.glob(os.path.join(folder_path, '*'))
    files.sort(key=os.path.getmtime, reverse=True)

    # Check if there are any files in the folder
    if files:
        most_recent_file = files[0]
        return most_recent_file
    else:
        lets.communicate("No files found in the folder.")
        return None

def LastNlines(fname, N):
    # opening file using with() method
    # so that file get closed
    # after completing work
    save_lines = ''
    with open(fname,encoding="utf8", errors='ignore') as file:

         
        # loop to read iterate 
        # last n lines and print it
        
        for line in (file.readlines() [-N:]):
            #print(line, end ='')
            save_lines = save_lines + line
    return save_lines

# add where it can be located
def check_rc_log(keyword):
    # try:
    stdout=run_ssh('./rclog_Copy.sh')
    #print(stdout)

    if stdout == 1:
        
        file_path = f"{folder_path}/rc.log"

        # Open the file in write mode, which truncates the file
        with open(file_path, 'w'):
            pass  # The 'with' block is empty, but the file is opened and closed immediately
        lets.communicate(f"Contents of '{file_path}' have been cleared. NO ERROR this is normal")
        return 2

    lines = 3
    if keyword == 'Starting the run#':
        lines = 6

    log=LastNlines(f"{folder_path}rc.log",lines)
    #print(log)
    output=log.find(f'{keyword}')
    #print(output)
    #output=log.find("Starting the run")
    #stopped= log.find("Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt")
    if output == -1:
        return 0
    else:
        return 1
    # except:
    #     return 0    

#print(check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt'))

def plot_triggerRate_scan():
    run_ssh('./rclog_Copy.sh')
    folder_lab_machine = folder_path
    log=LastNlines(f'{folder_lab_machine}rc.log',3)
    print(log)
    output=log.find(f'Moving Trigger Scan Counters to:')
    print(output)
    trigger_scan_fpath = log[output + 33:output + 113]
    file_path = log[output + 33:output + 113]
    #/home/trinity/Programs/Trinity/control_software//Archive/Tscan/231206/SF/061159/
    folder_date = file_path[63:69]
    folder_time = file_path[73:79]
    print(file_path)
    print(folder_date)
    print(folder_time)
    run_ssh(f'./triggerfile_Copy.sh {folder_date} {folder_time}')
    # get the file
    most_recent_file = get_most_recent_file(f'{folder_lab_machine}Tscan/')
    #print(most_recent_file)
    df = pd.read_csv(f'{most_recent_file}', delimiter='\t\t',skiprows = 1)
    df.columns = names=['Steps', 'Threshold', 'TriggerRate']
    print(df)
    plt.plot(df.Threshold, df.TriggerRate,'o-',color = 'g')
    plt.title('Selecting Trigger Rate')
    plt.xlabel('Threshold')
    plt.ylabel('Trigger Rate')
    plt.ylim([0,50])
    plt.savefig(f'{folder_lab_machine}Plots/Tscan_now_{folder_date}_{folder_time}.png')
    print(f'Plot saved to {folder_lab_machine}Plots/Tscan_now_{folder_date}_{folder_time}.png')
    #plt.show()
#plot_triggerRate_scan()
#check_rc_log('Staring Master Control at:') 


def count_lines_between_phrases(file_path, start_phrase, end_phrase,lines_to_check = 36):
    # try:
    with open(file_path, 'rb') as file:
        size=len([0 for _ in file])
        file.close()

    with open(file_path, 'r', errors='ignore') as file:
        between_phrases = False
        line_count = 0
        line_total = 0
        
        for line in file:
            line_total += 1
            if start_phrase in line and line_total > (size - lines_to_check):
                between_phrases = True

            if between_phrases:
                line_count += 1

            if end_phrase in line and between_phrases:
                between_phrases = False
                break
        #print(line_count)
    return line_count

    # except FileNotFoundError:
    #     return f"Error: File '{file_path}' not found."


def check_daq():
    run_ssh('./rclog_Copy.sh')
    rc_filepath = f"/data/TrinityLabComputer/obs_prgm/rcLogs/rc.log"
    start_phrase = 'Clearing the Counters ...'
    end_phrase = 'Saving the Events data ...'
    num_lines = count_lines_between_phrases(rc_filepath,start_phrase,end_phrase)
    #print(num_lines)
    if num_lines > 18:
        lets.communicate('Possible error in rclog stage: DAQ ')
        return 0
    else:
        #print('rclog DAQ seems good')
        lets.log_file('rclog DAQ seems good ')
        return 1
#check_daq()
