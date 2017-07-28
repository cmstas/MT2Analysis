#!/bin/bash

# environment on UAF:
export SCRAM_ARCH=slc6_amd64_gcc530
source /cvmfs/cms.cern.ch/cmsset_default.sh

# git clone git@github.com:cmstas/MT2Analysis.git
# cd MT2Analysis
# git checkout cmssw80x
cmsrel CMSSW_9_2_4
cd CMSSW_9_2_4/src
cmsenv
cd ../..
# git clone git@github.com:cmstas/CORE.git

if [ -d /home/users/$USER/CORE ]
then
  echo "Creating symbolic link to ~/CORE , make sure that it's up to date."
  ln -s /home/users/$USER/CORE . 
else 
  git clone git@github.com:cmstas/CORE.git
  cd CORE
  git checkout master
  cd ..
#  git clone git@github.com:cmstas/Tools.git
fi

cd babymaker
make -j 8
cd ..
