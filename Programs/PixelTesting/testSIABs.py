import pandas as pd
import math
import os
import time
import subprocess
from datetime import datetime
import logging
# procees

# start from intrigs
# DAQ stopped and SMenabled

# get to turning on SIABS
  # Sequence config
  # LVPS 44

# create pixel chart

# Loop through each SIAB
  # Each hot pixel
    
    
    # turn on that SIAB
    # set hot pixel threshold
    # run triggerScan 1 ,2 times and what if its all zero
    # sequence config
    # select theshold for SIAB
    # set hot pixel threshold same as before
    # take 10 data runs
    
    # turn off that SIAB
    # write to log the SIAB and the thresold and the pixel and threshold and log the time in UTC
    
    
 # Loop through hot pixel threshold levels this should be a flag and a different thing   

    
# some sort of monitoring? on Lab computer 

def run_expect(script, message):
    directory = os.path.expanduser("/home/trinity/Programs/PixelTesting/exc_scripts/")
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


def after_intrigs():
    # Specify the path to your Expect script
    run_expect("conf_SFO.exp", "config sequence SFO")
    time.sleep(20)
    run_expect("LVPS_44v.exp", "Set LVPS 44v")
  
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
    

def update_pixel_chart():
    # get the hot pixel chart
    directory = os.path.expanduser("/home/trinity/Programs/PixelTesting/")
    os.chdir(directory)
    df = pd.read_csv('DeadPixels.txt',header=0)
    df['PixelNum'] = df['PixelNum'].astype(int)
    # fill in the values to be used on the commands of the hot pixels
    df['r']= df['PixelNum']/16
    df['m']= df['r'].apply(lambda x: 'top' if x % 1 >= 0.5 else 'bottom')
    df['s']= df['r'].astype(int)
    df['r']= df['r'].apply(get_r)
    df['r']= df['r'].astype(int)
    df['v']= df['ThLevel'].apply(get_v)
    logging.info(df)
    # resave the chart to the same file
    df.to_csv('DeadPixels.txt', index=False)
    return df
    
def set_hot_pixels(df,siab):
    SIAB_hp = df[df['s'] == s]
    
    if len(SIAB_hp) != 0:
        pixel_num = []
        for index, value in SIAB_hp['PixelNum'].iteritems():       
            pixel_num.append(value)
        logging.info(f"Hot pixels to adjust: {pixel_num}")
        
        for row in range(len(SIAB_hp)):
            m = df.loc[row, 'm']
            r = df.loc[row, 'r']
            v = df.loc[row, 'v']
            run_expect(f'pixel_threshold.exp {m} {r} {v} {s} {s}',f'pixel_threshold.exp {m} {r} {v} {s} {s}') 
            #logging.info(f'pixel_threshold.exp {m} {r} {v} {s} {s}')
            time.sleep(10)
    else:
        logging.info('No hot pixels')

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
    
def list_txt_files_in_folder(folder_path):
    # Get the list of files and directories in the specified folder
    files = os.listdir(folder_path)
    # Filter out only files with a ".txt" extension
    txt_files = [file for file in files if os.path.isfile(os.path.join(folder_path, file)) and file.endswith('.txt')]
    #print(txt_files)
    return txt_files[0]
    
def find_tRate(df):
    closest_value = df['TriggerRate'].iloc[(df['TriggerRate'] - 10).abs().argsort()[:1]].values[0]

    # Find the index of the row with the closest value
    closest_index = (df['TriggerRate'] - 10).abs().idxmin()
    trate = df.loc[closest_index,'Threshold']
    
    # Check if there's a value within +/-3
    if closest_value in range(10 - 3, 10 + 4):
        logging.info(f"The closest value to 10 is {closest_value}, found in row {closest_index} threshold {trate}.")
        
    else:
        if closest_value < 10:
            logging.info("The closest value is never being low enough.")
            trate = 240
            
        else:
            logging.info("The closest value is never being high enough.")
            trate = 150
    return trate

def get_new_tRate():
    run_expect('trigger_scan.exp', 'running trigger rate scan')
    time.sleep(200)
    # get the value from the rc log
    rcLOG_path = r"/home/trinity/Programs/Trinity/control_software/fcutils/test/LOGS/rc.log"
    log=LastNlines(f'{rcLOG_path}',3)
    #print(log)
    output=log.find(f'Moving Trigger Scan Counters to:')
    #print(output)
    trigger_scan_fpath = log[output + 33:output + 113]
    folder_path = log[output + 33:output + 113]
    #folder_path = '/home/trinity/Programs/Trinity/control_software//Archive/Tscan/240229/SF/061845/'
    folder_date = folder_path[63:69]
    folder_time = folder_path[73:79]
    #print(folder_path)
    #print(folder_date)
    #print(folder_time)
    file_txt=list_txt_files_in_folder(folder_path)
    #print(file_txt[0])
    full_path = folder_path + file_txt
    df = pd.read_csv(f'{full_path}', delimiter='\t\t',skiprows = 1,engine='python')
    df.columns = names=['Steps', 'Threshold', 'TriggerRate']
    logging.info(df)
    trate=find_tRate(df)
    logging.info(f'The rate to be set {trate}')
    return trate

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
    
    logging.info("Size of most recently modified file: %s", sLFile)
    logging.info("Number of files in the directory: %s", num_Files)
    logging.info("Time since last file created: %s", dtFile)

    return num_Files, sLFile, dtFile

def take_10_runs():
    run_expect('startDAQ.exp', 'starting 10 data runs')
    time.sleep(240)
    nFiles, _ , _ = check_file_info()
    #tFiles = nFiles + 2
    tFiles = nFiles + 10
    while nFiles < tFiles: 
        nFiles, sLFiles, dtFiles = check_file_info()
        
        if sLFiles < 2 or dtFiles > 600:
          logging.info('bad news')
          logging.info('Turning camera off')
          break
    
    #check the files to make sure they exist 
    # get 10 data files
    run_expect('stopDAQ.exp', 'stoping daq') # turn off statemessages
    run_expect('SIAB_powerOff.exp', 'turn off SIABs')

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


# Configure logging
log_file = 'testSIABs.log'
logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
trim_log_file(log_file)
logging.info('Trimmed Log file')
logging.info('--SIAB Testing Starting--')    
    

#after_intrigs() # config and HV 
hot_pixel_chart = update_pixel_chart()
SIAB = list(range(16))
#print(SIAB)




for s in SIAB:
    run_expect(f'SIAB_powerOn.exp {s} {s}', f'pOn SIAB {s}')
    logging.info(f'Turning on SIAB {s}')
    set_hot_pixels(hot_pixel_chart,s)
    tRate=get_new_tRate()
    run_expect("conf_SFO.exp", "config sequence SFO")
    time.sleep(20)
    run_expect(f"SIAB_threshold.exp {tRate} {s} {s}",f'set rate to {tRate}')
    set_hot_pixels(hot_pixel_chart,s)
    take_10_runs()
    logging.info(f'pOff SIAB {s}')
    time.sleep(15)

logging.info('--SIAB Testing Completed--')



   
    


    
