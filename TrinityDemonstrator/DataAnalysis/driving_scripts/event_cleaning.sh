#!/bin/bash



DATE=$1

# create statemessage file for a given date
bash /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/AncillaryData/Data2/run_script.sh "$DATE" 1

# merge files
cd /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/
bash /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/submit.sh "$DATE"
# flasher calibration

cd /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/flasher_calibration/
bash /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/flasher_calibration/run_script.sh "$DATE" 

# event cleaning
cd /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/event_cleaning
bash /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/event_cleaning/submit_night.sh "$DATE"
