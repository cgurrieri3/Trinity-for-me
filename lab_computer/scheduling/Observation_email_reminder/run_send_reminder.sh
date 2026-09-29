#!/bin/bash


TIMESTAMP=$(date "+[%Y-%m-%d %H:%M:%S]")

echo "$TIMESTAMP Running email reminder script"

python3 /data/TrinityLabComputer/scheduling/Observation_email_reminder/send_reminder.py

