import subprocess
import os
import check_sm as csm
import check_wx as cwx
import check_lt as clt
import check_data as cdata
import check_rc as crc
import ssh_scripts as ssh
import time
import datetime
import multiprocessing
import smtplib, ssl
from email.message import EmailMessage

# moonrise = 
# moonset =

# sunrise = 
# sunset = 

# when is safe to operate based on light values

# States message make sure certian values are where hey are expect
# dont change too quickly 


# Weather paramters fo safe operating
def send_email(reason):
    port = 465  # For starttls
    smtp_server = "smtp.gmail.com"
    sender_email = "sofiastepanoff22@gmail.com"
    receiver_email = "TrinityObservations@groups.gatech.edu"
    password = 'jjxqrdecssjizosh'
    
    # Set the subject and body of the email
    subject = 'Trinity EON'
    body = f"""
    The Trinity demonstrator has shutdown due to: {reason}

    """

    em = EmailMessage()
    em['From'] = sender_email
    em['To'] = receiver_email
    em['Subject'] = subject
    em.set_content(body)


    context = ssl.create_default_context()
    #server.starttls(context=context)
    with smtplib.SMTP_SSL('smtp.gmail.com', 465, context=context) as server:
        server.ehlo()
        
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, em.as_string())
        server.close()
        print('successfully sent the mail')
        log_file('successfully sent the mail')

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/mpotts32/obs_prgm/trinity.log", "a") as file:
        file.write(message + str(current_time) + '\n')


# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10):
	log_file(f"Monitoring Observations: {state} ")
	intial_files = cdata.check_data_in_folder()
	log_file(f"Monitor start: {intial_files} files ")
	total_cycles = int(intrigs_nfiles) + int(intial_files)
	log_file(f"Total Cycles = {total_cycles} ")
	cwx.take_screenshot()
	exit_message= ""
	loops = 0
	errors = 0
	while True:
	# runs while observing to look for bad weather, state messages, or light levels to shut down the camera.

		

		# files are being saved
		# add second process thread and sperate state messages from this code
		if state == 'i':
			safe_weather=cwx.query_last_wx()
			if safe_weather ==1:
				print('CM: WX weather conditions safe')
				log_file('CM: WX weather conditions safe ')
			else:
				print('CM: WX weather condtions UNSAFE')
				log_file('CM: WX weather condtions UNSAFE ')
				break
				
			
			# check state messges
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1219,240,1,4)
			if safe_proceed == 1:
				print('CM: SM within accepted limits ')
				log_file('CM: SM within accepted limits ')
			else:
				print('CM: SM outside accepted limits ')
				log_file('CM: SM outside accepted limits ')
				break

			# check files increasing to a set number and being gernerated
			if loops > 6: 
				safe_files = cdata.check_files_internal(intial_files, total_cycles)
				if safe_files != 1:
					log_file('CM: Files failed ')
					print('CM: Files failed ')
					break
				else:
					print('CM: Files saved')
					log_file('CM: Files saved ')

				

		if state == 'e':

			# check state messges
			#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate,module
			safe_light=clt.check_current_time() # one bad condition this will break
			if safe_light != 1:
				print('CM: TIME condtions UNSAFE')
				log_file('CM: TIME conditions UNSAFE ')
				exit_message = 'EON'
				break
			else:
				#print('CM: TIME conditions Safe')
				log_file('CM: TIME conditions Safe')


			print(f'Errors in statemessages: {errors}')
			log_file(f'Errors in statemessages: {errors}')
			if errors < 20:
				safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
				if safe_proceed == 1:
					print('CM: SM within accepted limits')
					log_file(f'CM: SM within accepted limits')
					errors = 0
				elif safe_proceed == -1:
					print('CM: SM HV current or HV voltage out of limits')
					print('Shutting down...')
					log_file('CM: SM HV_current/voltage out of limits, shutting down')
					exit_message = 'State messaged HV current or HV voltage'
					break
				else:
					errors = errors + 1
					print(f'CM: SM outside accepted limits {errors}/20')
					log_file('CM: SM outside accepted limits {errors}/20')
					

					
			else:
				print('CM: SM error with state messages did not get correct automatically')
				log_file('CM: SM error with state messages did not get correct automatically')
				exit_message = 'State messages NOT hv current/hv voltage'
				break


			print(f'Errors in statemessages after next state message: {errors}')
			log_file(f'Errors in statemessages after next state message: {errors}')
			if loops > 4: 
				safe_number,intial_files=cdata.check_files_external(intial_files)
				if safe_number != 1:
					print('CM: Files outside accepted limits')
					log_file('CM: Files outside accepted limits')
					exit_message = 'File saving'
					break
				else:
					print('CM: Files within accepted limits')
					log_file('CM: Files within accepted limits')


			# Check weather station
			#   query weather station
			#   make sure conditions are met
			#   if they are not quit
			#   checks to make sure the databsae is updating

			safe_weather=cwx.query_last_wx() # one bad contiions it will break
			if safe_weather ==1:
				print('CM: WX weather conditions safe')
				log_file('CM: WX weather conditions safe')
			else:
				print('CM: WX weather condtions UNSAFE')
				log_file('CM: WX weather condtions UNSAFE')
				exit_message = 'Weather conditions'
				break

			# check radar (for the future)
			# mailing list for observers use halloween mailing list
			# checking the files - 0 evts trigger board file generated - check unmerged data and remove them or move them
			# last 5 minutes -Email Mahdi

			# Light levels Adams code to study moon and sun rise and set with moon illuminations

		time.sleep(15)
		loops = loops + 1
	return exit_message


