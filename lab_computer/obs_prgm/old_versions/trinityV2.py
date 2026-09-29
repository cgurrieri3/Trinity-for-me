import subprocess
import os
from influxdb import InfluxDBClient
import time

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

def check_data_directory():
	lab_directory()
	# Run the Bash script and capture its output
	result = str(run_ssh('./get_num_files.exp'))
	index = result.find('ls -1 | wc -l \\r\\n')
	number = result[index+18:index+22]
	number = ''.join(char for char in number if char.isnumeric())
	
	return number

def LVPS(status):
	lab_directory()
	run_ssh(f'./MagnaPS_power.exp {status}')

def accepted_values_16(meas, uc_t, simp_t, hv_s, siab_c, music_p):
	if meas == "UC_temps":
		acpt_low = uc_t * 0 + -10
		acpt_hi = uc_t * 1.3

	elif meas == "SiPM_temps":
		acpt_low = simp_t*0 + -10
		acpt_hi = simp_t*1.3

	elif meas == 'HV_Status':
		acpt_low = hv_s * 1
		acpt_hi = hv_s *1
	elif meas == "SIABcurrent":
		acpt_low = siab_c * .8
		acpt_hi = siab_c * 1.2
	elif meas == "MUSIC_Power":
		acpt_low = music_p *1
		acpt_hi = music_p *1
	elif meas == "other":
		acpt_low = 0
		acpt_hi = 0
	return acpt_low, acpt_hi

def dp16(ldp,meas,siab_c, simp_t, uc_t, music_p,hv_s):
	
	#print('hey',meas)
	low,hi=accepted_values_16(meas, uc_t, simp_t, hv_s,siab_c,music_p)
	#print(low, hi)

	checked = []
	index = list(range(1, 17))
	

	for i in index:
		
		value=float(ldp[f'{i}'])
		if value == low and value == hi:
			#print('yes')
			checked.append(1)

		elif value >= low and value <= hi:
			#print('yay')
			checked.append(1)
		else:
			#print('nay')
			checked.append(0)
	#if meas == 'SIABcurrent' or meas == 'HV_Status' or meas == 'MUSIC_Power' or meas =:
	total=sum(checked)
	if total == 16:
		#print(f'Conditions matched {meas}')
		return 1
	else:
		print(f'Conditions not matched {meas}')
		return 0
			
def accepted_values_1(meas, asad,tb_c, trigger_rate):
	#print(asad)
	#print(meas)
	if meas == "ASADCurrent":
		acpt_low = asad * 0.9
		acpt_hi = asad * 1.3
	elif meas == "TBCurrent":
		acpt_low = tb_c * 0.9
		acpt_hi = tb_c * 1.1
	elif meas == "TriggerRate":
		acpt_low = trigger_rate * 0
		acpt_hi = trigger_rate * 3999
	return acpt_low, acpt_hi


def dp1(ldp,meas,asad,tb_c,trigger_rate):
	low,hi=accepted_values_1(meas, asad,tb_c,trigger_rate)
	#print(meas)
	#print(low, hi)

	
		
	value=float(ldp['value1'])
	if value == low and value == hi:
		#print('yes')
		return 1

	elif value >= low and value <= hi:
		#print('yay')
		return 1
	else:
		#print('nay')
		print(f'Conditions not matched {meas}')
		return 0

def accepted_values_4(meas, hv_c,hv):
	#print(asad)
	#print(meas)
	if meas == "HV":
		acpt_low = hv * 0.95
		acpt_hi = hv * 1.05
	elif meas == "HV_currents":
		acpt_low = hv_c * 0.5
		acpt_hi = hv_c * 1.1
	return acpt_low, acpt_hi

