#!/bin/bash

run=$1

for FILE in `ls condor/${run}/*.cmd`; do
echo "condor_submit ${FILE}"
condor_submit $FILE
done