#!/bin/bash

INDIR=output/skim_apr19/
THISDIR=`pwd`

if [ ! -d "$INDIR" ]; then
  echo "Input directory does not exist" 
fi

cd $INDIR
echo "hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root"
hadd -f data_Run2015CD.root data_Run2015C.root data_Run2015D.root > histAdd.log
cd $THISDIR

cd $INDIR
echo "hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root"
hadd -f top.root ttsl.root ttdl.root singletop.root ttw.root ttz.root tth.root ttg.root >> histAdd.log
cd $THISDIR

cd $INDIR
echo "hadd -f diboson.root ww.root wz.root zz.root "
hadd -f diboson.root ww.root wz.root zz.root >> histAdd.log
cd $THISDIR

#all backgrounds except diboson and dyjets
cd $INDIR
echo "hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root "
hadd -f allBkg_noDiboson.root top.root qcd_ht.root zinv_ht.root wjets_ht.root >> histAdd.log
cd $THISDIR

#all backgrounds
cd $INDIR
echo "hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  "
hadd -f allBkg.root allBkg_noDiboson.root dyjetsll.root diboson.root  >> histAdd.log
cd $THISDIR
