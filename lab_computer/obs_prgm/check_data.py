import os
import datetime
import subprocess
import time
import communicate as lets


def lab_directory():
	# Set the working directory
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
		lets.log_file(f"{command} output: {result.stdout} ")
	else:
		# Command encountered an error
		lets.communicate(f"{command} output error: {result.stderr} ")
	return result.stdout

def run_get_file_info():
	lab_directory()
	results = str(run_ssh('./get_file_info.exp'))
	return results

def check_num_files_directory(result):
	
	index = result.find('ls -1 | wc -l \\r\\n')
	number = result[index+18:index+22]
	number = ''.join(char for char in number if char.isnumeric())
	return number

def check_file_time(results):

	index = results.find('CoBo0_AsAd0_')
	#print(index)
	file_time = results[index+12:index+35]
	# Parse the string into a datetime object
	lets.log_file(file_time)
	try: 
		file_time = datetime.datetime.strptime(file_time, "%Y-%m-%dT%H:%M:%S.%f")
	except:
		file_time = datetime.datetime(2000,1,1,1,1,1)
		lets.communicate("Something is wrong with the file information time")

	# strptime
	return file_time



def check_file_size(results): # to make this run faster get all information from one ssh run
	index = results.find('stat --printf="%s" "$(ls -t | head -n1)')
	
	file_size = results[index+44:index+80]
	#print(file_size)

	file_size = file_size.split(';', 1)[0]
	file_size = file_size[:-6]
	#print(file_size)
	
	return file_size

def check_data_in_folder():
	results = run_get_file_info()
	file_number=check_num_files_directory(results)

	return file_number


def check_files_internal(intial_files,total_cycles):
	
	results = run_get_file_info()
	#print(results)

	file_size=check_file_size(results)
	#print(type(file_size))
	#print(check_file_time(results))
	#print(file_size)

	time_difference = datetime.datetime.utcnow() -check_file_time(results)

	files = check_num_files_directory(results)
	lets.communicate(f'File Information: [#: {files}, time since last: {time_difference.total_seconds()}s, size = {file_size}] ')
	
	runs_left = total_cycles - int(files)
	print(files)
	print(intial_files)
	print(time_difference.total_seconds())
	print(file_size)

	# Checks if a file has been created in the first 6 minutes
	if files == intial_files or time_difference.total_seconds() > 1020 or file_size == '0':
		lets.communicate(f'DA Internal Trigger files not being generated ')
		return 0

	elif total_cycles > int(files):
		lets.communicate(f'DA Internal Triggers runs left {runs_left} ')
		return 1
	else:
		lets.communicate('DA Internal Triggers Complete ')
		return 2

def check_files_external(intial_files):
	results = run_get_file_info()
	files = check_num_files_directory(results)
	file_size = check_file_size(results)
	
	time_difference = datetime.datetime.utcnow() -check_file_time(results)
	
	lets.communicate(f'File Information: [#: {files}, time since last: {time_difference.total_seconds()}s, size = {file_size}] ')
	#check files is just always increasing 
	try:
		updated_files=int(check_num_files_directory(results))
	except: 
		lets.communicate("Data Error where Int for File infomation not registared")
		return 0, intial_files
	
	if time_difference.total_seconds() < 1020 and file_size != '0':
		# print(f'CM: DA on run {int(updated_files)}')
		# log_file(f'DA on run {int(updated_files)} ')
		intial_files = updated_files
		return 1,intial_files
	else:
		lets.log_file('DA is not working ')
		return 0,intial_files
		

def run_storage_ctcpu():
	lab_directory()
	results = str(run_ssh('./storage_ctcpu.exp'))
	return results

def check_num_storage_ctcpu(results):
	index = results.find('df | grep -oP')
	number = results[index+37:index+39]
	number = ''.join(char for char in number if char.isnumeric())
	return number



def check_ctcpu_storage():
	print("Retrieving CTCPU storage please wait...")
	result=run_storage_ctcpu()
	number=check_num_storage_ctcpu(result)
	#number = "78"
	if int(number) < 70:
		print('\033[4m' + "CTCPU Storage is " + number +"%" +'\033[0m')
	else: 
		print('\033[4m' + '\033[91m' + "CTCPU Storage is " + number +"%" +'\033[0m')

check_ctcpu_storage()

#run_get_file_info()

#print(check_files_internal(4,39))

#print(check_files_external(8))