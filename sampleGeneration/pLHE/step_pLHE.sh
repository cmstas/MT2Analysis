#!/bin/bash

#
# args
#

FILEOUT=$1
FILEIN=$2

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
cmsDriver.py step1 --filein file:$FILEIN --filetype LHE --fileout file:$FILEOUT.root --mc --eventcontent LHE --datatier LHE --conditions MCRUN2_71_V1::All --step NONE --python_filename cfg_step1_pLHE.py --customise Configuration/DataProcessing/Utils.addMonitoring -n 100000 
#cmsRun -e -j cfg_step1_pLHE.py
echo 100000 events were ran 
grep "TotalEvents" cfg_step1_pLHE.xml 
grep "Timing-tstoragefile-write-totalMegabytes" cfg_step1_pLHE.xml 
grep "PeakValueRss" cfg_step1_pLHE.xml 
grep "AvgEventTime" cfg_step1_pLHE.xml 
grep "AvgEventCPU" cfg_step1_pLHE.xml 
grep "TotalJobCPU" cfg_step1_pLHE.xml 
# cmsDriver.py lhetest --filein lhe:15488:0 --mc  --conditions MCRUN2_71_V1::All -n 1000000 --python lhetest_0.py --step NONE --no_exec --no_output
# cmsRun lhetest_0.py & 

for job in `jobs -p` ; do
    wait $job || exit $? ;
done
