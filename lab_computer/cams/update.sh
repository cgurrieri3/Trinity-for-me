#!/usr/bin/expect -f

set timeout -1
set DATE [exec date -u +%Y%m%d]

# Update images from control PC to local PC
# Limit bandwidth to 600 KB/s for image transfer (each image has a max size of 300 KB)
spawn rsync -Pau --bwlimit=600 root@172.16.17.74:/home/cam/OUT/$DATE* /home/mpotts32/cams/OUT/
expect "root@172.16.17.74's password: "
send "trinity22!\r"
expect eof

spawn rsync -Pau --bwlimit=600 root@172.16.17.74:/home/cam/IN/$DATE* /home/mpotts32/cams/IN/
expect "root@172.16.17.74's password: "
send "trinity22!\r"
expect eof

spawn rsync --remove-source-files -Pau --bwlimit=600 root@172.16.17.74:/home/cam/Horizon/ /home/mpotts32/cams/Horizon/
expect "root@172.16.17.74's password: "
send "trinity22!\r"
expect eof