def shut_down_CT():

	# close the door
	ssh.door('down')

	# power down camera
	ssh.sd_CTM()

	rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
	if rclog == 1:
		print('------------------------')
		print('****Camera is off****')
		print('------------------------')
	else:
		print('************************')
		print('Error on Camera off')
		print('************************')
	# power down lvps
	ssh.LVPS('off')
	

	# turn off the chiller, magna, micro
	ssh.MagnaPS('off')
	ssh.MicroTSA('off')
	ssh.chiller('off')



def turn_on_CT_init():


	# turn on the chiller, magna, micro
	ssh.MagnaPS('on')
	ssh.MicroTSA('on')
	ssh.chiller('on')
	
	# power up lvps
	ssh.LVPS('on')

	ssh.CTM_init()



def turn_on_CT_config(config):

	# turn on the chiller, magna, micro
	ssh.MagnaPS('on')
	ssh.MicroTSA('on')
	ssh.chiller('on')
	
	# power up lvps
	ssh.LVPS('on')

	ssh.CTM_init()
	#print('did init stuff')
	time.sleep(80)

	# check the state messages for the next 5 minutes if nothing then return error
	#siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate
	safe_proceed=csm.query_last_SM(270, 17, 17, 1, 0, 4, 42,170,320,0,4)
	print('------------------------')
	print('Sequence Init COMPLETE ')
	print('------------------------')
	log_file('Sequence Init COMPLETE ')
	if safe_proceed == 1:
		if config == 'internal':
			ssh.CTM_config_hled()
		elif config == 'external':
			ssh.CTM_config_single()
	

