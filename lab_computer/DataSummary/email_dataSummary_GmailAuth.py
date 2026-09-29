#!/usr/bin/env python3

import smtplib, ssl
from email.message import EmailMessage
import time
import logging
import os
import subprocess
import time
import sys
from datetime import datetime, timedelta
from pypdf import PdfWriter

def run_ssh(command):
    # Run the command and capture its output
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Remove ANSI escape codes
    #result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
    # Check for errors
    if result.returncode == 0:
        # Command was successful
        # logging.info(f"{command} output:")
        # logging.info(result.stdout)
        
        logging.info(f"{command} output: {result.stdout} ")
        
    else:
        # Command encountered an error
        logging.error(f"{command} output error: {result.stderr} ")
        
    #return result.stdout


# Sends Emails
def send_email(date,attachment_path=None):
    port = 465  # For starttls
    smtp_server = "smtp.gmail.com"
    sender_email = "sofiastepanoff22@gmail.com"
    #receiver_email = "sstepanoff3@gatech.edu"
    receiver_email = "TrinityObservations@groups.gatech.edu"
    password = 'jjxqrdecssjizosh'
    
    # Set the subject and body of the email
    subject = f'Trinity Data Summary {date}'
    body = f"""
    The Trinity Demonstrator Data Summary for {date}

    """

    em = EmailMessage()
    em['From'] = sender_email
    em['To'] = receiver_email
    em['Subject'] = subject
    em.set_content(body)

    # Add attachment if provided
    if attachment_path:
        with open(attachment_path, "rb") as attachment:
            em.add_attachment(
                attachment.read(),
                maintype="application",
                subtype="octet-stream",
                filename=attachment_path.split("/")[-1],  # Adjust filename as needed
            )

    context = ssl.create_default_context()
    #server.starttls(context=context)
    try:
        with smtplib.SMTP_SSL('smtp.gmail.com', 465, context=context) as server:
            server.ehlo()
            
            server.login(sender_email, password)
            server.sendmail(sender_email, receiver_email, em.as_string())
            server.close()
            logging.info('successfully sent the mail')
            logging.info('successfully sent the mail')
    except Exception as e:
        logging.info(f'Failed to send the mail: {e}')
        logging.error(f'Failed to send the mail: {e}')

def get_most_recent_file(folder_path):
    # Get list of files in the folder
    files = os.listdir(folder_path)
    if not files:
        return None

    # Filter out directories, get file paths and creation times
    file_info = [(os.path.join(folder_path, f), os.path.getmtime(os.path.join(folder_path, f))) for f in files if os.path.isfile(os.path.join(folder_path, f))]

    # Get the most recent file based on creation time
    most_recent_file = max(file_info, key=lambda x: x[1])[0]

    return most_recent_file

def compare_file_date_with_current(file_path):
    # Get file modification time
    file_modification_time = os.path.getmtime(file_path)
    file_modification_date = datetime.fromtimestamp(file_modification_time).date()
    
     # Get current date
    current_date = datetime.now().date()

    # Check file size
    file_size = os.path.getsize(file_path)  # size in bytes

    # Convert file size to MB
    file_size = file_size / 1024  # convert bytes to megabytes

    # Compare dates and file size
    if file_modification_date == current_date and file_size > 30:
        return file_modification_date.strftime('%Y%m%d') + ".pdf"
    else:
        logging.info('File size too small or none at all')
        return False

def merge_pdf(data,path):

  # Get current date
  current_date = datetime.now().date()
  # Calculate yesterday's date
  yesterday = current_date #- timedelta(days=1)

  formatted_date = yesterday.strftime('%Y%m%d')
  pdfs = [f'{path}scheduling/schedule_pdf/schedule_{formatted_date}.pdf',data]

  merger = PdfWriter()  
  for pdf in pdfs:
      merger.append(pdf)
  name = f"DataSummary_{formatted_date}.pdf"
  merger.write(name)
  merger.close()
  return name

# The command you want to run
system_path = "/data/TrinityLabComputer/"

# Configure logging
logging.basicConfig(filename=f'{system_path}/DataSummary/email.log', level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


os.chdir(system_path+"/DataSummary/SummaryFiles/")

command = f'./getDataSummary.sh'  # Replace with your desired command
run_ssh(command)

folder_path = system_path + "DataSummary/SummaryFiles/"
most_recent_file = get_most_recent_file(folder_path)

if most_recent_file:
    logging.info(f"Most recent file: {most_recent_file}")
    todays_file=compare_file_date_with_current(most_recent_file)
    if todays_file != False:
        current_date = datetime.now().date()
        datasum_path = folder_path + todays_file
        data_sum_final=merge_pdf(datasum_path,system_path)
        attachment_path = folder_path + data_sum_final
        send_email(current_date,attachment_path)
        logging.info("The most recent file was modified today.")
        os.remove(attachment_path)
    else:
        logging.error("No file for today")
else:
    logging.info("Folder is empty.")

