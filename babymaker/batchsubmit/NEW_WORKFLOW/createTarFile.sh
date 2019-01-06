#! /bin/bash
DIR=`pwd`
mkdir -p job_input
rm -r job_input/*
cd ../..
cp -r *.so ScanChain.* jetCorrections jsons btagsf lepsf data processBaby rebal $DIR/job_input
cd $DIR/job_input
cp -r ../sweeproot .
cp -r ../skim_bennettworkflow .
cp ../mergeHadoopFiles.C .
tar -hcJf input.tar.xz *
# gzip input.tar
cd ..
