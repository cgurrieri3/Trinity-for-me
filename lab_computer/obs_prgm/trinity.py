import subprocess
import os
import glob
import pandas as pd
import check_sm as csm
import check_wx as cwx
import check_lt as clt
import check_data as cdata
import check_rc as crc
import pixel_testing as pxlt
import ssh_scripts as ssh
import communicate as lets
import time
import datetime
import multiprocessing
from datetime import datetime,timedelta


# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10,wx_override = 'no',door_status='c'):
	lets.log_file(f"Monitoring Observations: {state} ")

	intial_files = cdata.check_data_in_folder()
	lets.log_file(f"Monitor start: {intial_files} files ")
	total_cycles = int(intrigs_nfiles) + int(intial_files)
	lets.log_file(f"Total Cycles = {total_cycles} ")
	# try:
	# 	cwx.take_screenshot()
	# except: 
	# 	lets.communicate("Screenshot failing moving on...")
	exit_message= ""
	loops = 0
	errors = 0
	errors_rclog = 0
	errors_WX = 0
	errors_data = 0
	bias_voltage = 44.0
	secondary_voltage = 41.5
	prior_time_check = clt.check_current_time()

	while True:
	# While the camera is operating this will check WEATHER, STATE MESSAGES, TIME, RCLOGS for DAQ, FILES
		lets.fancy_communicate("")
		if state == 'i':

			# check state messges
			safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)
			if safe_proceed == 1:
				lets.communicate('Monitor: SM within limits')

			else:
				lets.communicate(f'Monitor: SM OUTSIDE limits {safe_proceed}')
				break

			# check files
			if loops > 4: # **14
				safe_files = cdata.check_files_internal(intial_files, total_cycles)
				if safe_files == 0:
					lets.communicate('Monitor: Files FAILED after some have saved')
					break

				elif safe_files == 2:
					lets.communicate('Monitor: Files intrigs COMPLETE')
					break

				else:
					lets.communicate('Monitor: Files Saved')


			# checks the rc log for errors
			## **1 new lines to test
			if errors_rclog < 4:
				safe_rc_log=crc.check_daq()

				if safe_rc_log != 1:
					errors_rclog += 1
					lets.communicate(f'Monitor: rclog error in DAQ {errors_rclog}/4 ')
					exit_message = 'restart'

				else:
					errors_rclog = 0
					lets.communicate('Monitor: rclog DAQ good')
					exit_message = ''

			else:
				lets.communicate('Monitor: rclog had error for to many runs')
				break

		if state == 'e':
			#print('Before light')
			# Check time
			safe_light=clt.check_current_time() # one bad condition this will break
			if safe_light < 1:
				#lets.communicate('Monitor: Time Condition UNSAFE')
				exit_message = 'EON'
				break
			

			# if the time check value has changed since the last one the door needs to move if 
			# is has not already - this is easy for door closed it will just send the commmend again 
			# 2>1
			if safe_light > prior_time_check and door_status != 'never':
				# open the door 
				lets.communicate('Monitor: Time Condition safe for opening the door')
				ssh.door('up')
				door_status='o'
				lets.communicate('Door is UP')
				ssh.CTM_LVPS_HV(44)
				bias_voltage=44
				lets.communicate('Raising the bias voltage to 44 V')
			
			# 1<2
			if safe_light < prior_time_check and door_status != 'never':
				# close the door
				lets.communicate('Monitor: Time Condition required door closing')
				ssh.door('down')
				door_status='c'
				lets.communicate('Door is Down')
				
			prior_time_check = safe_light

			if door_status == 'o':
				lets.communicate('Door Status: open')
			elif door_status == 'c':
				lets.communicate('Door Status: closed')
			else:
				lets.communicate(f'Door Status: {door_status}')
				
			# check state messages
			#lets.communicate(f'Monitor: Number or errors StateMessages #{errors}.')
			safe_proceed = csm.query_last_SM(180,35,35,1,1,9,bias_voltage,1830,240,1,4)
			lets.log_file(f'SM check returned {safe_proceed}')
			if isinstance(safe_proceed, str):
				formessage = safe_proceed
				safe_proceed = 0
			if errors < 20:
				if safe_proceed == 1:
					lets.communicate('Monitor: SM within limits')
					errors = 0

				elif safe_proceed  > 5000:
					lets.communicate('Monitor: SiPM currents are above 9 mA')
					if bias_voltage==44 and door_status=='o':
						lets.communicate(f'Lowering HV to {secondary_voltage} V')
						ssh.CTM_LVPS_HV(secondary_voltage)
						time.sleep(75)
						bias_voltage=secondary_voltage
						lets.communicate("sleeping for 15 seconds after lowering HV")
					elif bias_voltage==secondary_voltage and door_status=='o':
						lets.communicate('Closing the door')
						ssh.door('down')
						door_status='c'
						lets.communicate('Raising the bias voltage to 44 V')
						ssh.CTM_LVPS_HV(44)
						bias_voltage=44
					elif bias_voltage==44 and door_status !='o':
						lets.communicate('Monitor: SM HV current to high and  door is already closed at 44V,  Shutting down... ')
						exit_message = 'State messaged HV current or HV voltage'
						break

				elif safe_proceed == -1:
					lets.communicate('Monitor: SM HV current or HV voltage out of limits, SHUTTING DOWN... ')
					exit_message = 'State messaged HV current or HV voltage'
					break

				else:
					errors = errors + 1
					lets.communicate(f'Monitor: SM {safe_proceed} is outide limits #{errors}/20')

			else:
				lets.communicate('Monitor: SM errors persisted to long, SHUTTING DOWN... ')
				exit_message = f"State Messages: {safe_proceed}"
				if safe_proceed == 0:
					exit_message = f"State Messages: {formessage}"
				break

			#lets.communicate(f'Monitor: Number or errors StateMessages #{errors}.')

			if loops > 4:
				if errors_data < 5:
					# checks the file data
					safe_number,intial_files=cdata.check_files_external(intial_files)
					if safe_number != 1:
						errors_data += 1
						lets.communicate(f'Monitor: Files OUTSIDES acceptable limits #{errors_data}/5')

					else:
						lets.communicate('Monitor: Files saving within acceptable limits')
				else:
					lets.communicate('Monitor: File data is not correct, SHUTTING DOWN...')
					exit_message = 'File saving'
					break

				## **2 new code to check when run
				# check the rc logs for the DAQ doesnt have errors
				if errors_rclog < 20:
					safe_rc_log=crc.check_daq()
					#print(safe_rc_log)
					if safe_rc_log != 1:
						errors_rclog += 1
						lets.communicate(f'Monitor: rclog error in DAQ # {errors_rclog}/20')

					else:
						errors_rclog = 0
						lets.communicate(f'Monitor: rclog for DAQ good')

				else:
					lets.communicate('Monitor: rclog DAQ did not correct, SHUTTING DOWN...')
					exit_message = 'Error in RCLOG in DAQ'
					break


		if wx_override != 'yes': # only overrides if it is yes
			if errors_WX < 4:
				safe_weather=cwx.query_last_wx() # one bad contiions it will break
				if safe_weather ==1:
					cwx.airport_report()
					errors_WX = 0
					lets.communicate('Monitor: WX condition safe')

				else:
					errors_WX +=1
					lets.communicate(f'Monitor: Weather out of bounds # {errors_WX}/4')

			else:
				# lets.communicate('Monitor: WX conditions unsafe, SHUTTING DOWN...')
				# exit_message = 'Monitor: WX condition unsafe'
				# break
				lets.communicate('Monitor: WX has become unsafe door is now closed and switched to never')
				lets.communicate('Closing the door')
				ssh.door('down')
				door_status='never'
				time.sleep(10)
				if bias_voltage != 44:
					lets.communicate('Raising the bias voltage to 44 V')
					ssh.CTM_LVPS_HV(44)
					bias_voltage=44
				wx_override = 'yes'


		else:
			lets.communicate('Monitor: Weather OVERRIDE in place')
			print('Camera will NOT shut off due to weather')

		time.sleep(15)
		print("Stop monitoring \"cntl+c\": Note this will not stop the DAQ")
		loops = loops + 1 # updates the loop for checking the intrigs data files
	return exit_message


