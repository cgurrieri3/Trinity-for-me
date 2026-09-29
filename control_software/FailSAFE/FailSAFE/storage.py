import subprocess
import os
import time
import smtplib, ssl
from email.message import EmailMessage

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/trinity/control_software/FailSAFE/storage.log", "a") as file:
        file.write(message + str(current_time) + '\n')
        
def send_email(reason):
    port = 465  # For starttls
    smtp_server = "smtp.gmail.com"
    sender_email = "sofiastepanoff22@gmail.com"
    receiver_email = "sofiastepanoff@gatech.edu"
    #receiver_email = "TrinityObservations@groups.gatech.edu"
    password = 'jjxqrdecssjizosh'
    
    # Set the subject and body of the email
    subject = 'CTCPU Storage getting full'
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

def get_available_storage():
    try:
        # Run the df command and capture its output
        df_output = subprocess.check_output(['df', '-h']).decode('utf-8')
        
        # Split the output into lines and find the line containing the root filesystem
        root_fs_line = [line for line in df_output.split('\n') if '/' in line][2]

        # Extract the available storage percentage
        available_percentage = root_fs_line.split()[4]
        
        return int(available_percentage[:-1])

    except subprocess.CalledProcessError as e:
        return f"Error: {e} "

if __name__ == "__main__":
    result = get_available_storage()
    log_file(f"Used Storage: {result}% ")
    if result > 80: 
      send_email(result)