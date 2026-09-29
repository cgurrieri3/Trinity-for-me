#!/bin/bash
# run_script.sh

# Change only this to run on a new machine
LocalPath="/home/sofia.stepanoff" 

echo "Running for $1"
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/event_cleaning/reducedata.sh $1
