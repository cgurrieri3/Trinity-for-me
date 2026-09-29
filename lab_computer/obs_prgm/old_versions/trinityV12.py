import subprocess
import os
import check_sm as csm
import check_wx as cwx
import check_lt as clt
import check_data as cdata
import check_rc as crc
import ssh_scripts as ssh
import communicate as lets
import time
import datetime
import multiprocessing


# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10,wx_override = 'no'):
	lets.log_file(f"Monitoring Observations: {state} ")

	intial_files = cdata.check_data_in_folder()
	lets.log_file(f"Monitor start: {intial_files} files ")
	total_cycles = int(intrigs_nfiles) + int(intial_files)
	lets.log_file(f"Total Cycles = {total_cycles} ")
	cwx.take_screenshot()
	exit_message= ""
	loops = 0
	errors = 0
	errors_rclog = 0
	while True:
	# While the camera is operating this will check WEATHER, STATE MESSAGES, TIME, RCLOGS for DAQ, FILES
		lets.fancy_communicate("")
		if state == 'i':
			# safe_weather=cwx.query_last_wx()
			# if safe_weather ==1:
			# 	print('CM: WX weather conditions safe')
			# 	lets.log_file('CM: WX weather conditions safe ')
			# else:
			# 	print('CM: WX weather condtions UNSAFE')
			# 	lets.log_file('CM: WX weather condtions UNSAFE ')
			# 	break
				
			
			# check state messges
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1219,240,1,4)
			if safe_proceed == 1:
				lets.communicate('Monitor: SM within limits')
				
			else:
				lets.communicate('Monitor: SM OUTSIDE limits')
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

				else:
					errors = errors + 1
					lets.communicate(f'Monitor: SM outide limits #{errors}/20')
					
			else:
				lets.communicate('Monitor: SM errors persisted to long, SHUTTING DOWN... ')
				exit_message = 'State messages NOT hv current/hv voltage'
				break

			#lets.communicate(f'Monitor: Number or errors StateMessages #{errors}.')
			
			if loops > 4: 
				
				# checks the file data
				safe_number,intial_files=cdata.check_files_external(intial_files)
				if safe_number != 1:
					lets.communicate('Monitor: Files OUTSIDES acceptable limits')
					exit_message = 'File saving'
					break

				else:
					lets.communicate('Monitor: Files saving within acceptable limits')

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

			safe_weather=cwx.query_last_wx() # one bad contiions it will break
			if safe_weather ==1:
				lets.communicate('Monitor: WX condition safe')
				
			else:
				lets.communicate('Monitor: WX conditions UNSAFE')
				exit_message = 'Weather conditions'
				break

		else:
			lets.communicate('Monitor: Weather OVERRIDE in place')
			print('Camera will NOT shut off due to weather')

		time.sleep(15)
		loops = loops + 1 # updates the loop for checking the intrigs data files
	return exit_message


def shut_down_CT():

	# close the door
	# ssh.door('down')
	# lets.fancy_communicate('Door down')

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



def turn_on_CT_init():

	lets.communicate('Turning on equipment...')
	ssh.MagnaPS('on')
	ssh.MicroTSA('on')
	ssh.chiller('on')
	ssh.LVPS('on')

	ssh.CTM_init()
	lets.fancy_communicate('Sequence init complete')



def turn_on_CT_config(config):

	# ssh.MagnaPS('on')
	# ssh.MicroTSA('on')
	# ssh.chiller('on')
	
	# ssh.LVPS('on')

	# ssh.CTM_init()
	turn_on_CT_init()
	## **14
	time_counter = 30
	time.sleep(30) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed=csm.query_last_SM(270, 17, 17, 1, 0, 4, 42,170,320,0,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(10)
		time_counter = time_counter + 10
		lets.communicate(f'Waiting for statemessage to update after init {time_counter}')
		safe_proceed=csm.query_last_SM(270, 17, 17, 1, 0, 4, 42,170,320,0,4)
	

	# check the state messages for the next 5 minutes if nothing then return error
	#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate
	
	if safe_proceed == 1:
		lets.fancy_communicate('Sequence init complete')
		lets.log_file('Sequence Init COMPLETE ')
		if config == 'internal':
			ssh.CTM_config_hled()
		elif config == 'external':
			ssh.CTM_config_single()

def full_shutdown(exit_mess):
	lets.fancy_communicate('Shuting Down Operations')
	lets.log_file('Camera shuting down ')
	
	shut_down_CT()
	lets.log_file('Camera shutdown ')
	
	lets.fancy_communicate('Shut Down Complete')
	
	lets.communicate('Sending Email to group')
	lets.send_email(exit_mess)
	lets.log_file(f'Email sent reason {exit_mess}')

def monitor_to_shutdown(wx):
	try:
		exit_mess=monitor_observations('e',wx_override = wx)

	except KeyboardInterrupt:
		lets.communicate('User stopped monitoring')
		exit_mess = 'keyinterupt'

	except:
		lets.communicate('error in monitoring')
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

	turn_on_CT_config('internal')
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)

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

