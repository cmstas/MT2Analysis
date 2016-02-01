#!/bin/bash

#MODEL=T1tttt
#MODEL=T1bbbb
#MODEL=T1qqqq
#MODEL=T2tt
#MODEL=T2bb
MODEL=T2-4bd
INDIR=/hadoop/cms/store/user/gzevi/combine/softLimits/$MODEL
CARDDIR="/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/scripts/softLepSyst/cards_$MODEL"

rm r-values_$MODEL.root

while read i
do
  echo $i
#  MASS1=$(echo "$i"|awk -F- 'split($1,a,"_")&&$0=a[2]')
#  MASS2=$(echo "$i"|awk -F- 'split($1,a,"_")&&$0=a[3]')
  MASS1=$(echo "$i"|awk -F- 'split($0,a,"_")&&$0=a[2]')
  MASS2=$(echo "$i"|awk -F- 'split($0,a,"_")&&$0=a[3]')
  if [ "$MASS2" == "" ]
  then
    MASS2="0"
  fi
  echo $MASS1
  echo $MASS2
  root -b -q "make_rValues.C(\"$INDIR\",\"$MODEL\",$MASS1,$MASS2)"
done < $CARDDIR/points_$MODEL.txt

#root -b -q "smooth.C(\"$MODEL\")" #FIXME
#root -b -q "make_contour.C(\"$MODEL\")"
