#!/bin/bash

INDIR=../MT2looper/output/temp/
OUTDIR=cards

## set to 1 if doing full signal scans (instead of benchmark signal points)
DOFULLSCAN=0

mkdir -p $OUTDIR/hcand
mkdir -p $OUTDIR/Hcand
mkdir -p $OUTDIR/Zcand

#declare -a samples=(T1tttt_1500_100 T1tttt_1200_800 T1bbbb_1500_100 T1bbbb_1000_900 T1qqqq_1400_100 T1qqqq_1000_800 T2tt_850_100 T2tt_500_325 T2bb_900_100 T2bb_600_580 T2qq_1200_100 T2qq_600_550)
declare -a samples=(T2ttZH_800_200 T2ttZH_800_400 T5qqqqWH_1100_950 T5qqqqWH_1400_700)

for i in "${samples[@]}"; do
  echo nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",${DOFULLSCAN})"
  # nohup root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",${DOFULLSCAN})" > make_cards_$i.log 2>&1 &
  root -b -q "cardMaker.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",${DOFULLSCAN})"
done
