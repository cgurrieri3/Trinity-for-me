#!/bin/bash
# run_script.sh

echo "Opening container to compile code"
value=$1
apptainer exec --bind /data/TrinityLabComputer/TrinityDemonstrator:/mnt /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value