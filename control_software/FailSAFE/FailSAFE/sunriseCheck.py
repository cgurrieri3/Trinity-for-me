import subprocess
import os
import time
from datetime import datetime
import logging
logging.basicConfig(filename='/home/trinity/control_software/FailSAFE/sunriseCheck.log', level=logging.INFO,format='%(asctime)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')

def is_telesceope_safe():
	# Read thesunrise from the text file
	with open('/home/trinity/control_software/FailSAFE/sunrise.txt', 'r') as file:
		sunrise_str = file.read().strip()

	# Parse the sunrise string into a datetime object
	sunrise = datetime.strptime(sunrise_str, '%Y-%m-%d %H:%M:%S')

	# Get the current time
	current_time = datetime.now()

	# Compare the two datetime objects
	if current_time < sunrise:
		print("The sunrise from the file is in the future.")
		return True
	elif current_time > sunrise:
		print("The sunrise from the file is in the past.")
		return False
	else:
		print("Thesunrise from the file is the same as the current time.")
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
        logging.info(f'Shut down camera Successful ')
    else:
        print("Error running Expect script")
        print("Error Output:\n", stdout)
        logging.info(f'Shut down camera failed')

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


if is_telesceope_safe() == False:
    logging.info("Passed Sunrise ")
    check=check_rc_log()
    if check == 1:
        logging.info('Shutting Down Camera ')
        run_shut_down_ct()
        logging.info('CTM complete. Powering Down MagnaPS ')
        shut_down_magna()
        logging.info('Shutdown Complete ')
        
    else:
        logging.info('CT Camera not operating ')

# enable this if you want status checks on wether there is an internet connection
else:
    logging.info("Before Sunrise ")
