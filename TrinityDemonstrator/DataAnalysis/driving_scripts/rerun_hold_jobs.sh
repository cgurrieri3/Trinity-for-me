#!/bin/bash

# This script finds all held HTCondor jobs for the current user
# that failed due to insufficient disk space, increases their RequestDisk,
# and releases them to run again.

# ---------------------------
# User configurable settings:
# ---------------------------
NEW_DISK="20000000"   # in KB (≈20 GB)
USER_NAME=$(whoami)

echo ">>> Checking held jobs for user: $USER_NAME"
echo ">>> Updating RequestDisk to ${NEW_DISK} KB (~20 GB) and releasing..."

# Get held job IDs for this user
held_jobs=$(condor_q -hold -af ClusterId ProcId Owner | awk -v user="$USER_NAME" '$3 == user {print $1"."$2}')

if [ -z "$held_jobs" ]; then
    echo "No held jobs found for $USER_NAME."
    exit 0
fi

count=0

for jobid in $held_jobs; do
    # Get hold reason
    hold_reason=$(condor_q -long "$jobid" | grep -m1 "HoldReason" | cut -d '"' -f2)

    # Check if it's a disk-related error
    if echo "$hold_reason" | grep -Eiq "disk|errno 28|space"; then
        echo "Updating job $jobid (reason: $hold_reason)"
        condor_qedit "$jobid" RequestDisk "$NEW_DISK"
        condor_release "$jobid"
        ((count++))
    elif echo "$hold_reason" | grep -Eiq "No such file or directory|errno 2"; then
        echo "Updating job $jobid (reason: $hold_reason)"
        # condor_qedit "$jobid" RequestDisk "$NEW_DISK"
        condor_release "$jobid"
        ((count++))
    else
        echo "Skipping job $jobid (not disk-related: $hold_reason)"
    fi
done

echo ">>> Done. Released $count job(s) with updated disk request."
