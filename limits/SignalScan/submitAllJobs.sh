#!/bin/bash

#MODEL="T1tttt"
#MODEL="T1bbbb"
#MODEL="T1qqqq"
#MODEL="T2tt"
#MODEL="T2bb"
#MODEL="T2-4bd_scan"
MODEL="T5qqqqWW_modified"
#DATE="24Apr16new"
#DATE="29Apr16unblind"
DATE="3May16unblind"

INDIR="/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/scripts/softLepSyst/cards_${MODEL}_${DATE}"

OUTPUTDIR="/hadoop/cms/store/user/$USER/combine/softLimits/${MODEL}_${DATE}"

rm OUTPUTDIR/*

while read i
do
  echo $i
  ./submit.sh $i ${DATE} ${MODEL}
done < $INDIR/points_$MODEL.txt
