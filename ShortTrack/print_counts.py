# Make nice tables and plots of results

import sys
import os

from printing_functions import *

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
printTables = False
full_unblind = True
doMC = True
doBenchmarks = False
doMonojet = False
makePullPlots = False
makePostFitPlots = False
makeSSRplots = False
format = "pdf"
colorTables = False

if len(sys.argv) < 2: 
    print "Which tag?"
    exit()

tag = sys.argv[1]
tabledir = "region_tables/{}".format(tag)
plotdir = "region_plots/{}".format(tag)

SetPlotDir(plotdir)
SetTableDir(tabledir)
SetTag(tag)
SetColorTables(colorTables)
SetFormat(format)

os.system("mkdir -p {}".format(tabledir))
os.system("mkdir -p {}".format(plotdir))

#fs_regions = [cat + " " + nj + " " + pt for cat in ["P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
#fs_regions += ["L 23","L 4"]
#output = open("{0}/fshorts_data_{1}.tex".format(tabledir,tag),"w")
#printHeader(output)
#start17vs18FshortTableData(output)
#for fs_region in fs_regions:
#    output.write(get17vs18FSLineData(fs_region))
#printFooter(output)
#output.close()

#M17f,eM17f,sM17f=getFshorts(m17)
#M18f,eM18f,sM18f=getFshorts(m18)
#output = open("{0}/fshorts_mc_{1}.tex".format(tabledir,tag),"w")
#printHeader(output)
#start17vs18FshortTableMC(output)
#for fs_region in fs_regions:
#    output.write(get17vs18FSLineMC(fs_region))
#printFooter(output)
#output.close()

#print ("Done 17 vs 18 fshort")
#exit(1)

d1718=ROOT.TFile.Open("output_merged/data_2017and2018_{}.root".format(tag))
d18=ROOT.TFile.Open("output_merged/data_2018_{}.root".format(tag))
d17=ROOT.TFile.Open("output_merged/data_2017_{}.root".format(tag))
d16=ROOT.TFile.Open("output_merged/data_2016_{}.root".format(tag))

