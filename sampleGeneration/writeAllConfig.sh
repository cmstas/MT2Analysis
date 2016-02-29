#!/bin/bash

#
# script for submitting pLHE jobs
#

TAG="softLep"


#
# LHE directories and mass points
#

#./writeConfig.sh /hadoop/cms/store/user/mderdzinski/mcProduction/test/ ${TAG}_T5qqqqWWmodified fragmentTemplate.py
./writeConfig.sh /hadoop/cms/store/user/gzevi/LHE/T5qqqqWWmodified/T5qqqqWWmodifiedA/ ${TAG}_T5qqqqWWmodifiedA fragmentTemplate.py

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
