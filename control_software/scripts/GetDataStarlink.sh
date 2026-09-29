#!/bin/bash

set -e

DWNLDTYPE=$1
DATESTMP=`date "+%b_%d"`
if [ $DWNLDTYPE -eq 0 ];
then
	STARLINKSYNC="/home/cherenkov/Documents/StarLinkSyncEngineer/*"
	DESTSYNC="~/CTData/Flight/Engineering/${DATESTMP}"
fi

if [ $DWNLDTYPE -eq 1 ];
then
        STARLINKSYNC="/home/cherenkov/Documents/StarLinkSyncRawData/*"
	DESTSYNC="~/CTData/Flight/RawData/${DATESTMP}"
fi
#DESTSYNC="/CTData/RawFlightData"
echo "Starting to send files over Starlink"

rsync -auvz --remove-source-files ${STARLINKSYNC} CTLehman:${DESTSYNC}
#for f in ${STARLINKSYNC}
#do
#	echo Transferring ${f}
#	scp ${f} CTDrop:${DESTSYNC}
#	rm ${f}
#done

#scp ${STARLINKSYNC}* CTDrop:${DESTSYNC}
#rm ${STARLINKSYNC}*
