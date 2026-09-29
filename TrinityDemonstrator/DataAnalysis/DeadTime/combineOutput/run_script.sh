#!/bin/bash
# run_script.sh

DATE=$1
FILE=$2
echo "Running for $DATE"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/DeadTime/combineOutput/Combine temp temp temp

# sleep 10
