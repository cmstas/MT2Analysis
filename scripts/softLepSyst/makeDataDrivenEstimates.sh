#!/bin/bash

INDIRSR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/
INDIRCR=/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/CR19Jan16/
THISDIR=`pwd`

DATAFILE=data_Run2015CD
#DATAFILE=allBkg

if [ ! -d "$INDIRSR" ]; then
  echo "Input directory does not exist" 
fi
if [ ! -d "$INDIRCR" ]; then
  echo "Input directory does not exist" 
fi

cd $INDIRSR
echo "hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root"
hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root >> dataDrivenEstimates.log
cd $THISDIR

cd $INDIRCR
echo "hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root"
hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRSR}/data_Run2015CD.root,2)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRSR}/data_Run2015CD.root\",2)" >> dataDrivenEstimates.log
echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRCR}/data_Run2015CD.root,2)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRCR}/data_Run2015CD.root\",2)" >> dataDrivenEstimates.log

cd $INDIRSR
echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root"
hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root >> dataDrivenEstimates.log
cd $THISDIR

cd $INDIRCR
echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root"
hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRSR}/top.root,6)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRSR}/top.root\",6)" >> dataDrivenEstimates.log
echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRCR}/top.root,6)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRCR}/top.root\",6)" >> dataDrivenEstimates.log

cd $INDIRSR
echo " hadd -f bkg.root top.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root ww.root wz.root zz.root"
hadd -f allBkg.root top.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root ww.root wz.root zz.root >> dataDrivenEstimates.log
cd $THISDIR

cd $INDIRCR
echo " hadd -f bkg.root top.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root ww.root wz.root zz.root"
hadd -f allBkg.root top.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root ww.root wz.root zz.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRSR}/allBkg.root,8)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRSR}/allBkg.root\",8)" >> dataDrivenEstimates.log
echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRCR}/allBkg.root,8)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIRCR}/allBkg.root\",8)" >> dataDrivenEstimates.log


# Now run the 1 lepton and 2 lepton estimate makers: makeCR1Lestimate.C
echo "root -b -q makeCR1Lestimate.C+(${INDIRSR},${INDIRCR},${DATAFILE})"
root -b -q "makeCR1Lestimate.C+(\"${INDIRSR}\",\"${INDIRCR}\",\"${DATAFILE}\")" >> dataDrivenEstimates.log


echo "done"


