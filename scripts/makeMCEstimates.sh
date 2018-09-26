#!/bin/bash

INDIR=/home/users/dpgilber/2017/MT2AnalysisRegions/MT2Looper/output/RegionTest5

THISDIR=`pwd`

## to use data for lostlepton
#LOSTLEPFILE=data_Run2016
## to use MC for lostlepton
LOSTLEPFILE=lostlep

GJETFILE=qcdplusgjet

if [ ! -d "$INDIR" ]; then
  echo "Input directory does not exist" 
fi

cd $INDIR

# combine wjets_ht bins
echo "hadd -f wjets_ht.root wjets_ht100to200.root wjets_ht200to400.root wjets_ht400to600.root wjets_ht600to800.root wjets_ht800to1200.root wjets_ht1200to2500.root wjets_ht2500toInf.root"
hadd -f wjets_ht.root wjets_ht100to200.root wjets_ht200to400.root wjets_ht400to600.root wjets_ht600to800.root wjets_ht800to1200.root wjets_ht1200to2500.root wjets_ht2500toInf.root >> dataDrivenEstimates.log

# (temporarily replaced tth with ttg, because of sample availability)
echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root ttg.root"
hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root ttg.root >> dataDrivenEstimates.log

# make "lostlep" from sum of MC lostlep backgrounds (except QCD, to avoid spikes)
echo "hadd -f lostlep.root ttsl.root ttdl.root singletop.root ttw.root ttz.root wjets_ht.root"
hadd -f lostlep.root ttsl.root ttdl.root singletop.root ttw.root ttz.root wjets_ht.root >> dataDrivenEstimates.log

# make full background soup file, our standin for "data"
echo "hadd -f background.root lostlep.root dyjetsll_ht.root zinv_ht.root"
hadd -f background.root lostlep.root dyjetsll_ht.root zinv_ht.root

echo "hadd qcdplusgjet.root gjets_dr0p4_ht.root qcd_ht.root"
hadd -f qcdplusgjet.root gjets_dr0p4_ht.root qcd_ht.root  >> $THISDIR/dataDrivenEstimates.log

cd $THISDIR

#this script scales the HI and LOW boundary histograms by 1/numSamples since we don't want these hadd'ed
echo "root -b -q rescaleBoundaryHists.C+(${INDIR}/wjets_ht.root,7)"
root -b -q "rescaleBoundaryHists.C+(\"${INDIR}/wjets_ht.root\",7)" >> dataDrivenEstimates.log

echo "root -b -q rescaleBoundaryHists.C+(${INDIR}/top.root,6)"
root -b -q "rescaleBoundaryHists.C+(\"${INDIR}/top.root\",6)" >> dataDrivenEstimates.log

echo "root -b -q rescaleBoundaryHists.C+(${INDIR}/lostlep.root,13)"
root -b -q "rescaleBoundaryHists.C+(\"${INDIR}/lostlep.root\",13)" >> dataDrivenEstimates.log

echo "root -b -q rescaleBoundaryHists.C+(${INDIR}/background.root,3)"
root -b -q "rescaleBoundaryHists.C+(\"${INDIR}/background.root\",3)" >> dataDrivenEstimates.log

echo "root -b -q rescaleBoundaryHists.C+(${INDIR}/qcdplusgjet.root,2)"
root -b -q "rescaleBoundaryHists.C+(\"${INDIR}/qcdplusgjet.root\",2)" >> dataDrivenEstimates.log

# make the data driven background files
echo "root -b -q lostlepMaker.C+(${INDIR},${LOSTLEPFILE})"
root -b -q "lostlepMaker.C+(\"${INDIR}\",\"${LOSTLEPFILE}\")" >> dataDrivenEstimates.log
echo "root -b -q ZinvMakerMC.C+(${INDIR})"
root -b -q "ZinvMakerMC.C+(\"${INDIR}\")" >> dataDrivenEstimates.log

echo "root -b -q purity.C+(${INDIR})"
root -b -q "purity.C+(\"${INDIR}\",\"${GJETFILE}\")" >> dataDrivenEstimates.log


echo "done"


