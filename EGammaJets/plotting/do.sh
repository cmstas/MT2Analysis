#!/bin/bash

cd ~/MT2AnalysisEdit/MyCode/EGammaJets/output

hadd -f data_2016.root data_Run*

cd ~/2017/MT2AnalysisTrigFix/EGammaJets/output/

hadd -f data_2017.root data_Run*

cd ../plotting

mkdir -p PNGs
mkdir -p PDFs

python plotter.py