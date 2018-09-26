declare -a Samples=(T1tttt T1qqqq T1bbbb T2bb T2tt T2qq)
#declare -a Samples=(T1qqqq)

if [ "$#" -ne 1 ]; then
    return 1
fi

dir=$1

pushd combine

command="cp -r template Set${dir}"
echo $command
eval $command

pushd Set${dir}

for sample in ${Samples[@]}; do    
    pushd $sample
    masspoints=`ls`
    for masspoint in ${masspoints[@]}; do
	point=${masspoint%/}
	pushd $point
	command="combineCards.py ../../../../cards_RegionTest${dir}_${sample}/*${point}.txt > combined_${point}.txt"
	echo $command
	eval $command
	popd
    done
    popd
done

popd
popd