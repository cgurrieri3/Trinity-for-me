import subprocess
import os
import check_sm as csm
import check_wx as cwx
import check_lt as clt
import check_data as cdata
import time
import datetime

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
	return result.stdout

def door(dir):
	lab_directory()
	# The command you want to run
	command = f'./doorcontrol.exp {dir}'  # Replace with your desired command
	run_ssh(command)
	
def reboot_CTCPU():
	lab_directory()
	run_ssh(f'./reboot_CTCPU.exp')

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

def CTM_set_trigger(rate):
	lab_directory()
	command = f'./CTM_set_trigger.exp {rate}'
	run_ssh(command)


def LVPS(status):
	lab_directory()
	run_ssh(f'./MagnaPS_power.exp {status}')


# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10):
	intial_files = cdata.check_data_in_folder()
	print(intial_files)
	total_cycles = int(intrigs_nfiles) + int(intial_files)
	print(f'Total Cycles = {total_cycles}')
	
	
	while True:
	# runs while observing to look for bad weather, state messages, or light levels to shut down the camera.

		

		# files are being saved
		# add second process thread and sperate state messages from this code
		if state == 'i':
			safe_weather=cwx.query_last_wx()
			if safe_weather ==1:
				print('CM: WX weather conditions safe')
			else:
				print('CM: WX weather condtions UNSAFE')
				break
				
			
			# check state messges
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1219,240,1,4)
			if safe_proceed == 1:
				print('CM: SM within accepted limits')
			else:
				print('CM: SM outside accepted limits')
				break

			# check files increasing to a set number and being gernerated

			safe_files = cdata.check_files_internal(intial_files, total_cycles)
			if safe_files != 1:
				#print('CM: SM outside accepted limits')
				break
				

		if state == 'e':
			# check state messges
			#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate,module
			safe_light=clt.check_current_time()
			if safe_light != 1:
				print('CM: TIME condtions UNSAFE')
				break


			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			if safe_proceed == 1:
				print('CM: SM within accepted limits')
			else:
				print('CM: SM outside accepted limits')
				break

			safe_number,intial_files=cdata.check_files_external(intial_files)
			if safe_number != 1:
				print('CM: SM outside accepted limits')
				break

			# Check weather station
			#   query weather station
			#   make sure conditions are met
			#   if they are not quit
			#   checks to make sure the databsae is updating

			safe_weather=cwx.query_last_wx()
			if safe_weather ==1:
				print('CM: WX weather conditions safe')
			else:
				print('CM: WX weather condtions UNSAFE')
				break


			


			

			# check radar (for the future)
			# mailing list for observers use halloween mailing list
			# checking the files - 0 evts trigger board file generated - check unmerged data and remove them or move them
			# last 5 minutes -Email Mahdi

			# Light levels Adams code to study moon and sun rise and set with moon illuminations

		time.sleep(15)



def shut_down_CT():

	# close the door
	door('down')

	# power down camera
	sd_CTM()
		
	# power down lvps
	LVPS('off')
	

	# turn off the chiller, magna, micro
	MagnaPS('off')
	MicroTSA('off')
	chiller('off')

def turn_on_CT_init():

	# turn on the chiller, magna, micro
	MagnaPS('on')
	MicroTSA('on')
	chiller('on')
	
	# power up lvps
	LVPS('on')

	CTM_init()



def turn_on_CT_config(config):

	# turn on the chiller, magna, micro
	MagnaPS('on')
	MicroTSA('on')
	chiller('on')
	
	# power up lvps
	LVPS('on')

	CTM_init()
	#print('did init stuff')
	time.sleep(80)

	# check the state messages for the next 5 minutes if nothing then return error
	#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate
	safe_proceed=csm.query_last_SM(270, 15, 17, 1, 0, 4, 42,170,320,0,4)
	print('Init Complete')
	if safe_proceed == 1:
		if config == 'internal':
			CTM_config_hled()
		elif config == 'external':
			CTM_config_single()
	print("system configured ")

