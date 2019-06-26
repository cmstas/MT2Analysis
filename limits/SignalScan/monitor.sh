#! /bin/bash

# run in a screen session to monitor/resubmit limit jobs

TAG=V00-10-15_FullRunII
OUTFILE=~/public_html/mt2/limits/progress.txt

while [ 1 == 1 ]; do
    condor_rm -constraint "JobStartDate<=`date +%s`-3600*24*2.5"
    python checkJobs.py
    # python checkJobs.py 1

    sleep 30m

done