def internal_triggers(amount):
	print('------------------------')
	print('Rebooting CTCPU')
	print('------------------------')
	log_file(f'Rebooting CTCPU')
	ssh.reboot_CTCPU()
	time.sleep(10)

	turn_on_CT_config('internal')
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		log_file(f'Waiting for statemessage to update after config sequence {time_counter} ')
		print(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)

	if safe_proceed == 1:
		print('------------------------')
		print(f"Sequence -cs  COMPLETE ")
		print('------------------------')
		log_file(f"Sequence -cs COMPLETE ")
		
		ssh.CTM_start()
		print('------------------------')
		print('Sequence start COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('Sequence start & SM enable COMPLETE ')
		
		# check info while taking data
		monitor_observations('i',amount)
		# data completed or errror
		ssh.CTM_stop() # stops data but leaves Statemessages enablebd
		print('------------------------')
		print('Sequence stop COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('Sequence stop & SM enable COMPLETE ')
		print('To continue to exxternal triggers use command  \'extrigs\' and then \'re\' ' )


	else:
		print('########################')
		print('SM config FIALED')
		print('########################')
		log_file('Internal triggers config sm incorrect ')

def body_extrigs():	
	print('------------------------')
	print('External Config COMPLETED')
	print('SM ENABLED')
	print('------------------------')
	log_file('external config complete ')
	time_counter = 60
	time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
	while safe_proceed != 1 and time_counter < 600:
		time.sleep(30)
		time_counter = time_counter + 30
		log_file(f'Waiting for statemessage to update after config sequence {time_counter}')
		print(f'Waiting for statemessage to update after config sequence {time_counter}')
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)

	if safe_proceed == 1:
		print('------------------------')
		print('State messages safe moving on')
		print('------------------------')
		
		ssh.CTM_LVPS_HV(44) # State messages enabled
		print('------------------------')
		print('HV updated 44.0 COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('HV to 44.0 COMPLETE ')
		time.sleep(40)
		safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)
		if safe_proceed == 1:
			
			print('------------------------')
			print('turning on HV ')
			print('------------------------')
			
			ssh.CTM_HV_ON() # state messages enabled
			print('------------------------')
			print('HV ON COMPLETED')
			print('SM ENABLED')
			print('------------------------')
			log_file('HV ON COMPLETE ')

			time.sleep(120)
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			if safe_proceed == 1:
				print('Set trigger_rate')
				ssh.CTM_set_trigger(170) # state messages enabled
				print('------------------------')
				print('Trigger Rate Set 170')
				print('SM ENABLED')
				print('------------------------')
				log_file('Trigger Rate set 170 ')
					# set trigger rate
					# enable state messges
				ssh.door('up')
				print('------------------------')
				print('Door Up')
				print('------------------------')
				log_file('Door up ')
				time.sleep(5)
					# open door

				ssh.CTM_start() 
				print('------------------------')
				print('DAQ start Completed')
				print('SM ENABLED')
				print('------------------------')
				log_file('starting data taking ')
					# start data
				# check data aquiastion
				# add continious monitoring
				# check info while taking data
				exit_mess=monitor_observations('e')
				# data completed or errror
				print('------------------------')
				print('Camera shuting down')
				print('------------------------')

				shut_down_CT() # stops data but t
				print('------------------------')
				print('Camera shutdown')
				print('SM Disabled')
				print('------------------------')
				log_file('Camera shutdown ')
				send_email(exit_mess)
				log_file(f'Email sent for reason {exit_mess} ')	

def external_triggers(process): # LEFT OFF COMMENTING HERE
	clt.create_file()
	# check the weather real fast
	safe_weather=cwx.query_last_wx()
	if safe_weather ==1:
		print('CM: Pre check WX weather conditions safe')
		log_file('CM: pre check WX weather conditions safe ')


		if process == 'start':
			# starting from scratch
			# add to check the rclog
			print('------------------------')
			print('Rebooting CTCPU')
			print('------------------------')
			log_file(f'Rebooting CTCPU')
			ssh.reboot_CTCPU()
			time.sleep(10)
			turn_on_CT_config('external')
			body_extrigs()

		
		if process == 're':
			
			# check if statemessages are running and if everthing is configured
			# add to check the rclog
			#print('uh oh')
			ssh.CTM_config_single() # leave state messages enabled
			body_extrigs()
			

		

		

	else:
		print('Weather conditions unsafe can not proceed')
		log_file('CM: pre check WX weather condtions UNSAFE ')


def trigger_scan(command,start =0,step = 0,size = 0,rate = 170):
	if command == 'fromDAQ':
		rclog=crc.check_rc_log('Starting the run#') # maybe add another line in here
		if rclog == 1: 
			ssh.CTM_stop()
			time.sleep(5)

			ssh.trigger_scan(start,step,size)

			time.sleep(step*25)

			crc.plot_triggerRate_scan()
			print('------------------------')
			print('Trigger Rate scan complete: \n')
			print('To start a new scan type: triggerScan, newScan')
			print('To start DAQ type: triggerScan, startDAQ')
			print('SM not Enabled')
			print('------------------------')

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

			print('------------------------')
			print('External Config COMPLETED')
			print('SM ENABLED')
			print('------------------------')
			log_file('external config complete ')
			time.sleep(60)
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
			if safe_proceed == 1:
				
				ssh.CTM_LVPS_HV(44) # State messages enabled
				print('------------------------')
				print('HV updated 44.0 COMPLETED')
				print('SM ENABLED')
				print('------------------------')
				log_file('HV to 44.0 COMPLETE ')
				time.sleep(40)
				safe_proceed = csm.query_last_SM(180,17,17,1,0,4,44,1200,240,1,4)
				if safe_proceed == 1:
					
					print('------------------------')
					print('turning on HV ')
					print('------------------------')
					
					ssh.CTM_HV_ON() # state messages enabled
					print('------------------------')
					print('HV ON COMPLETED')
					print('SM ENABLED')
					print('------------------------')
					log_file('HV ON COMPLETE ')

					time.sleep(120)
					safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
					if safe_proceed == 1:
						ssh.trigger_scan(start,step,size)
						# do the chekcing
						# add state message
		else:
			print(f'Incorrect step in rclog to continue with {command}')


	elif command == 'startDAQ':
		# this needs to be from whatever is at the end of a trigger scan
		rclog=crc.check_rc_log('Moving Trigger Scan Counters to: /home/trinity/Programs/Trinity/control_software')
		if rclog == 1: 
			
			ssh.CTM_config_single()

			
			time_counter = 60
			time.sleep(60) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner
			safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,2,4)
			while safe_proceed != 1 and time_counter < 600:
				time.sleep(30)
				time_counter = time_counter + 30
				log_file(f'Waiting for statemessage to update after config sequence {time_counter}')
				print(f'Waiting for statemessage to update after config sequence {time_counter}')
				safe_proceeceed = csm.query_last_SM(180, 17, 17, 1, 1, 18, 44,1200,240,2,4)

			if safe_proceed == 1:
				print('------------------------')
				print('External Config COMPLETED')
				print('SM ENABLED')
				print('------------------------')
				log_file('external config complete ')
				print('Set trigger_rate')
				ssh.CTM_set_trigger(rate) # state messages enabled
				print('------------------------')
				print(f'Trigger Rate Set {rate}')
				print('SM ENABLED')
				print('------------------------')
				log_file(f'Trigger Rate set {rate} ')
					# set trigger rate
					# enable state messges
				ssh.door('up')
				print('------------------------')
				print('Door Up')
				print('------------------------')
				log_file('Door up ')
				time.sleep(5)
					# open door

				ssh.CTM_start() 
				print('------------------------')
				print('DAQ start Completed')
				print('SM ENABLED')
				print('------------------------')
				log_file('starting data taking ')
					# start data
				# check data aquiastion
				# add continious monitoring
				# check info while taking data
				exit_mess=monitor_observations('e')
				# data completed or errror
				print('------------------------')
				print('Camera shuting down')
				print('------------------------')

				shut_down_CT() # stops data but t
				print('------------------------')
				print('Camera shutdown')
				print('SM Disabled')
				print('------------------------')
				log_file('Camera shutdown ')
				send_email(exit_mess)
				log_file(f'Email sent for reason {exit_mess} ')
			else:
				print(f'Incorrect step in rclog to continue with {command}')


