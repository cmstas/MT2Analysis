#!/bin/bash

# environment on UAF:
export SCRAM_ARCH=slc6_amd64_gcc530
source /cvmfs/cms.cern.ch/cmsset_default.sh

# git clone git@github.com:cmstas/MT2Analysis.git
# cd MT2Analysis
# git checkout cmssw80x
cmsrel CMSSW_8_0_5
cd CMSSW_8_0_5/src
cmsenv
cd ../..
# git clone git@github.com:cmstas/CORE.git

git clone git@github.com:cmstas/CORE.git
cd CORE
git checkout cms4
cd ..
#  git clone git@github.com:cmstas/Tools.git

cd babymaker
make -j 8
cd ..
