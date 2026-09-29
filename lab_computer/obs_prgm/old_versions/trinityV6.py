import subprocess
import os
import check_sm as csm
import check_wx as cwx
import check_lt as clt
import check_data as cdata
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
	    # print(f"{command} output:")
	    # print(result.stdout)
	    print(f'{command} succecsful ')
	    log_file(f"{command} output: {result.stdout} ")
	else:
	    # Command encountered an error
	    print(f'{command} failed with an error:')
	    print(result.stderr)
	    log_file(f"{command} output error: {result.stderr} ")
	return result.stdout

def door(dir):
	lab_directory()
	# The command you want to run
	command = f'./doorcontrol.exp {dir}'  # Replace with your desired command
	run_ssh(command)
	#print('Fake door up')
	
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

def trigger_scan(start,size,step):
	lab_directory()
	run_ssh(f'./CTM_run_trigScan.exp {start} {step} {size}')


# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10):
	log_file(f"Monitoring Observations: {state} ")
	intial_files = cdata.check_data_in_folder()
	log_file(f"Monitor start: {intial_files} files ")
	total_cycles = int(intrigs_nfiles) + int(intial_files)
	log_file(f"Total Cycles = {total_cycles} ")
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
	print('------------------------')
	print('Sequence Init COMPLETE ')
	print('------------------------')
	log_file('Sequence Init COMPLETE ')
	if safe_proceed == 1:
		if config == 'internal':
			CTM_config_hled()
		elif config == 'external':
			CTM_config_single()
	print('------------------------')
	print(f"Sequence -cs {config} COMPLETE ")
	print('------------------------')
	log_file(f"Sequence -cs {config} COMPLETE ")