def shut_down_CT():
	try:
		# close the door
		ssh.door('down')
		lets.fancy_communicate('Door down')

		# power down camera
		ssh.sd_CTM()

		rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
		if rclog == 1:
			lets.fancy_communicate('Camera is off')
		else:
			lets.fancy_communicate('Error in Turnning Camera off')

		ssh.LVPS('off')
		ssh.MagnaPS('off')
		ssh.MicroTSA('off')
		ssh.chiller('off')
		ssh.FlasherMat('off')

####################################################################################################
# Failsafe Shutdown -Adam ##########################################################################
		adam_fs_dir = '/data/TrinityLabComputer/obs_prgm/AdamTesting'
		adam_fsOFF_path = '/data/TrinityLabComputer/obs_prgm/AdamTesting/turn_off.sh'
		resultFS = subprocess.run(['bash', adam_fsOFF_path], cwd=adam_fs_dir, capture_output=True, text=True)
		lets.communicate('Failsafe shutdown completed')
####################################################################################################
####################################################################################################


	except KeyboardInterrupt:
		lets.communicate('shut down INCOMPLETE User stopped...')
		#exit_mess = 'keyinterupt'
	except Exception as e:
		lets.communicate('error in shutdown')
		print("An error occurred:", e)
		#exit_mess = 'error in monitoring'



