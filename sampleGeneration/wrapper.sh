#!/bin/bash

#
# args
#

FILEID=$1
FILE=$2
COPYDIR=$3
FRAGMENT=$4
EVENTS=$5
JOBNUMBER=$6
TOTAL=$7

echo "[wrapper] FILEID    = " ${FILEID}
echo "[wrapper] FILE      = " ${FILE}
echo "[wrapper] COPYDIR   = " ${COPYDIR}
echo "[wrapper] FRAGMENT  = " ${FRAGMENT}
echo "[wrapper] EVENTS    = " ${EVENTS}
echo "[wrapper] JOBNUMBER = " ${JOBNUMBER}
echo "[wrapper] TOTAL     = " ${TOTAL}

echo "[wrapper] printing env"
printenv

#
# run the steps
#

echo "[wrapper] running: ./step_pLHE.sh ${FILEID} ${FILE} ${TOTAL}"
./step_pLHE.sh ${FILEID} ${FILE} ${TOTAL}

echo "[wrapper] running: ./step_AOD.sh ${FILEID} ${FRAGMENT} ${EVENTS} ${JOBNUMBER}"
./step_AOD.sh ${FILEID} ${FRAGMENT} ${EVENTS} ${JOBNUMBER}

echo "[wrapper] running: ./step_MiniAOD.sh ${FILEID} ${EVENTS} ${JOBNUMBER}"
./step_MiniAOD.sh ${FILEID} ${EVENTS} ${JOBNUMBER}

#
# do something with output
#

echo "[wrapper] output is"
ls

#
# clean up
#

# echo "[wrapper] dumping config file"
# cat cfg_step2_AOD.py

echo "[wrapper] copying MiniAOD file"
OUTPUT=`ls | grep ${FILEID}_MiniAOD-v2`
echo "[wrapper] OUTPUT = " ${OUTPUT}

if [ ! -d "${COPYDIR}" ]; then
    echo "creating output directory " ${COPYDIR}
    mkdir ${COPYDIR}
fi

lcg-cp -b -D srmv2 --vo cms -t 2400 --verbose file:`pwd`/${OUTPUT} srm://bsrm-3.t2.ucsd.edu:8443/srm/v2/server?SFN=${COPYDIR}/${OUTPUT}

echo "[wrapper] cleaning up"
for FILE in `find . -not -name "*stderr" -not -name "*stdout"`; do rm -rf $FILE; done
echo "[wrapper] cleaned up"
ls


