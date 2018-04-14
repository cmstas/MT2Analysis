#!/bin/bash

SAMPLE=$1
INPUT_FILE_FULLPATH=$2
OUTPUT_FILE=${SAMPLE}.root
COPYDIR=$3

echo "[wrapper] INPUT_SAMPLE    = " ${SAMPLE}
echo "[wrapper] INPUT_FILE_FULLPATH      = " ${INPUT_FILE_FULLPATH}
echo "[wrapper] COPYDIR   = " ${COPYDIR}

CMSSW_VERSION=CMSSW_9_4_1

echo "[wrapper] setting env"
export SCRAM_ARCH=slc6_amd64_gcc630
source /cvmfs/cms.cern.ch/cmsset_default.sh
OLDDIR=`pwd`
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
# portable equivalent to cmsenv
eval `scramv1 runtime -sh`
cd $OLDDIR

echo

echo "[wrapper] printing env"
printenv
echo 

echo "[wrapper] hostname  = " `hostname`
echo "[wrapper] date      = " `date`
echo "[wrapper] linux timestamp = " `date +%s`
echo "[wrapper] checking input file with ls"

# Is file accessible?
ls -alrth ${INPUT_FILE_FULLPATH}
# If not...
if [ $? -ne 0 ]; then
    echo "[wrapper] could not find input file, trying xrootd instead"
    FILESHORT=${INPUT_FILE_FULLPATH#/hadoop/cms}
    xrdfs xrootd.t2.ucsd.edu ls ${FILESHORT}
    if [ $? -ne 0 ]; then
        echo "[wrapper] could not find input file with xrootd either, exiting"
        exit 1
    else
        echo "[wrapper] found file with xrootd, will proceed"
        INPUT_FILE_FULLPATH="root://xrootd.t2.ucsd.edu/"${FILESHORT}
    fi
fi

echo "[wrapper] current directory is:"
pwd

echo "[wrapper] extracting input sandbox"
tar -zxf input.tar.gz

cd job_input
echo "[wrapper] input contents are"
ls -a

echo "[wrapper] directory contents are"
ls

COMMAND="./STCandLooper.exe ${INPUT_FILE_FULLPATH} ${SAMPLE} 1"

echo "[wrapper] running: ${COMMAND}"

eval ${COMMAND}

echo "[wrapper] output is"
ls

echo "[wrapper] copying file"
OUTPUT=`ls output_LepVeto`
echo "[wrapper] OUTPUT = " ${OUTPUT}

if [ ! -d "${COPYDIR}" ]; then
    echo "creating output directory ${COPYDIR}"
    mkdir ${COPYDIR}
fi

gfal-copy -p -f -t 4200 --verbose file://`pwd`/output_LepVeto/${OUTPUT} gsiftp://gftp.t2.ucsd.edu${COPYDIR}/${OUTPUT}

echo "[wrapper] cleaning up"
for FILE in `find . -not -name "*stderr" -not -name "*stdout"`; do rm -rf $FILE; done
echo "[wrapper] cleaned up"
pwd
ls

