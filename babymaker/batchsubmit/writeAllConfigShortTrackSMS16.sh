#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

TAG="ShortTrackScans_2016_Apr28"

indir=/hadoop/cms/store/group/snt/run2_mc2016_94x/

for directory in $indir/*LLChipm*; do
    model=${directory#$indir/}
    shortmodel=${model%_MINIAODSIM_CMS4_V10-02-08}
    suffix=${model#${shortmodel}_MINIAODSIM_CMS4_}
    echo $suffix
    echo $shortmodel
    command="./writeConfig.sh $directory ${TAG}_${suffix}/${shortmodel}"
    echo $command
    eval $command
done
