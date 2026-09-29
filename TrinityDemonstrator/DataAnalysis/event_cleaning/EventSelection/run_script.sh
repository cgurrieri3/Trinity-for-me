#!/bin/bash
# run_script.sh

FOLDER=$1
FILE_LIST=$2 # put n for no list
echo "Running for $FOLDER"
apptainer exec --bind /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/mnt /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/event_cleaning/EventSelection/EventSelection y $FOLDER $FILE_LIST
