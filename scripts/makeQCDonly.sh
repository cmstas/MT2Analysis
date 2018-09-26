INDIR=/home/users/dpgilber/2017/MT2AnalysisRegions/MT2Looper/output/RegionTestDefault

THISIDR=`pwd`

QCDFILE=qcd_ht
QCDESTIMATE=./inputs/qcdEstimateData
QCDMONOJET=./inputs/qcdEstimateMonojet


echo "root -b -q qcdRphiMaker.C+(${INDIR},${QCDFILE},${QCDESTIMATE},${QCDMONOJET})"
root -b -q "qcdRphiMaker.C+(\"${INDIR}\",\"${QCDFILE}\",\"${QCDESTIMATE}\",\"${QCDMONOJET}\")"
