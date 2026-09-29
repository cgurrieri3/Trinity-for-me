#!/bin/bash

#DATEVAR=$(/bin/date +%Y%m%d)
DATEVAR=$(date -d "yesterday" +%Y%m%d) # changed on 6/7/2024 due to an api problem 

/usr/bin/python3 /data/TrinityLabComputer/cams/upload_vid.py --file="/data/TrinityLabComputer/cams/VIDS/OUT_$DATEVAR.mp4" --title="OUT $DATEVAR" --privacyStatus="public" --playlist OUTDOOR
sleep 60
/usr/bin/python3 /data/TrinityLabComputer/cams/upload_vid.py --file="/data/TrinityLabComputer/cams/VIDS/IN_$DATEVAR.mp4" --title="IN $DATEVAR" --privacyStatus="public" --playlist INDOOR
sleep 60
/usr/bin/python3 /data/TrinityLabComputer/cams/upload_vid.py --file="/data/TrinityLabComputer/cams/VIDS/Horizon_$DATEVAR.mp4" --title="HORIZON $DATEVAR" --privacyStatus="public" --playlist HORIZON

file_count=$(ls -1A /data/TrinityLabComputer/cams/VIDS/ | wc -l)

# Check if there are 16 files
if [[ "$file_count" -eq 16 ]]; then
	# Sort the files by modification time in ascending order
    	files=($(ls -t1 /data/TrinityLabComputer/cams/VIDS/))
