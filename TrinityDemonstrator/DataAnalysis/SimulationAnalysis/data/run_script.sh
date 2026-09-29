#!/bin/bash
# run_script.sh

# Change only this to run on a new machine
LocalPath="/data/TrinityLabComputer"  

DATE=$1
echo "Running for $DATE"
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt,/projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator:/cedar /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /cedar/DataAnalysis/SimulationAnalysis/data/cleanupscript/mergedandpdf.sh $DATE    
