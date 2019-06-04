#!/bin/bash

tag=$1

#declare -a Samples=(T1qqqq T2qq T2bt)
#declare -a Samples=(T1qqqq T2bt)
declare -a Samples=(T1qqqq)
#declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
declare -a ctaus=(1 5 10 10val 30 50 50val 70 100 150 200 200val 300 500 1000)

#rm -rf output_unmerged/2017and2018_${tag}

#cp -r output_unmerged/2017_${tag} output_unmerged/2017and2018_${tag}

#out_1718=output_unmerged/2017and2018_${tag}/signal
#mkdir -p $out_1718

#for SAMPLE in ${Samples[@]}; do
#    for ctau in ${ctaus[@]}; do
#	hadd -f ${out_1718}/${SAMPLE}_${ctau}.root output_unmerged/2017_${tag}/signal/${SAMPLE}_${ctau}.root output_unmerged/2018_${tag}/signal/${SAMPLE}_${ctau}.root &
#	hadd -f ${out_1718}/${SAMPLE}_${ctau}_ISR.root output_unmerged/2017_${tag}/signal/${SAMPLE}_${ctau}_ISR.root output_unmerged/2018_${tag}/signal/${SAMPLE}_${ctau}_ISR.root &
#	hadd -f ${out_1718}/${SAMPLE}_${ctau}_GENMET.root output_unmerged/2017_${tag}/signal/${SAMPLE}_${ctau}_GENMET.root output_unmerged/2018_${tag}/signal/${SAMPLE}_${ctau}_GENMET.root
#    done
#done

for SAMPLE in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do
	command="nohup nice -n 10 python genmet_and_isr.py ${SAMPLE}_${ctau} ${tag} 2016 >& /dev/null &"    
	eval $command
	command="nohup nice -n 10 python genmet_and_isr.py ${SAMPLE}_${ctau} ${tag} 2017 >& /dev/null &"
#	eval $command
	command="nohup nice -n 10 python genmet_and_isr.py ${SAMPLE}_${ctau} ${tag} 2018 >& /dev/null &"    
#    eval $command
	command="nohup nice -n 10 python genmet_and_isr.py ${SAMPLE}_${ctau} ${tag} 2017and2018 >& /dev/null &"
	eval $command
    done
done
