#!/usr/bin/env python3

import os
import subprocess
import time
import sys

def lab_directory():
	# directory with all expect scripts are located
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
	    # print(f"{command} output:")
	    # print(result.stdout)
	    
	    print(f"{command} output: {result.stdout} ")
	else:
	    # Command encountered an error
	    print(f"{command} output error: {result.stderr} ")
	return result.stdout

def heated_mat(status):
	lab_directory()
	# The command you want to run
	command = f'./outletcontrol.exp wc 3 {status}'  # Replace with your desired command
	run_ssh(command)

def main():
	# Check if a command-line argument is provided
	if len(sys.argv) > 1:
	    argument = sys.argv[1]
	    #print("Command-line argument:", argument)
	else:
		argument = ''
		#print("No command-line argument provided.")

	if argument == 'on' or argument == 'off' or argument == 'reboot':
		print('Running command...')
		heated_mat(argument)


if __name__ == "__main__":
    main()

