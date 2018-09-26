declare -a Samples=(T1tttt T1qqqq T1bbbb T2bb T2tt T2qq)
#declare -a Samples=(T1qqqq)

if [ "$#" -ne 1 ]; then
    return 1
fi

dir=$1

if [ $dir == "Default" ]; then
    SSR_start=20
elif [ $dir == "2" ]; then
    SSR_start=50
elif [ $dir == "4" ]; then
    SSR_start=50
elif [ $dir == "PJ" ]; then
    SSR_start=40
elif [ $dir == "5" ]; then
    SSR_start=50
elif [ $dir == "6" ]; then
    SSR_start=50
else
    echo "Assuming SSRs start at 50"
    SSR_start=50
fi

for sample in ${Samples[@]}; do
    command="nohup python cardMakerFalseQCD.py $sample ../MT2Looper/output/RegionTest${dir} cards_${dir}WithQCD_${sample} True False $SSR_start &> logs/${dir}_${sample}_WithQCD.txt &"
    echo $command
    eval $command
done