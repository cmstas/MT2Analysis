#!/bin/bash

#INDIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/
#INDIR=../../SoftLepLooper/output/softLep29Jan16/
INDIR=../../SoftLepLooper/output/softLep_unblind_skim_may25/
#OUTDIR=cards_softLep3Feb16
OUTDIR=cards_T2-4bd
#OUTDIR=cards_T5qqqqWW

if [ ! -d "$OUTDIR" ]; then
  mkdir -p $OUTDIR
fi

declare -a samples=("T2-4bd_275_235" )
#declare -a samples=("T5qqqqWW" )

for i in "${samples[@]}"
do
  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\", 0, 1)" > make_cards_$i.log 2>&1 &
### option for full scans
#  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",1, 0)" > make_cards_$i.log 2>&1 &
done
