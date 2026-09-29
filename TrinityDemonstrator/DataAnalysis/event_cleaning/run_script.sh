#!/bin/bash
# run_script.sh

# Change only this to run on a new machine
LocalPath="/home/sofia.stepanoff" 

DATE=$1
FILENAME=$2
echo "Running for $DATE"
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/event_cleaning/ClusterCleaning/EventCleaning $DATE y $FILENAME    
