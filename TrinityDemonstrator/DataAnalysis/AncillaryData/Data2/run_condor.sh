#!/bin/bash
# run_condor.sh

DATE=$1
echo "Running job for $DATE"
echo "Directories"
ls -lh
pwd
python3 /srv/DailyData2file.py -d "$DATE" -i 1 -ifile "/srv/" -ofile "/srv/statemessages$DATE.csv"
echo "ran python script"
ls -lh

echo "Created output file:"
ls -lh statemessages*.csv


FILE="statemessages${DATE}.csv"
FULL_PATH=$(realpath "$FILE")
echo "Full path to output file: $FULL_PATH"
