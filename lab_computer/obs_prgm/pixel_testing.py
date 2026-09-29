import subprocess
import os
import check_sm as csm
import check_wx as cwx
import check_lt as clt
#import check_data as cdata
#import check_rc as crc
import ssh_scripts as ssh
import communicate as lets
import time
import datetime
import multiprocessing
from datetime import datetime


def check_SIAB_testing_log(): 
  string_to_check = "--SIAB Testing Completed--"
  try:
    with open('/home/mpotts32/obs_prgm/camera_testing/testSIABs.log', 'r') as file:
      last_line = file.readlines()[-1].strip()  # Read the last line and remove leading/trailing whitespace
      if string_to_check in last_line:
        lets.communicate('SIAB testing completed')
        return 0  # Return 0 if the string is found
      else:
        lets.communicate('SIAB testing still in progress')
        return 1  # Return 1 if the string is not found
  except FileNotFoundError:
    print(f"Log SIAB testing not found.")
    return 1  # Return 1 if the file is not found

    
def monitoring_SIAB_testing(wx_override='no'):
  # state: internal or external, intrigs_nfiles: number of internal trigger files.

	lets.log_file(f"Monitoring Pixel testing:")
	cwx.take_screenshot()
	exit_message= ""
	errors = 0
	while True:
	# While the camera is operating this will check WEATHER, STATE MESSAGES, TIME

		# Check time
		safe_light=clt.check_current_time() # one bad condition this will break
		if safe_light != 1:
			#lets.communicate('Monitor: Time Condition UNSAFE')
			exit_message = 'EON'
			break

  	# check state messages
  	#lets.communicate(f'Monitor: Number or errors StateMessages #{errors}.')
		if errors < 20:
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			if safe_proceed == 1:
				lets.communicate('Monitor: SM within limits')
				errors = 0

			elif safe_proceed == -1:
				lets.communicate('Monitor: SM HV current or HV voltage out of limits, SHUTTING DOWN... ')
				exit_message = 'State messaged HV current or HV voltage'
				break
        
			elif safe_proceed == 2:
				print('Ignoring that state messages are old')

			else:
				errors = errors + 1
				lets.communicate(f'Monitor: SM outide limits #{errors}/20')
		else:
			lets.communicate('Monitor: SM errors persisted to long, SHUTTING DOWN... ')
			exit_message = 'State messages NOT hv current/hv voltage'	
			break
  				
		if wx_override != 'yes': # only overrides if it is yes
  
			safe_weather=cwx.query_last_wx() # one bad contiions it will break
			if safe_weather ==1:
				lets.communicate('Monitor: WX condition safe')
  			
			else:
				lets.communicate('Monitor: WX conditions UNSAFE')
				exit_message = 'Weather conditions'
				break
			cwx.airport_report()
  
		else:
			lets.communicate('Monitor: Weather OVERRIDE in place')	
			print('Camera will NOT shut off due to weather')
      
   #check the file of the log of the data
		ssh.get_testing_log()
		lets.communicate('Transferred SIAB Log')
		SIAB_log = check_SIAB_testing_log()
		if SIAB_log == 0:
			exit_message = 'SIAB testing complete'
			break
  
		time.sleep(15)
	return exit_message
 
 
def SIAB_testing(wx_override='no'):
  ssh.SIAB_testing()
  exit_message=monitoring_SIAB_testing(wx_override)
  lets.communicate('killing python process if still running')
  ssh.kill_python_process('testSIABs.py')
   
 # running the script
  # running the monitoring
    # dont care about files outside the scipt
    # dont care about how long since the last SM
    # problem with monitoring shutdown
  return exit_message
#SIAB_testing()
