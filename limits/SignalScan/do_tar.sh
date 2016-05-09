#!/bin/bash

#MODEL="T1tttt"
#MODEL="T1bbbb"
#MODEL="T1qqqq"
#MODEL="T2tt"
#MODEL="T2bb"
#MODEL="T2-4bd_scan" #Need to remove points beyond 400 by hand from points_$MODEL
#MODEL="T5qqqqWW_scan"
MODEL="T5qqqqWW_modified"
#DATE="24Apr16new"
DATE="3May16unblind"

#INDIR="/home/users/jgran/limits_for_paper/MT2Analysis/scripts/cards_$MODEL"
INDIR="/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/scripts/softLepSyst/cards_${MODEL}_${DATE}"
CARD_DIR="cards_${MODEL}_${DATE}"

if [ -d $CARD_DIR ]
then
  rm -r $CARD_DIR
  mkdir  $CARD_DIR
else
  mkdir  $CARD_DIR
fi

if [ -e log_tar.log ]
then
  rm log_tar.log
fi

pushd .
cd $INDIR
while read i
do
  echo $i
  tar -czvf cards_$i.tar.gz "datacard_"*"_$i.txt" >> log_tar.log 2>&1
done < $INDIR/points_$MODEL.txt
popd

pushd .
cd $CARD_DIR
cp $INDIR/cards_*.tar.gz .
popd