def turn_on_CT_init():

	lets.communicate('Turning on equipment...')
	ssh.MagnaPS('on')
	ssh.MicroTSA('on')
	ssh.chiller('on')
	ssh.LVPS('on')
	ssh.FlasherMat('on')

	lets.fancy_communicate('Sequence init starting')
	ssh.CTM_init()



def turn_on_CT_config(config):


	turn_on_CT_init()
	## **14
	# time_counter = 30
	# time.sleep(30) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	# safe_proceed=csm.query_last_SM(270, 35, 35, 1, 0, 4, 42,170,320,0,4)
	# while safe_proceed != 1 and time_counter < 600:
	# 	time.sleep(10)
	# 	time_counter = time_counter + 10
	# 	lets.communicate(f'Waiting for statemessage to update after init {time_counter}')
	# 	safe_proceed=csm.query_last_SM(270, 35, 35, 1, 0, 4, 42,170,320,0,4)


	# check the state messages for the next 5 minutes if nothing then return error
	#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate

	#if safe_proceed == 1:
	time.sleep(5)
	lets.fancy_communicate('Sequence init complete')
	lets.log_file('Sequence Init COMPLETE ')
	if config == 'internal':
		ssh.CTM_config_hled()
	elif config == 'external':
		ssh.CTM_config_single()

def daqRECONFIGURE(rate, wx_override,door_status):

	ssh.CTM_stop()
	time.sleep(5)

	lets.fancy_communicate('DAQ stop \n SM disabled')
	lets.log_file('stopping DAQ')

	ssh.CTM_config_single()
	lets.communicate("sequence config single focus only complete")
	lets.log_file("sequence singlefocus only complete")

	ssh.CTM_set_trigger(rate) # state messages enabled

	lets.fancy_communicate(f'Trigger Threshold {rate} \n SM enabled')
	lets.log_file(f'Trigger Threshold set {rate} ')

	time.sleep(5)
	ssh.CTM_start()

	lets.fancy_communicate('DAQ start \n SM enabled')
	lets.log_file('Starting DAQ')

	monitor_to_shutdown(wx_override,door_status)




def full_shutdown(exit_mess):
	lets.fancy_communicate('Shuting Down Operations')
	lets.log_file('Camera shuting down ')

	shut_down_CT()
	lets.log_file('Camera shutdown ')

	lets.fancy_communicate('Shut Down Complete')

	lets.communicate('Sending Email to group')
	lets.send_email(exit_mess)
	lets.log_file(f'Email sent reason {exit_mess}')

def monitor_to_shutdown(wx, door_status='c'):
	# exit_mess=monitor_observations('e',wx_override = wx)
	try:
		exit_mess=monitor_observations('e',wx_override = wx, door_status=door_status)

	except KeyboardInterrupt:
		lets.communicate('User stopped monitoring')
		exit_mess = 'keyinterupt'

	except Exception as e:
		lets.communicate('error in monitoring')
		lets.communicate(f"An error occurred: {e}")
		exit_mess = 'error in monitoring'

	if exit_mess != 'keyinterupt':

		full_shutdown(exit_mess)



def internal_datastop(exit_mess):
	if exit_mess == 'restart':
		lets.fancy_communicate('Camera restart due to rclog')
		lets.log_file('Camera restart due to rclog ')
		shut_down_CT()
		lets.communicate('Camera shutdown')
		time.sleep(10)
		return 1
	else:

		# data completed or errror
		ssh.CTM_stop() # stops data but leaves Statemessages enablebd
		lets.fancy_communicate('DAQ stopping\n SM enabled')
		lets.log_file('Sequence stopping & SM enabled COMPLETE ')
		print('To continue to exxternal triggers use command  \'extrigs\' and then \'re\' ' )

def internal_triggers(amount,wx_override):
	lets.fancy_communicate('Rebooting CTCPU')
	lets.log_file(f'Rebooting CTCPU')

	ssh.reboot_CTCPU()
	time.sleep(10)

####################################################################################################>
# Failsafe Start -Adam #############################################################################>
	adam_fs_dir = '/data/TrinityLabComputer/obs_prgm/AdamTesting'
	adam_fsON_path = '/data/TrinityLabComputer/AdamTesting/turn_on.sh'
	resultFS = subprocess.run(['bash', adam_fsON_path], cwd=adam_fs_dir, capture_output=True, text=True)
	lets.communicate('Failsafe started')
