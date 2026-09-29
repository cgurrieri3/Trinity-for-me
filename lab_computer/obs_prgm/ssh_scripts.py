import os
import subprocess
import time
import communicate as lets


def lab_directory():
	# directory with all expect scripts are located
	directory = os.path.expanduser("/data/TrinityLabComputer/control_comp")
	os.chdir(directory)


def run_ssh(command):
	# Run the command and capture its output
	result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# Remove ANSI escape codes
	#result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
	# Check for errors
	if result.returncode == 0:
	    # Command was successful
	    # print(f"{command} output:")
	    # print(result.stdout)
	    
	    lets.log_file(f"{command} output: {result.stdout} ")
	else:
	    # Command encountered an error
	    lets.communicate(f"{command} output error: {result.stderr} ")
	return result.stdout

def rclog_sync():
	os.chdir("/data/TrinityLabComputer/obs_prgm/rcLogs/")
	command = f'./rclog_Copy.sh'
	run_ssh(command)
	lab_directory()
 
def SIAB_testing():
  lab_directory()
  command = f'./SIAB_testing.exp'
  run_ssh(command)
  
def kill_python_process(pyfile):
  lab_directory()
  command = f'./kill_python_process.exp {pyfile}'
  run_ssh(command)
  
def get_testing_log():
  directory = os.path.expanduser("/home/mpotts32/obs_prgm/camera_testing")
  os.chdir(directory)
  command = f'./SIAB_testingLOG_Copy.sh'
  run_ssh(command)
  
  

def door(dir):
	lab_directory()
	# The command you want to run
	command = f'./doorcontrol.exp {dir}'  # Replace with your desired command
	run_ssh(command)
	#print('Fake door up')
	
def reboot_CTCPU():
	lab_directory()
	run_ssh(f'./reboot_ctcpu.exp')
	time.sleep(10)

def MagnaPS(status):
	lab_directory()
	# The command you want to run
	command = f'./outletcontrol.exp rc 6 {status}'  # Replace with your desired command
	run_ssh(command)
 


def MicroTSA(status):
	lab_directory()
	# The command you want to run
	command = f'./outletcontrol.exp rc 7 {status}'  # Replace with your desired command
	run_ssh(command)

def chiller(status):
	lab_directory()
	# The command you want to run
	command = f'./outletcontrol.exp rc 1 {status}'  # Replace with your desired command
	run_ssh(command)

def sd_CTM():
	lab_directory()
	run_ssh('./CTM_shutdown.exp')

def CTM_init():
	lab_directory()
	run_ssh('./CTM_poweron_Sinit.exp')

def CTM_config_hled():
	lab_directory()
	run_ssh('./CTM_config_hled.exp')# only checks the currents and temps are real, hv set, 

def CTM_config_single():
	lab_directory()
	run_ssh('./CTM_config_single.exp')# only checks the currents and temps are real, hv set, 

def CTM_start():
	lab_directory()
	run_ssh('./CTM_start.exp')

def CTM_stop():
	lab_directory()
	run_ssh('./CTM_stop.exp')

def CTM_LVPS_HV(hv):
	lab_directory()
	# add a check for number maybe?
	command = f'./CTM_LVPS_HV.exp {hv}'
	run_ssh(command)

def CTM_HV_ON():
	lab_directory()
	run_ssh(f'./CTM_HV_ON.exp')

def CTM_set_trigger(rate=170):
	lab_directory()
	command = f'./CTM_set_trigger.exp {rate}'
	run_ssh(command)


def LVPS(status):
	lab_directory()
	run_ssh(f'./MagnaPS_power.exp {status}')

def trigger_scan(start=120,size=10,step=10):
	lab_directory()
	run_ssh(f'./CTM_run_trigScan.exp {start} {step} {size}')
 

