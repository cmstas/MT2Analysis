#!/bin/bash

# environment on UAF:
export SCRAM_ARCH=slc6_amd64_gcc530
source /cvmfs/cms.cern.ch/cmsset_default.sh

echo "[setup] Checking out CMSSW_8_0_26"
cmsrel CMSSW_8_0_26
cd CMSSW_8_0_26/src
cmsenv

echo "[setup] Installing Short Track Babymaker"
git clone https://github.com/mmasciov/shortTrackNtuplizer.git ShortTrackNtuplizer
cd ShortTrackNtuplizer
git checkout dpg
cd HistoAnalyzer
scram b
ln -s ../../../.. MT2AnalysisHome

cd MT2AnalysisHome
ln -s CMSSW_8_0_26/src/ShortTrackNtuplizer babymakerST

echo "Checking for CORE..."
if [ -d /home/users/$USER/CORE ]
then
  echo "Creating symbolic link to ~/CORE , make sure that it's up to date."
  ln -s /home/users/$USER/CORE . 
else 
  echo "CORE not found in ~, installing..."
  git clone git@github.com:cmstas/CORE.git
  cd CORE
  git checkout CMS3_archive
  cd ..
fi

cd babymaker
make -j 8
cd ..
