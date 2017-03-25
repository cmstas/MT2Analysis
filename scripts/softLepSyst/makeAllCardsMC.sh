#!/bin/bash

#INDIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/
#INDIR=../../SoftLepLooper/output/softLep29Jan16/
#INDIR=../../SoftLepLooper/output/softLep_unblind_skim_may25/

#2016 result
INDIR=/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/SoftLepLooper/output/softLep_unblind_skim_may25_mark/ 

#OUTDIR=cards_softLep3Feb16
OUTDIR=cards_T2-4bd_MC
#OUTDIR=cards_T5qqqqWW

if [ ! -d "$OUTDIR" ]; then
  mkdir -p $OUTDIR
fi

declare -a samples=("T2-4bd_275_235" )
#declare -a samples=("T5qqqqWW" )

for i in "${samples[@]}"
do
#default
#  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\", 0, 1)" > make_cards_$i.log 2>&1 &
#uses MC only
  nohup root -b -q "cardMakerMC.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\", 0, 1)" > make_cards_$i.log 2>&1 &
### option for full scans
#  nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",1, 0)" > make_cards_$i.log 2>&1 &
done
