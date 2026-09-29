#!/bin/bash

# Usage: ./move_merged_files.sh /path/to/source /to/path
# Example: ./move_merged_files.sh /data/incoming /data/organized

src_dir="$1"
dest_root="$2"

# Check input arguments
if [ -z "$src_dir" ] || [ -z "$dest_root" ]; then
    echo "Usage: $0 <source_directory> <destination_root>"
    exit 1
fi

# Ensure source exists
if [ ! -d "$src_dir" ]; then
    echo "Error: Source directory '$src_dir' not found."
    exit 1
fi

# Loop over matching files
shopt -s nullglob
count=0
for file in "$src_dir"/Merged_CoBo0_AsAd0_*.root; do
    # Extract date in format YYYY-MM-DD
    filename=$(basename "$file")
    date_part=$(echo "$filename" | grep -oP '\d{4}-\d{2}-\d{2}')

    if [ -z "$date_part" ]; then
        echo "Skipping '$filename' — no valid date found."
        continue
    fi

    # Convert to YYYYMMDD format
    date_compact=${date_part//-/}

    # Make destination directory
    dest_dir="$dest_root/$date_compact"
    mkdir -p "$dest_dir"
    chmod 774 "$dest_dir"
   ((count++))

    # Print progress every 50 files
    if (( count % 50 == 0 )); then
        echo "Still working... ($count files moved)"
    fi 
    # Move the file
    #echo "Moving $filename → $dest_dir"
    mv -f "$file" "$dest_dir"/
done
