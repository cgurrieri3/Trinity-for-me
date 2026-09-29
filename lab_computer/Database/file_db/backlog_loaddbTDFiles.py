import numpy as np
import math
import pandas as pd
from pprint import pprint
import ephem
import subprocess
import argparse
import os

# Initialize parser
parser = argparse.ArgumentParser()

# Adding optional argument
parser.add_argument("-d", "--date", help = "YYYYMMDD")
parser.add_argument("-n", "--num", help = "number of days")

# Read arguments from command line
args = parser.parse_args()

if args.date:
    print("Displaying Output as: % s" % args.date)

# In[5]:


# STARTING DATE FOR FILE CREATIONS AND FOLLOWING DAYS AFTER 
start_date = args.date
num_days = int(args.num)

# CREATES DATE RANGE
dates = pd.date_range(start_date, periods = num_days).date

for d in dates:
    date=d.strftime('%Y%m%d')
    print("\n Starting:", date)
    # os.system(f'python load_dbTD.py -d {date}')
    subprocess.run(["python3", "load_dbTDFiles.py", "-d", date], check=True)