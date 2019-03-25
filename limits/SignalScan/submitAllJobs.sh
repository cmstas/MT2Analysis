#!/bin/bash

MODEL=$1
DATE=$2
INDIR=$3
COMBINED=$4

# OUTPUTDIR="/hadoop/cms/store/user/$USER/combine/${MODE}/${MODEL}_${DATE}"

# rm -r $OUTPUTDIR/*

while read i
do
  echo $i
  if [[ $i == -* ]]; then
      continue # skip lines starting with -
  fi
  if [ -f /hadoop/cms/store/user/bemarsh/combine/limits/${MODEL}_${DATE}/limit_$i.root ]; then
      continue
  fi
  ./submit.sh $i ${DATE} ${MODEL} ${COMBINED}
done < $INDIR/$MODEL/points.txt
