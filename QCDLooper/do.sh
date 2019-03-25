#!/bin/bash

make -j 8

OUTDIR=output/V00-10-10_2016fullYear

LOGDIR=logs

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}


# #most recent MC
CONFIG=mc_94x_Summer16
INDIR=/nfs-7/userdata/mt2/V00-10-10_2016fullYear_skim_qcd/extmerge
# CONFIG=mc_94x_Fall17
# INDIR=/nfs-7/userdata/mt2/V00-10-09_2017fullYear_skim_qcd/
# CONFIG=mc_102x_Autumn18
# INDIR=/nfs-7/userdata/mt2/V00-10-09_2017fullYear_skim_qcd/
# declare -a Samples=(qcd_ht ttsl ttdl singletop ttw ttz ttg wjets_ht zinv_ht)
declare -a Samples=(qcd_ht ttsl ttdl singletop ttw ttz ttg zinv_ht wjets_ht200to400 wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf )
# declare -a Samples=(ttsl ttdl)

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done

# data
CONFIG=data_2017_31Mar2018
INDIR=/nfs-7/userdata/mt2/V00-10-09_2017fullYear_skim_qcd/

declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)

# for SAMPLE in ${Samples[@]};
#   do echo ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR}
#   nohup ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
# done

# data
CONFIG=data_2018_17Sep2018
INDIR=/nfs-7/userdata/mt2/V00-10-09_2018fullYear_skim_qcd/

declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C data_Run2018D)

# for SAMPLE in ${Samples[@]};
#   do echo ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR}
#   nohup ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
# done

# data
CONFIG=data_2016_94x
INDIR=/nfs-7/userdata/mt2/V00-10-10_2016fullYear_skim_qcd/extmerge

declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} $CONFIG ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done
