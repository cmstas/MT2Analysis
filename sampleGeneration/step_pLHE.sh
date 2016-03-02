#!/bin/bash

#
# args
#

FILEID=$1
FILEIN=$2
TOTAL=$3

################
## pLHE  step ##
################

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc481
if [ -r CMSSW_7_1_20_patch3/src ] ; then 
 echo release CMSSW_7_1_20_patch3 already exists
else
scram p CMSSW CMSSW_7_1_20_patch3
fi
cd CMSSW_7_1_20_patch3/src
eval `scram runtime -sh`

export X509_USER_PROXY=$HOME/private/personal/voms_proxy.cert

scram b
cd ../../
cmsDriver.py step1 --filein file:$FILEIN --filetype LHE --fileout file:${FILEID}_pLHE.root --mc --eventcontent LHE --datatier LHE --conditions MCRUN2_71_V1::All --step NONE --python_filename cfg_step1_pLHE.py --customise Configuration/DataProcessing/Utils.addMonitoring -n ${TOTAL} 
echo ${TOTAL} pLHE events were ran 

for job in `jobs -p` ; do
    wait $job || exit $? ;
done
