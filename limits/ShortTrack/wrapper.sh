#!/bin/bash

#
# args
#

CARDTAR=$1
COPYDIR=$2

MASSPOINT=${CARDTAR%.tar}

echo "[wrapper] CARDTAR   = " ${CARDTAR}
echo "[wrapper] COPYDIR   = " ${COPYDIR}
echo "[wrapper] MASSPOINT = " ${MASSPOINT}

echo "[wrapper] extracting input sandbox"
tar xfz job_input.tar
tar xfz $CARDTAR


BASE=$PWD

#Set environment
export SCRAM_ARCH=slc6_amd64_gcc530
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd CMSSW_8_1_0/src/HiggsAnalysis/CombinedLimit
echo "[wrapper] in directory: " ${PWD}
echo "[wrapper] attempting to build"
eval `scramv1 runtime -sh`
scramv1 b ProjectRename
scram b
eval `scramv1 runtime -sh`
cd -

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
export PATH=$PATH:.

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

export PYTHONPATH="${PYTHONPATH}:${PWD}/HiggsAnalysis/CombinedLimit"
echo "PYTHONPATH is set to:"
echo $PYTHONPATH

echo "[wrapper] after untarring combine and card directory:"
ls

echo "[wrapper] combining cards in ${MASSPOINT}"

./combineDir.sh ${MASSPOINT}

echo "[wrapper] done combining cards. Is combined.txt present?"
ls

text2workspace.py combined.txt -b -o $MASSPOINT.root

echo "[wrapper] Now run limits"
combine -M AsymptoticLimits -n ${MASSPOINT} $MASSPOINT.root

mv higgsCombine${MASSPOINT}.AsymptoticLimits.mH120.root limit_$MASSPOINT.root

echo "[wrapper] limit finished"
ls

#echo "[wrapper] Running significance"
#echo ""
#echo "[wrapper] Generating toys"
#combine -M HybridNew $MASSPOINT.root --LHCmode LHC-significance  --saveToys --fullBToys --saveHybridResult -T 250 -i 2 -s 123456789 

#echo "[wrapper] Calculating significance"
#combine -M HybridNew $MASSPOINT.root --LHCmode LHC-significance --readHybridResult --toysFile=higgsCombineTest.HybridNew.mH120.123456789.root

#combine -M Significance --uncapped=1 --rMin=-2 -n ${MASSPOINT} ${MASSPOINT}.root

#mv higgsCombineTest.HybridNew.mH120.root sig_$MASSPOINT.root

#echo "[wrapper] significance finished"
#ls


echo "[wrapper] copying file"

if [ ! -d "${COPYDIR}" ]; then
    echo "creating output directory " ${COPYDIR}
    mkdir ${COPYDIR}
fi

export LD_PRELOAD=/usr/lib64/gfal2-plugins/libgfal_plugin_xrootd.so
gfal-copy -p -f -t 4200 --verbose file://`pwd`/limit_$MASSPOINT.root gsiftp://gftp.t2.ucsd.edu${COPYDIR}/
#gfal-copy -p -f -t 4200 --verbose file://`pwd`/sig_$MASSPOINT.root gsiftp://gftp.t2.ucsd.edu${COPYDIR}/

echo "[wrapper] cleaning up"
for FILE in `find . -not -name "*stderr" -not -name "*stdout"`; do rm -rf $FILE; done
echo "[wrapper] cleaned up"
pwd
ls