def internal_triggers(amount):
	# print('Rebooting CTCPU')
	# reboot_CTCPU()
	#time.sleep(10)

	turn_on_CT_config('internal')

	time.sleep(190) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner

	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)
	if safe_proceed == 1:
		print('Starting Data Aquisition')
		print('Statem messgaes ENABLED')
		CTM_start()
		
		# check info while taking data
		monitor_observations('i',amount)
		# data completed or errror
		CTM_stop() # stops data but leaves Statemessages enablebd
		print('Stopping Data Aquisition')
		print('State messgaes ENABLED')


	else:
		print('Statemessges for config hled are wrong')

def external_triggers(process):
	clt.create_file()
	if process == 're':
	# check if statemessages are running and if everthing is configured

		CTM_config_single() # leave state messages enabled

	elif process == 'start':
		# starting from scratch
		turn_on_CT_config('external')

	time.sleep(60)
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
	if safe_proceed == 1:
		print("updatnig HV == 44.0")
		CTM_LVPS_HV(44) # State messages enabled
		time.sleep(40)
		safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)
		if safe_proceed == 1:
			

			print("Turning on HV")
			CTM_HV_ON() # state messages enabled
			time.sleep(120)
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			if safe_proceed == 1:
				print('Set trigger_rate')
				CTM_set_trigger(170) # state messages enabled
					# set trigger rate
					# enable state messges
				door('up')
				time.sleep(5)
					# open door
				CTM_start() 
					# start data
				# check data aquiastion
				# add continious monitoring
				# check info while taking data
				monitor_observations('e')
				# data completed or errror
				shut_down_CT() # stops data but t
				print('Shutting Down Camera')


	

def main():
	while True:
		#check_data_directory()
		com_in = input("Enter a command (off, init, hled, single, intrigs, extrigs): ")

		if com_in.lower() == "quit" or com_in.lower() == 'exit':
			print("Exiting...")
			break

		if com_in == 'off':
			print('Shuting Down Operations')
			shut_down_CT()

		elif com_in == 'init':
			print('Steps to Sequence init')
			turn_on_CT_init()
			print('State messages are ENABLED')

		elif com_in == 'hled':
			print('Steps to Sequence hled')
			#siab_c, simp_t, uc_t, music_p, hv_s, hv_c, hv,trigger
			#query_last_SM(270, 15, 17, 1, 0, 4, 42,0)
			turn_on_CT_config(com_in)

		elif com_in == 'single':
			print('Steps to Sequence single')
			turn_on_CT_config(com_in)

		elif com_in == 'intrigs':
			internal_triggers(10)

		elif com_in == 'extrigs':
			com_in = input("Reconfig or start from power off re, start:")
			if com_in == 're' or com_in == 'start':
				external_triggers(com_in)
		elif com_in == 'set_EON':
			time_close_night = input("Manual set end of night: example in UTC 103123T02:45, 110123T17:34")
			# convert to datetime object
			# save to file
			#
		elif com_in == 'monitor_i':
                        runs = input("how many internal trigger runs do you need?")
                        monitor_observations('i',runs)
                        CTM_stop() # stops data but leaves Statemessages enablebd
                        print('Stopping Data Aquisition')
                        print('State messgaes ENABLED')

		elif com_in == 'monitor_e':
			monitor_observations('e')
			# close the door
			#door('down')
			print('Shutting down')
			time.sleep(10)
			shut_down_CT() # stops data but leaves Statemessages enablebd
			# print('Shutting Down Camera')
		elif com_in == 'set cutoff':
			clt.create_file(cutoff)
			print('see EON_time.txt to confirm')

		# Add a Email / teams messages if the camera shuts down prior to EON due to statemessages, weather station
		# future weather and files (talk about at meeting)
		# add files to have checked sizes
		# make trigger rate scan option and HVscan options
if __name__ == "__main__":
	main()

#sd_CTM()
#LVPS('off')
#shut_down_CT()
#turn_on_CT()
#on_CTM()
