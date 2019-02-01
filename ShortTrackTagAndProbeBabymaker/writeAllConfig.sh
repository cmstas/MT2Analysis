#!/bin/bash

RUNDIR=$1

for DATADIR in ${RUNDIR}/*Single*; do
    if [[ ${DATADIR} == *Photon* ]]; then
	continue
    fi
    command="./writeConfig.sh ${DATADIR}"
    echo $command
    eval $command
done

for DATADIR in ${RUNDIR}/*EGamma*; do
    command="./writeConfig.sh ${DATADIR}"
    echo $command
    eval $command
done