#!/bin/bash

#
# script for submitting pLHE jobs
#

TAG="test"


#
# LHE directories and mass points
#

./writeConfig_AOD.sh /hadoop/cms/store/user/mderdzinski/mcProduction/test_T5qqqqWWA/LHE/test ${TAG}_T5qqqqWWA fragmentTemplate.py


# --- write submit script ---
mkdir -p configs_AOD_${TAG}

mv condor_${TAG}*.cmd configs_AOD_${TAG}
echo "#!/bin/bash" > submitAll_AOD.sh
echo "voms-proxy-init -voms cms -valid 240:00" >> submitAll_AOD.sh
for file in configs_AOD_${TAG}/*.cmd
do 
    echo "condor_submit ${file}" >> submitAll_AOD.sh
done
chmod +x submitAll_AOD.sh
echo "[writeAllConfig] wrote submit script submitAll_AOD.sh"
