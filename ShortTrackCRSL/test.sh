#!/bin/bash

make all

INDIR=/nfs-6/userdata/dpgilber/MT2_ST_Friends
# bitwise: <doLep><doMT2><doDphiDiff><doHTMET><doHT>
# if any bits are high, do Nj > 0

declare -a SELECTIONS=(10000)
declare -a Samples=(ttdl)

for SELECTION in ${SELECTIONS[@]}; do
    for SAMPLE in ${Samples[@]}; do
	echo "./ShortTrackCRSL.exe ${INDIR} ${SAMPLE} ${SELECTION}"
	eval "./ShortTrackCRSL.exe ${INDIR} ${SAMPLE} ${SELECTION}"
    done
done