####################################################################################################>
####################################################################################################>

	turn_on_CT_config('internal')
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,0,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,0,4)

	if safe_proceed == 1:
		lets.fancy_communicate('Sequence config complete')
		lets.log_file(f"Sequence -cs COMPLETE ")

		ssh.CTM_start()
		lets.fancy_communicate('DAQ starting\n SM enabled')
		lets.log_file('Sequence start & SM enabled COMPLETE ')

		# check info while taking data
		try:
			exit_mess=monitor_observations('i',amount,wx_override)
			internal_datastop(exit_mess)

		except KeyboardInterrupt:
			lets.communicate('User stopped monitoring')

		except:
			lets.communicate('Error in monitoring')
			exit_mess = 'Error in monitoring'
			internal_datastop(exit_mess)

	else:
		lets.fancy_communicate('SM config FAILED')
		lets.log_file('Internal triggers config sm failed ')


def get_most_recent_file(folder_path):
    # Use glob to list all files in the folder sorted by modification time
    files = glob.glob(os.path.join(folder_path, '*'))
    files.sort(key=os.path.getmtime, reverse=True)

    # Check if there are any files in the folder
    if files:
        most_recent_file = files[0]
        return most_recent_file
    else:
        lets.communicate("No files found in the folder.")
        return None

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
    #trate = trate - 30 #adding 7/13/2024  with triggerthreshold at 0

    ## Check if there's a value within +/-3
    # if closest_value in range(10 - 5, 10 + 5):
    #     lets.log_file(f"The closest value to 10 is {closest_value}, found in row {closest_index} threshold {trate}.")

    # else:
    #     if closest_value < 10:
    #         lets.log_file("The closest value is never being low enough.")
    #         trate = 220

    #     else:
    #         lets.log_file("The closest value is never being high enough.")
    #         trate = 120
    #trate = 290
    lets.communicate(f"The trigger threshold was set to {trate}")

    return trate


def get_new_tRate(wx_override):
    lets.communicate('Starting Trigger Rate Scan')
    trigger_scan('newScan',250,11,5,270,wx_override)
    lets.communicate('Finished Trigger Rate Scan')
    # get the value from the rc log
    time.sleep(20)
    ssh.rclog_sync()
    rcLOG_path = r"/data/TrinityLabComputer/obs_prgm/rcLogs/rc.log"
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
    os.chdir("/data/TrinityLabComputer/obs_prgm/rcLogs/")
    ssh.run_ssh(f'./triggerfile_Copy.sh {folder_date} {folder_time}')
    # get the file
    most_recent_file = get_most_recent_file(f'/data/TrinityLabComputer/obs_prgm/rcLogs/Tscan/')
    #print(most_recent_file)
    df = pd.read_csv(f'{most_recent_file}', delimiter='\t\t',skiprows = 1)
    df.columns = names=['Steps', 'Threshold', 'TriggerRate']
    lets.communicate(df)
    trate=find_tRate(df)
    lets.fancy_communicate(f'New trigger thresold to be set to {trate}')
    lets.communicate("Single Focus Only reconfigure starting")
    ssh.CTM_config_single()
    time.sleep(120)
    lets.communicate("Single Focus Only reconfigure complete")
    return trate



