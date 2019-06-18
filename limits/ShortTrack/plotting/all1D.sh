#!/bin/bash

tag=$1

declare -a Samples=(T1qqqq)
for sample in ${Samples[@]}; do
    eval "nohup python make_1d.py ${sample} ${tag} 2500 1500 &> logs/${sample}_2500_1500.log &"
#    eval "nohup python make_1d.py ${sample} ${tag} 2400 1500 &> logs/${sample}_2400_1500.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 2000 1000 &> logs/${sample}_2000_1000.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1600 1575 &> logs/${sample}_1600_1575.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1800 200  &> logs/${sample}_1800_200.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 2400 150  &> logs/${sample}_2400_150.log &"
done

#declare -a Samples=(T2qq T2qq_1fold)
declare -a Samples=(T2qq)
for sample in ${Samples[@]}; do
    eval "nohup python make_1d.py ${sample} ${tag} 2000 1000 &> logs/${sample}_2000_1000.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1800 200  &> logs/${sample}_1800_200.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1300 1275  &> logs/${sample}_1300_1275.log &"
done

declare -a Samples=(T2bt)
for sample in ${Samples[@]}; do
    eval "nohup python make_1d.py ${sample} ${tag} 1300 700 &> logs/${sample}_1300_700.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1400 100  &> logs/${sample}_1400_100.log &"
    eval "nohup python make_1d.py ${sample} ${tag} 1100 1000  &> logs/${sample}_1100_1000.log &"
done
