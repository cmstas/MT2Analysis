#!/bin/bash

#MODEL="T1tttt"
#MODEL="T1bbbb"
#MODEL="T1qqqq"
#MODEL="T2tt"
#MODEL="T2bb"
MODEL="T2-4bd"
INDIR="/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/scripts/softLepSyst/cards_$MODEL"


while read i
do
  echo $i
  ./submit.sh $i 
done < $INDIR/points_$MODEL.txt
