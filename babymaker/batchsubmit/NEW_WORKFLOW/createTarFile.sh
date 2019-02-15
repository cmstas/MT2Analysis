#! /bin/bash
DIR=`pwd`
INPDIR=job_input2
mkdir -p $INPDIR
rm -r $INPDIR/*
cd ../..
cp -r *.so ScanChain.* jetCorrections jsons btagsf lepsf data processBaby rebal $DIR/$INPDIR
cd $DIR/$INPDIR
cp -r ../sweeproot .
cp -r ../skim_bennettworkflow .
cp ../../mergeHadoopFiles.C .
tar -hcJf input.tar.xz *
tar -hcJf skiminput.tar.xz skim_bennettworkflow
# gzip input.tar
cd ..

echo "Remember to move from job_input2 to job_input!!!"