def main():
	while True:
		
		#check_data_directory()
		com_in = input("Enter a command (off, intrigs, extrigs, monitor_e, monitor_i, set_cutoff): ")

		if com_in.lower() == "quit" or com_in.lower() == 'exit':
			print("Exiting...")
			break

		if com_in == 'off': # dont need to check the rc logs shuts down from any state
			print('------------------------')
			print('Shuting Down Operations')
			print('------------------------')
			log_file('Camera shuting down ')
			shut_down_CT()
			log_file('Camera shutdown ')
			print('------------------------')
			print('Shut Down Complete')
			print('------------------------')

		elif com_in == 'init': # no data running, from sequence off or reboot
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0  or rclog2 > 0:
				print('------------------------')
				print('Steps to Sequence init')
				print('------------------------')
				
				turn_on_CT_init()
				log_file('init complete ')
				print('------------------------')
				print('State messages are ENABLED')
				print('------------------------')
			else:
				print('Incorrect process based on rc log')
			
		elif com_in == 'hled': # after init
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0 or rclog2 > 0:
				print('------------------------')
				print('Steps to Sequence config hled')
				print('------------------------')
				turn_on_CT_config(com_in)
				log_file('config hled complete ')
				print('------------------------')
				print('State messages are ENABLED')
				print('------------------------')
			else:
				print('Incorrect process based on rc log')

		elif com_in == 'single': # After trigger rates, after init, NOT after data, not from start
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
			if rclog1 > 0 or rclog2 > 0:
				print('------------------------')
				print('Steps to Sequence config single')
				print('------------------------')
				turn_on_CT_config(com_in)
				log_file('config single complete')
				print('------------------------')
				print('State messages are ENABLED')
				print('------------------------')
			else:
				print('Incorrect process based on rc log')

		elif com_in == 'intrigs': # from off and after sequence off
			
			rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
			rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')

			if rclog1 > 0 or rclog2 > 0:
				print('------------------------')
				print('Starting internal triggers')
				print('------------------------')
				log_file('Starting internal triggers ')
				internal_triggers(10)

			else:
				print('Incorrect process based on rc log')



		elif com_in == 'extrigs': # re only after intrigs - so the command intrigs and statemessages on and daq stopped 
			# start seqence off or freshly rebooted 
			com_in = input("Reconfig or start from power off re, start:")
			if com_in == 're': 
				rclog=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/stop_daq_seq.txt') # maybe add another line in here
				print(rclog)
				if rclog == 1:
					print('------------------------')
					print('Starting external triggers')
					print('------------------------')
					log_file(f'Starting external {com_in} triggers ')

					external_triggers(com_in)
				else:
					print('Incorrect process based on rc log')

			if com_in == 'start': 
				rclog1=crc.check_rc_log('Staring Master Control at:') # maybe add another line in here
				rclog2=crc.check_rc_log('Finished loading this sequence: /home/trinity/Programs/Trinity/control_software/sequences/power_off_seq.txt')
				if rclog1 > 0 or rclog2 > 0:
					print('------------------------')
					print('Starting external triggers')
					print('------------------------')
					log_file(f'Starting external {com_in} triggers ')
					external_triggers(com_in)
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
			runs = input("how many internal trigger runs do you need?")
			monitor_observations('i',runs)
			monitor_observations('i',4)
			ssh.CTM_stop() # stops data but leaves Statemessages enablebd
			print('Stopping Data Aquisition')
			print('State messgaes ENABLED')

		elif com_in == 'monitor_e': # doesnt run anything wiht camera except turing it off
			exit_mess=monitor_observations('e')
			# close the door
			#ssh.door('down')
			print('------------------------')
			print('Shuting Down Operations')
			print('------------------------')
			log_file('Camera shuting down ')
			shut_down_CT()
			log_file('Camera shutdown ')
			print('------------------------')
			print('Shut Down Complete')
			print('------------------------')
			print('Sending Email to group')
			send_email(exit_mess)
			log_file(f'Email sent reason {exit_mess}')

		elif com_in == 'set_cutoff': # does not have anythong to do with camera 
			cutoff = input('Manual cut off date and time ex: 2023-11-15T12:43:09: ')
			clt.create_file(cutoff)
			log_file(f'Manual cutoff time set {cutoff}')
			print('see EON_time.txt to confirm')

		# Add a Email / teams messages if the camera shuts down prior to EON due to statemessages, weather station
		# future weather and files (talk about at meeting)
		# add files to have checked sizes
		# make trigger rate scan option and HVscan options
if __name__ == "__main__":
	#send_email('Testing Email')
	main()

#sd_CTM()
#LVPS('off')
#shut_down_CT()
#turn_on_CT()
#on_CTM()
