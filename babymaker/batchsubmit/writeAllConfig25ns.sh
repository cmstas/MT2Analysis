#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

TAG="V00-01-02_25ns_softmus"

#
# DATA
#
#

# TTBAR
#

#
# ALTERNATIVE TTBAR

#
# W+JETS
#


./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/V07-04-03/ ${TAG}_wjets_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/V07-04-03/ ${TAG}_wjets_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/V07-04-03/ ${TAG}_wjets_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/V07-04-03/ ${TAG}_wjets_ht600toInf
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/V07-04-07/ ${TAG}_wjets_ht600to800
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/V07-04-03/ ${TAG}_wjets_ht800to1200 
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/V07-04-03/ ${TAG}_wjets_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/V07-04-07/ ${TAG}_wjets_ht2500toInf


#
# SINGLE TOP
#

#
# ALTERNATIVE SINGLE TOP
#


#
# DY+JETS
#

#
# GAMMA + JETS
#


#
# Z INVISIBLE
#

#
# DIBOSON
#

#
# TRIBOSON
#

#
# TTV
#


#
# QCD
#


#
# SIGNAL
#



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
