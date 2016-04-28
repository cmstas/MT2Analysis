#!/bin/bash

function run () {
    echo root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\)
    nohup nice -n 19 root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\) >& log_merge_${TAG}_$1.txt &
}

TAG=mt2Higgs

HADOOPDIR=/hadoop/cms/store/user/${USER}/mt2babies
OUTPUTDIR=/nfs-6/userdata/mt2/$TAG

mkdir -p $OUTPUTDIR
chmod -R a+wrx $OUTPUTDIR


# Signals
run T2ttZH_800_200
run T2ttZH_800_400
run T5qqqqWH_1100_950
run T5qqqqWH_1400_700

# ttbar
run ttsl_mg_lo_top
