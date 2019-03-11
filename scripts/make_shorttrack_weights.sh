INDIR=/nfs-6/userdata/dpgilber/ShortTrackScans/
declare -a signals=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200)

for signal in ${signals[@]}; do
    root -b -q "make_nsig_weight_hists.C+(\"${INDIR}\",\"${signal}\")" &> ${signal}_weights.log &
done