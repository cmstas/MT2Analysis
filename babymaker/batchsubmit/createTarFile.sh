#! /bin/bash

mkdir -p job_input
cd ..
cp -r *.so ScanChain.* jetCorrections jsons btagsf lepsf data processBaby rebal batchsubmit/job_input
cd batchsubmit/job_input
rm input.tar*
tar -hcf input.tar *
gzip input.tar
cd ..
