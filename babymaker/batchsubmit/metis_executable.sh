#!/bin/bash

OUTPUTDIR=$1
OUTPUTNAME=$2
INPUTFILENAME=$3
IFILE=$4
CMSSWVERSION=$5
SCRAMARCH=$6

# Make sure OUTPUTNAME doesn't have .root since we add it manually
OUTPUTNAME=$(echo $OUTPUTNAME | sed 's/\.root//')

echo -e "\n--- begin header output ---\n" #                     <----- section division
echo "OUTPUTDIR: $OUTPUTDIR"
echo "OUTPUTNAME: $OUTPUTNAME"
echo "INPUTFILENAME: $INPUTFILENAME"
echo "IFILE: $IFILE"
echo "CMSSWVERSION: $CMSSWVERSION"
echo "SCRAMARCH: $SCRAMARCH"

echo "GLIDEIN_CMSSite: $GLIDEIN_CMSSite"
echo "hostname: $(hostname)"
echo "uname -a: $(uname -a)"
echo "time: $(date +%s)"
echo "args: $@"

echo -e "\n--- end header output ---\n" #                       <----- section division

source /cvmfs/cms.cern.ch/cmsset_default.sh

export SCRAM_ARCH=${SCRAMARCH}

eval `scramv1 project CMSSW $CMSSWVERSION`
cd $CMSSWVERSION
eval `scramv1 runtime -sh`
mv ../package.tar.gz package.tar.gz
tar xf package.tar.gz

# need this to find the .so files, even though they are in the same
# directory
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

echo "before running: ls -lrth"
ls -lrth 

echo -e "\n--- begin running ---\n" #                           <----- section division

echo "Converting Metis-style inputs into MT2-style inputs"
FILEID=`echo ${INPUTFILENAME##*/} | sed 's/\.root//g'`
DOREBAL=0
if [[ $OUTPUTDIR = *"RebalanceAndSmear"* ]]; then
    DOREBAL=1
fi

echo "FILEID = $FILEID"
echo "DOREBAL = $DOREBAL"

echo "Executing ./processBaby $FILEID $INPUTFILENAME -1 $DOREBAL"
./processBaby $FILEID $INPUTFILENAME -1 $DOREBAL

OUTPUT=`ls | grep $FILEID`
echo "OUTPUT = " $OUTPUT

# Rigorous sweeproot which checks ALL branches for ALL events.
# If GetEntry() returns -1, then there was an I/O problem, so we will delete it
python << EOL
import ROOT as r
import os

foundBad = False
try:
    f1 = r.TFile("${OUTPUT}")
    t = f1.Get("mt2")
    nevts = t.GetEntries()
    for i in range(0,t.GetEntries(),1):
        if t.GetEntry(i) < 0:
            foundBad = True
            print "[RSR] found bad event %i" % i
            break
except: foundBad = True

if foundBad:
    print "[RSR] removing output file because it does not deserve to live"
    os.system("rm ${OUTPUT}")
else: print "[RSR] passed the rigorous sweeproot"
EOL

echo -e "\n--- end running ---\n" #                             <----- section division

echo "after running: ls -lrth"
ls -lrth

echo -e "\n--- begin copying output ---\n" #                    <----- section division
echo "Sending output file $OUTPUT"
gfal-copy -p -f -t 4200 --verbose file://`pwd`/${OUTPUT} gsiftp://gftp.t2.ucsd.edu${OUTPUTDIR}/${OUTPUTNAME}_${IFILE}.root --checksum ADLER32
if [ "$?" != "0" ]; then
    echo "Removing output file because gfal-copy crashed"
    gfal-rm --verbose gsiftp://gftp.t2.ucsd.edu${OUTPUTDIR}/${OUTPUT}
fi