def body_extrigs(wx_override = 'no',noise_runs='no'):
	lets.fancy_communicate('External Configure Complete \n SM enabled')
	lets.log_file('external config complete ')

	# checks the statemessage after waiting 60 seconds and then again every 30 since the asad board needs time to update
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)


	if safe_proceed == 1:
		lets.communicate('State messages safe moving on starting DAQ')

		lets.fancy_communicate('DAQ starting')
		ssh.CTM_start()
		lets.communicate('Sleeping for 15 seconds...')
		lets.log_file('Starting DAQ sleeping for 15 seconds')
		time.sleep(15)

		lets.fancy_communicate('HV updating to 44.0')
		ssh.CTM_LVPS_HV(44) # State messages enabled
		lets.communicate('HV to 44.0 COMPLETE')
		time.sleep(5)

		lets.fancy_communicate('Turning on SIABS')
		ssh.CTM_HV_ON() # state messages enabled
		lets.communicate('HV on completed \n SM enabled')
		lets.log_file('HV ON COMPLETE and SM enabled')


		lets.fancy_communicate('Setting trigger threshold')
		# add the trigger rate scan
		rate = 150
		ssh.CTM_set_trigger(rate) # state messages enabled
		lets.fancy_communicate(f'Trigger threshold {rate} \n SM enabled')
		lets.log_file(f'Trigger threshold set {rate} ')

		if noise_runs== "yes" or noise_runs == "y":
			lets.log_file('starting noise data runs')
			lets.fancy_communicate("Starting Noise Data Runs for 22 minutes...")
			lets.communicate("\033[1;32mPlease check on ctcpu: \x1B[3mcdCData\x1B[0m for file creation.")
			current_time = datetime.now()
			time_change = timedelta(minutes=22) 
			new_time = (current_time + time_change).strftime("%H:%M:%S")
			lets.communicate(f"The data collection period will end at {new_time} ET.\nPlease check the horizon cam before opening the door\n ---")
			time.sleep(1320)
			lets.communicate("Noise Data runs are complete. \n --- ")
		while True:
			inputbyuser = input("To OPEN the door type: \"o\"\nTo keep the door CLOSED type: \"cl\" \nTo NEVER open the door: \"never\" \nTo return to the main prompt type: \"q\"\n") 
			if inputbyuser == "o":
				# user wants to open the door lets set the HV value accordingly
				set_hv=clt.get_hv_value()
				lets.communicate(f'Setting HV to {set_hv} V before opening the door')
				
				ssh.CTM_LVPS_HV(set_hv)
				time.sleep(30)

				ssh.door('up')
				lets.fancy_communicate('Door Up')
				lets.log_file('Door up ')

				

				monitor_to_shutdown(wx_override,"o")
				break
			elif inputbyuser == "q":
				lets.communicate("Not opening the door back to main prompt. ")
				break

			elif inputbyuser == "cl":
				lets.fancy_communicate("Keeping door closed.")
				lets.log_file("Keeping door closed.")

				monitor_to_shutdown(wx_override,"c")
				break
			elif inputbyuser == "never":
				lets.fancy_communicate("Never opening the door.")
				lets.log_file("Never opening the door.")
				wx_override = 'yes'
				monitor_to_shutdown(wx_override,"never")
				break

			else: 
				print("Invalid input. Please try again. ")


def noiseDataOnly():
	input1=input("Are you sure you want to run noise data? (y/n): ")	
	if input1 != "y":
		return "q"
	input2=input("HV will turn on after this command.\nIt needs to be dark\n\"c\" to continue... (c/q): ")	
	if input2 != "c":
		return "q"
	
	lets.fancy_communicate('Starting Noise Only Data')
	lets.log_file(f'Starting Noise Only Data')
	ssh.CTM_config_single() # leave state messages enabled

	lets.fancy_communicate('External Configure Complete \n SM enabled')
	lets.log_file('external config complete ')

	#checks the statemessage after waiting 60 seconds and then again every 30 since the asad board needs time to update
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)
	

	if safe_proceed == 1:
		lets.communicate('State messages safe moving on starting DAQ')

		lets.fancy_communicate('DAQ starting')
		ssh.CTM_start()
		lets.communicate('Sleeping for 15 seconds...')
		lets.log_file('Starting DAQ sleeping for 15 seconds')
		time.sleep(15)

		lets.fancy_communicate('HV updating to 44.0')
		ssh.CTM_LVPS_HV(44) # State messages enabled
		lets.communicate('HV to 44.0 COMPLETE')
		time.sleep(5)

		lets.fancy_communicate('Turning on SIABS')
		ssh.CTM_HV_ON() # state messages enabled
		lets.communicate('HV on completed \n SM enabled')
		lets.log_file('HV ON COMPLETE and SM enabled')


		lets.fancy_communicate('Setting trigger threshold')
		# add the trigger rate scan
		rate = 150
		ssh.CTM_set_trigger(rate) # state messages enabled
		lets.fancy_communicate(f'Trigger threshold {rate} \n SM enabled')
		lets.log_file(f'Trigger threshold set {rate} ')

		
		lets.log_file('starting noise data runs')
		lets.fancy_communicate("Starting Noise Data Runs for 22 minutes...")
		lets.communicate("\033[1;32mPlease check on ctcpu: \x1B[3mcdCData\x1B[0m for file creation.")
		current_time = datetime.now()
		time_change = timedelta(minutes=22) 
		new_time = (current_time + time_change).strftime("%H:%M:%S")
		lets.communicate(f"The data collection period will end at {new_time} ET.\nDOOR WILL NOT OPEN \n ---")
		time.sleep(1320)
		lets.communicate("Noise Data runs are complete. \n --- ")

		full_shutdown("Complete Noise Data Runs")


def external_triggers(process,wx_override='no',noise_runs='no'): # LEFT OFF COMMENTING HERE
	clt.create_file()
	## add an while loop that will wait till after sunset to start the telesocpe 
	safe_light=clt.check_current_time()
	while safe_light < 1:
		lets.communicate("Waiting to check the time again in 60 seconds...")
		time.sleep(60) # check every minute
		safe_light=clt.check_current_time()

	lets.communicate('Time is safe starting extrigs')
	# overides the weather
	if wx_override != 'no':
		safe_weather = 1
	else:
		safe_weather = cwx.query_last_wx() # **3

	if safe_weather == 1 and safe_light >= 1:
		lets.communicate('Time and weather are safe')

		if process == 'start':
			lets.fancy_communicate('Rebooting CTCPU...')
			lets.log_file(f'Rebooting CTCPU')

			ssh.reboot_CTCPU()
			time.sleep(10)

