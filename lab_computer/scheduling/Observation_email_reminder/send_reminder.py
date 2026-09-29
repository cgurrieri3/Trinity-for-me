import smtplib
import os
from datetime import datetime
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
import pandas as pd

# Log file path
log_file = "/data/TrinityLabComputer/scheduling/Observation_email_reminder/log_scheduling_email.txt"

def log_message(message):
    """Append log messages to the log file with a timestamp."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(log_file, "a") as log:
        log.write(f"[{timestamp}] {message}\n")

# Send email to the group
def send_email(date, observer, oncall, obs_start, obs_end):
    group_email = "TrinityObservations@groups.gatech.edu"
    sender_email = "sofiastepanoff@gatech.edu"  # Replace if needed
    cc_email = "azhang421@gatech.edu"      # You will be CC'ed

    #Change from jpg to png
    # File to attach
    attachment_path = "/data/TrinityLabComputer/scheduling/schedule.png"
    attachment_name = "schedule.png"
    
    
    # Default subject and body
    subject = f"Observation Reminder - {date}"
    body = ""

    if observer and oncall:
        if oncall=='Angelina':
            subject = f"🔭 Observation Reminder - {date} | On-call: {oncall}, Observer: {observer}"
            body = f"""
            <html>
            <body>
            <p>Dear <b>{observer}</b>,</p>

                <p>This is a friendly reminder about your scheduled observations <b>tonight</b>.</p>
            
                <p>The on-call person for tonight is <b>{oncall}</b> who shall be available in case you encounter any issues during operations.</p>
            
                <p>The observation window starts at <b>{obs_start} ET</b> and ends at <b>{obs_end} ET</b>.</p>          

                <p>Please ensure that you are prepared to begin the observations on time, set your alarm to check the system at the end of night and have your phone ringer and emails notifications sound on. </p>
            
                <p>If you are unable to observe tonight, kindly notify <b>me</b> as soon as possible. You can find our contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

                <p>Let me know if you have any questions. Wishing you a smooth observation shift!</p>

            <p>Best regards,<br>
            Angelina</p>
            </body>
            </html>
            """
            
        else:
            subject = f"🔭 Observation Reminder - {date} | On-call: {oncall}, Observer: {observer}"
            body = f"""
            <html>
            <body>
            <p>Dear <b>{observer}</b>,</p>

                <p>This is a friendly reminder about your scheduled observations <b>tonight</b>.</p>
            
                <p>The on-call person for tonight is <b>{oncall}</b> who shall be available in case you encounter any issues during operations.</p>
            
                <p>The observation window starts at <b>{obs_start} ET</b> and ends at <b>{obs_end} ET</b>.</p>          

                <p>Please ensure that you are prepared to begin the observations on time, set your alarm to check the system at the end of night and have your phone ringer and emails notifications sound on. </p>
            
                <p>If you are unable to observe tonight, kindly notify <b>both</b> <b>{oncall}</b> and <b>me</b> as soon as possible. You can find our contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

                <p>Let me know if you have any questions. Wishing you a smooth observation shift!</p>

            <p>Best regards,<br>
            Angelina</p>
            </body>
            </html>
            """
            
    elif observer:
        subject = f"🔭 Observation Reminder - {date} | Observer: {observer}"
        body = f"""
        <html>
        <body>
        <p>Dear <b>{observer}</b>,</p>

            <p>This is a friendly reminder about your scheduled observations <b>tonight</b>.</p>
            
            <p>The observation window starts at <b>{obs_start} ET</b> and ends at <b>{obs_end} ET</b>.</p>

            <p>Please ensure that you are prepared to begin the observations on time, set your alarm to check the system at the end of night and have your phone ringer and emails notifications sound on. </p>
            
            <p>If you are unable to observe tonight, kindly notify <b>me</b> as soon as possible. You can find my contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

            <p>Let me know if you have any questions. Wishing you a smooth observation shift!</p>

        <p>Best regards,<br>
        Angelina</p>
        </body>
        </html>
        """
    elif oncall:
        subject = f"🔭 Observation Reminder - {date} | On-call: {oncall}"
        body = f"""
        <html>
        <body>
        <p>Dear {oncall},</p>

            <p>This is a friendly reminder about your scheduled observations <b>tonight</b>.</p>
            
            <p>The observation window starts at <b>{obs_start} ET</b> and ends at <b>{obs_end} ET</b>.</p>

            <p>Please ensure that you are prepared to begin the observations on time, set your alarm to check the system at the end of night and have your phone ringer and emails notifications sound on. </p>
            
            <p>If you are unable to observe tonight, kindly notify me as soon as possible. You can find our contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

            <p>Let me know if you have any questions. Wishing you a smooth observation shift!</p>

        <p>Best regards,<br>
        Angelina</p>
        </body>
        </html>
        """
    else:
        log_message(f"No observer or on-call person found for {date}. No email sent.")
        return

    msg = MIMEMultipart()
    msg['Subject'] = subject
    msg['From'] = sender_email
    msg['To'] = group_email
    msg['Cc'] = cc_email
    msg.attach(MIMEText(body, "html"))

    # Attach the schedule image
    try:
        with open(attachment_path, "rb") as attachment:
            part = MIMEBase("application", "octet-stream")
            part.set_payload(attachment.read())
            encoders.encode_base64(part)
            part.add_header(
                "Content-Disposition",
                f"attachment; filename={attachment_name}",
            )
            msg.attach(part)
    except Exception as e:
        log_message(f"Failed to attach schedule.png: {str(e)}") 
    recipients = [group_email, cc_email] 
    try:
        with smtplib.SMTP('outbound.mail.gatech.edu', 25) as smtp_server:
            smtp_server.sendmail(sender_email, recipients, msg.as_string())
        log_message(f"Email sent successfully to {group_email} for {date}. Subject: {subject}")
    except Exception as e:
        log_message(f"Error sending email: {str(e)}")

# Function to read Excel and send emails
def send_reminder_email(file_path):
    today = datetime.now()
    month_year = today.strftime("%B %Y")

    try:
        df = pd.read_excel(file_path, sheet_name=month_year, header=None, engine="openpyxl")
    except ValueError:
        log_message(f"Sheet '{month_year}' not found in the Excel file.")
        return

    #log_message(f"Processing schedule for {month_year}")

    day_rows = [3, 11, 19, 27, 35, 43]
    observer_rows = [4, 12, 20, 28, 36, 44]
    oncall_rows = [6, 14, 22, 30, 38, 46]
    window_rows = [10, 18, 26, 34, 42, 50]  # Observation window times

    for i in range(len(day_rows)):
        row = day_rows[i]
        observer_row = observer_rows[i]
        oncall_row = oncall_rows[i]
        window_row = window_rows[i]

        for col in range(2, 9):  # Columns C to I (weekdays)
            try:
                day = df.iloc[row, col]

                if pd.notna(day) and int(day) == today.day:
                    observer = df.iloc[observer_row, col] if pd.notna(df.iloc[observer_row, col]) else None
                    oncall = df.iloc[oncall_row, col] if pd.notna(df.iloc[oncall_row, col]) else None
                    window_str = df.iloc[window_row, col] if pd.notna(df.iloc[window_row, col]) else None
                    
                    obs_start, obs_end = None, None
                    if window_str and " - " in window_str:
                        obs_start, obs_end = [s.strip() for s in window_str.split(" - ", 1)]

                    if observer or oncall:
                        observer = observer.strip() if observer else None
                        oncall = oncall.strip() if oncall else None
                        log_message(f"Found entry for {today.strftime('%Y-%m-%d')}: Observer = {observer}, On-Call = {oncall}, Window = {window_str}")
                        send_email(today.strftime('%Y-%m-%d'), observer, oncall, obs_start, obs_end)  # Now passes times
                        return
            except (ValueError, IndexError):
                continue

    log_message("No matching date found in the schedule.")

# Local file path
file_path = "/data/TrinityLabComputer/scheduling/Observation_email_reminder/Observer_Schedule.xlsx"
send_reminder_email(file_path)

