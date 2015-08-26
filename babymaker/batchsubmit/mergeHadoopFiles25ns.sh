#!/bin/bash

function run () {
    echo root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\)
    nohup nice -n 19 root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\) >& log_merge_${TAG}_$1.txt &
}

TAG=V00-01-02_25ns_softLepTest

HADOOPDIR=/hadoop/cms/store/user/${USER}/mt2babies/
OUTPUTDIR=/nfs-6/userdata/mt2/$TAG/

mkdir -p $OUTPUTDIR
chmod -R a+wrx $OUTPUTDIR

#
# DATA
#
#

#
# TTBAR
#

#
# ALTERNATIVE TTBAR
#

#
# W+JETS
#

run wjets_ht100to200
run wjets_ht200to400
run wjets_ht400to600
run wjets_ht600toInf
run wjets_ht600to800
run wjets_ht800to1200
run wjets_ht1200to2500
run wjets_ht2500toInf

#
# SINGLE TOP
#

#
# ALTERNATIVE SINGLE TOP
#

#
# DY+JETS
#

#
# GAMMA + JETS
#

#
# Z INVISIBLE
#

#
# DIBOSON
#

#
# TRIBOSON
#

#
# TTV
#

#
# QCD
#

#
# SIGNAL
#





