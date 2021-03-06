#!/bin/bash

HOME=`pwd`

SAMPLE=$1
OUTPUT_DIR=$2

echo "No.of args:  $#"
echo "args: $@"

#Tell us where we're running
echo "host: `hostname`" 

function getjobad {
    grep -i "^$1" "$_CONDOR_JOB_AD" | cut -d= -f2- | xargs echo
}

echo "RequestMemory: $(getjobad RequestMemory)"

TZ='America/Los_Angeles'
echo "JOB START: `date`"

#untar tarball containing input files
if [ -e job_input.tar.gz ]
then
  tar -xzvf job_input.tar.gz
else
  echo "job input missing!"
  exit 1
fi

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

echo "AFTER BUILD: `date`"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
export PATH=$PATH:.

mv $BASE/cards_$SAMPLE.tar.gz .
if [ -e cards_$SAMPLE.tar.gz ]
then
  tar -xzvf cards_$SAMPLE.tar.gz
else
  echo "datacards missing!"
  exit 1
fi

export PYTHONPATH="${PYTHONPATH}:${PWD}/HiggsAnalysis/CombinedLimit"
echo "PYTHONPATH is set to:"
echo $PYTHONPATH

which root

if [ ! -d "${OUTPUT_DIR}" ]; then
    echo "creating output directory " ${OUTPUT_DIR}
    mkdir ${OUTPUT_DIR}
fi

echo "HOSTNAME"`hostname`"HOSTNAME" >> log_$SAMPLE.log

echo "BEFORE combineCards: `date`"
echo "running combineCards.py"
combineCards.py -S "datacard_"*"_$SAMPLE.txt" > "card_all_$SAMPLE.txt"  
#mv "datacard_${SAMPLE}_combined.txt" "card_all_$SAMPLE.txt"  #use this line instead of combineCards.py if cards are already combined
echo "ls -lrth after combineCards.py"
ls -lrth
echo "BEFORE text2workspace: `date`"
echo "running text2workspace.py"
text2workspace.py card_all_$SAMPLE.txt -b -o $SAMPLE.root
echo "ls -lrth after text2workspace.py"
ls -lrth

# compute significance
echo "BEFORE Significance: `date`"
echo "Running command: combine -M Significance --uncapped=1 --rMin=-2 -n $SAMPLE $SAMPLE.root >> log_$SAMPLE.log 2>&1"
combine -M Significance --uncapped=1 --rMin=-2 -n "$SAMPLE" "$SAMPLE.root" >> "log_$SAMPLE.log" 2>&1
mv "higgsCombine"$SAMPLE".Significance.mH120.root" "sig_"$SAMPLE".root"
gfal-copy -p -f -t 4200 --verbose file://`pwd`/sig_${SAMPLE}.root gsiftp://gftp.t2.ucsd.edu${OUTPUT_DIR}/sig_${SAMPLE}.root

# compute limit
echo "BEFORE Asymptotic: `date`"
echo "Running command: combine -M AsymptoticLimits -n $SAMPLE $SAMPLE.root >> log_$SAMPLE.log 2>&1"
combine -M AsymptoticLimits -n "$SAMPLE" "$SAMPLE.root" >> "log_$SAMPLE.log" 2>&1

mv "higgsCombine"$SAMPLE".AsymptoticLimits.mH120.root" "limit_"$SAMPLE".root"
echo "AFTER Asymptotic: `date`"
echo "ls -lrth after calculating the limit"
ls -lrth

echo "Log file:"
cat log_$SAMPLE.log

# stageout_error=1
# while [ $stageout_error != 0 ]; do
#Copy the output
env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-copy -p -f -t 4200 --verbose file://`pwd`/limit_${SAMPLE}.root gsiftp://gftp.t2.ucsd.edu${OUTPUT_DIR}/limit_${SAMPLE}.root
env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-copy -p -f -t 4200 --verbose file://`pwd`/sig_${SAMPLE}.root gsiftp://gftp.t2.ucsd.edu${OUTPUT_DIR}/sig_${SAMPLE}.root
env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-copy -p -f -t 4200 --verbose file://`pwd`/log_${SAMPLE}.log gsiftp://gftp.t2.ucsd.edu${OUTPUT_DIR}/log_${SAMPLE}.log
stageout_error=$?

    # if [ $stageout_error != 0 ]; then
        # sleep 60m
    # fi
# done

if [ $stageout_error != 0 ]
then
  echo "Stageout with lcg-cp failed."
fi

#cleanup
echo "cleaning up"
for FILE in `find . -not -name "*stderr" -not -name "*stdout"`; do rm -rf $FILE; done
echo "cleaned up"
ls
