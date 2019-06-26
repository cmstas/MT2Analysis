#! /bin/bash

# INDIR=FullRunII_17MCfor18_ttbbWeights_v3_T1tttt_2100_0
INDIR=V00-10-18_FullRunII
# INDIR=V00-10-15_FullRunII_decorrHalfLepEff_T2qq_700_350
# INDIR=V00-10-15_FullRunII_decorrHalfLepEff_T2qq_950_0
cd $INDIR
mkdir -p logs

EXTRA=""
SUFFIX=""

# # b-only asimov dataset
# EXTRA="-t -1 --expectSignal 0"
# SUFFIX="_t0"

# # b+s asimov dataset
# EXTRA="-t -1 --expectSignal 1"
# SUFFIX="_t1"

nohup combine -M FitDiagnostics -n ${SUFFIX}_monojet --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_monojet.root &> logs/log_monojet.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_VL --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_VL.root &> logs/log_VL.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_L --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_L.root &> logs/log_L.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_M --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_M.root &> logs/log_M.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_H --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_H.root &> logs/log_H.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_UH --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_UH.root &> logs/log_UH.txt &
nohup combine -M FitDiagnostics -n ${SUFFIX}_all --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes $EXTRA -d cards_all.root &> logs/log_all.txt &

# combine -M FitDiagnostics -n ${SUFFIX}_monojet --robustFit=1 --saveShapes --saveWithUncertainties --saveOverallShapes --toysFrequentist $EXTRA -v 2 -d cards_monojet.root

cd ..
