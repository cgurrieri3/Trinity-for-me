#!/bin/bash

# Output CSV file
output="folders.csv"
echo "Date,ID_ers" > "$output"

# Loop through all directories in the specified folder
for dir in /projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/simdata/daviddata/*/; do
    dirname=$(basename "$dir")
    
    # Match pattern YYYYMMDD_#########_ers
    if [[ "$dirname" =~ ^([0-9]{8})_([0-9]+_daro)$ ]]; then
        date="${BASH_REMATCH[1]}"
        id_ers="${BASH_REMATCH[2]}"
        echo "$date,$id_ers" >> "$output"
    fi
done

echo "Saved to $output"
