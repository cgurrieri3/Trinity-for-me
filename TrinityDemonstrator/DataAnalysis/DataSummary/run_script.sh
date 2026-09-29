#!/bin/bash
# run_script.sh
LocalPath="/data/TrinityLabComputer" 

DATE=$1
echo "Running for $DATE"
# normal
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/DataSummary/FolderDataSum $DATE y


# # if using data from cedar
# apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt,/projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/cedar $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/DataSummary/FolderDataSum $DATE y x