def body_extrigs(wx_override = 'no',rate = 170):
	lets.fancy_communicate('External Configure Complete \n SM enabled')	
	lets.log_file('external config complete ')

	# checks the statemessage after waiting 60 seconds and then again every 30 since the asad board needs time to update
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)


	if safe_proceed == 1:
		lets.communicate('State messages safe moving on')
		
		ssh.CTM_LVPS_HV(44) # State messages enabled
		lets.fancy_communicate('HV updated 44.0 COMPLETED \n SM enabled')
		lets.log_file('HV to 44.0 COMPLETE')

		time_counter = 20 # **4
		time.sleep(20)
		safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)
		while safe_proceed != 1 and time_counter < 300:
			time.sleep(10)
			time_counter = time_counter + 10
			lets.communicate(f'Waiting for statemessage to update after LVPS update {time_counter}')
			safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)

		# add a time check for light levels!!
		if safe_proceed == 1:
			lets.fancy_communicate('Turning on HV')
			
			
			ssh.CTM_HV_ON() # state messages enabled
			lets.fancy_communicate('HV on completed \n SM enabled')
			lets.log_file('HV ON COMPLETE and SM enabled')

			time_counter = 20 # **5
			time.sleep(20)
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			while safe_proceed != 1 and time_counter < 500:
				time.sleep(20)
				time_counter = time_counter + 20
				lets.communicate(f'Waiting for statemessage to update after HV on {time_counter}')
				safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)

			if safe_proceed == 1:
				lets.fancy_communicate('Setting trigger rate')
				
				ssh.CTM_set_trigger(rate) # state messages enabled
				
				lets.fancy_communicate(f'Trigger Rate {rate} \n SM enabled')
				lets.log_file(f'Trigger Rate set {rate} ')
				
				ssh.door('up')
				lets.fancy_communicate('Door Up')
				lets.log_file('Door up ')

				time.sleep(5)
				ssh.CTM_start()

				lets.fancy_communicate('DAQ start \n SM enabled')
				lets.log_file('Starting DAQ')
				
				monitor_to_shutdown(wx_override)
				
				# data completed or error

				# lets.fancy_communicate('Camera shutting down')
				
				# shut_down_CT() # stops data

				# lets.fancy_communicate('Camera SHUT DOWN')
				# lets.log_file('Camera shutdown')
				# lets.send_email(exit_mess)
				# lets.log_file(f'Email sent for reason {exit_mess} ')	

def external_triggers(process,wx_override='no',trate = 170): # LEFT OFF COMMENTING HERE
	clt.create_file()
	safe_light=clt.check_current_time() 

	# overides the weather
	if wx_override != 'no':
		safe_weather = 1
	else:
		safe_weather = cwx.query_last_wx() # **3
 
	if safe_weather == 1 and safe_light == 1:
		lets.communicate('Time and weather are safe')
		
		if process == 'start':
			lets.fancy_communicate('Rebooting CTCPU...')
			lets.log_file(f'Rebooting CTCPU')

			ssh.reboot_CTCPU()
			time.sleep(10)

			turn_on_CT_config('external')
			body_extrigs(wx_override,trate)
		
		if process == 're':
			
			ssh.CTM_config_single() # leave state messages enabled
			body_extrigs(wx_override,trate)

	else:
		lets.communicate('Weather or Time unsafe')

