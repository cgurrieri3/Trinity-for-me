import pandas as pd
import math
import os
import time
import subprocess
from datetime import datetime
import logging
import readchar
import sys

##################################################################################################################################
##################################################################################################################################
################################################################################################################################## 

def main():
    DAQruns = 5

    y = input('Do you need to turn the telescope on? y/n: ')
    if y == 'y':
        start() # config and HV 

    loop = 0
    trate = 0

    log_print('------- Starting MUSIC Chip Mapping Testing ------') 

    SIAB = list(range(16))
    PIXEL = list(range(8))
    MUSIC_OFF = ['top','bottom']

    try:
        for m in MUSIC_OFF: # goes between top and bottom     
            for s in SIAB: # goes through each SIAB
                
                run_expect(f'SIAB_powerOn.exp {s} {s}', f'SIAB {s} turning on')

                log_print('Check Graphana to ensure camera is operating.')
                input("Hit \"ENTER\" to continue: ...")
                #trate = first_loop(loop,trate)
                
                print('get trate')
                #tRate=get_new_tRate()
                tRate = 17
                run_expect("conf_SFO.exp", "configure sequence SingleFocusOnly")
               
                run_expect(f"SIAB_threshold.exp {tRate} {s} {s}",f'set rate to {tRate} for {s}')

                # turn off the half of the music chip
                for p in PIXEL:
                    run_expect(f'pixel_threshold.exp {m} {p} 788 {s} {s}',f'pixel_threshold.exp {m} {p} 788 {s} {s}') \


                take_n_runs(DAQruns)
                #time.sleep(5)
                
                log_print("Hit \"c\" to continue or \"q\" to quit: ...")
                while True: 
                    key = readchar.readchar()  # Wait for keypress
                    if key == 'c':  # Check if Enter key is pressed
                        break
                    elif key == 'q':
                        log_print("\nMUSIC Mapping Testing terminated by user. \n Make sure to shut the camera off\n")
                        sys.exit(0)

                    else:
                        log_print("Invalid key. Press \"c\" to continue...")
                
                print('------------------------------------------------------')
                loop +=1

    except KeyboardInterrupt:
        print('------------------------------------------------------')
        log_print("\nMUSIC Mapping Testing terminated by user. \n Make sure to shut the camera off\n")
        # Perform any necessary cleanup or exit operations here
    print('------------------------------------------------------')
    log_print("\nMUSIC Mapping Testing is Complete. \n Make sure to shut the camera off\n")
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

    run_expect("PowerOnInit.exp", "Power on and Sequence init")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")

    run_expect("conf_SFO.exp", "config sequence SFO")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")

    run_expect("LVPS_44v.exp", "Set LVPS 44v")
    log_print('Check Graphana to ensure camera is operating.')
    input("Hit \"ENTER\" to continue: ...")
  

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

def take_n_runs(n):
    run_expect('startDAQ.exp', f'starting {n} data runs')

    #time.sleep(2)
    print('running file checking')
    # nFiles, _ , _ = check_file_info()

    # tFiles = nFiles + n
    # while nFiles < tFiles: 
    #     nFiles, sLFiles, dtFiles = check_file_info()
        
    #     if sLFiles < 2 or dtFiles > 600:
    #       log_print('bad news')
    #       log_print('Turning camera off')
    #       break
    
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
    

def log_print(mes):
    logging.info(mes)
    print(mes)
  

if __name__ == "__main__":
    # Configure logging
    log_file = 'testMusicMapping.log'
    logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    trim_log_file(log_file)
    logging.info('Trimmed Log file')

    main()



   
    


    