####################################################################################################>
# Failsafe Start -Adam #############################################################################>
			adam_fs_dir = '/data/TrinityLabComputer/obs_prgm/AdamTesting'
			adam_fsON_path = '/data/TrinityLabComputer/obs_prgm/AdamTesting/turn_on.sh'
			resultFS = subprocess.run(['bash', adam_fsON_path], cwd=adam_fs_dir, capture_output=True, text=True)
			lets.communicate('Failsafe started')
####################################################################################################>
####################################################################################################>


			turn_on_CT_config('external')
			body_extrigs(wx_override,"no")

		if process == 're':

			ssh.CTM_config_single() # leave state messages enabled
			body_extrigs(wx_override,noise_runs)

	else:
		lets.communicate('Weather or Time unsafe')


def trigger_scan(command,start =0,step = 0,size = 0,rate = 170,weather='no'):
	if command == 'fromDAQ':
		rclog=crc.check_rc_log('Starting the run#') # maybe add another line in here
		if rclog == 1:
			ssh.CTM_stop()
			time.sleep(5)

			ssh.trigger_scan(start,step,size)

			time.sleep(step*25)

			crc.plot_triggerRate_scan()
			lets.fancy_communicate('Trigger Rate scan complete: \n To start a new scan type: triggerScan, newScan \n To start DAQ type: triggerScan, startDAQ \n SM not Enabled')


		else:
			print(f'Incorrect step in rclog to continue with {command}')

	elif command == 'newScan':
		ssh.trigger_scan(start,step,size)
		lets.communicate(f'Time for TriggerScan to complete: {step*25}s')
		time.sleep(step*25)
		crc.plot_triggerRate_scan()


	# elif command == 'fromIntrigs':
		
	# 	rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt')
		
	# 	if rclog == 1: 

	# 		ssh.CTM_config_single()
	# 		lets.communicate('External Config COMPLETED \n SM enabled')
	# 		lets.log_file('external config complete ')

	# 		time_counter = 60 # **7
	# 		time.sleep(60)
	# 		safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)
	# 		while safe_proceed != 1 and time_counter < 600:
	# 			time.sleep(30)
	# 			time_counter = time_counter + 30
	# 			lets.communicate(f'Waiting for statemessage to update after Sequence config {time_counter}s')
	# 			safe_proceed = csm.query_last_SM(180, 35, 35, 1, 0, 4, 42,1830,240,1,4)


	# 		if safe_proceed == 1:

	# 			ssh.CTM_LVPS_HV(44) # State messages enabled
	# 			lets.fancy_communicate('LVPS updated to 44V \n SM enabled')
	# 			lets.log_file('LVPS updated to 44V \n SM enabled')

	# 			time_counter = 20 ## **8
	# 			time.sleep(20)
	# 			safe_proceed = csm.query_last_SM(180,35,35,1,0,4,44,1830,240,1,4)
	# 			while safe_proceed != 1 and time_counter < 300:
	# 				time.sleep(10)
	# 				time_counter = time_counter + 10
	# 				lets.communicate(f'Waiting for statemessage to update after LVPS update {time_counter}s ')
	# 				safe_proceed = csm.query_last_SM(180,35,35,1,0,4,44,1830,240,1,4)

	# 			if safe_proceed == 1:

	# 				lets.fancy_communicate('Turnning on HV')

	# 				ssh.CTM_HV_ON() # state messages enabled
	# 				lets.fancy_communicate('HV ON \n SM enabled')
	# 				lets.log_file('HV ON COMPLETE ')

	# 				time_counter = 30 ## **9
	# 				time.sleep(30)
	# 				safe_proceed = csm.query_last_SM(180,35,35,1,1,12,44,1830,240,1,4)
	# 				while safe_proceed != 1 and time_counter < 600:
	# 					time.sleep(10)
	# 					time_counter = time_counter + 10
	# 					lets.communicate(f'Waiting for statemessage to update after HV on {time_counter}s ')
	# 					safe_proceed = csm.query_last_SM(180,35,35,1,1,12,44,1830,240,1,4)

	# 				if safe_proceed == 1:
	# 					ssh.trigger_scan(start,step,size)
	# 					# do the chekcing
	# 					# add state message

	# 				else:
	# 					lets.fancy_communicate('Error in turning on HV')
	# 			else:
	# 				lets.fancy_communicate('Error in LVPS update')
	# 		else:
	# 			lets.fancy_communicate('Error in config sequence')
	# 	else:
	# 		print(f'Incorrect step in rclog to continue with {command}')


	elif command == 'startDAQ':
		# this needs to be from whatever is at the end of a trigger scan
		rclog=crc.check_rc_log('Moving Trigger Scan Counters to: /home/trinity/Programs/Trinity/control_software')
		if rclog == 1:

			ssh.CTM_config_single()


			time_counter = 60 ## **12
			time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
			safe_proceed = csm.query_last_SM(180,35,35,1,1,9,44,1830,240,1,4)
			while safe_proceed != 1 and time_counter < 600:
				time.sleep(30)
				time_counter = time_counter + 30
				lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
				safe_proceed = csm.query_last_SM(180,35,35,1,1,9,44,1830,240,1,4)

			if safe_proceed == 1:
				lets.fancy_communicate('Extrenal Config complete \n SM enabled')
				lets.log_file('Extrenal Config complete \n SM enabled')

				lets.fancy_communicate('Setting trigger_rate')
				ssh.CTM_set_trigger(rate) # state messages enabled

				lets.fancy_communicate(f'Trigger threshold Set {rate} \n SM enabled')
				lets.log_file(f'Trigger threshold set {rate} ')

				ssh.door('up')
				lets.fancy_communicate('The door is up')
				lets.log_file('Door up ')
				time.sleep(5)
					# open door

				ssh.CTM_start() 
				lets.fancy_communicate('DAQ starting... ')
				lets.log_file('DAQ starting ')
					# start data
				# check data aquiastion
				# add continious monitoring
				# check info while taking data
				monitor_to_shutdown(weather,"o")

			else:
				lets.communicate('Config failed to update')
		else:
			lets.communicate(f'Incorrect step in rclog to continue with {command}')

