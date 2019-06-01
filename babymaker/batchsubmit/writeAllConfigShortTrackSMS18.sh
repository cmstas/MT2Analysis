#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

TAG="ShortTrackScans_2018_decayZ"

indir=/hadoop/cms/store/group/snt/run2_mc2018/

#
# T2qq
#

for directory in $indir/*LLChipm*09; do
    model=${directory#$indir/}
    shortmodel=${model%_MINIAODSIM_CMS4_V10-02-09}
    suffix=${model#${shortmodel}_MINIAODSIM_CMS4_}
    echo $suffix
    echo $shortmodel
    command="./writeConfig.sh $directory ${TAG}_${suffix}/${shortmodel}"
    echo $command
    eval $command
done

exit

# --- write submit script ---
mkdir -p configs_${TAG}

mv condor_${TAG}*.cmd configs_${TAG}
echo "#!/bin/bash" > submitAll.sh
echo "voms-proxy-init -voms cms -valid 240:00" >> submitAll.sh
for file in configs_${TAG}/*.cmd
do 
    echo "condor_submit ${file}" >> submitAll.sh
done
chmod +x submitAll.sh
echo "[writeAllConfig] wrote submit script submitAll.sh"
