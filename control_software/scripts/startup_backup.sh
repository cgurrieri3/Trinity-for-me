#!/bin/bash

#source ${UPROFILE}
echo "Starting Up CT CPU"

#ping 192.168.2.1;

##if [ $0 -ne 0 ]; then
#	sleep 30
#fi
#systemd-notify --ready --status="Waiting for commands"

#sudo rmmod i2c-i801.ko
#sudo insmod /home/cherenkov/Documents/TempDriver/i2c-i801.ko
U_HOME=/home/trinity
SFWR=$U_HOME/Programs
CONTROLSFWR=$SFWR/Trinity/control_software
sudo rm /dev/mqueue/*

#sudo mkdir -p /tmp/src/20210208/
#sudo cp -r /home/cherenkov/Programs/control_software/CoBo/CoBoFrameViewer/ /tmp/src/20210208/

#cd /home/cherenkov/Programs/control_software/fcutils/test/build
#sudo ./control_software -n true >> /home/cherenkov/Programs/control_software/fcutils/test/LOGS/cs.log 2>&1 &
#CS_PID=`echo $!`

#echo "STARTUP SERVICE: Control Started"

#systemd-notify --ready --status="Waiting for commands"

sleep 5

sudo $CONTROLSFWR/fcutils/test/build/master_control >> ${CONTROLSFWR}/fcutils/test/LOGS/rc.log 2>&1 &
RC_PID=`echo $!`

echo "STARTUP SERVICE: Master Started"
sleep 10

$SFWR/eventbuilder/DataProcess.sh >> ${CONTROLSFWR}/fcutils/test/LOGS/dp.log 2>&1 &
DP_PID=`echo $!`

echo "STARTUP SERVICE: File Searching Started"
sleep 5

#sudo iptables-restore < ~/rules.v4
#echo "STARTUP SERVICE: UDP Packets Blocked"

systemd-notify --ready

while :
do
	if ! sudo kill -0 ${RC_PID};
	then
		systemd-notify --status="Master Control Crashed! Restarting..."
		exit 128
		#sudo service startupCT stop
	else
		systemd-notify --status="Listening on ${RC_PID}"
		sleep 60
	fi
done
