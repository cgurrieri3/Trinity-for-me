#!/bin/bash
# run_script.sh

DATE=$1
SIMRUN=$2
echo "Running for $DATE"
apptainer exec --bind /data/TrinityLabComputer/TrinityDemonstrator:/mnt,/projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/cedar /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /cedar/DataAnalysis/event_cleaning/ClusterCleaning/EventCleaning $DATE /cedar/ $SIMRUN   
