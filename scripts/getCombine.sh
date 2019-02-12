#!/bin/bash

#USER INPUTS
CMSSW_release=CMSSW_8_1_0

#--Here there be dragons----
export SCRAM_ARCH=slc6_amd64_gcc530
export CMS_PATH=/cvmfs/cms.cern.ch
source /cvmfs/cms.cern.ch/cmsset_default.sh
scramv1 p -n ${CMSSW_release} CMSSW $CMSSW_release
pushd ${CMSSW_release}
eval `scramv1 runtime -sh`

cd src

git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git fetch origin
git checkout v7.0.9

cd ../..

git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
scramv1 b clean; scramv1 b

popd