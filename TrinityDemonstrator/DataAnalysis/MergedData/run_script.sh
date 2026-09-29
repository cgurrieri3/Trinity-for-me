#!/bin/bash
# run_script.sh

# Change only this to run on a new machine
LocalPath="/home/sofia.stepanoff" 

# Variables date and files 
DATE=$1 # YYYYMMDD
FILE=$2 # *.root or n
# -------------------------------------------------
echo "Running for $DATE and File $FILE"
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/MergedData/scripts/MergeData/FileMerge $DATE /mnt/ $FILE
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/MergedData/scripts/IncludeCalibrationData/AddCalibData $DATE /mnt/ Merged_$FILE
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/MergedData/scripts/ReduceInfo/ReduceDataInfoSaved $DATE /mnt/ Merged_$FILE
chmod 774 Output/$DATE
chmod 664 Output/$DATE/*
