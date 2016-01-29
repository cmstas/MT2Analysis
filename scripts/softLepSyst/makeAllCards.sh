#!/bin/bash

INDIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/
OUTDIR=cards_test

if [ ! -d "$OUTDIR" ]; then
  mkdir -p $OUTDIR
fi

declare -a samples=(T2-4bd_275_235 )

for i in "${samples[@]}"
do
  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\", 0, 0)" > make_cards_$i.log 2>&1 &
### option for full scans
#  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",1)" > make_cards_$i.log 2>&1 &
done
