#!/usr/bin/env bash
# combine_by_date_exact.sh
# Merge ROOT files that share the same date in their filenames.

set -euo pipefail

DIR=${1:-.}
cd "$DIR"

echo "Combining ROOT files in: $PWD"

# Find all .root files that are not already combined (no trailing time)
FILES=$(find . -maxdepth 1 -type f -name "*.root" ! -regex ".*_[0-9][0-9]:[0-9][0-9]\.root")

# Also include time-based files for merging
ALL_FILES=$(find . -maxdepth 1 -type f -name "*.root")

# Extract unique dates
DATES=$(echo "$ALL_FILES" | grep -oE '20[0-9]{6}' | sort | uniq)

if [ -z "$DATES" ]; then
  echo "No date patterns found in filenames!"
  exit 1
fi

for DATE in $DATES; do
  # Gather all files with that date (ignore combined)
  FILES_FOR_DATE=$(ls *${DATE}*.root 2>/dev/null | grep -E '_[0-9]{2}:[0-9]{2}\.root$' || true)

  if [ -z "$FILES_FOR_DATE" ]; then
    echo "No time-tagged files for $DATE — skipping."
    continue
  fi

  # Derive output filename by taking first file and stripping the time part
  FIRST_FILE=$(echo "$FILES_FOR_DATE" | head -n1)
  OUTFILE=$(echo "$FIRST_FILE" | sed -E 's/_[0-9]{2}:[0-9]{2}\.root$/.root/')

  echo "Merging $(echo "$FILES_FOR_DATE" | wc -l) files for $DATE → $OUTFILE"
  hadd -f "$OUTFILE" $FILES_FOR_DATE
  chmod 664 "$OUTFILE"
  # If hadd succeeded, delete originals
  echo "Cleaning up original time-stamped files for $DATE..."
  rm -f $FILES_FOR_DATE

  echo "Completed $DATE"
  echo "--------------------------------------"
done

echo " Done merging all date-based ROOT files."
