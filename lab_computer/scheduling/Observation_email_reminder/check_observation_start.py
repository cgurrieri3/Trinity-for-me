import os
import smtplib
import pandas as pd
import pytz
from datetime import datetime, timedelta
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import subprocess

# Config
EXCEL_FILE = "/data/TrinityLabComputer/scheduling/Observation_email_reminder/Observer_Schedule.xlsx"
LOG_FILE = "/data/TrinityLabComputer/scheduling/Observation_email_reminder/check_log.txt"
GROUP_EMAIL = "TrinityObservations@groups.gatech.edu"  
SENDER_EMAIL = "sofiastepanoff@gatech.edu"
CC_EMAIL = "azhang421@gatech.edu"
SSH_TARGET = "trinity@172.16.17.82"
REMOTE_PATH_TEMPLATE = "/home/trinity/Documents/Data/{}/RawDataMerged"

def parse_time_string(time_str):
    time_str = time_str.strip().upper()

    # Fix incorrect formats like "13:15 PM"
    if "PM" in time_str or "AM" in time_str:
        parts = time_str.replace("AM", "").replace("PM", "").strip().split(":")
        try:
            hour = int(parts[0])
        except ValueError:
            raise ValueError(f"Invalid hour in time string: {time_str}")

        minute = int(parts[1]) if len(parts) > 1 else 0

        ## Fix for 0-hour, which is not valid in 12-hour format
        if hour == 0:
            hour = 12
        elif hour > 12:
            hour -= 12  # Fix 13:00 PM → 1:00 PM, for example
        time_str = f"{hour}:{minute:02d} {'PM' if 'PM' in time_str else 'AM'}"
        fmt = "%I:%M %p"
    else:
        fmt = "%H:%M"

    try:
        return datetime.strptime(time_str, fmt)
    except ValueError:
        raise ValueError(f"Unrecognized time format after correction: {time_str}")

def log(msg):
    with open(LOG_FILE, "a") as f:
        f.write(f"[{datetime.now()}] {msg}\n")

def notification_already_sent(date_str, start_time_str):
    try:
        with open(LOG_FILE, "r") as f:
            lines = f.readlines()

        
        now = datetime.now()

        for line in reversed(lines):  # Check recent logs first
            timestamp_str = line.split("]")[0].strip("[")
            try:
                log_time = datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S.%f")
            except ValueError:
                continue
            # Check if log was written within last 20 minutes
            if (now - log_time).total_seconds() <= 20 * 60:
                return True
    except Exception as e:
        log(f"Error checking log file for duplicates: {e}")
    return False


def send_notification(observer, oncall, date, start_time, ctcpu_folder):
    subject = f"⚠️ Observations May Not Have Started - {date}"
    body = ""

    if observer and oncall:
    	body = f"""
    	<html>
    	<body>
    	<p>Dear <b>{observer}</b> and <b>{oncall}</b>,</p>

    	<p>This is an automated alert that it is less than 20 minutes before the scheduled observation window start, <b>{start_time} ET tonight</b>, <br>
    	but no merged data (from internal triggers and noise DAQ) has appeared in the expected folder on the CT CPU:<br> <code>{ctcpu_folder}.</code></p>

    	<p>Please verify the system status and start observations asap. Otherwise, please make sure to log the reason the operations were not started as expected.</p>
    	
    	 <p> In case you have any issues, you can find our contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

    	<p>Best regards,<br>
    	Angelina</p>
    	</body>
    	</html>
    	"""

    elif observer:
    	body = f"""
    	<html>
    	<body>
    	<p>Dear <b>{observer}</b>,</p>

    	<p>This is an automated alert that it is less than 20 minutes before the scheduled observation window start, <b>{start_time} ET tonight</b>, <br>
    	but no merged data (from internal triggers and noise DAQ) has appeared in the expected folder on the CT CPU:<br> <code>{ctcpu_folder}.</code></p>

    	<p>Please verify the system status and start observations asap. Otherwise, please make sure to log the reason the operations were not started as expected.</p>
    	
    	<p> In case you have any issues, you can find my contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

    	<p>Best regards,<br>
    	Angelina</p>
    	</body>
    	</html>
    	"""
    
    elif oncall:
    	body = f"""
    	<html>
    	<body>
    	<p>Dear <b>{oncall}</b>,</p>

    	<p>This is an automated alert that it is less than 20 minutes before the scheduled observation window start, <b>{start_time} ET tonight</b>, <br>
    	but no merged data (from internal triggers and noise DAQ) has appeared in the expected folder on the CT CPU:<br> <code>{ctcpu_folder}.</code></p>

    	<p>Please verify the system status and start observations asap. Otherwise, please make sure to log the reason the operations were not started as expected.</p>
    	
    	<p> In case you have any issues, you can find my contact information <a href="https://trinity.physics.gatech.edu/internal/index.php?title=Manual#EM">here (see "Emergency Contacts")</a>. </p>

    	<p>Best regards,<br>
    	Angelina</p>
    	</body>
    	</html>
    	"""
    
    msg = MIMEMultipart()
    msg['Subject'] = subject
    msg['From'] = SENDER_EMAIL
    msg['To'] = GROUP_EMAIL
    msg['Cc'] = CC_EMAIL
    msg.attach(MIMEText(body, "html"))
    recipients = [GROUP_EMAIL, CC_EMAIL]

    try:
        with smtplib.SMTP('outbound.mail.gatech.edu', 25) as smtp_server:
            smtp_server.sendmail(SENDER_EMAIL, recipients, msg.as_string())
        log(f"Notification sent: {subject}. Observation window starts at {start_time} ET.")
    except Exception as e:
        log(f"Failed to send email: {str(e)}")

