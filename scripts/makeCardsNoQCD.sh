declare -a Samples=(T1tttt T1qqqq T1bbbb T2bb T2tt T2qq)
#declare -a Samples=(T1qqqq)

if [ "$#" -ne 1 ]; then
    return 1
fi

dir=$1

for sample in ${Samples[@]}; do
    command="nohup python cardMakerNoQCD.py $sample ../MT2Looper/output/RegionTest${dir} cards_RegionTest${dir}_${sample} True False &> logs/${dir}_${sample}.txt &"
    echo $command
    eval $command
done