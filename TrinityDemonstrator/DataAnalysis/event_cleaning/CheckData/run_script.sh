#!/bin/bash
# run_script.sh

echo "Running for $1"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/event_cleaning/CheckData/CheckData y $1 $2
