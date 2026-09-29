#!/bin/bash
# run_script.sh

echo "Opening container to compile code"
value=$1
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value