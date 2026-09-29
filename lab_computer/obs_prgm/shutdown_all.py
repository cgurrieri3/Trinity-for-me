import subprocess
import os

# moonrise = 
# moonset =

# sunrise = 
# sunset = 

# when is safe to operate based on light values

# States message make sure certian values are where hey are expect
# dont change too quickly 


# Weather paramters fo safe operating


def lab_directory():
	# Set the working directory
	directory = os.path.expanduser("~/control_comp")
	os.chdir(directory)


def run_ssh(command):
	# Run the command and capture its output
	result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# Remove ANSI escape codes
	#result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
	# Check for errors
	if result.returncode == 0:
	    # Command was successful
	    print("Command output:")
	    print(result.stdout)
	else:
	    # Command encountered an error
	    print("Command failed with an error:")
	    print(result.stderr)

def shut_door():
	lab_directory()
	# The command you want to run
	command = "./doorcontrol.exp down"  # Replace with your desired command
	run_ssh(command)
	

def sd_MagnaPS():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 6 off"  # Replace with your desired command
	run_ssh(command)

def sd_MicroTSA():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 7 off"  # Replace with your desired command
	run_ssh(command)

def sd_chiller():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 1 off"  # Replace with your desired command
	run_ssh(command)

def sd_CTM():
	lab_directory()
	run_ssh('./CTM_shutdown.exp')


def sd_LVPS():
	lab_directory()
	run_ssh('./MagnaPS_shutdown.exp off')

def shut_down_CT():

	# close the door
	shut_door()

	# power down camera
	sd_CTM()
		
	# power down lvps
	sd_LVPS()
	

	# turn off the chiller, magna, micro
	sd_MagnaPS()
	sd_MicroTSA()
	sd_chiller()

	
shut_down_CT()
