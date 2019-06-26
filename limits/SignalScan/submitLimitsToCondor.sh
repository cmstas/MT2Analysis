#!/bin/bash

#wrapper script for submitting combine jobs to condor
#Define MODEL, DATE, INDIR, and COMBINED

#the model name
# MODEL="T1tttt"
# MODEL="T1bbbb"
# MODEL="T1qqqq"
# MODEL="T2tt"
# MODEL="T2bb"
# MODEL="T2qq"
# MODEL="T2cc"
# MODEL="T2bW"
# MODEL="T2bt"
# MODEL=T5qqqqWW
# MODEL=T5qqqqVV
# MODEL=TprimeTprime_2xZ
#MODEL="T2-4bd_scan"
# MODEL="T1qqqq_10"
# MODEL="T1qqqq_50"
# MODEL="T1qqqq_200"
# MODEL="T2bt_10"
# MODEL="T2bt_50"
# MODEL="T2bt_200"
# MODEL="T2qq_10"
# MODEL="T2qq_50"
# MODEL="T2qq_200"
# MODEL="rpvMonoPhi"
MODEL="extraT2tt"

#the suffix
DATE="V00-10-18_FullRunII"
# DATE="V00-10-16_FullRunII_newT2ttBinning"

#directory where cards and points_MODEL.txt can be found
INDIR="/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_${DATE}"

#bool, true if cards are already combined
COMBINED=0

source do_tar.sh ${MODEL} ${DATE} ${INDIR} ${COMBINED}

source submitAllJobs.sh ${MODEL} ${DATE} ${INDIR} ${COMBINED}
