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

def sopen_door():
	lab_directory()
	# The command you want to run
	command = "./doorcontrol.exp up"  # Replace with your desired command
	run_ssh(command)
	

def on_MagnaPS():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 6 on"  # Replace with your desired command
	run_ssh(command)

def on_MicroTSA():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 7 on"  # Replace with your desired command
	run_ssh(command)

def on_chiller():
	lab_directory()
	# The command you want to run
	command = "./outletcontrol.exp rc 1 on"  # Replace with your desired command
	run_ssh(command)

def on_CTM():
	lab_directory()
	run_ssh('./CTM_poweron_Sinit.exp')


def on_LVPS():
	lab_directory()
	run_ssh('./MagnaPS_power.exp on')

def turn_on_CT():


	# turn off the chiller, magna, micro
	on_MagnaPS()
	on_MicroTSA()
	on_chiller()
	
	# power down lvps
	on_LVPS()

	# power down camera
	on_CTM()
		
	
	
	# close the door
	#open_door()
	

	
turn_on_CT()
