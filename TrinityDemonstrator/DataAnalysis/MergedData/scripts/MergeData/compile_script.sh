#!/bin/bash
# run_script.sh

echo "Opening container to compile code"
value=$1
#apptainer exec --containall --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif bash -c "cd /mnt/DataAnalysis/MergedData/scripts/MergeData && make $value"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value