def get_schedule_info():

    now_et = datetime.now(pytz.timezone("US/Eastern"))
    
    # Determine observation date in ET (use previous day if it's early AM)
    if now_et.hour < 12:
        observation_date_et = now_et - timedelta(days=1)
    else:
        observation_date_et = now_et
        
    month_year = observation_date_et.strftime("%B %Y")

    try:
        df = pd.read_excel(EXCEL_FILE, sheet_name=month_year, header=None, engine="openpyxl")
    except Exception as e:
        log(f"Failed to read Excel: {e}")
        return None

    day_rows = [3, 11, 19, 27, 35, 43]
    observer_rows = [4, 12, 20, 28, 36, 44]
    oncall_rows = [6, 14, 22, 30, 38, 46]
    window_rows = [10, 18, 26, 34, 42, 50]

    for i in range(len(day_rows)):
        row, obs_row, oncall_row, win_row = day_rows[i], observer_rows[i], oncall_rows[i], window_rows[i]

        for col in range(2, 9):  # C to I
            try:
                cell_day = df.iloc[row, col]
                if pd.notna(cell_day) and int(cell_day) == observation_date_et.day:
                    observer = str(df.iloc[obs_row, col]).strip() if pd.notna(df.iloc[obs_row, col]) else None
                    oncall = str(df.iloc[oncall_row, col]).strip() if pd.notna(df.iloc[oncall_row, col]) else None
                    window = str(df.iloc[win_row, col]).strip() if pd.notna(df.iloc[win_row, col]) else None

                    if observer or oncall:
                        return {
                            "date_et": observation_date_et.strftime("%Y-%m-%d"),
                            "observer": observer,
                            "oncall": oncall,
                            "start_time_et": window.split(" - ")[0] if window and " - " in window else None
                        }
            except:
                continue
    return None

def check_and_notify():
    #log(f"Check started")
    info = get_schedule_info()
    if not info or not info["start_time_et"]:
        #log(f"No scheduled observation found for current ET time window.")
        return

    now_et = datetime.now(pytz.timezone("US/Eastern"))
    eastern = pytz.timezone("US/Eastern")

    # Parse time string
    start_dt_naive = parse_time_string(info["start_time_et"])

    # Shift date to next day if time is between 00:00 AM and 12:00 PM
    if start_dt_naive.hour < 12:
        obs_date = datetime.now(pytz.timezone("US/Eastern")) + timedelta(days=1)
    else:
        obs_date = datetime.now(pytz.timezone("US/Eastern"))

    # Adjust the naive time with correct date
    start_dt_naive = start_dt_naive.replace(
        year=obs_date.year, month=obs_date.month, day=obs_date.day
    )

    start_dt = eastern.localize(start_dt_naive)

    # === NEW: use adjusted time string as notification key ===
    notification_key_date = start_dt.strftime("%Y-%m-%d")
    notification_key_time = start_dt.strftime("%H:%M")

    if notification_already_sent(notification_key_date, notification_key_time):
        return

    # Check time window
    delta = (start_dt - now_et).total_seconds() / 60
    if 0 <= delta <= 10:
        # Correct UTC folder date (always based on UTC start time, and often day +1)
        folder_utc = start_dt.astimezone(pytz.utc).strftime("%Y%m%d")
        #folder_utc = (start_dt + timedelta(days=1)).astimezone(pytz.utc).strftime("%Y%m%d")
        remote_path = REMOTE_PATH_TEMPLATE.format(folder_utc)
        ssh_cmd = f'ssh -i /home/sstepanoff3/.ssh/ctcpu {SSH_TARGET} "ls -A {remote_path} 2>/dev/null | wc -l"'

        try:
            result = subprocess.check_output(ssh_cmd, shell=True).decode().strip()
            if result == "0":
                send_notification(info["observer"], info["oncall"], start_dt.strftime("%Y-%m-%d"),
                                  info["start_time_et"], remote_path)
            else:
                log(f"Folder {remote_path} is not empty. Observational operations likely started.")
        except subprocess.CalledProcessError as e:
            log(f"SSH error or folder does not exist: {e}")
    #else:
        #log(f"Not within 20 min of observation start at {start_dt}. Δ = {delta:.1f} min")

if __name__ == "__main__":
    check_and_notify()

