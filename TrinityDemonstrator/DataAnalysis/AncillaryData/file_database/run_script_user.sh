#!/bin/bash
# run_script.sh

apptainer exec --bind /data/TrinityLabComputer/TrinityDemonstrator:/mnt /data/TrinityLabComputer/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif python3 /mnt/DataAnalysis/AncillaryData/file_database/UpdateDB/userUpdateDatabase.py