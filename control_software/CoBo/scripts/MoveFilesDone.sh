#!/bin/bash

source /home/trinity/.bashrc
FILEDIR="/home/trinity/Programs/Trinity/control_software/CoBo/format"
TBDIR="/home/trinity/Programs/Trinity/control_software/tb/mesaflash"
TIMESTAMP=$(date +'%Y%m%d')
SAVEDATA="/home/trinity/Documents/Data"
DESTDIR="${SAVEDATA}/${TIMESTAMP}/RawDataUnmerged"

ASAD_TIMEOUT=15
ASAD_INTERVAL=5
ASAD_ELAPSED=0
until sudo mv ${FILEDIR}/*AsAd0* ${DESTDIR} 2>/dev/null; do
    if [ ${ASAD_ELAPSED} -ge ${ASAD_TIMEOUT} ]; then
        echo "ERROR: AsAD move failed after ${ASAD_TIMEOUT} seconds" >&2
        exit 1
    fi
    echo "AsAD move failed, retrying in ${ASAD_INTERVAL}s..."
    sleep ${ASAD_INTERVAL}
    ASAD_ELAPSED=$((ASAD_ELAPSED + ASAD_INTERVAL))
done

sudo mv ${TBDIR}/TB_data* ${DESTDIR}
