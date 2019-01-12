#!/bin/bash

#
# args
#

FILEID=$1
FILE=$2
COPYDIR=$3
DOREBAL=$4

echo "[wrapper] FILEID    = " ${FILEID}
echo "[wrapper] FILE      = " ${FILE}
echo "[wrapper] COPYDIR   = " ${COPYDIR}
echo "[wrapper] DOREBAL   = " ${DOREBAL}

#
# set up environment
#
CMSSW_VERSION=CMSSW_9_4_1

###version using cvmfs install of CMSSW
echo "[wrapper] setting env"
export SCRAM_ARCH=slc6_amd64_gcc630
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

echo "[wrapper] hostname  = " `hostname`
echo "[wrapper] date      = " `date`
echo "[wrapper] linux timestamp = " `date +%s`
echo "[wrapper] checking input file with ls"
ls -alrth ${FILE}

# catch exit code
if [ $? -ne 0 ]; then
    echo "[wrapper] could not find input file, trying xrootd instead"
    FILESHORT=${FILE#/hadoop/cms}
    xrdfs xrootd.t2.ucsd.edu ls ${FILESHORT}
    if [ $? -ne 0 ]; then
        echo "[wrapper] could not find input file with xrootd either, exiting"
        exit 1
    else
        echo "[wrapper] found file with xrootd, will proceed"
        FILE="root://xrootd.t2.ucsd.edu/"${FILESHORT}
    fi
fi

#
# untar input sandbox
#

mkdir condorsucks
mv input.tar.xz *.sh condorsucks
cd condorsucks

echo "[wrapper] current directory is:"
pwd

echo "[wrapper] extracting input sandbox"
tar -xJf input.tar.xz

#source job_input/setupenv.sh
#printenv

echo "[wrapper] directory contents are"
ls

#
# run it
#
echo "[wrapper] running: ./processBaby ${FILEID} ${FILE}"

./processBaby ${FILEID} ${FILE} -1 ${DOREBAL}

#
# do something with output
#

echo "[wrapper] output is"
ls -lh


echo "[wrapper] copying file"
OUTPUT=`ls | grep ${FILEID}`
echo "[wrapper] OUTPUT = " ${OUTPUT}
if [[ $OUTPUT != *".root" ]]; then
    echo "[wrapper] could not find output! Exiting."
    exit 1
fi
echo "[wrapper] moving output to output.root"
mv ${OUTPUT} output.root

# Rigorous sweeproot which checks ALL branches for ALL events.
# If GetEntry() returns -1, then there was an I/O problem, so we will delete it
cat > rigorousSweepRoot.py << EOL
import ROOT as r
import os, sys

f1 = r.TFile("output.root")
if not f1 or not f1.IsOpen() or f1.IsZombie():
    print "[RSR] removing zombie output.root because it does not deserve to live"
    os.system("rm output.root")
    sys.exit()

t = f1.Get("mt2")
if type(t)==type(r.TObject()):
    print "[RSR] no tree named 'mt2' in file! Deleting."
    os.system("rm output.root")
    sys.exit()

print "[RSR] ntuple has %i events" % t.GetEntries()

foundBad = False
for i in range(0,t.GetEntries(),1):
    if t.GetEntry(i) < 0:
        foundBad = True
        print "[RSR] found bad event %i" % i
        break

if foundBad:
    print "[RSR] removing output.root because it does not deserve to live"
    os.system("rm output.root")
else:
    print "[RSR] passed the rigorous sweeproot"
EOL

date +%s
echo "[wrapper] running rigorousSweepRoot.py"
python rigorousSweepRoot.py
date +%s

#
# clean up
#

if [ ! -d "${COPYDIR}" ]; then
    echo "creating output directory " ${COPYDIR}
    mkdir ${COPYDIR}
fi

export LD_PRELOAD=/usr/lib64/gfal2-plugins/libgfal_plugin_xrootd.so
gfal-copy -p -f -t 4200 --verbose file://`pwd`/output.root gsiftp://gftp.t2.ucsd.edu${COPYDIR}/${FILEID}.root

