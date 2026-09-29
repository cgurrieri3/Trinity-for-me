import subprocess
import os
from influxdb import InfluxDBClient

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

def door(dir):
	lab_directory()
	# The command you want to run
	command = f'./doorcontrol.exp {dir}'  # Replace with your desired command
	run_ssh(command)
	

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


def LVPS(status):
	lab_directory()
	run_ssh(f'./MagnaPS_power.exp {status}')

def query_last_SM(SIAB_current, HVcurrent, SIMP_Temp, UC_Temp,MUSICpower,HVswitch,HV,trigger):
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

	for measurement in measurements:
		query = f"SELECT * FROM {measurement} ORDER BY time DESC LIMIT 1"
		result = client.query(query)
		points = list(result.get_points())

		if points:
			last_data_point = points[0]
			print(f"Last data point for measurement '{measurement}':____ {last_data_point}")
		else:
			print(f"No data found for measurement '{measurement}'")
	print(points)

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

	# check the state messages for the next 5 minutes if nothing then return error
	
	# power on camera
	if config == 'internal':
		CTM_config_hled()
	elif config == 'external':
		CTM_config_single()
	# close the door
	#open_door()


def main():
	com_in = input("Enter a command (off, init,internal(NOT DONE), external): ")

	if com_in == 'off':
		#print('off')
		shut_down_CT()
	elif com_in == 'init':
		turn_on_CT_init()
		print('State messages are ENABLED')

	elif com_in == 'internal':
		#print('internal')
		query_last_SM()
		#turn_on_CT_config(com_in)
	elif com_in == 'external':
		print('external')
		#turn_on_CT_init(com_in)

if __name__ == "__main__":
	main()

#sd_CTM()
#LVPS('off')
#shut_down_CT()
#turn_on_CT()
#on_CTM()