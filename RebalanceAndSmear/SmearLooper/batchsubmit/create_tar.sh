#!/bin/bash

suffix=$1

mkdir -p job_input_${1}
cp ../*.so job_input_${suffix}/
cp ../runLooper job_input_${suffix}/
cp ../../data/*.root job_input_${suffix}/
cp -r ../../../babymaker/jsons job_input_${suffix}/
cp -r ../njet_reweighting/ job_input_${suffix}/
cp ../HEM_smears.root job_input_${suffix}/
cp ../puWeight*.root job_input_${suffix}
THISDIR=$PWD
cd job_input_${suffix}
tar -czf input.tar.gz *.root *.so runLooper jsons njet_reweighting
cd ${THISDIR}

