#!/bin/bash

make b -j 12 || return $?

RUNTAG=$1

PREFIX=/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-03_2016fullYear_17Jul2018_data_Run2016
declare -a INDIRS=(
B_DoubleMuon_17Jul2018_ver1-v1
B_DoubleMuon_17Jul2018_ver2-v1
B_EGamma_17Jul2018_ver1-v1
B_EGamma_17Jul2018_ver2-v1
B_JetHT_17Jul2018_ver1-v1
B_JetHT_17Jul2018_ver2-v1
B_MET_17Jul2018_ver1-v1
B_MET_17Jul2018_ver2-v1
B_MuonEG_17Jul2018_ver1-v1
B_MuonEG_17Jul2018_ver2-v1
B_SingleMuon_17Jul2018_ver1-v1
B_SingleMuon_17Jul2018_ver2-v1
C_DoubleMuon_17Jul2018-v1
C_EGamma_17Jul2018-v1
C_JetHT_17Jul2018-v1
C_MET_17Jul2018-v1
C_MuonEG_17Jul2018-v1
C_SingleMuon_17Jul2018-v1
D_DoubleMuon_17Jul2018-v1
D_EGamma_17Jul2018-v1
D_JetHT_17Jul2018-v1
D_MET_17Jul2018-v1
D_MuonEG_17Jul2018-v1
D_SingleMuon_17Jul2018-v1
E_DoubleMuon_17Jul2018-v1
E_EGamma_17Jul2018-v1
E_JetHT_17Jul2018-v1
E_MET_17Jul2018-v1
E_MuonEG_17Jul2018-v1
E_SingleMuon_17Jul2018-v1
F_DoubleMuon_17Jul2018-v1
F_EGamma_17Jul2018-v1
F_JetHT_17Jul2018-v1
F_MET_17Jul2018-v1
F_MuonEG_17Jul2018-v1
F_SingleMuon_17Jul2018-v1
G_DoubleMuon_17Jul2018-v1
G_EGamma_17Jul2018-v1
G_JetHT_17Jul2018-v1
G_MET_17Jul2018-v1
G_MuonEG_17Jul2018-v1
G_SingleMuon_17Jul2018-v1
H_DoubleMuon_17Jul2018-v1
H_EGamma_17Jul2018-v1
H_JetHT_17Jul2018-v1
H_MET_17Jul2018-v1
H_MuonEG_17Jul2018-v1
H_SingleMuon_17Jul2018-v1
)

OUTDIR=output/${RUNTAG}
CONFIG="data_2016_94x"
for INDIR in ${INDIRS[@]}; do
    command="./writeConfig.sh ${PREFIX}${INDIR} ${RUNTAG} ${CONFIG}"
    echo $command
    eval $command
done