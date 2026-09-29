#!/bin/bash
# run_script.sh

DATE=$1
SIMRUN=$2
SIMTYPE=${3:-GROPT}   # GROPT (default) or CARE
echo "Running for $DATE ($SIMTYPE)"
apptainer exec --bind /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/mnt /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/SimulationAnalysis/scripts/SimRootFiles $DATE $SIMRUN y $SIMTYPE
