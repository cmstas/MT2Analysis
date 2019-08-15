#! /bin/bash

if [ ! -d "CMSSW_8_1_0/src" ]; then
    export SCRAM_ARCH=slc6_amd64_gcc530
    cmsrel CMSSW_8_1_0
fi
pushd CMSSW_8_1_0/src 
cmsenv
if [ ! -d "HiggsAnalysis/CombinedLimit" ]; then
    git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
fi
cd HiggsAnalysis/CombinedLimit

scram b -j12

popd

