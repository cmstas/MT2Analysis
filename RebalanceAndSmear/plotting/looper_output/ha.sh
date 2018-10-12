#! /bin/bash

declare -a tags=()

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_hybrid)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_hybrid)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_nonbHybrid_bStraight)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_noBResp)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_core25)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_tail50)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_tail100)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_mean4)
# tags+=(V00-10-01_31Mar2018_2016JRTs)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_core10)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_TG)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_Bennett_jetReweight)

# tags+=(V00-10-01_31Mar2018_2016JRTs_jetReweight)
# tags+=(V00-10-01_31Mar2018_80x_v2)
# tags+=(V00-10-01_31Mar2018_80x_v2_Bennett)
# tags+=(V00-10-01_31Mar2018_2016JRTs_try2)

# tags+=(V00-10-01_31Mar2018_usedByJason)
# tags+=(V00-10-01_31Mar2018_2016JRTs_withMonojet_v2)
# tags+=(V00-10-01_31Mar2018_usedByJason_withMonojet)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_withMonojet_v2)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_tail0p66)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_noJERsmear)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_fixMatching5_ecalDeadCell_tail0p66_noJERsmear)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_noJERsmear)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_jetReweight)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM1)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM2)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM2_v2)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_v2)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM2veto_pt20)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_vetoHEM_pt20)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM2veto_pt30)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_vetoHEM_pt30)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_HEM2veto_pt40)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_RecoBJet_noJERsmear_vetoHEM_pt40)

# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear_jetReweight
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs)
# tags+=(V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma)

# # tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs)
# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core10)
# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core25)
# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_tail50)
# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_tail100)
# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_mean04)

# tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear_v2)

tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma)
tags+=(V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma_core10)

# for TAG in ${tags[@]}; do
#     nohup nice -n 10 ./hadd_output.py /hadoop/cms/store/user/bemarsh/smearoutput/${TAG}/ &> logs/log_${TAG}.txt &
# done


for TAG in ${tags[@]}; do
    for samp in `ls ${TAG}`; do
        echo ${TAG}/${samp}
        rm -f ${TAG}/${samp}/*.root
        nohup nice -n 10 python merge.py ${TAG}/${samp} &> logs/log_merge_${TAG}_${samp}.txt &
    done
    # if [[ -d ${TAG}/qcd ]] ; then
    #     echo ${TAG}/qcd
    #     nohup nice -n 10 python merge.py ${TAG}/qcd &> logs/log_merge_${TAG}_qcd.txt &
    # fi
done
