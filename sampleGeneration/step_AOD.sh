#!/bin/bash

#
# args
#

FILEID=$1
FRAGMENT=$2
EVENTS=$3
JOBNUMBER=$4

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
cd ../../
cmsDriver.py Configuration/GenProduction/python/${FRAGMENT} --filein file:${FILEID}_pLHE.root --fileout file:${FILEID}_AOD.root --pileup_input file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/0021F46D-ED25-E511-9376-0025905B8576.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/0040E519-2226-E511-A1C9-002618FDA277.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00458809-B226-E511-952B-002354EF3BE0.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00946A66-4726-E511-BA1E-0025905A60F2.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/009FA86E-5F26-E511-A542-003048FFD732.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00AE3880-B826-E511-9B01-00261894394F.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00AF43F2-2B26-E511-BC56-0025905A6088.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00B5E935-3C26-E511-A7DF-0025905B85D6.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00CC8235-3C26-E511-8D60-0025905A60F4.root,file:/hadoop/cms/phedex/store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/MCRUN2_74_V9-v1/20000/00D70232-4126-E511-B79A-0025905938A4.root --mc --eventcontent AODSIM --fast --customise SLHCUpgradeSimulations/Configuration/postLS1CustomsPreMixing.customisePostLS1,Configuration/DataProcessing/Utils.addMonitoring --datatier AODSIM --conditions MCRUN2_74_V9 --beamspot NominalCollision2015 --step GEN,SIM,RECOBEFMIX,DIGIPREMIX_S2:pdigi_valid,DATAMIX,L1,L1Reco,RECO,HLT:@frozen25ns --magField 38T_PostLS1 --datamix PreMix --python_filename cfg_step2_AOD.py --no_exec -n ${EVENTS} || exit $? ; 

python insertSkipEvents.py ${EVENTS} ${JOBNUMBER}

cmsRun cfgAOD_withSkip.py

echo ${EVENTS} AOD events were ran 

for job in `jobs -p` ; do
    wait $job || exit $? ;
done
