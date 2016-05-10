#!/bin/bash

INDIR=../../SoftLepLooper/output/softLep_unblind_skim_may10/
INDIRfake=../../SoftLepLooper/output/softLepFake/
THISDIR=`pwd`

DATAFILE=data_Run2015CD

if [ ! -d "$INDIR" ]; then
  echo "Input directory does not exist" 
fi

if [ ! -d "$INDIRfake" ]; then
  echo "Input directory does not exist" 
fi

cd $INDIR
echo "hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root"
hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root > dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/data_Run2015CD.root,2)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/data_Run2015CD.root\",2)" >> dataDrivenEstimates.log

cd $INDIR
echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root"
hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/top.root,7)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/top.root\",7)" >> dataDrivenEstimates.log

cd $INDIR
echo "hadd -f diboson.root ww.root wz.root zz.root "
hadd -f diboson.root ww.root wz.root zz.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/diboson.root,3)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/diboson.root\",3)" >> dataDrivenEstimates.log

#all backgrounds except diboson and dyjets
cd $INDIR
echo "hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root "
hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_noDiboson.root,4)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_noDiboson.root\",4)" >> dataDrivenEstimates.log

#all backgrounds except dyjets
cd $INDIR
echo "hadd -f allBkg_noDY.root top.root qcd_ht.root zinv_ht.root wjets_ht.root diboson.root"
hadd -f allBkg_noDY.root top.root qcd_ht.root zinv_ht.root wjets_ht.root diboson.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_noDY.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_noDY.root\",5)" >> dataDrivenEstimates.log

#all backgrounds except top
cd $INDIR
echo "hadd -f allBkg_noTop.root diboson.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root "
hadd -f allBkg_noTop.root diboson.root dyjetsll.root qcd_ht.root zinv_ht.root wjets_ht.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_noTop.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_noTop.root\",5)" >> dataDrivenEstimates.log

#all backgrounds except wjets
cd $INDIR
echo "hadd -f allBkg_noWjets.root diboson.root dyjetsll.root qcd_ht.root zinv_ht.root top.root "
hadd -f allBkg_noWJets.root diboson.root dyjetsll.root qcd_ht.root zinv_ht.root top.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_noWjets.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_noWjets.root\",5)" >> dataDrivenEstimates.log

#all backgrounds, top scaled up by 50%
cd $INDIR
echo "hadd -f allBkg_topUP.root allBkg_noTop.root allBkg_noTop.root top.root top.root top.root"
hadd -f allBkg_topUP.root allBkg_noTop.root allBkg_noTop.root top.root top.root top.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_topUP.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_topUP.root\",5)" >> dataDrivenEstimates.log

#all backgrounds, wjets scaled up by 50%
cd $INDIR
echo "hadd -f allBkg_wjetsUP.root allBkg_noWjets.root allBkg_noWjets.root wjets_ht.root wjets_ht.root wjets_ht.root"
hadd -f allBkg_wjetsUP.root allBkg_noWJets.root allBkg_noWJets.root wjets_ht.root wjets_ht.root wjets_ht.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_wjetsUp.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_wjetsUP.root\",5)" >> dataDrivenEstimates.log

#all backgrounds, DY scaled up by 50%
cd $INDIR
echo "hadd -f allBkg_dyUP.root allBkg_noDY.root allBkg_noDY.root dyjetsll.root dyjetsll.root dyjetsll.root"
hadd -f allBkg_dyUP.root allBkg_noDY.root allBkg_noDY.root dyjetsll.root dyjetsll.root dyjetsll.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_dyUP.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_dyUP.root\",5)" >> dataDrivenEstimates.log

#all backgrounds, DY scaled down by 50%
cd $INDIR
echo "hadd -f allBkg_dyDN.root allBkg_noDY.root allBkg_noDY.root allBkg_noDY.root dyjetsll.root dyjetsll.root"
hadd -f allBkg_dyDN.root allBkg_noDY.root allBkg_noDY.root allBkg_noDY.root dyjetsll.root dyjetsll.root >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg_dyDN.root,5)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg_dyDN.root\",5)" >> dataDrivenEstimates.log

#all backgrounds
cd $INDIR
echo "hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  "
hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  >> dataDrivenEstimates.log
cd $THISDIR

echo "root -b -q ../rescaleBoundaryHists.C+(${INDIR}/allBkg.root,3)"
root -b -q "../rescaleBoundaryHists.C+(\"${INDIR}/allBkg.root\",3)" >> dataDrivenEstimates.log

# Now run the 1 lepton and 2 lepton estimate makers: makeCR1Lestimate.C, makeCR2Lestimate.C
echo "root -b -q makeCR1Lestimate.C+(${INDIR},${DATAFILE})"
root -b -q "makeCR1Lestimate.C+(\"${INDIR}\",\"${DATAFILE}\")" >> dataDrivenEstimates.log
echo "root -b -q makeCR2Lestimate.C+(${INDIR},${DATAFILE})"
root -b -q "makeCR2Lestimate.C+(\"${INDIR}\",\"${DATAFILE}\")" >> dataDrivenEstimates.log
echo "root -b -q makeCR2LestimateALT.C+(${INDIR},${DATAFILE})"
root -b -q "makeCR2LestimateALT.C+(\"${INDIR}\",\"${DATAFILE}\")" >> dataDrivenEstimates.log


# cd $INDIRfake
# echo "hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root"
# hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root > dataDrivenEstimates.log
# cd $THISDIR

# echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRfake}/data.root,2)"
# root -b -q "../rescaleBoundaryHists.C+(\"${INDIRfake}/data.root\",2)" >> dataDrivenEstimates.log

# cd $INDIRfake
# echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root"
# hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root >> dataDrivenEstimates.log
# cd $THISDIR

# echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRfake}/top.root,7)"
# root -b -q "../rescaleBoundaryHists.C+(\"${INDIRfake}/top.root\",7)" >> dataDrivenEstimates.log

# cd $INDIRfake
# echo "hadd -f diboson.root ww.root wz.root zz.root "
# hadd -f diboson.root ww.root wz.root zz.root >> dataDrivenEstimates.log
# cd $THISDIR

# echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRfake}/diboson.root,3)"
# root -b -q "../rescaleBoundaryHists.C+(\"${INDIRfake}/diboson.root\",3)" >> dataDrivenEstimates.log

# #all backgrounds except diboson and dyjets
# cd $INDIRfake
# echo "hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root "
# hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root >> dataDrivenEstimates.log
# cd $THISDIR

# echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRfake}/allBkg_noDiboson.root,4)"
# root -b -q "../rescaleBoundaryHists.C+(\"${INDIRfake}/allBkg_noDiboson.root\",4)" >> dataDrivenEstimates.log

# #all backgrounds
# cd $INDIRfake
# echo "hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  "
# hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  >> dataDrivenEstimates.log
# cd $THISDIR

# echo "root -b -q ../rescaleBoundaryHists.C+(${INDIRfake}/allBkg.root,3)"
# root -b -q "../rescaleBoundaryHists.C+(\"${INDIRfake}/allBkg.root\",3)" >> dataDrivenEstimates.log

echo "done"


