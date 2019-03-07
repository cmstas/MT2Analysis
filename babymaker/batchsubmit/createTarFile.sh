#! /bin/bash

mkdir -p job_input
rm -r job_input/*
cd ..
cp -r *.so ScanChain.* jetCorrections jsons btagsf lepsf data processBaby rebal *.pcm batchsubmit/job_input
cd batchsubmit/job_input
tar -hcJf input.tar.xz *
# gzip input.tar
cd ..
