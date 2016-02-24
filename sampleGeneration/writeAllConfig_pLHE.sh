#!/bin/bash

#
# script for submitting pLHE jobs
#

TAG="test"


#
# LHE directories and mass points
#

./writeConfig_pLHE.sh /hadoop/cms/store/user/mderdzinski/mcProduction/T5qqqqWW/1025_775/ ${TAG}_T5qqqqWW_1025_775


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
