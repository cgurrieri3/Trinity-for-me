#!/bin/bash
# run_script.sh

# need to include second argument for path

DATE=$1
P=$2

if [ -z "$DATE" ] || [ -z "$P" ]; then
  echo "Usage: $0 <DATE> <P>"
  exit 1
fi

echo "Running for \"$DATE\""
echo "Using: \"$P\""

apptainer exec --bind /data/TrinityLabComputer/TrinityDemonstrator:/mnt /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/ClassifyData $DATE $P
