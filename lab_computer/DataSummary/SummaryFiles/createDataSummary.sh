#!/usr/bin/expect -f

set timeout -1

set remote_user "sstepanoff3"
set remote_server "login-hive.pace.gatech.edu"


# Get the current date in the format yyyymmdd
set current_date [clock format [clock seconds] -format "%Y%m%d"]

spawn ssh -i /home/$remote_user/.ssh/hive $remote_user@$remote_server "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/sumDir/FolderDataSum $current_date"

expect eof
