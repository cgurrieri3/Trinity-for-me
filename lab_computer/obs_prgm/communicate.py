import smtplib, ssl
from email.message import EmailMessage
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
import time
import sources as src

# Sends Emails
def send_email(reason):
    # SMTP server details
    # Email configuration
    sender_email = 'sstepanoff3@gatech.edu'
    receiver_email = 'TrinityObservations@groups.gatech.edu'
    # Set the subject and body of the email
    subject = 'Trinity EON'
    body = f"""
    The Trinity demonstrator has shutdown due to: {reason}

    """

    # Create the email message
    msg = MIMEMultipart()
    msg['Subject'] = subject
    msg['From'] = sender_email
    msg['To'] = receiver_email
    msg.attach(MIMEText(body, "plain"))

    # Connect to the SMTP server
    smtp_server = smtplib.SMTP('outbound.mail.gatech.edu', 25)  # Assuming smtp.gatech.edu is the SMTP server for gatech.edu
    smtp_server.set_debuglevel(1)  # Optional: This will print debug information
    smtp_server.sendmail(sender_email, receiver_email, msg.as_string())

    # Close the connection
    smtp_server.quit()
    log_file(f'Email send with EON reason: {reason}')

# Saves to log file

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/data/TrinityLabComputer/obs_prgm/LOGS/trinity.log", "a") as file:
        file.write(message + str(current_time) + '\n')


# prints to user and saves to log file to make the code easier to read
def communicate(m):
    print(m)
    log_file(f'{m} ')

def pad_word(word, total_length=30):
    if len(word) >= total_length:
        return word
    else:
        padding_length = total_length - len(word)
        left_padding = padding_length // 2
        right_padding = padding_length - left_padding
        padded_word = '*' * left_padding + word + '*' * right_padding
        return padded_word

def symbol_decorator(func):
    def wrapper(*args, **kwargs):
        print("==============================")
        result = func(*args, **kwargs)
        print("==============================")
        return result
    return wrapper

# makes pretty communications for the user to make things easier to read
@symbol_decorator
def fancy_communicate(m):
    result = pad_word(m)
    print(result)

#fancy_communicate('Trigger Rate scan complete: \nTo start a new scan type: triggerScan, newScan \nTo start DAQ type: triggerScan, startDAQ \nSM not Enabled')

def get_source_times():
    source1, source2 =src.get_source_times()
    print('NGC1068:')
    communicate(source1)
    print('TXS 0506+056:')
    communicate(source2)
    
get_source_times()

