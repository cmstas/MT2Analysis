#!/bin/bash

declare -a RUNS=(data_Run2017B data_Run2017C) 
#declare -a DATASETS=(DoubleEG DoubleMuon HTMHT JetHT MET MuonEG SingleElectron SingleMuon SinglePhoton)
declare -a DATASETS=(JetHT)

INDIR=/nfs-6/userdata/dpgilber/V00-09-02_json_294927-301997_PromptReco_13p88fb/
OUTDIR=output/

mkdir -p ${OUTDIR}

for RUN in ${RUNS[@]}; do
    for DATASET in ${DATASETS[@]}; do
	SAMPLE=${RUN}_${DATASET}
	eval 'nohup nice -n 10 ./EGammaJetsSkim.exe ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}_log.txt &'
    done
done

