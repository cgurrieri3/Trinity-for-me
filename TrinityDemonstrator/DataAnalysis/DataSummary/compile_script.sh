#!/bin/bash
# run_script.sh

LocalPath="/data/TrinityLabComputer" 

echo "Opening container to compile code"
value=$1
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value