def internal_triggers(amount):
	# print('Rebooting CTCPU')
	# reboot_CTCPU()
	#time.sleep(10)

	turn_on_CT_config('internal')

	time.sleep(190) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner

	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)
	if safe_proceed == 1:
		print('------------------------')
		print('Sequence start COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('Sequence start & SM enable COMPLETE ')
		CTM_start()
		
		# check info while taking data
		monitor_observations('i',amount)
		# data completed or errror
		CTM_stop() # stops data but leaves Statemessages enablebd
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

def external_triggers(process): # LEFT OFF COMMENTING HERE
	clt.create_file()
	if process == 're':
	# check if statemessages are running and if everthing is configured

		CTM_config_single() # leave state messages enabled

	elif process == 'start':
		# starting from scratch
		turn_on_CT_config('external')

	print('------------------------')
	print('External Config COMPLETED')
	print('SM ENABLED')
	print('------------------------')
	log_file('external config complete ')
	time.sleep(60)
	safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
	if safe_proceed == 1:
		
		CTM_LVPS_HV(44) # State messages enabled
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
			
			CTM_HV_ON() # state messages enabled
			print('------------------------')
			print('HV ON COMPLETED')
			print('SM ENABLED')
			print('------------------------')
			log_file('HV ON COMPLETE ')

			time.sleep(120)
			safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
			if safe_proceed == 1:
				print('Set trigger_rate')
				CTM_set_trigger(170) # state messages enabled
				print('------------------------')
				print('Trigger Rate Set 170')
				print('SM ENABLED')
				print('------------------------')
				log_file('Trigger Rate set 170 ')
					# set trigger rate
					# enable state messges
				door('up')
				print('------------------------')
				print('Door Up')
				print('------------------------')
				log_file('Door up ')
				time.sleep(5)
					# open door

				CTM_start() 
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

def trigger_scan(command,start =0,step = 0,size = 0,rate = 170):
	if command == 'fromDAQ':
		CTM_stop()
		sleep(5)

		trigger_scan(start,step,size)
	elif command == 'newScan':
		trigger_scan(start,step,size)
		#add seeing file info

	elif command == 'fromIntrigs':
		turn_on_CT_config('external')

		print('------------------------')
		print('External Config COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('external config complete ')
		time.sleep(60)
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
		if safe_proceed == 1:
			
			CTM_LVPS_HV(44) # State messages enabled
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
				
				CTM_HV_ON() # state messages enabled
				print('------------------------')
				print('HV ON COMPLETED')
				print('SM ENABLED')
				print('------------------------')
				log_file('HV ON COMPLETE ')

				time.sleep(120)
				safe_proceed = csm.query_last_SM(180,17,17,1,1,18,44,1200,240,1,4)
				if safe_proceed == 1:
					trigger_scan(start,step,size)


	elif command == 'startDAQ':
		trig=input('New trigger rate:')
		turn_on_CT_config('external')

		print('------------------------')
		print('External Config COMPLETED')
		print('SM ENABLED')
		print('------------------------')
		log_file('external config complete ')
		time.sleep(60)
		safe_proceed = csm.query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,1,4)
		if safe_proceed == 1:
			print('Set trigger_rate')
			CTM_set_trigger(rate) # state messages enabled
			print('------------------------')
			print(f'Trigger Rate Set {rate}')
			print('SM ENABLED')
			print('------------------------')
			log_file(f'Trigger Rate set {rate} ')
				# set trigger rate
				# enable state messges
			door('up')
			print('------------------------')
			print('Door Up')
			print('------------------------')
			log_file('Door up ')
			time.sleep(5)
				# open door

			CTM_start() 
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


def main():
	while True:
		#check_data_directory()
		com_in = input("Enter a command (off, init, hled, single, intrigs, extrigs): ")

		if com_in.lower() == "quit" or com_in.lower() == 'exit':
			print("Exiting...")
			break

		if com_in == 'off':
			print('------------------------')
			print('Shuting Down Operations')
			print('------------------------')
			log_file('Camera shuting down ')
			shut_down_CT()
			log_file('Camera shutdown ')
			print('------------------------')
			print('Shut Down Complete')
			print('------------------------')

		elif com_in == 'init':
			print('------------------------')
			print('Steps to Sequence init')
			print('------------------------')
			
			turn_on_CT_init()
			log_file('init complete ')
			print('------------------------')
			print('State messages are ENABLED')
			print('------------------------')
			
		elif com_in == 'hled':
			print('------------------------')
			print('Steps to Sequence config hled')
			print('------------------------')
			turn_on_CT_config(com_in)
			log_file('config hled complete ')
			print('------------------------')
			print('State messages are ENABLED')
			print('------------------------')

		elif com_in == 'single':
			print('------------------------')
			print('Steps to Sequence config single')
			print('------------------------')
			turn_on_CT_config(com_in)
			log_file('config single complete')
			print('------------------------')
			print('State messages are ENABLED')
			print('------------------------')

		elif com_in == 'intrigs':
			print('------------------------')
			print('Starting internal triggers')
			print('------------------------')
			log_file('Starting internal triggers ')
			internal_triggers(10)


		elif com_in == 'extrigs':
			com_in = input("Reconfig or start from power off re, start:")
			if com_in == 're' or com_in == 'start':
				print('------------------------')
				print('Starting external triggers')
				print('------------------------')
				log_file(f'Starting external {com_in} triggers ')
				external_triggers(com_in)

		elif com_in == 'triggerScan':
			com_in = input('Enter next command fromIntrigs, fromDAQ,startDAQ,newscan:')
			if com_in == 'fromIntrigs' or com_in == 'fromDAQ' or com_in == 'newScan':
				inputs_trig=input('start step size ex 140 10 3:')
				list_trig = [int(num) for num in inputs_trig.split(' ')]
				#check that they are numbers and then seperate	
				if len(list_trig) == 3:
					trigger_scan(com_in,start=start,step=step,size=size)
			elif com_in== 'startDAQ':
				rate = input('New trigger rate: ')
				#check that it is a number
				if rate.isdigit() == True:
					trigger_scan(com_in,rate=rate)
				
		elif com_in == 'monitor_i':
			runs = input("how many internal trigger runs do you need?")
			monitor_observations('i',runs)
			monitor_observations('i',4)
			CTM_stop() # stops data but leaves Statemessages enablebd
			print('Stopping Data Aquisition')
			print('State messgaes ENABLED')

		elif com_in == 'monitor_e':
			exit_mess=monitor_observations('e')
			# close the door
			#door('down')
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

		elif com_in == 'set cutoff':
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
