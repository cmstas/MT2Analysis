#!bin/bash

declare -a Models=(T2qq_1300_850)
inputDir=$PWD/../CardMaker/cards_FullRunII_17MCfor18_ttbbWeights_v3/T2qq/T2qq_1300_850/

tag=RegionTesting

thisDir=$PWD

for model in ${Models[@]}
do
    copyDir=$thisDir'/ranking_'${model}'/'
    workDir=$thisDir/scratch/ranking_${model}    
    mkdir -p $workDir
    cd $workDir

	regionNum=0
	for datacard in $inputDir/*'datacard_'*${model}*
	do
	    if [[ $datacard != *'combine'* && $datacard != *'list'* && $datacard != *'partialCombination'* ]]
	    then
		region=${datacard##*datacard_}
		if [[ "$regionNum" = "0" ]]
		then
		    TopoRegions[regionNum]=${region%_m*}
		    listOfCards[regionNum]=`ls $inputDir/datacard_${TopoRegions[$regionNum]}*${model}*`
		    echo `combineCards.py -S ${listOfCards[$regionNum]} > datacard_${model}_${TopoRegions[$regionNum]}.txt`
		    echo `combine -M Asymptotic datacard_${model}_${TopoRegions[$regionNum]}.txt > log_${model}_${TopoRegions[$regionNum]}_${tag}.txt`
		    echo `rm -f datacard_${model}_${TopoRegions[$regionNum]}.txt higgsCombineTest.Asymptotic.mH120.root roostats-*`
		    regionNum=$(($regionNum+1))
		elif [[ "$regionNum" != "0" && "${region%_m*}" != "${TopoRegions[regionNum-1]}" ]]
		then
		    TopoRegions[regionNum]=${region%_m*}
		    listOfCards[regionNum]=`ls $inputDir/*${TopoRegions[$regionNum]}*${model}*`
		    echo `combineCards.py -S ${listOfCards[$regionNum]} > datacard_${model}_${TopoRegions[$regionNum]}.txt`
		    echo `combine -M Asymptotic datacard_${model}_${TopoRegions[$regionNum]}.txt > log_${model}_${TopoRegions[regionNum]}_${tag}.txt`
		    echo `rm -f datacard_${model}_${TopoRegions[$regionNum]}.txt higgsCombineTest.Asymptotic.mH120.root roostats-*`
		    regionNum=$(($regionNum+1))
		fi
	    fi
	done

	unset TopoRegions
	unset listOfCards
    
done


cd -