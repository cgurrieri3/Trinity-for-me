#!/bin/bash
# submit_by_date_or_file.sh
# Submits Condor jobs for one or more dates.
# Accepts either a single YYYYMMDD or a file containing multiple dates.


# Change only this to run on a new machine
LocalPath="/home/projects/GATech_Otte" 
OSDF_path="/ospool/uw-shared/projects/GATech_Otte"

INPUT=$1
# add user arg
# theUser=$2

if [ -z "$INPUT" ]; then
  echo "Usage: $0 <YYYYMMDD | date_file>"
  exit 1
fi

SUBMIT_TEMPLATE="condense_SM.submit"
# ##
# chmod log 775
# chmod error 775
# chmod output 775
# ##
BASE_DIR="$LocalPath/TrinityDemonstrator/DataAnalysis/MergedData/Output"
OSDF_BASE_DIR="$OSDF_path/TrinityDemonstrator/DataAnalysis/MergedData/Output"
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
LOG_DIR="$LocalPath/TrinityDemonstrator/DataAnalysis/flasher_calibration/.log"
USER_LOG_DIR="${LOG_DIR}/${USER}"
echo "ensuring user log directory exists: $USER_LOG_DIR"
mkdir -p "$USER_LOG_DIR"
chmod 775 "$USER_LOG_DIR"
##

echo "Using submit template: $SUBMIT_TEMPLATE"
echo "---------------------------------------------"

for DATE in $DATES; do
  TARGET_DIR="${OSDF_BASE_DIR}/${DATE}/"

  if [ ! -d "$TARGET_DIR" ]; then
    echo "Directory not found: $TARGET_DIR — skipping."
    continue
  fi

  echo "Processing date: $DATE"

  #echo "Submitting job for: $BASENAME"
  # condor_submit Date="$DATE"


  ## from event cleaning submit_night.sh
  chmod 774 $LocalPath/TrinityDemonstrator/DataAnalysis/flasher_calibration/Output/
  #condor_submit Date="$DATE" "$SUBMIT_TEMPLATE"

  #echo "Submitted all jobs for date $DATE"
  #echo "---------------------------------------------"
done

echo "All submissions complete."
