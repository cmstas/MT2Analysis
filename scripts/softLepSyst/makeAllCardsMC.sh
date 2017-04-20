#!/bin/bash

#INDIR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/
#INDIR=../../SoftLepLooper/output/softLep29Jan16/
#INDIR=../../SoftLepLooper/output/softLep_unblind_skim_may25/

#2016 result
#INDIR=/home/users/gzevi/MT2/MT2AnalysisSoft2016/MT2Analysis/SoftLepLooper/output/softLep_unblind_skim_may25_mark/ 
#OUTDIR=cards_T2-4bd_MC

#2017 old SRs
#INDIR=/home/users/mderdzinski/winter2017/soft/SoftLepLooper/output/test_newSoftBabies_Mar25
#INDIR=../../SoftLepLooper/output/oldSRs_test_Mar29/
#OUTDIR=cards_T6qqWW_oldSRsMC_Mar29_testdm20

#2017 new SRs
#INDIR=../../SoftLepLooper/output/fromMark_test_newSoftBabies_newSRs_Mar25/
INDIR=../../SoftLepLooper/output/newSRs_test_Mar29/
OUTDIR=cards_T6qqWW_newSRsMC_Mar29_dM20full

if [ ! -d "$OUTDIR" ]; then
  mkdir -p $OUTDIR
fi

#declare -a samples=("T2-4bd_275_235" )
declare -a samples=(T6qqWW_dM20)
# T6qqWW_dM10 T6qqWW_dM15  )

for i in ${samples[@]}
do
#default
#  nohup root -b -q "cardMakerMC.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\", 0, 1)" > make_cards_$i.log 2>&1 &
### option for full scans
  nohup root -b -q "cardMakerMC.C+(\"$i\",\"${INDIR}\",\"${OUTDIR}\",1, 0)" > make_cards_$i.log 2>&1 &
done


#void cardMakerMC(string signal, string input_dir, string output_dir, bool isScan = false, bool doData = false){
