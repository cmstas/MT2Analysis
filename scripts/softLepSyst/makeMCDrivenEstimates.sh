#!/bin/bash

INDIR=../../SoftLepLooper/output/fromMark_test_newSoftBabies_Mar25/
THISDIR=`pwd`


if [ ! -d "$INDIR" ]; then
  echo "Input directory does not exist" 
fi



cd $INDIR
echo "hadd -f allBkg.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root ww.root wz.root zz.root  qcd_ht.root zinv_ht.root wjets_ht.root dyjetsll.root"
hadd -f allBkg.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root ww.root wz.root zz.root qcd_ht.root zinv_ht.root wjets_ht.root dyjetsll.root>> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/top.root,14)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/top.root\",14)" >> dataDrivenEstimates.log

echo "done"


