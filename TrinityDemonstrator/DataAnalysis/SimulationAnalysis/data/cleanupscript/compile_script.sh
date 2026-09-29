#!/bin/bash
# run_script.sh

echo "Opening container to compile code"
value=$1
apptainer exec --bind /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/mnt /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value