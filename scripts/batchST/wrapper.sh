#!/bin/bash

#
# args
#

TAG=$1
SAMPLE=$2
CTAU=$3
COPYDIR=$4

echo "[wrapper] SAMPLE      = " ${SAMPLE}
echo "[wrapper] CTAU        = " ${CTAU}
echo "[wrapper] COPYDIR     = " ${COPYDIR}

#
# set up environment
#
CMSSW_VERSION=CMSSW_10_2_5

###version using cvmfs install of CMSSW
echo "[wrapper] setting env"
export SCRAM_ARCH=slc6_amd64_gcc700
source /cvmfs/cms.cern.ch/cmsset_default.sh
OLDDIR=`pwd`
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
#cmsenv
eval `scramv1 runtime -sh`
cd $OLDDIR

# ### version using nfs install of CMSSW
# echo "[wrapper] setting env"
# export SCRAM_ARCH=slc6_amd64_gcc493
# source /nfs-7/cmssoft/cms.cern.ch/cmssw/cmsset_default.sh
# OLDDIR=`pwd`
# cd /nfs-7/cmssoft/cms.cern.ch/cmssw/slc6_amd64_gcc493/cms/cmssw-patch/$CMSSW_VERSION/src
# #cmsenv
# eval `scramv1 runtime -sh`
# cd $OLDDIR

echo

echo "[wrapper] printing env"
printenv
echo 

echo "[wrapper] hostname  = " `hostname`
echo "[wrapper] date      = " `date`
echo "[wrapper] linux timestamp = " `date +%s`

#
# untar input sandbox
#

echo "[wrapper] current directory is:"
pwd

# cd job_input
echo "[wrapper] input contents are"
ls -a

echo "[wrapper] directory contents are"
ls

#
# run it
#
echo "[wrapper] running python cardMakerShortTrack.py ${SAMPLE}_${CTAU} ${TAG} cards_${TAG}_${SAMPLE}_${CTAU}_Data_SR 1 0 1"

python cardMakerShortTrack.py ${SAMPLE}_${CTAU} ${TAG} cards_${TAG}_${SAMPLE}_${CTAU}_Data_SR 1 0 1

#
# do something with output
#

echo "[wrapper] output is"
ls -lh

echo "[wrapper] tarring output for transfer"

tar cfz cards_${TAG}_${SAMPLE}_${CTAU}.tar cards_${TAG}_${SAMPLE}_${CTAU}_Data_SR

echo "[wrapper] copying file"
OUTPUT=cards_${TAG}_${SAMPLE}_${CTAU}.tar
echo "[wrapper] OUTPUT = " ${OUTPUT}

export LD_PRELOAD=/usr/lib64/gfal2-plugins/libgfal_plugin_xrootd.so
gfal-copy -p -f -t 4200 --verbose file://`pwd`/${OUTPUT} gsiftp://gftp.t2.ucsd.edu${COPYDIR}/${OUTPUT}

echo "[wrapper] cleaning up"
for FILE in `find . -not -name "*stderr" -not -name "*stdout"`; do rm -rf $FILE; done
echo "[wrapper] cleaned up"
pwd
ls
