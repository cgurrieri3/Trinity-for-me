#!/bin/bash
# run_script.sh

DATE=$1
FILENAME=$2
echo "Running for $DATE"
apptainer exec --bind /home/sstepanoff/TrinityDemonstrator:/mnt /home/sstepanoff/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/MergedData/IncludeCalibrationData/AddCalibData  $DATE /mnt/ $FILENAME  
