#!/bin/bash
# run_script.sh

echo "Opening container to compile code"
value=$1
apptainer exec --bind /home/sstepanoff/TrinityDemonstrator:/mnt /home/sstepanoff/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value