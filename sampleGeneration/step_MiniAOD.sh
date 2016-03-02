#!/bin/bash

#
# args
#

FILEID=$1
EVENTS=$2
JOBNUMBER=$3

################
# MiniAOD step #
################


source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc491
if [ -r CMSSW_7_4_14/src ] ; then 
 echo release CMSSW_7_4_14 already exists
else
scram p CMSSW CMSSW_7_4_14
fi
cd CMSSW_7_4_14/src
eval `scram runtime -sh`

export X509_USER_PROXY=$HOME/private/personal/voms_proxy.cert

scram b
cd ../../
cmsDriver.py step1 --filein file:${FILEID}_AOD.root --fileout file:${FILEID}_MiniAOD-v2_job${JOBNUMBER}.root --mc --eventcontent MINIAODSIM --runUnscheduled --fast --customise SLHCUpgradeSimulations/Configuration/postLS1CustomsPreMixing.customisePostLS1,Configuration/DataProcessing/Utils.addMonitoring --datatier MINIAODSIM --conditions 74X_mcRun2_asymptotic_v2 --step PAT --python_filename cfg_step2_MiniAOD.py -n ${EVENTS} || exit $? ;

echo ${EVENTS} MiniAOD events were ran 

for job in `jobs -p` ; do
    wait $job || exit $? ;
done
