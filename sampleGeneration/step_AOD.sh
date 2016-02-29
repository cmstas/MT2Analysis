#!/bin/bash

#
# args
#

FILEID=$1
FRAGMENT=$2

################
### AOD step ###
################

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc491
if [ -r CMSSW_7_4_4/src ] ; then 
 echo release CMSSW_7_4_4 already exists
else
scram p CMSSW CMSSW_7_4_4
fi
cd CMSSW_7_4_4/src
eval `scram runtime -sh`

export X509_USER_PROXY=$HOME/private/personal/voms_proxy.cert

mkdir -p Configuration/GenProduction/python
cp ../../${FRAGMENT} Configuration/GenProduction/python/

# curl -s --insecure https://cms-pdmv.cern.ch/mcm/public/restapi/requests/get_fragment/SUS-RunIISpring15FSPremix-00243 --retry 2 --create-dirs -o Configuration/GenProduction/python/SUS-RunIISpring15FSPremix-00243-fragment.py 
# [ -s Configuration/GenProduction/python/SUS-RunIISpring15FSPremix-00243-fragment.py ] || exit $?;

scram b
cmsenv
cd ../../
cmsDriver.py Configuration/GenProduction/python/${FRAGMENT} --filein file:${FILEID}_pLHE.root --fileout file:${FILEID}_AOD.root --pileup_input "dbs:/Neutrino_E-10_gun/RunIISpring15PrePremix-MCRUN2_74_V9-v1/GEN-SIM-DIGI-RAW" --mc --eventcontent AODSIM --fast --customise SLHCUpgradeSimulations/Configuration/postLS1CustomsPreMixing.customisePostLS1,Configuration/DataProcessing/Utils.addMonitoring --datatier AODSIM --conditions MCRUN2_74_V9 --beamspot NominalCollision2015 --step GEN,SIM,RECOBEFMIX,DIGIPREMIX_S2:pdigi_valid,DATAMIX,L1,L1Reco,RECO,HLT:@frozen25ns --magField 38T_PostLS1 --datamix PreMix --python_filename cfg_step2_AOD.py -n 100000 || exit $? ; 

echo 100000 AOD events were ran 

for job in `jobs -p` ; do
    wait $job || exit $? ;
done