def trigger_scan(command,start =0,step = 0,size = 0,rate = 170):
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
		rclog=crc.check_rc_log('Moving Trigger Scan Counters to:') # maybe add another line in here
		if rclog == 1:
			ssh.trigger_scan(start,step,size)
			# enable state messasges is NEEDED
			time.sleep(step*25)
			crc.plot_triggerRate_scan()
		else:
			print(f'Incorrect step in rclog to continue with {command}')
		

	elif command == 'fromIntrigs':
		rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt')
		if rclog == 1: 
			turn_on_CT_config('external')
			lets.communicate('External Config COMPLETED \n SM enabled')
			lets.log_file('external config complete ')

			time_counter = 60 # **7
			time.sleep(60)
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
			while safe_proceed != 1 and time_counter < 600:
				time.sleep(30)
				time_counter = time_counter + 30
				lets.communicate(f'Waiting for statemessage to update after Sequence config {time_counter}s')
				safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)


			if safe_proceed == 1:
				
				ssh.CTM_LVPS_HV(44) # State messages enabled
				lets.fancy_communicate('LVPS updated to 44V \n SM enabled')
				lets.log_file('LVPS updated to 44V \n SM enabled')

				time_counter = 20 ## **8
				time.sleep(20)
				safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)
				while safe_proceed != 1 and time_counter < 300:
					time.sleep(10)
					time_counter = time_counter + 10
					lets.communicate(f'Waiting for statemessage to update after LVPS update {time_counter}s ')
					safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)

				if safe_proceed == 1:
					
					lets.fancy_communicate('Turnning on HV')
					
					ssh.CTM_HV_ON() # state messages enabled
					lets.fancy_communicate('HV ON \n SM enabled')
					lets.log_file('HV ON COMPLETE ')

					time_counter = 30 ## **9
					time.sleep(30)
					safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
					while safe_proceed != 1 and time_counter < 600:
						time.sleep(10)
						time_counter = time_counter + 10
						lets.communicate(f'Waiting for statemessage to update after HV on {time_counter}s ')
						safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)

					if safe_proceed == 1:
						ssh.trigger_scan(start,step,size)
						# do the chekcing
						# add state message

					else:
						lets.fancy_communicate('Error in turning on HV')
				else:
					lets.fancy_communicate('Error in LVPS update')
			else: 
				lets.fancy_communicate('Error in config sequence')
		else:
			print(f'Incorrect step in rclog to continue with {command}')


	elif command == 'startDAQ':
		# this needs to be from whatever is at the end of a trigger scan
		rclog=crc.check_rc_log('Moving Trigger Scan Counters to: /home/trinity/Programs/Trinity/control_software')
		if rclog == 1: 
			
			ssh.CTM_config_single()

			
			time_counter = 60 ## **12
			time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			while safe_proceed != 1 and time_counter < 600:
				time.sleep(30)
				time_counter = time_counter + 30
				lets.communicate(f'Waiting for statemessage to update after config sequence {time_counter}')
				safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
				
			if safe_proceed == 1:
				lets.fancy_communicate('Extrenal Config complete \n SM enabled')
				lets.log_file('Extrenal Config complete \n SM enabled')
				
				lets.fancy_communicate('Setting trigger_rate')
				ssh.CTM_set_trigger(rate) # state messages enabled
				
				lets.fancy_communicate(f'Trigger Rate Set {rate} \n SM enabled')
				lets.log_file(f'Trigger Rate set {rate} ')
					
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
				monitor_to_shutdown(wx_override='no')

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
				print('Incorrect process based on rc log')



		elif com_in == 'extrigs': # re only after intrigs - so the command intrigs and statemessages on and daq stopped 
			# start seqence off or freshly rebooted 
			com_in = input("Reconfig or start from power off re, start: ")

			wx_override = input("Do you want to override the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'
			snow = input("Is there a lot of snow on the mountian? ex. yes or no:  ")
			if snow == 'yes':
				trate = 155
			else:
				trate = 170

			if com_in == 're': 
				rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt') # maybe add another line in here
				#print(rclog)
				if rclog == 1:
					lets.fancy_communicate('Starting external triggers')
					lets.log_file(f'Starting external {com_in} triggers ')
					external_triggers(com_in,wx_override,trate)

				else:
					print('Incorrect process based on rc log')

			if com_in == 'start': 
				rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
				rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
				if rclog1 > 0 or rclog2 > 0:
					lets.fancy_communicate('Starting external triggers')
					lets.log_file(f'Starting external {com_in} triggers ')
					external_triggers(com_in,wx_override,trate)

				else:
					print('Incorrect process based on rc log')

		elif com_in == 'triggerScan':
			password = input('Password: ')
			if password == 'oct3':
				com_in = input('Enter next command fromIntrigs, fromDAQ,startDAQ,newScan:')
				if com_in == 'fromIntrigs' or com_in == 'fromDAQ' or com_in == 'newScan':
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
					rte = input('New trigger rate: ')
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

			runs = input("how many internal trigger runs do you need?")
			try:
				exit_mess=monitor_observations('i',amount,wx_override)
				internal_datastop(exit_mess)

			except KeyboardInterrupt:
				lets.communicate('User stopped monitoring')

			except:
				lets.communicate('Error in monitoring')
				exit_mess = 'Error in monitoring'
				internal_datastop(exit_mess)
			
			# ssh.CTM_stop() # stops data but leaves Statemessages enablebd

			# lets.communicate('Stopping Data Aquisition')
			# lets.communicate('State messgaes ENABLED')

		elif com_in == 'monitor_e': # doesnt run anything wiht camera except turing it off
			wx_override = input("Do you want to overrride the weather ex. yes or no: ")
			if wx_override == 'yes':
				pwd_wx_ovrd = input("Please enter a password: ")
				if pwd_wx_ovrd != "oct3":
					wx_override = 'no'

			monitor_to_shutdown(wx_override)
			

		elif com_in == 'set_cutoff': # does not have anythong to do with camera 
			cutoff = input('Manual cut off date and time ex: 2023-11-15T12:43:09: ')
			clt.create_file(cutoff)
			lets.log_file(f'Manual cutoff time set {cutoff}')
			print('see EON_time.txt to confirm')

		# make trigger rate scan option and HVscan options
if __name__ == "__main__":
	#lets.send_email('Testing Email')
	main()

