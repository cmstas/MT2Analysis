#!/bin/bash

#
# script for submitting pLHE jobs
#

TAG="softLep"


#
# LHE directories and mass points
#

#./writeConfig_pLHE.sh /hadoop/cms/store/user/gzevi/LHE/T5qqqqWW/T5qqqqWWA/ ${TAG}_T5qqqqWWA
./writeConfig_pLHE.sh /hadoop/cms/store/user/gzevi/LHE/T5qqqqWWmodified/T5qqqqWWmodifiedA/ ${TAG}_T5qqqqWWmodified

# --- write submit script ---
mkdir -p configs_pLHE_${TAG}

mv condor_${TAG}*.cmd configs_pLHE_${TAG}
echo "#!/bin/bash" > submitAll_pLHE.sh
echo "voms-proxy-init -voms cms -valid 240:00" >> submitAll_pLHE.sh
for file in configs_pLHE_${TAG}/*.cmd
do 
    echo "condor_submit ${file}" >> submitAll_pLHE.sh
done
chmod +x submitAll_pLHE.sh
echo "[writeAllConfig] wrote submit script submitAll_pLHE.sh"