def dp4(ldp,meas,hv_c,hv):
	#print('hey',meas)
	low,hi=accepted_values_4(meas, hv_c,hv)
	#print(low, hi)

	checked = []
	index = list(range(1, 5))
	

	for i in index:
		
		value=float(ldp[f'{i}'])
		if value == low and value == hi:
			#print('yes')
			checked.append(1)

		elif value >= low and value <= hi:
			#print('yay')
			checked.append(1)
		else:
			#print('nay')
			checked.append(0)
	#if meas == 'SIABcurrent' or meas == 'HV_Status' or meas == 'MUSIC_Power' or meas =:
	total=sum(checked)
	if total == 4:
		#print(f'Conditions matched {meas}')
		return 1
	else:
		print(f'Conditions not matched {meas}')
		return 0

def dp8(ldp,meas,total):

	index = list(range(1,9))
	s = 0
	for i in index:
		value = float(ldp[f'{i}'])
		s = value + s

	if s == 4:
		return 1
	else:
		print(f'Conditions not matched {meas}')
		return 0

def query_last_SM(siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate,module):
	#SIAB_current, HVcurrent, SIMP_Temp, UC_Temp,MUSICpower,HVswitch,hv_Current, hv, asad, tb_current,trigger
	# Local host lines for access
	host = 'localhost'
	port = 8086
	username = 'mpotts32'
	password = 'Ttys@210'
	database = 'trinity_ct' # Database for independent measurements for each day, need to change the measurent line in cre_df_list()
    #database = 'Trinity1'

    # Initialize the InfluxDB client and write the points in batches
	client = InfluxDBClient(host = host, port=port, username=username, password=password,database=database)

    # Create a new database if it does not already exist
    # client.create_database(database)
	result = client.query('SHOW MEASUREMENTS')
	measurements = [measurement['name'] for measurement in result.get_points()]

	matched_conditions = []
	for measurement in measurements:
		query = f"SELECT * FROM {measurement} ORDER BY time DESC LIMIT 1"
		result = client.query(query)
		points = list(result.get_points())
		#print(points)
		for data in points:
		    for key, value in data.items():
		        if value is None:
		            data[key] = 0
		#print(points)
		if points:
			last_data_point = points[0]
			try:
				print(f"Last data point for measurement '{measurement}': {last_data_point['1']}")

				try:
					#print(f"Last data point for measurement '{measurement}': {last_data_point['16']}")
					#siab_c, simp_t, uc_t, music_p,hv_s
					match_status = dp16(last_data_point,measurement,siab_c, simp_t, uc_t, music_p,hv_s)	
					matched_conditions.append(match_status)
					#print(matched_conditions)
				except Exception as e:
					#print(e)
					try:

						#print(f"Last data point for measurement '{measurement}': {last_data_point['8']}")
						matched_status=dp8(last_data_point,measurement,module)
						#print(matched_conditions)
					except:
						#print(f"Last data point for measurement '{measurement}': {last_data_point['4']}")
						match_status = dp4(last_data_point,measurement,hv_c, hv)
						matched_conditions.append(match_status)
						#print(matched_conditions)
			except:
				print(f"Last data point for measurement '{measurement}': {last_data_point['value1']}")
				match_status=dp1(last_data_point,measurement,asad,tb_c,trigger_rate)
				matched_conditions.append(match_status)
				#print(matched_conditions)
		else:
			print(f"No data found for measurement '{measurement}'")
		#print(points)
	all_conditions = sum(matched_conditions)
	if len(matched_conditions) == all_conditions:
		print('proceeding to next command')
		return 1
	else:
		print('check statemessages something is wrong')
		return 0

