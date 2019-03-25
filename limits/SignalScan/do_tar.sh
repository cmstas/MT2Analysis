#!/bin/bash

MODEL=$1
DATE=$2
INDIR=$3
COMBINED=$4

CARD_DIR="card_tarballs/cards_${MODEL}_${DATE}"

if [ -d $CARD_DIR ]
then
  rm -r $CARD_DIR
  mkdir -p $CARD_DIR
else
  mkdir -p $CARD_DIR
fi

pushd $INDIR/$MODEL

if [ -e log_tar.log ]
then
  rm log_tar.log
fi

while read i
do
  echo $i
  if [[ $i == -* ]]; then
      continue # skip lines starting with -
  fi
  pushd $i
  if [ $COMBINED == 1 ]
  then
      tar -czvf cards_$i.tar.gz "datacard_"*"${i}_combined.txt" >> log_tar.log 2>&1  #for cards already combined
  else
      tar -czvf cards_$i.tar.gz "datacard_"*"${i}.txt" >> log_tar.log 2>&1
  fi
  popd
done < $INDIR/$MODEL/points.txt

popd

pushd $CARD_DIR
cp $INDIR/$MODEL/*/cards_*.tar.gz .
popd