if doMC:
    m1718=ROOT.TFile.Open("output_merged/mc_2017and2018_{}.root".format(tag))
    m17=ROOT.TFile.Open("output_merged/mc_2017_{}.root".format(tag))
    m18=ROOT.TFile.Open("output_merged/mc_2018_{}.root".format(tag))
    m16=ROOT.TFile.Open("output_merged/mc_2016_{}.root".format(tag))

    if doBenchmarks:
        sig1718 = {}
        sig1718[(1800,1400,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1400.root".format(tag))
        sig1718[(1800,1600,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1600.root".format(tag))
        sig1718[(1800,1700,10)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_10cm_1800-1700.root".format(tag))
        sig1718[(1800,1400,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1400.root".format(tag))
        sig1718[(1800,1600,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1600.root".format(tag))
        sig1718[(1800,1700,90)]=ROOT.TFile("output_unmerged/2017_{0}/signal/fastsim_90cm_1800-1700.root".format(tag))
        
        siglimits = {}
        siglimits[(1800,1400,10)] = 0.56
        siglimits[(1800,1600,10)] = 1.3
        siglimits[(1800,1700,10)] = 3.3
        siglimits[(1800,1400,90)] = 0.058
        siglimits[(1800,1600,90)] = 0.14
        siglimits[(1800,1700,90)] = 0.28

D18f,eD18f,sD18f=getFshorts(d18)
D17f,eD17f,sD17f=getFshorts(d17)
D1718f,eD1718f,sD1718f=getFshorts(d1718)
D16f,eD16f,sD16f=getFshorts(d16)
if doMC:
    M1718f,eM1718f,sM1718f=getFshorts(m1718)
    M16f,eM16f,sM16f=getFshorts(m16)
    M17f,eM17f,sM17f=getFshorts(m17)
    M18f,eM18f,sM18f=getFshorts(m18)

if doBenchmarks:
    # Mglu, Mlsp, ctau, limit mu
    signal_points=[(1800,1400,10),(1800,1600,10),(1800,1700,10),(1800,1400,90),(1800,1600,90),(1800,1700,90)]
    signal_points_10=[(1800,1400,10),(1800,1600,10),(1800,1700,10)]
    signal_points_90=[(1800,1400,90),(1800,1600,90),(1800,1700,90)]

D1718,eD1718,sD1718=getCounts(d1718,D1718f,sD1718f)
D17,eD17,sD17=getCounts(d17,D17f,sD17f)
D18,eD18,sD18=getCounts(d18,D18f,sD18f)
D16,eD16,sD16=getCounts(d16,D16f,sD16f)
if doMC:
    M1718,eM1718,sM1718=getCounts(m1718,M1718f,sM1718f,True)
    M17,eM17,sM17=getCounts(m17,M17f,sM17f,True)
    M18,eM18,sM18=getCounts(m18,M18f,sM18f,True)
    M16,eM16,sM16=getCounts(m16,M16f,sM16f,True)

if doBenchmarks:
    S1718 = {}
    eS1718 = {}
    for sp in signal_points:
        S1718[sp],eS1718[sp],dummy_systs = getCounts(sig1718[sp],{},{},True,True) # sig doesn't care about fshort, just feed dummies
        sig1718[sp].Close()

#d18.Close()
#d17.Close()
d1718.Close()
d16.Close()
if doMC:
    m1718.Close()
    m16.Close()


#############
# Make output
#############

# Kinematic order (Old)
#regionsNoL16VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
#regionsNoL16SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ]
#regionsNoL1718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ]
#regionsNoL1718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3","P4","M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"] ]
#regionsLVR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["VR"]]
#regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]

#regionsNicelyOrderedSR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","P3","P4","M"]  for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]] + regionsLSR
#regionsNicelyOrderedVR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","P3","P4","M"]  for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]] + regionsLVR

# Correlation order (New)
regionsNoL16VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsNoL16SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsNoL1718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P3","P4","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"] ]
regionsNoL1718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3","P4","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"] ]
regionsLVR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["VR"]]
regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]

regionsNicelyOrderedSR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","P3","P4","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"] ] + regionsLSR
regionsNicelyOrderedVR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P","P3","P4","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]] + regionsLVR


allVRnoL = regionsNoL16VR + regionsNoL1718VR
allSRnoL = regionsNoL16SR + regionsNoL1718SR

allVR1718 = regionsNoL1718VR+regionsLVR
allVR16 = regionsNoL16VR+regionsLVR

allSR1718 = regionsNoL1718SR+regionsLSR
allSR16 = regionsNoL16SR+regionsLSR

allVR = allVR16 + allVR1718
allSR = allSR16 + allSR1718

regions_monojetL = ["L " + kin + " " + reg for kin in ["1LM","1H"] for reg in ["SR"]]
regions_monojet = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P","P3","P4","M"] for pt in ["lo","hi"] for kin in ["1L","1M","1H"] ]
regions_monojet += regions_monojetL

regions_monojetVR = [ region.replace("SR","VR") for region in regions_monojet ]

allregions = allVR + allSR


rescale16 = 1/4.0 if not full_unblind else 1.0
rescale1718 = 1/5.0 if not full_unblind else 1.0

# SSRs, 2017-18
regionsPVR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsP3VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P3"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsP4VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P4"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsMVR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["M"]  for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsLVR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["VR"]]

svr_16 = [regionsPVR, regionsMVR, regionsLVR]
svr_16_names = ["P SVR","M SVR","L SVR"]
svr_1718 = [regionsP3VR, regionsP4VR, regionsMVR, regionsLVR]
svr_1718_names = ["P3 SVR","P4 SVR","M SVR","L SVR"]

regionsPSR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsP3SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsP4SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P4"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsMSR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["M"]  for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"]]
regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]

ssr_16 = [regionsPSR, regionsMSR, regionsLSR]
ssr_16_names = ["P SSR","M SSR","L SSR"]
ssr_1718 = [regionsP3SR, regionsP4SR, regionsMSR, regionsLSR]
ssr_1718_names = ["P3 SSR","P4 SSR","M SSR","L SSR"]

#for region in allVR:
#    print region, D16[region+" pre"],eD16[region+" pre"],sD16[region+" fs"],sD16[region+" nc"]

#print getAsymmetricErrors(0)
#print getAsymmetricErrors(1)
#print getAsymmetricErrors(2)
#print getAsymmetricErrors(3)
#print getAsymmetricErrors(20)


#makePullPlot(allVR1718,D1718,eD1718,sD1718,"2017-18 DATA VR")
#makePullPlot(allVR16,D16,eD16,sD16,"2016 DATA VR")
#makePullPlot(allSR1718,D1718,eD1718,sD1718,"2017-18 DATA SR",rescale1718)
#makePullPlot(allSR16,D16,eD16,sD16,"2016 DATA SR",rescale16)

#print "Made pull plots"
#exit(1)

makePlotRaw(allVR1718,D1718,eD1718,sD1718,D1718f,"2017-18 DATA VR",doPullPlot=False)
makePlotRaw(allVR16,D16,eD16,sD16,D16f,"2016 DATA VR",doPullPlot=False)
makePlotRaw(allSR1718,D1718,eD1718,sD1718,D1718f,"2017-18 DATA SR",rescale1718,doPullPlot=False)
makePlotRaw(allSR16,D16,eD16,sD16,D16f,"2016 DATA SR",rescale16,doPullPlot=False)

if makePullPlots:
    makePlotRaw(allVR1718,D1718,eD1718,sD1718,D1718f,"2017-18 DATA VR",doPullPlot=True)
    makePlotRaw(allVR16,D16,eD16,sD16,D16f,"2016 DATA VR",doPullPlot=True)
    makePlotRaw(allSR1718,D1718,eD1718,sD1718,D1718f,"2017-18 DATA SR",rescale1718,doPullPlot=True)
    makePlotRaw(allSR16,D16,eD16,sD16,D16f,"2016 DATA SR",rescale16,doPullPlot=True)

    # VR pull without NC syst
    makePlotRaw(allVR1718,D1718,eD1718,sD1718,D1718f,"2017-18 DATA VR NoNC",combineSysts=False,doPullPlot=True,inclNCsyst=False)
    makePlotRaw(allVR16,D16,eD16,sD16,D16f,"2016 DATA VR NoNC",combineSysts=False,doPullPlot=True,inclNCsyst=False)

if makePostFitPlots:
    fpostfit=ROOT.TFile.Open("../scripts/fits_approval_T1qqqq_10_Data_SR/fitDiagnostics_fit_CRonly_result.root".format(tag))
    if fpostfit == None:
        print "Couldn't find postfit file"
        print "Run scripts/getShortTrackFits.sh, or produced manually using something like combine -M FitDiagnostics -d combined.txt -n _fit_CRonly_result --saveShapes --saveWithUncertainties --setParameters mask_ch1=1 (with correctly named channels; see scripts/combineDir.sh)"
        exit(1)
    D16p,eD16p=getCountsPostfit(fpostfit,"2016")
    D1718p,eD1718p=getCountsPostfit(fpostfit,"2017and2018")

#    f_pf = ROOT.TFile.Open("../scripts/fits_{}_10cm_1800-1400_Data_SR/fitDiagnostics_fit_result.root".format(tag))
#    if f_pf == None:
#        print "Couldn't find postfit signal counts for 10cm 1800-1400. Use combine -M FitDiagnostics -d combined.txt -n _fit_10cm_1800-1400 --saveShapes --saveWithUncertainties --setParameters"
#    S16p,eS16p = getSignalCountsPostfit(f_pf,"2016")
#    S1718p,eS1718p = getSignalCountsPostfit(f_pf,"2017and2018")
#    f_pf.Close()
#    makePlotPostfitSvsBG(allSR1718,D1718,S1718p,eS1718p,"2017-18 DATA SR 10-1800-1400",rescale1718,doPullPlot=False)
#    makePlotPostfitSvsBG(allSR16,D16,S16p,eS16p,"2016 DATA SR 10-1800-1400",rescale16,doPullPlot=False)
#    f_pf = ROOT.TFile.Open("../scripts/fits_{}_90cm_1800-1600_Data_SR/fitDiagnostics_fit_result.root".format(tag))
#    if f_pf == None:
#        print "Couldn't find postfit signal counts for 90cm 1800-1600. Use combine -M FitDiagnostics -d combined.txt -n _fit_90cm_1800-1600 --saveShapes --saveWithUncertainties --setParameters"
#    S16p,eS16p = getSignalCountsPostfit(f_pf,"2016")
#    S1718p,eS1718p = getSignalCountsPostfit(f_pf,"2017and2018")
#    f_pf.Close()
#    makePlotPostfitSvsBG(allSR1718,D1718,S1718p,eS1718p,"2017-18 DATA SR 90-1800-1600",rescale1718,doPullPlot=False)
#    makePlotPostfitSvsBG(allSR16,D16,S16p,eS16p,"2016 DATA SR 90-1800-1600",rescale16,doPullPlot=False)

    # Pull plots not implemented yet
    # Only makes sense to do these for SR
    makePlotPostfit(allSR1718,D1718,eD1718,sD1718,D1718f,D1718p,eD1718p,"2017-18 DATA SR",rescale1718)
    makePlotPostfit(allSR16,D16,eD16,sD16,D16f,D16p,eD16p,"2016 DATA SR",rescale16)

    makePlotPostfit(allSR1718,D1718,eD1718,sD1718,D1718f,D1718p,eD1718p,"2017-18 DATA SR",rescale1718)
    makePlotPostfit(allSR16,D16,eD16,sD16,D16f,D16p,eD16p,"2016 DATA SR",rescale16)

    makePlotPostfitOnly(allSR1718,D1718,D1718p,eD1718p,"2017-18 DATA SR",rescale1718,doPullPlot=False)
    makePlotPostfitOnly(allSR16,D16,D16p,eD16p,"2016 DATA SR",rescale16,doPullPlot=False)

    makePlotPostfitOnly(allSR1718,D1718,D1718p,eD1718p,"2017-18 DATA SR",rescale1718,doPullPlot=False)
    makePlotPostfitOnly(allSR16,D16,D16p,eD16p,"2016 DATA SR",rescale16,doPullPlot=False)

    if makePullPlots:
        makePlotPostfitOnly(allSR1718,D1718,D1718p,eD1718p,"2017-18 DATA SR",rescale1718,doPullPlot=True)
        makePlotPostfitOnly(allSR16,D16,D16p,eD16p,"2016 DATA SR",rescale16,doPullPlot=True)

        makePlotPostfitOnly(allSR1718,D1718,D1718p,eD1718p,"2017-18 DATA SR",rescale1718,doPullPlot=True)
        makePlotPostfitOnly(allSR16,D16,D16p,eD16p,"2016 DATA SR",rescale16,doPullPlot=True)

if makeSSRplots:
    fcorr = ROOT.TFile.Open("../scripts/fits_approval_T1qqqq_10_Data_SR/fitDiagnostics_covariance.root")
    if fcorr == None:
        print "Couldn't find a background-only covariance file. Make it using combine -M FitDiagnostics -t -1 --expectSignal 0 --rMin -1 [COMBINED DATACARD] --forceRecreateNLL  --saveWithUncertainties --saveOverallShapes --numToysForShapes 200"
    CORR = getCorr(fcorr)
    makePlotSSRsCorr(svr_1718,D1718,eD1718,sD1718,CORR,"2017-18 DATA SVR",svr_1718_names)
    makePlotSSRsCorr(svr_16,D16,eD16,sD16,CORR,"2016 DATA SVR",svr_16_names)
    makePlotSSRsCorr(ssr_1718,D1718,eD1718,sD1718,CORR,"2017-18 DATA SSR",ssr_1718_names,rescale1718)
    makePlotSSRsCorr(ssr_16,D16,eD16,sD16,CORR,"2016 DATA SSR",ssr_16_names,rescale16)

    makePlotSSRsCorr(svr_1718,D1718,eD1718,sD1718,CORR,"2017-18 DATA SVR",svr_1718_names,doFullCovariance=False)
    makePlotSSRsCorr(svr_16,D16,eD16,sD16,CORR,"2016 DATA SVR",svr_16_names,doFullCovariance=False)
    makePlotSSRsCorr(ssr_1718,D1718,eD1718,sD1718,CORR,"2017-18 DATA SSR",ssr_1718_names,rescale1718,doFullCovariance=False)
    makePlotSSRsCorr(ssr_16,D16,eD16,sD16,CORR,"2016 DATA SSR",ssr_16_names,rescale16,doFullCovariance=False)

if doMC:
    makePlotRaw(allVR1718,M1718,eM1718,sM1718,M1718f,"2017-18 MC VR",doPullPlot=False)
    makePlotRaw(allVR16,M16,eM16,sM16,M16f,"2016 MC VR",doPullPlot=False)
    makePlotRaw(allSR1718,M1718,eM1718,sM1718,M1718f,"2017-18 MC SR",doPullPlot=False)
    makePlotRaw(allSR16,M16,eM16,sM16,M16f,"2016 MC SR",doPullPlot=False)

if doBenchmarks:
    for sp in signal_points:
        m1=sp[0]
        m2=sp[1]
        ct=sp[2]
        makeSignalPlot(allSR1718,D1718,eD1718,sD1718,[S1718[sp]],[eS1718[sp]],1+(59.97/41.97),"2017-18 ({}, {}) GeV, {} cm".format(m1,m2,ct),["({}, {}) GeV".format(m1,m2)],[ROOT.kGreen],rescale1718)
        makeSignalPlot(allSR16,D16,eD16,sD16,[S1718[sp]],[eS1718[sp]],35.9/41.97,"2016 ({}, {}) GeV, {} cm".format(m1,m2,ct),["({}, {}) GeV".format(m1,m2)],[ROOT.kGreen],rescale16)

        list_of_vals_10 = [S1718[sp] for sp in signal_points_10]
        list_of_errs_10 = [eS1718[sp] for sp in signal_points_10]
        list_of_vals_90 = [S1718[sp] for sp in signal_points_90]
        list_of_errs_90 = [eS1718[sp] for sp in signal_points_90]
        list_of_tags = ["(1800, {}) GeV".format(m2) for m2 in [1400,1600,1700]]
        colors = [ROOT.kMagenta,ROOT.kYellow+1,ROOT.kGreen+2]
        makeSignalPlot(allSR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_errs_10,1+(59.97/41.97),"2017-18 10 cm",list_of_tags,colors,rescale1718)
        makeSignalPlot(allSR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_errs_90,1+(59.97/41.97),"2017-18 90 cm",list_of_tags,colors,rescale1718)
        makeSignalPlot(allSR16,D16,eD16,sD16,list_of_vals_10,list_of_errs_10,35.9/41.97,"2016 10 cm",list_of_tags,colors,rescale16)
        makeSignalPlot(allSR16,D16,eD16,sD16,list_of_vals_90,list_of_errs_90,35.9/41.97,"2016 90 cm",list_of_tags,colors,rescale16)

fshort_regions_16 = [cat + " " + nj + " " + pt for cat in ["P","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fshort_regions_16 += [ "L 23", "L 4" ]

fshort_regions_1718 = [cat + " " + nj + " " + pt for cat in ["P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fshort_regions_1718 += [ "L 23", "L 4" ]

if doMC:
    makePlotFshort(fshort_regions_16,D16f,eD16f,sD16f,M16f,eM16f,sM16f,"2016")
    makePlotFshort(fshort_regions_1718,D1718f,eD1718f,sD1718f,M1718f,eM1718f,sM1718f,"2017-2018")

if printTables:
    output = open("{0}/regions_stc_data_VR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineDataSTC(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_SR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineDataSTC(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16,rescale1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_VR_combinedSyst.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_SR_combinedSyst.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16,rescale1718))
    printFooter(output)
    output.close()

    if doMonojet:
        output = open("{0}/regions_stc_data_monojetSR_combinedSyst.tex".format(tabledir),"w")
        printHeader(output)
        startMergedRegionTableDataSTC(output)
        for region in regions_monojet:
            output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
        printFooter(output)
        output.close()

        output = open("{0}/regions_stc_data_monojetVR_combinedSyst.tex".format(tabledir),"w")
        printHeader(output)
        startMergedRegionTableDataSTC(output)
        for region in regions_monojetVR:
            output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
        printFooter(output)
        output.close()

    if doMC and doBenchmarks:
#        output = open("{0}/counts_data_SR.tex".format(tabledir),"w")
#        printHeader(output)
#        startMergedCountsTable(output)
#        for region in regionsNicelyOrderedSR:
#            output.write(getMergedCountsLine(region))
#        printFooter(output)
#        output.close()

        output = open("{0}/counts_data_2016_SR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedCountsTable(output,"2016")
        for region in allSR16:
            output.write(getMergedCountsLine(region,"2016",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

        output = open("{0}/counts_data_2017and2018_SR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedCountsTable(output,"2017-18")
        for region in allSR1718:
            output.write(getMergedCountsLine(region,"2017and2018",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

#        output = open("{0}/counts_data_VR.tex".format(tabledir),"w")
#        printHeader(output)
#        startMergedCountsTable(output)
#        for region in regionsNicelyOrderedVR:
#            output.write(getMergedCountsLine(region))
#        printFooter(output)
#        output.close()

        output = open("{0}/counts_data_2016_VR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedCountsTable(output,"2016")
        for region in allVR16:
            output.write(getMergedCountsLine(region,"2016",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

        output = open("{0}/counts_data_2017and2018_VR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedCountsTable(output,"2017-18")
        for region in allVR1718:
            output.write(getMergedCountsLine(region,"2017and2018",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

    output = open("{0}/regions_data_VR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineData(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


    output = open("{0}/regions_data_SR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineData(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_data_VR_combinedSyst.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineData_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


    output = open("{0}/regions_data_SR_combinedSyst.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineData_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


if doMC and printTables:
    output = open("{0}/regions_mc_VR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_VR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_mc_SR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_SR.tex".format(tabledir),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    if doMonojet:
        output = open("{0}/regions_stc_mc_monojetSR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedRegionTableMCSTC(output)
        for region in regions_monojet:
            output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
        printFooter(output)
        output.close()

        output = open("{0}/regions_stc_mc_monojetVR.tex".format(tabledir),"w")
        printHeader(output)
        startMergedRegionTableMCSTC(output)
        for region in regions_monojetVR:
            output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
        printFooter(output)
        output.close()

fs_regions = [cat + " " + nj + " " + pt for cat in ["P","P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fs_regions += ["L 23","L 4"]

output = open("{0}/fshorts_data.tex".format(tabledir),"w")
printHeader(output)
startMergedFshortTableData(output)
for fs_region in fs_regions:
    output.write(getMergedFSLineData(fs_region,D16f,eD16f,sD16f,D1718f,eD1718f,sD1718f))
printFooter(output)
output.close()

if doMC:
    output = open("{0}/fshorts_mc.tex".format(tabledir),"w")
    printHeader(output)
    startMergedFshortTableMC(output)
    for fs_region in fs_regions:
        output.write(getMergedFSLineMC(fs_region,M16f,eM16f,sM16f,M1718f,eM1718f,sM1718f))
    printFooter(output)
    output.close()


print "Done test; still need to fix makePlotSigErr and MRvsSR table"
exit(1)

output = open("{0}/MRvsSR.tex".format(tabledir),"w")
printHeader(output)
startMRvsSRTable(output)
for region in regionsNicelyOrderedSR:
    output.write(getMRvsSRLine(region))
printFooter(output)
output.close()


if doBenchmarks:
    limits_10 = [siglimits[sp] for sp in signal_points_10]
    limits_90 = [siglimits[sp] for sp in signal_points_90]
    makePlotSigErr(allVR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_tags,colors,limits_10,1+(59.97/41.97),"2017-18 VR 10 cm")
    makePlotSigErr(allVR16,D16,eD16,sD16,list_of_vals_10,list_of_tags,colors,limits_10,35.9/41.97,"2016 VR 10 cm")
    makePlotSigErr(allVR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_tags,colors,limits_90,1+(59.97/41.97),"2017-18 VR 90 cm")
    makePlotSigErr(allVR16,D16,eD16,sD16,list_of_vals_90,list_of_tags,colors,limits_90,35.9/41.97,"2016 VR 90 cm")
    
    makePlotSigErr(allSR1718,D1718,eD1718,sD1718,list_of_vals_10,list_of_tags,colors,limits_10,1+(59.97/41.97),"2017-18 SR 10 cm")
    makePlotSigErr(allSR16,D16,eD16,sD16,list_of_vals_10,list_of_tags,colors,limits_10,35.9/41.97,"2016 SR 10 cm")
    makePlotSigErr(allSR1718,D1718,eD1718,sD1718,list_of_vals_90,list_of_tags,colors,limits_90,1+(59.97/41.97),"2017-18 SR 90 cm")
    makePlotSigErr(allSR16,D16,eD16,sD16,list_of_vals_90,list_of_tags,colors,limits_90,35.9/41.97,"2016 SR 90 cm")

print "Done"


# 2017 vs 2018 proportionality check
if False:
    regionsP31718MR = [cat + " " + kin + " " + reg + " " + pt for reg in ["MR"] for cat in ["P3"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsP41718MR = [cat + " " + kin + " " + reg + " " + pt for reg in ["MR"] for cat in ["P4"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsM1718MR = [cat + " " + kin + " " + reg + " " + pt for reg in ["MR"] for cat in ["M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsLMR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["MR"]]

    MRP3 = [0,0]
    MRP4 = [0,0]
    MRM = [0,0]
    MRL = [0,0]

    for region in regionsP31718MR:
        MRP3[0] += D17[region+" STC"]
        MRP3[1] += D18[region+" STC"]

    for region in regionsP41718MR:
        MRP4[0] += D17[region+" STC"]
        MRP4[1] += D18[region+" STC"]

    for region in regionsM1718MR:
        MRM[0] += D17[region+" STC"]
        MRM[1] += D18[region+" STC"]

    for region in regionsLMR:
        MRL[0] += D17[region+" STC"]
        MRL[1] += D18[region+" STC"]

    MR_total=[ MRP3[0] + MRP4[0] + MRM[0] + MRL[0], MRP3[1] + MRP4[1] + MRM[1] + MRL[1] ]

    regionsP31718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P3"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsP41718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["P4"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsM1718VR = [cat + " " + kin + " " + reg + " " + pt for reg in ["VR"] for cat in ["M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsLVR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["VR"]]

    VRP3 = [0,0]
    VRP4 = [0,0]
    VRM = [0,0]
    VRL = [0,0]

    for region in regionsP31718VR:
        VRP3[0] += D17[region+" STC"]
        VRP3[1] += D18[region+" STC"]

    for region in regionsP41718VR:
        VRP4[0] += D17[region+" STC"]
        VRP4[1] += D18[region+" STC"]

    for region in regionsM1718VR:
        VRM[0] += D17[region+" STC"]
        VRM[1] += D18[region+" STC"]

    for region in regionsLVR:
        VRL[0] += D17[region+" STC"]
        VRL[1] += D18[region+" STC"]

    VR_total=[ VRP3[0] + VRP4[0] + VRM[0] + VRL[0], VRP3[1] + VRP4[1] + VRM[1] + VRL[1] ]

    regionsP31718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsP41718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P4"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsM1718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["M"] for kin in ["LL","LM","LH","HL","HM","HH"] for pt in ["lo","hi"]]
    regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]

    SRP3 = [0,0]
    SRP4 = [0,0]
    SRM = [0,0]
    SRL = [0,0]

    for region in regionsP31718SR:
        SRP3[0] += D17[region+" STC"]
        SRP3[1] += D18[region+" STC"]

    for region in regionsP41718SR:
        SRP4[0] += D17[region+" STC"]
        SRP4[1] += D18[region+" STC"]

    for region in regionsM1718SR:
        SRM[0] += D17[region+" STC"]
        SRM[1] += D18[region+" STC"]

    for region in regionsLSR:
        SRL[0] += D17[region+" STC"]
        SRL[1] += D18[region+" STC"]

    SR_total=[ SRP3[0] + SRP4[0] + SRM[0] + SRL[0], SRP3[1] + SRP4[1] + SRM[1] + SRL[1] ]

    print "2017 lumi    :","{:.0f}%".format(41.97/100.8*100)
    print " "
    print "MR 2017 total:","{:.0f}% +".format(MR_total[0]/(MR_total[0]+MR_total[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MR_total[0])[0]/MR_total[0])**2 + (getAsymmetricErrors(MR_total[0]+MR_total[1])[0]/(MR_total[0]+MR_total[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MR_total[0])[1]/MR_total[0])**2 + (getAsymmetricErrors(MR_total[0]+MR_total[1])[1]/(MR_total[0]+MR_total[1]))**2))
    print "MR 2017 P3   :","{:.0f}% +".format(MRP3[0]/(MRP3[0]+MRP3[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRP3[0])[0]/MRP3[0])**2 + (getAsymmetricErrors(MRP3[0]+MRP3[1])[0]/(MRP3[0]+MRP3[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRP3[0])[1]/MRP3[0])**2 + (getAsymmetricErrors(MRP3[0]+MRP3[1])[1]/(MRP3[0]+MRP3[1]))**2))
    print "MR 2017 P4   :","{:.0f}% +".format(MRP4[0]/(MRP4[0]+MRP4[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRP4[0])[0]/MRP4[0])**2 + (getAsymmetricErrors(MRP4[0]+MRP4[1])[0]/(MRP4[0]+MRP4[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRP4[0])[1]/MRP4[0])**2 + (getAsymmetricErrors(MRP4[0]+MRP4[1])[1]/(MRP4[0]+MRP4[1]))**2))
    print "MR 2017 M    :","{:.0f}% +".format(MRM[0]/(MRM[0]+MRM[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRM[0])[0]/MRM[0])**2 + (getAsymmetricErrors(MRM[0]+MRM[1])[0]/(MRM[0]+MRM[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRM[0])[1]/MRM[0])**2 + (getAsymmetricErrors(MRM[0]+MRM[1])[1]/(MRM[0]+MRM[1]))**2))
    print "MR 2017 L    :","{:.0f}% +".format(MRL[0]/(MRL[0]+MRL[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRL[0])[0]/MRL[0])**2 + (getAsymmetricErrors(MRL[0]+MRL[1])[0]/(MRL[0]+MRL[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRL[0])[1]/MRL[0])**2 + (getAsymmetricErrors(MRL[0]+MRL[1])[1]/(MRL[0]+MRL[1]))**2))
    print " "
    print "VR 2017 total:","{:.0f}% +".format(VR_total[0]/(VR_total[0]+VR_total[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VR_total[0])[0]/VR_total[0])**2 + (getAsymmetricErrors(VR_total[0]+VR_total[1])[0]/(VR_total[0]+VR_total[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VR_total[0])[1]/VR_total[0])**2 + (getAsymmetricErrors(VR_total[0]+VR_total[1])[1]/(VR_total[0]+VR_total[1]))**2))
    print "VR 2017 P3   :","{:.0f}% +".format(VRP3[0]/(VRP3[0]+VRP3[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRP3[0])[0]/VRP3[0])**2 + (getAsymmetricErrors(VRP3[0]+VRP3[1])[0]/(VRP3[0]+VRP3[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRP3[0])[1]/VRP3[0])**2 + (getAsymmetricErrors(VRP3[0]+VRP3[1])[1]/(VRP3[0]+VRP3[1]))**2))
    print "VR 2017 P4   :","{:.0f}% +".format(VRP4[0]/(VRP4[0]+VRP4[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRP4[0])[0]/VRP4[0])**2 + (getAsymmetricErrors(VRP4[0]+VRP4[1])[0]/(VRP4[0]+VRP4[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRP4[0])[1]/VRP4[0])**2 + (getAsymmetricErrors(VRP4[0]+VRP4[1])[1]/(VRP4[0]+VRP4[1]))**2))
    print "VR 2017 M    :","{:.0f}% +".format(VRM[0]/(VRM[0]+VRM[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRM[0])[0]/VRM[0])**2 + (getAsymmetricErrors(VRM[0]+VRM[1])[0]/(VRM[0]+VRM[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRM[0])[1]/VRM[0])**2 + (getAsymmetricErrors(VRM[0]+VRM[1])[1]/(VRM[0]+VRM[1]))**2))
    print "VR 2017 L    :","{:.0f}% +".format(VRL[0]/(VRL[0]+VRL[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRL[0])[0]/VRL[0])**2 + (getAsymmetricErrors(VRL[0]+VRL[1])[0]/(VRL[0]+VRL[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRL[0])[1]/VRL[0])**2 + (getAsymmetricErrors(VRL[0]+VRL[1])[1]/(VRL[0]+VRL[1]))**2))
    print " "
    print "SR 2017 total:","{:.0f}% +".format(SR_total[0]/(SR_total[0]+SR_total[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(SR_total[0])[0]/SR_total[0])**2 + (getAsymmetricErrors(SR_total[0]+SR_total[1])[0]/(SR_total[0]+SR_total[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(SR_total[0])[1]/SR_total[0])**2 + (getAsymmetricErrors(SR_total[0]+SR_total[1])[1]/(SR_total[0]+SR_total[1]))**2))
    print "SR 2017 P3   :","{:.0f}% +".format(SRP3[0]/(SRP3[0]+SRP3[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(SRP3[0])[0]/SRP3[0])**2 + (getAsymmetricErrors(SRP3[0]+SRP3[1])[0]/(SRP3[0]+SRP3[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(SRP3[0])[1]/SRP3[0])**2 + (getAsymmetricErrors(SRP3[0]+SRP3[1])[1]/(SRP3[0]+SRP3[1]))**2))
    print "SR 2017 P4   :","{:.0f}% +".format(SRP4[0]/(SRP4[0]+SRP4[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(SRP4[0])[0]/SRP4[0])**2 + (getAsymmetricErrors(SRP4[0]+SRP4[1])[0]/(SRP4[0]+SRP4[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(SRP4[0])[1]/SRP4[0])**2 + (getAsymmetricErrors(SRP4[0]+SRP4[1])[1]/(SRP4[0]+SRP4[1]))**2))
    print "SR 2017 M    :","{:.0f}% +".format(SRM[0]/(SRM[0]+SRM[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(SRM[0])[0]/SRM[0])**2 + (getAsymmetricErrors(SRM[0]+SRM[1])[0]/(SRM[0]+SRM[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(SRM[0])[1]/SRM[0])**2 + (getAsymmetricErrors(SRM[0]+SRM[1])[1]/(SRM[0]+SRM[1]))**2))
    print "SR 2017 L    :","{:.0f}% +".format(SRL[0]/(SRL[0]+SRL[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(SRL[0])[0]/SRL[0])**2 + (getAsymmetricErrors(SRL[0]+SRL[1])[0]/(SRL[0]+SRL[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(SRL[0])[1]/SRL[0])**2 + (getAsymmetricErrors(SRL[0]+SRL[1])[1]/(SRL[0]+SRL[1]))**2))
    print " "
    print "****"
    print " "
    print " Now STs "
    print " "
    for region in regionsP31718MR:
        MRP3[0] += D17[region+" obs"]
        MRP3[1] += D18[region+" obs"]

    for region in regionsP41718MR:
        MRP4[0] += D17[region+" obs"]
        MRP4[1] += D18[region+" obs"]

    for region in regionsM1718MR:
        MRM[0] += D17[region+" obs"]
        MRM[1] += D18[region+" obs"]

    for region in regionsLMR:
        MRL[0] += D17[region+" obs"]
        MRL[1] += D18[region+" obs"]

    for region in regionsP31718VR:
        VRP3[0] += D17[region+" obs"]
        VRP3[1] += D18[region+" obs"]

    for region in regionsP41718VR:
        VRP4[0] += D17[region+" obs"]
        VRP4[1] += D18[region+" obs"]

    for region in regionsM1718VR:
        VRM[0] += D17[region+" obs"]
        VRM[1] += D18[region+" obs"]

    for region in regionsLVR:
        VRL[0] += D17[region+" obs"]
        VRL[1] += D18[region+" obs"]

    print "MR 2017 total:","{:.0f}% +".format(MR_total[0]/(MR_total[0]+MR_total[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MR_total[0])[0]/MR_total[0])**2 + (getAsymmetricErrors(MR_total[0]+MR_total[1])[0]/(MR_total[0]+MR_total[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MR_total[0])[1]/MR_total[0])**2 + (getAsymmetricErrors(MR_total[0]+MR_total[1])[1]/(MR_total[0]+MR_total[1]))**2))
    print "MR 2017 P3   :","{:.0f}% +".format(MRP3[0]/(MRP3[0]+MRP3[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRP3[0])[0]/MRP3[0])**2 + (getAsymmetricErrors(MRP3[0]+MRP3[1])[0]/(MRP3[0]+MRP3[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRP3[0])[1]/MRP3[0])**2 + (getAsymmetricErrors(MRP3[0]+MRP3[1])[1]/(MRP3[0]+MRP3[1]))**2))
    print "MR 2017 P4   :","{:.0f}% +".format(MRP4[0]/(MRP4[0]+MRP4[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRP4[0])[0]/MRP4[0])**2 + (getAsymmetricErrors(MRP4[0]+MRP4[1])[0]/(MRP4[0]+MRP4[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRP4[0])[1]/MRP4[0])**2 + (getAsymmetricErrors(MRP4[0]+MRP4[1])[1]/(MRP4[0]+MRP4[1]))**2))
    print "MR 2017 M    :","{:.0f}% +".format(MRM[0]/(MRM[0]+MRM[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRM[0])[0]/MRM[0])**2 + (getAsymmetricErrors(MRM[0]+MRM[1])[0]/(MRM[0]+MRM[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRM[0])[1]/MRM[0])**2 + (getAsymmetricErrors(MRM[0]+MRM[1])[1]/(MRM[0]+MRM[1]))**2))
    print "MR 2017 L    :","{:.0f}% +".format(MRL[0]/(MRL[0]+MRL[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(MRL[0])[0]/MRL[0])**2 + (getAsymmetricErrors(MRL[0]+MRL[1])[0]/(MRL[0]+MRL[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(MRL[0])[1]/MRL[0])**2 + (getAsymmetricErrors(MRL[0]+MRL[1])[1]/(MRL[0]+MRL[1]))**2))
    print " "
    print "VR 2017 total:","{:.0f}% +".format(VR_total[0]/(VR_total[0]+VR_total[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VR_total[0])[0]/VR_total[0])**2 + (getAsymmetricErrors(VR_total[0]+VR_total[1])[0]/(VR_total[0]+VR_total[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VR_total[0])[1]/VR_total[0])**2 + (getAsymmetricErrors(VR_total[0]+VR_total[1])[1]/(VR_total[0]+VR_total[1]))**2))
    print "VR 2017 P3   :","{:.0f}% +".format(VRP3[0]/(VRP3[0]+VRP3[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRP3[0])[0]/VRP3[0])**2 + (getAsymmetricErrors(VRP3[0]+VRP3[1])[0]/(VRP3[0]+VRP3[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRP3[0])[1]/VRP3[0])**2 + (getAsymmetricErrors(VRP3[0]+VRP3[1])[1]/(VRP3[0]+VRP3[1]))**2))
    print "VR 2017 P4   :","{:.0f}% +".format(VRP4[0]/(VRP4[0]+VRP4[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRP4[0])[0]/VRP4[0])**2 + (getAsymmetricErrors(VRP4[0]+VRP4[1])[0]/(VRP4[0]+VRP4[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRP4[0])[1]/VRP4[0])**2 + (getAsymmetricErrors(VRP4[0]+VRP4[1])[1]/(VRP4[0]+VRP4[1]))**2))
    print "VR 2017 M    :","{:.0f}% +".format(VRM[0]/(VRM[0]+VRM[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRM[0])[0]/VRM[0])**2 + (getAsymmetricErrors(VRM[0]+VRM[1])[0]/(VRM[0]+VRM[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRM[0])[1]/VRM[0])**2 + (getAsymmetricErrors(VRM[0]+VRM[1])[1]/(VRM[0]+VRM[1]))**2))
    print "VR 2017 L    :","{:.0f}% +".format(VRL[0]/(VRL[0]+VRL[1])*100),"{:.0f}% -".format(100*sqrt((getAsymmetricErrors(VRL[0])[0]/VRL[0])**2 + (getAsymmetricErrors(VRL[0]+VRL[1])[0]/(VRL[0]+VRL[1]))**2)),"{:.0f}%".format(100*sqrt((getAsymmetricErrors(VRL[0])[1]/VRL[0])**2 + (getAsymmetricErrors(VRL[0]+VRL[1])[1]/(VRL[0]+VRL[1]))**2))
    print " "
    print "Done 17-18 MR/VR/SR compare"
    exit(1)

