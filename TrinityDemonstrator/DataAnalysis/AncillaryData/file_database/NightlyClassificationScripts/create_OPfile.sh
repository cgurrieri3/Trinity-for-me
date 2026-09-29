#!/bin/bash
 
current_date=$(date +%Y%m%d)  # or however you set it
# current_date=20260323 
echo "nightly classification"
# cd /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/flasher_calibration
# ./run_script.sh $current_date n
 
 
# echo "event_cleaning local first"
# cd /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/event_cleaning
# ./run_script.sh $current_date n
 
 
 
 
cd /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts
exec ./run_script_cpp.sh $current_date /mnt
expect eof
 
