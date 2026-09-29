#!/usr/bin/expect -f

set timeout -1

set remote_user "sstepanoff3"
set remote_server "login-hive.pace.gatech.edu"


# Get the current date in the format yyyymmdd
set current_date [clock format [clock seconds] -format "%Y%m%d"]
set filename "${current_date}.pdf"

set remote_file "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataSummary/Output/$filename"
set local_file "/data/TrinityLabComputer/DataSummary/SummaryFiles/$filename"

spawn scp -i /home/sstepanoff3/.ssh/hive $remote_user@$remote_server:$remote_file $local_file


expect eof
