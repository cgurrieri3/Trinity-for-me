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
        logging.info(f"Expect {script} executed successfully")
        #logging.info("Output:\n", string(stdout))
        logging.info(f'{message}')
    else:
        logging.info(f"Error running {script} script")
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

def change_pixel_threshold(df, s, th):
    print(df)
    if isinstance(th, int):
         m = df.loc['m']
         r = df.loc['r']
         v = get_v(th)
         run_expect(f'pixel_threshold.exp {m} {r} {v} {s} {s}',f'pixel_threshold.exp {m} {r} {v} {s} {s}')
         #logging.info(f'pixel_threshold.exp {m} {r} {v} {s} {s}')
         time.sleep(10)
    else:
        for t in range(len(th)):
            m = df.loc[t,'m']
            r = df.loc[t,'r']
            v = get_v(th[t])
            run_expect(f'pixel_threshold.exp {m} {r} {v} {s} {s}',f'pixel_threshold.exp {m} {r} {v} {s} {s}')
            #logging.info(f'pixel_threshold.exp {m} {r} {v} {s} {s}')
            time.sleep(10)

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
    # get the value from the rc log
    time.sleep(200)
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
    print(f'New rate {trate}')
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
    logging.info("Time since last data file: %s", dtFile)

    return num_Files, sLFile, dtFile

def take_10_runs(s):
    run_expect('startDAQ.exp', 'starting 10 data runs')
    nFiles, _ , _ = check_file_info()
    time.sleep(240)
    
    tFiles = nFiles + 5
    #tFiles = nFiles + 1
    while nFiles < tFiles: 
        nFiles, sLFiles, dtFiles = check_file_info()
        
        if sLFiles < 2 or dtFiles > 700:
          logging.info('bad news')
          logging.info('Turning camera off')
          break
        time.sleep(60)
    
    #check the files to make sure they exist 
    # get 10 data files
    run_expect('stopDAQ.exp', 'stoping daq') # turn off statemessages
    #run_expect(f'SIAB_powerOff.exp {s} {s}', 'turn off SIABs')

def trim_log_file(log_file_path, max_lines=100000):
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
log_file = 'testPIXELs.log'
logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
trim_log_file(log_file)
logging.info('Trimmed Log file')
logging.info('--SIAB Testing Starting--')    
    

after_intrigs() # config and HV 

hot_pixel_chart = update_pixel_chart()
SIAB = list(range(16))
#SIAB = list(range(5))
#print(SIAB)




for s in SIAB:
    subset_hot_pixel_chart = hot_pixel_chart[hot_pixel_chart['s'] == s]
    run_expect(f'SIAB_powerOn.exp {s} {s}', f'pOn SIAB {s}')
    
    logging.info(f'Turning on SIAB {s}')
    PIXEL = list(range(len(subset_hot_pixel_chart)))
    print(PIXEL)
    once_per_SIAB = 0
    for p in PIXEL:
        print('p  ',p)
        # for the first test in each SIAB the trigger threshold will be set and all the reconfiguring will occur
        # one set of 10 data runs will take place
        if once_per_SIAB == 0:
        
            #set bad pixel to th 1
            #set other bad pixels to th 1
            th_array = [1] * len(PIXEL)
            print(f'th_array intial {th_array}')
            change_pixel_threshold(subset_hot_pixel_chart, s,th_array)
            #get trigger rate with no pixels on
            tRate=get_new_tRate()
            print(tRate)
            run_expect("conf_SFO.exp", "config sequence SFO")
            time.sleep(20)
            run_expect(f"SIAB_threshold.exp {tRate} {s} {s}",f'set rate to {tRate}')
            logging.info(f'th_array reintial {th_array}')
            change_pixel_threshold(subset_hot_pixel_chart, s,th_array)
            take_10_runs(s)
            # now we want to go through and find the best pixel th 
            once_per_SIAB = 1
        
        # for each pixel multiple thresholds will be tested 
        #THRESHOLD = [10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240]
        #THRESHOLD = [i for i in range(10, 241, 20)]
        THRESHOLD = [10,60,120,180,240]
        #print(THRESHOLD)
        for th in THRESHOLD:
            logging.info(f'Threshold value being tested {th}')
            
            #th_array[p] = THRESHOLD[th]
            #print('updated threshold',th_array)
            change_pixel_threshold(subset_hot_pixel_chart.iloc[p], s,th)
            # set bad pixel to th 
            # take 10 data runs
            take_10_runs(s)
        
        logging.info('reset all in siab to th =1',th_array)
        change_pixel_threshold(subset_hot_pixel_chart, s,th_array)
        
    # set_hot_pixels(df,siab)
    run_expect(f'SIAB_powerOff.exp {s} {s}', f'pOff SIAB {s}')
    
    #time.sleep(15000000)
logging.info('--SIAB Testing Completed--')



   
    


    
