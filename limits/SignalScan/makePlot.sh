#!/bin/bash

#MODEL=T1tttt
#MODEL=T1bbbb
#MODEL=T1qqqq
#MODEL=T2tt
#MODEL=T2bb
#MODEL=T2-4bd
MODEL=T5qqqqWW
#DIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/Limits/24Apr16
#DIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/Limits/28Apr16unblind
DIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/Limits/3May16unblind

cp "${DIR}/r-values_${MODEL}_modified.root" PlotsSMS/config/mt2/${MODEL}_results.root
#cp "${DIR}/r-values_${MODEL}_scan.root" PlotsSMS/config/mt2/${MODEL}_results.root
cd PlotsSMS
python python/makeSMSplots.py config/mt2/${MODEL}_mt2.cfg $MODEL