# state: internal or external, intrigs_nfiles: number of internal trigger files.
def monitor_observations(state,intrigs_nfiles=10):
	files = check_data_directory()
	print(files)
	total_cycles = int(intrigs_nfiles) + int(files)
	print(f'Total Cycles = {total_cycles}')
	while True:
	# runs while observing to look for bad weather, state messages, or light levels to shut down the camera.

		

		# files are being saved
		# add second process thread and sperate state messages from this code
		if state == 'i':
			# check state messges
			safe_proceed = query_last_SM(180, 17, 17, 1, 0, 4, 42,1219,240,1,4)
			if safe_proceed == 1:
				print('CM: SM within accepted limits')
			else:
				print('CM: SM outside accepted limits')
				break

			# check files increasing to a set number
			# possible error the number never gets high enough and doesnt stop
			files=check_data_directory()
			if total_cycles > int(files):
				print(f'CM: DA Internal Triggers runs left {total_cycles - int(files)}')
			else:
				print('CM: DA Internal Triggers Complete')
				break
				

		if state == 'e':
			# check state messges
			safe_proceed = query_last_SM(180,17,17,1,1,18,44,160,240,1,4)
			if safe_proceed == 1:
				print('CM: SM within accepted limits')
			else:
				print('CM: SM outside accepted limits')
				break

			# check files is just always increasing
			files=check_data_directory()
			if total_cycles > int(files):
				print(f'CM: DA on run {total_cycles - int(files)}')
			else:
				print('CM: DA is not working')
				break


			# Check weather station
			  # query weather station
			  # make sure conditions are met
			  # if they are not quit

			# check radar (for the future)

			# Light levels Adams code to study moon and sun rise and set with moon illuminations

		time.sleep(60)



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
	safe_proceed=query_last_SM(270, 15, 17, 1, 0, 4, 42,170,320,0,4)
	print('Init Complete')
	if safe_proceed == 1:
		if config == 'internal':
			CTM_config_hled()
		elif config == 'external':
			CTM_config_single()
	print("system configured ")

def internal_triggers(amount):
	print('Rebooting CTCPU')
	reboot_CTCPU()
	#time.sleep(10)

	turn_on_CT_config('internal')

	time.sleep(120) # change this to try try for 3 minutes every 20 seconds rather then wait incase it is sooner

	safe_proceed = query_last_SM(180, 17, 17, 1, 0, 4, 42,1200,240,0,4)
	if safe_proceed == 1:
		print('Starting Data Aquisition')
		print('Statem messgaes ENABLED')
		CTM_start()
		
		# check info while taking data
		monitor_observations('i',intrigs_nfiles=10)
		# data completed or errror
		CTM_stop() # stops data but leaves Statemessages enablebd
		print('Stopping Data Aquisition')
		print('State messgaes ENABLED')


	else:
		print('Statemessges for config hled are wrong')

def external_triggers(process):
	if process == 're':
	# check if statemessages are running and if everthing is configured

		CTM_config_single() # leave state messages enabled

	elif process == 'start':
		# starting from scratch
		turn_on_CT_config('external')

	time.sleep(60)
	safe_proceed = query_last_SM(180, 17, 17, 1, 0, 4, 42,160,240,1,4)
	if safe_proceed == 1:
		print("updatnig HV == 44.0")
		CTM_LVPS_HV(44) # State messages enabled
		safe_proceed = query_last_SM(180,17,17,1,0,4,44,160,240,1,4)
		if safe_proceed == 1:
			

			print("Turning on HV")
			CTM_HV_ON() # state messages enabled
			safe_proceed = query_last_SM(180,17,17,1,1,18,44,160,240,1,4)
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


	

def main():
	while True:

		#check_data_directory()
		com_in = input("Enter a command (off, init, hled, single, intrigs, extrigs): ")

		if com_in.lower() == "quit" or com_in.lower() == 'exit':
			print("Exiting...")
			break

		if com_in == 'off':
			print('Stutting Down Operations')
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
			print('Steps to Sequence hled')
			turn_on_CT_init(com_in)

		elif com_in == 'intrigs':
			internal_triggers(3)

		elif com_in == 'extrigs':
			com_in = input("Reconfig or start from power off re, start:")
			if com_in == 're' or com_in == 'start':
				external_triggers(com_in)
		

if __name__ == "__main__":
	main()

#sd_CTM()
#LVPS('off')
#shut_down_CT()
#turn_on_CT()
#on_CTM()
