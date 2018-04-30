#!/bin/bash

outdir=$1_vs_$2_vs_$3
mkdir -p $outdir

python compare_factor.py -slicelen $1/pngs_HT_GV_EPV $2/pngs_HT_GV_EPV $3/pngs_HT_GV_EPV
mkdir -p ${outdir}/HT
mv *.png ${outdir}/HT

python compare_factor.py -slicelen $1/pngs_CRSL_GV_EPV $2/pngs_CRSL_GV_EPV $3/pngs_CRSL_GV_EPV
mkdir -p ${outdir}/CRSL
mv *.png ${outdir}/CRSL

python compare_factor.py -slicelen $1/pngs_SR_GV_EPV $2/pngs_SR_GV_EPV $3/pngs_SR_GV_EPV
mkdir -p ${outdir}/SR
mv *.png ${outdir}/SR