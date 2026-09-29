#!/bin/bash
# submit_by_date_or_file.sh
# Submits Condor jobs for one or more dates.
# Accepts either a single YYYYMMDD or a file containing multiple dates.

INPUT=$1

if [ -z "$INPUT" ]; then
  echo "Usage: $0 <YYYYMMDD | date_file>"
  exit 1
fi

SUBMIT_TEMPLATE="condense_SM.submit"

BASE_DIR="/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output"
# BASE_DIR="/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/lsheram6_MergedFiles"


# Determine if input is a file or a single date
if [ -f "$INPUT" ]; then
  echo "Reading dates from file: $INPUT"
  DATES=$(grep -v '^#' "$INPUT" | grep -E '^[0-9]{8}$')
else
  # Validate date format
  if [[ ! "$INPUT" =~ ^[0-9]{8}$ ]]; then
    echo "Error: '$INPUT' is not a valid YYYYMMDD or a file."
    exit 1
  fi
  DATES="$INPUT"
fi

##
LOG_DIR="/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/DeadTime/.log"
USER_LOG_DIR="${LOG_DIR}/${USER}"
echo "ensuring user log directory exists: $USER_LOG_DIR"
mkdir -p "$USER_LOG_DIR"
chmod 775 "$USER_LOG_DIR"
##

echo "Using submit template: $SUBMIT_TEMPLATE"
echo "---------------------------------------------"

for DATE in $DATES; do
  TARGET_DIR="${BASE_DIR}/${DATE}/"

  if [ ! -d "$TARGET_DIR" ]; then
    echo "Directory not found: $TARGET_DIR — skipping."
    continue
  fi

  echo "Processing date: $DATE"

  #echo "Submitting job for: $BASENAME"
  # condor_submit Date="$DATE"


  ## from event cleaning submit_night.sh
  ## chmod doesnt work here
  chmod 774 /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/DeadTime/Output/
  condor_submit Date="$DATE" "$SUBMIT_TEMPLATE"

  #echo "Submitted all jobs for date $DATE"
  #echo "---------------------------------------------"
done

echo "All submissions complete."

# mkdir -p /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/$DATE
# echo "Watcher started..."
# apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt \
#   /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif \
#   python3 /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/watcher.py \
#   -i /mnt/DataAnalysis/MergedData/ \
#   -o /mnt/DataAnalysis/MergedData/Output/ \
#   -l /mnt/DataAnalysis/MergedData/.logs/watcher.log \
#   -t 120
# echo "Watcher finished."