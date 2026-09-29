#!/bin/bash
# run_script.sh

DATE=$1
echo "Running job for $DATE"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif python3 /mnt/DataAnalysis/AncillaryData/Data2/DailyData2file.py -d "$DATE" -i 1 -ifile "/mnt/MiscData/StateMessages/cos4a-10345/"  -ofile "/mnt/DataAnalysis/AncillaryData/Data2/statemessages$DATE.csv"
