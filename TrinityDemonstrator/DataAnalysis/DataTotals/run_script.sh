#!/bin/bash
# run_script.sh

DATE=$1
echo "Running job for $DATE"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif python3 /mnt/DataAnalysis/DataTotals/singleNightData.py -d $DATE
