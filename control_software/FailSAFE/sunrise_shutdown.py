import subprocess
import os
import time
from datetime import datetime, timedelta
import logging
logging.basicConfig(filename='/home/trinity/control_software/FailSAFE/sunriseCheck.log', level=logging.INFO,format='%(asctime)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')


def send_email(reason):
    port = 465  # For starttls
    smtp_server = "smtp.gmail.com"
    sender_email = "sofiastepanoff22@gmail.com"
    receiver_email = "sofiastepanoff@gatech.edu"
    #receiver_email = "TrinityObservations@groups.gatech.edu"
    password = 'jjxqrdecssjizosh'
    
    # Set the subject and body of the email
    subject = 'Sunrise Fail Safe Script has failed'
    body = f"""
    The CTCPU has {reason}

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
        #print('successfully sent the mail')
        log_file('Successfully sent email ')




def is_telesceope_safe():
	# Read thesunrise from the text file
	with open('/home/trinity/control_software/FailSAFE/sunrise.txt', 'r') as file:
		sunrise_str = file.read().strip()

	# Parse the sunrise string into a datetime object
	sunrise = datetime.strptime(sunrise_str, '%Y-%m-%d %H:%M:%S')

	# Get the current time
	current_time = datetime.now()

	# Compare the two datetime objects
	if sunrise - timedelta(minutes=5) < current_time < sunrise + timedelta(minutes=5):
		print("The sunrise from the file is around now")
		return False
	elif current_time < sunrise:
		print("The sunrise from the file is in the future.")
		return True
	elif current_time > sunrise:
		print("The sunrise from the file is in the past.")
		return True
	else:
		print("Something else happened")
		return False


def run_shut_down_ct():
    # Specify the path to your Expect script
    expect_script = "/home/trinity/control_software/FailSAFE/ct_exact_scripts/shut_down.exp"

    # Run the Expect script using the 'expect' command
    process = subprocess.Popen(["expect", expect_script], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check the output and any errors
    if process.returncode == 0:
        print("Expect script executed successfully")
        print("Output:\n", stdout)
        logging.info(f'Shut down camera Successful ')
    else:
        print("Error running Expect script")
        print("Error Output:\n", stdout)
        logging.info(f'Shut down camera failed')

def shut_down_magna():
    # Specify the path to the Python script you want to run
    script_to_run = "/home/trinity/control_software/MagnaPS/magna_off.py"

    # Run the Python script
    process = subprocess.Popen(["python3", script_to_run], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check the output and any errors
    if process.returncode == 0:
        print("Python script executed successfully")
        print("Output:\n", stdout)
    else:
        print("Error running Python script")
        print("Error Output:\n", stderr)


try:
	while True:
		
		if is_telesceope_safe() == False:
			logging.info("Passed Sunrise ")
			logging.info('Shutting Down Camera ')
			run_shut_down_ct()
			logging.info('CTM complete. Powering Down MagnaPS ')
			shut_down_magna()
			logging.info('Shutdown Complete ')
      sleep(400)
      True
        

		# enable this if you want status checks on wether there is an internet connection
		else:
			logging.info("Before Sunrise ")
      sleep(400)
      True


except:
	logging.info("Script failed sending email")
	send_email("sunrise script has failed")
	