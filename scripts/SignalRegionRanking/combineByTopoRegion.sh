#!bin/bash

Models=(T1tttt_1200_800)
homeDir=..
inputDir=$homeDir/cards_WithSigContam/cards_DefaultWithQCD_T1tttt/

tag=RegionTesting

thisDir=$PWD
copyDir=$thisDir'/ranking_'${Model[0]}'/'

workDir=$thisDir/scratch/ranking_${Model[0]}

mkdir -p $workDir
cd $workDir

modelNum=0
while [ "x${Models[modelNum]}" != "x" ]
do
	model=${Models[$modelNum]}
	regionNum=0
	for datacard in $(ls $inputDir/*'datacard_'*${Model[$modelNum]}*)
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
    
     modelNum=$(( $modelNum + 1 ))
done
unset Model

cd -