declare -a Samples=(T1tttt T1qqqq T1bbbb T2bb T2tt T2qq)

if [ "$#" -ne 1 ]; then
    return 1
fi

dir=$1

THISDIR=`pwd`/combine

pushd combine/Set${dir}

for sample in ${Samples[@]}; do    
    pushd $sample
    masspoints=`ls`
    echo $sample >> ${THISDIR}/limit_${dir}.txt
    for masspoint in ${masspoints[@]}; do
	point=${masspoint%/}
	pushd $point
	echo $point >> ${THISDIR}/limit_${dir}.txt
	command="grep \"50.0%\" limit.txt >> ${THISDIR}/limit_${dir}.txt"
	echo $command
	eval $command
	popd
    done
    echo "" >> ${THISDIR}/limit_${dir}.txt
    popd
done

popd

cat ${THISDIR}/limit_${dir}.txt