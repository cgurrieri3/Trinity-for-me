#!/usr/bin/expect -f

set timeout -1
set DATE [exec date -u +%Y%m%d]

# Update images from control PC to local PC
# Limit bandwidth to 600 KB/s for image transfer (each image has a max size of 300 KB)
#spawn rsync -Pau --bwlimit=600 -e "ssh -i /home/sstepanoff3/.ssh/cntl_cpu" root@172.16.17.74:/home/cam/OUT/$DATE* /data/TrinityLabComputer/cams/OUT/
#expect eof

#spawn rsync -Pau --bwlimit=600 -e "ssh -i /home/sstepanoff3/.ssh/cntl_cpu" root@172.16.17.74:/home/cam/IN/$DATE* /data/TrinityLabComputer/cams/IN/
#expect eof

spawn rsync -Pau --bwlimit=600 -e "ssh -i /home/sstepanoff3/.ssh/cntlcpu" root@172.16.17.74:/home/cam/Horizon/jpg/ /data/TrinityLabComputer/cams/Horizon/ 
expect eof


#--remove-source-files