def main():
	while True:

		#check_data_directory()
		com_in = input("Enter a command (off, intrigs, extrigs, monitor_e, monitor_i, set_cutoff): ")

		if com_in.lower() == "quit" or com_in.lower() == 'exit':
			print("Exiting...")
			break

		if com_in == 'off': # dont need to check the rc logs shuts down from any state
			lets.fancy_communicate('Shuting Down Operations')
			lets.log_file('Camera shuting down ')

			shut_down_CT()
			lets.log_file('Camera shutdown ')
			lets.fancy_communicate('Shut Down Complete')

		elif com_in == 'init': # no data running, from sequence off or reboot
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0  or rclog2 > 0:
				lets.fancy_communicate('Steps to Sequence init')

				turn_on_CT_init()

				lets.fancy_communicate('Sequence init complete')
				lets.log_file('init complete ')
			else:
				print('Incorrect process based on rc log')

		elif com_in == 'hled': # after init
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0 or rclog2 > 0:
				lets.fancy_communicate('Steps to Sequence config hled')
				turn_on_CT_config(com_in)

				lets.fancy_communicate('Sequence config complete')
				lets.log_file('config hled complete ')

			else:
				print('Incorrect process based on rc log')

		elif com_in == 'single': # After trigger rates, after init, NOT after data, not from start
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0 or rclog2 > 0:
				lets.fancy_communicate('Steps to Sequence config singleFocus')

				turn_on_CT_config(com_in)
				lets.fancy_communicate(' Sequence config complete')
				lets.log_file('config single complete')

			else:
				print('Incorrect process based on rc log')

		elif com_in == 'intrigs': # from off and after sequence off
			wx_override = input("Do you want to override the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')

			if rclog1 > 0 or rclog2 > 0:

				lets.fancy_communicate('Starting internal triggers')
				lets.log_file('Starting internal triggers ')

				restart=internal_triggers(10,wx_override)

				if restart == 'restart':
					restart=internal_triggers(10,wx_override)

					if restart == 'restart':
						lets.communicate('Problem with rclog tried restarting Camera is off')
				else:
					internal_datastop("none")


			else:
				print('Incorrect process based on rc log')



		elif com_in == 'extrigs': # re only after intrigs - so the command intrigs and statemessages on and daq stopped
			# start seqence off or freshly rebooted
			com_in = input("Reconfig or start from power off re, start, DAQre: ")

			wx_override = input("Do you want to override the weather (yes or no): ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			
			# add later  option to change the LVPS voltage setting

			if com_in == 're':
				noise_runs = input("Take HV ON noise runs (yes or no): ")
				rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt') # maybe add another line in here
				#print(rclog)
				if rclog == 1:
					lets.fancy_communicate('Starting external triggers')
					lets.log_file(f'Starting external {com_in} triggers ')
					external_triggers(com_in,wx_override,noise_runs)

				else:
					print('Incorrect process based on rc log')

			if com_in == 'noiseOnly':
				rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt') # maybe add another line in here
				#print(rclog)
				if rclog == 1:
					
					#external_triggers(com_in,wx_override,'yes')
					noiseDataOnly()

				else:
					print('Incorrect process based on rc log')

			if com_in == 'start':
				rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
				rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
				if rclog1 > 0 or rclog2 > 0:
					lets.fancy_communicate('Starting external triggers')
					lets.log_file(f'Starting external {com_in} triggers ')
					external_triggers(com_in,wx_override,"no")

				else:
					print('Incorrect process based on rc log')

			if com_in == 'DAQre':
				double_check = input("This is to reconfigure after DAQ starts do you want to proceed enter a trigger threshold!: ")
				if double_check.isdigit():
					door_status = input("Is the door open or closed or never open? ex. (o/c/never):")
					if door_status == 'never':
						wx_override = 'yes'
					daqRECONFIGURE(double_check,wx_override,door_status)

		elif com_in == 'triggerScan':
			password = input('Password: ')
			if password == 'oct3':
				com_in = input('Enter next command fromDAQ,startDAQ,newScan:')
				if com_in == 'fromDAQ' or com_in == 'newScan':
					inputs_trig=input('start step size ex 140 10 3:')
					try:
						list_trig = [int(num) for num in inputs_trig.split(' ')]
						#check that they are numbers and then seperate
						if len(list_trig) == 3:
							strt = list_trig[0]
							stp = list_trig[1]
							sze = list_trig[2]
							trigger_scan(com_in,start=strt,step=stp,size=sze)
					except:
						pass
				elif com_in== 'startDAQ':
					rte = input('New trigger threshold: ')
					#check that it is a number
					if rte.isdigit() == True:
						trigger_scan(com_in,rate=rte)

			else:
				print('password incorrect no access to triggerScan')

		elif com_in == 'monitor_i': # doesnt run anythong with camera except turning it off
			wx_override = input("Do you want to overrride the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			amount = input("how many internal trigger runs do you need?")
			exit_mess=monitor_observations('i',amount,wx_override)
			# try:
			# 	exit_mess=monitor_observations('i',amount,wx_override)
			# 	internal_datastop(exit_mess)

			# except KeyboardInterrupt:
			# 	lets.communicate('User stopped monitoring')

			# except:
			# 	lets.communicate('Error in monitoring')
			# 	exit_mess = 'Error in monitoring'
			# 	internal_datastop(exit_mess)

			# ssh.CTM_stop() # stops data but leaves Statemessages enablebd

			# lets.communicate('Stopping Data Aquisition')
			# lets.communicate('State messgaes ENABLED')

		elif com_in == 'monitor_e': # doesnt run anything wiht camera except turing it off
			wx_override = input("Do you want to overrride the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			door_status = input("Is the door open or closed or never open? ex. (o/c/never):")
			if door_status == 'never':
				wx_override = 'yes'
			# hv_status = float(input("Is the HV 44.0 or 41.5:"))
			lets.log_file(f'Starting external monitor with door {door_status}')
			monitor_to_shutdown(wx_override,door_status)


		elif com_in == 'set_cutoff': # does not have anythong to do with camera 
			cutoff = input('Manual cut off date and time ex: "2023-11-15 12:43:09": ')
			if cutoff == '' or cutoff == ' ':
				clt.create_file()

			else:
				date_format = "%Y-%m-%d %H:%M:%S"
				try:
					cutoff_fmt = datetime.strptime(cutoff, date_format)
					clt.create_file(cutoff_fmt)
					lets.log_file(f'Manual cutoff time set {cutoff}')
					print('see EON_time.txt to confirm')
				except:
					print('Invalid input please try again')
		elif com_in == 'SIAB_test': # used to run the pixel testing so there can be monitoring

			wx_override = input("Do you want to overrride the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			print('Starting SAIB testing script on CTCPU')
			exit_mesesage=pxlt.SIAB_testing(wx_override)
			#	full_shutdown(exit_message)
			print(f'shut down {exit_mesesage}')

		elif com_in == "siaboff":
			uinput = input("Which SIAB would you like to turn off? ")
			if 0<= int(uinput) <=15:
				yes=input("The DAQ is running? y or n: ")
				if yes == 'y':
					lets.communicate(f"Turing off SIAB {uinput}")
					ssh.CTM_HV_OFF(uinput)
					lets.communicate(f"Completed")
				else:
					print("DAQ needs to be running")
			else:
				print("Please enter a number between 0-15 ")
		# make trigger rate scan option and HVscan options


if __name__ == "__main__":
	#lets.send_email('Testing Email')
	main()

