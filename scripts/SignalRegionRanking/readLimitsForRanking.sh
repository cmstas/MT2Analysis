#!bin/bash

Model=T1tttt_1200_800
tag=RegionTesting

thisDir=`pwd`
inputDir=$thisDir/../cards_WithSigContam/cards_DefaultWithQCD_T1tttt/

for i in $(ls $thisDir/scratch/ranking_${Model}/log_${Model}*${tag}.txt)
do
    if [[ $i ]]
    then
	region=${i##*log_${Model}_}
	region=${region%_${tag}.txt*}
	limit=$(grep "50.0%" $i | awk '{print $5}')
	obslimit=$(grep "Observed" $i | awk '{print $5}')
	if [[ $limit ]]
	then
            yield=$(grep rate $inputDir/datacard_${region}_m*_${Model}.txt | awk '{ sum+=$2} END {print sum}')
	    bkg=$(grep rate $inputDir/datacard_${region}_m*_${Model}.txt | awk '{ sum+=$3+$4+$5} END {print sum}')
	    echo $region $limit $obslimit $yield $bkg>> Ranking_${Model}.txt
	    
	    echo $region >> Ranking_${Model}_plusBG.txt
	    for d in $(ls -rt $inputDir/datacard_${region}_m*${Model}*)
	    do
		bin=${d##*${region}_}
		bin=${bin%_${Model}.txt}
		thisyield=$(grep rate $inputDir/datacard_${region}_${bin}_${Model}.txt | awk '{ sum=$2} END {print sum}')
		thisbkg=$(grep rate $inputDir/datacard_${region}_${bin}_${Model}.txt | awk '{ sum=$3+$4+$5} END {print sum}') 
		echo $bin '&' $thisyield '&' $thisbkg '\\'>>Ranking_${Model}_plusBG.txt
	    done

	fi
    fi
done
