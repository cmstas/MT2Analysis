#!/bin/bash

mkdir -p drpngs_$1

rm ../drHists_$1/ttsl.root

rm ../drHists_$1/total.root

hadd ../drHists_$1/total.root ../drHists_$1/*.root

hadd ../drHists_$1/ttsl.root ../drHists_$1/ttsl*.root

python ttbar.py $1

tar cfz drpngs_$1.tar drpngs_$1