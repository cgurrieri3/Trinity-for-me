#!/bin/bash

echo "Starting Up CT CPU"

U_HOME=/home/trinity
SFWR=$U_HOME/Programs
CONTROLSFWR=$SFWR/Trinity/control_software
sudo rm /dev/mqueue/*

list_ftdi_devices() {
  for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
      (
          syspath="${sysdevpath%/dev}"
          devname="$(udevadm info -q name -p $syspath)"
          [[ "$devname" == "bus/"* ]] && exit
          eval "$(udevadm info -q property --export -p $syspath)"
          [[ -z "$ID_SERIAL" ]] && exit
          ftdi_device=$(echo "/dev/$devname - $ID_SERIAL" | grep FTDI | grep -oE '/dev/ttyUSB[0-9]+')
          [[ -n "$ftdi_device" ]] && echo "$ftdi_device"
      )
  done
}

ftdi=$(list_ftdi_devices)

sudo $CONTROLSFWR/fcutils/test/build/master_control $ftdi >> ${CONTROLSFWR}/fcutils/test/LOGS/rc.log 2>&1 &
RC_PID=`echo $!`
echo "STARTUP SERVICE: Master Started"
sleep 10

$SFWR/eventbuilder/DataProcess.sh >> ${CONTROLSFWR}/fcutils/test/LOGS/dp.log 2>&1 &
DP_PID=`echo $!`
echo "STARTUP SERVICE: File Searching Started"
sleep 5

systemd-notify --ready

while :
do
	if ! sudo kill -0 ${RC_PID};
	then
		systemd-notify --status="Master Control Crashed! Restarting..."
		exit 128
	else
		systemd-notify --status="Listening on ${RC_PID}"
		sleep 60
	fi
done
