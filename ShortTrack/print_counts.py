# Make nice tables and plots of results

import sys
import os

from printing_functions import *

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
printTables = True
full_unblind = True
doMC = True
makePullPlots = False
makePostFitPlots = True
makeSSRplots = True
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

if makePostFitPlots:
    fpostfit=ROOT.TFile.Open("../scripts/fits_{}_10cm_1800-1400_Data_SR/fitDiagnostics_fit_CRonly_result.root".format(tag))
    if fpostfit == None:
        print "Couldn't find postfit file"
        print "Run scripts/getShortTrackFits.sh, or produced manually using something like combine -M FitDiagnostics -d combined.txt -n _fit_CRonly_result --saveShapes --saveWithUncertainties --setParameters mask_ch1=1 (with correctly named channels; see scripts/combineDir.sh)"
        exit(1)
    D16p,eD16p=getCountsPostfit(fpostfit,"2016")
    D1718p,eD1718p=getCountsPostfit(fpostfit,"2017and2018")

    f_pf = ROOT.TFile.Open("../scripts/fits_{}_10cm_1800-1400_Data_SR/fitDiagnostics_fit_result.root".format(tag))
    if f_pf == None:
        print "Couldn't find postfit signal counts for 10cm 1800-1400. Use combine -M FitDiagnostics -d combined.txt -n _fit_10cm_1800-1400 --saveShapes --saveWithUncertainties --setParameters"
    S16p,eS16p = getSignalCountsPostfit(f_pf,"2016")
    S1718p,eS1718p = getSignalCountsPostfit(f_pf,"2017and2018")
    f_pf.Close()
    makePlotPostfitSvsBG(allSR1718,D1718,S1718p,eS1718p,"2017-18 DATA SR 10-1800-1400",rescale1718,doPullPlot=False)
    makePlotPostfitSvsBG(allSR16,D16,S16p,eS16p,"2016 DATA SR 10-1800-1400",rescale16,doPullPlot=False)
    f_pf = ROOT.TFile.Open("../scripts/fits_{}_90cm_1800-1600_Data_SR/fitDiagnostics_fit_result.root".format(tag))
    if f_pf == None:
        print "Couldn't find postfit signal counts for 90cm 1800-1600. Use combine -M FitDiagnostics -d combined.txt -n _fit_90cm_1800-1600 --saveShapes --saveWithUncertainties --setParameters"
    S16p,eS16p = getSignalCountsPostfit(f_pf,"2016")
    S1718p,eS1718p = getSignalCountsPostfit(f_pf,"2017and2018")
    f_pf.Close()
    makePlotPostfitSvsBG(allSR1718,D1718,S1718p,eS1718p,"2017-18 DATA SR 90-1800-1600",rescale1718,doPullPlot=False)
    makePlotPostfitSvsBG(allSR16,D16,S16p,eS16p,"2016 DATA SR 90-1800-1600",rescale16,doPullPlot=False)

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
    fcorr = ROOT.TFile.Open("../scripts/fits_preapproval_10cm_1800-1400_Data_SR/fitDiagnostics_covariance.root")
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

if doMC:
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
    makePlotFshort(fshort_regions_1718,D1718f,eD1718f,sD1718f,M1718f,eM1718f,sM1718f,"2017-18")

if printTables:
    output = open("{0}/regions_stc_data_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineDataSTC(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineDataSTC(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16,rescale1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_{1}_VR_combinedSyst.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_data_{1}_SR_combinedSyst.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableDataSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineDataSTC_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718,rescale16,rescale1718))
    printFooter(output)
    output.close()

    if doMC:
#        output = open("{0}/counts_data_{1}_SR.tex".format(tabledir,tag),"w")
#        printHeader(output)
#        startMergedCountsTable(output)
#        for region in regionsNicelyOrderedSR:
#            output.write(getMergedCountsLine(region))
#        printFooter(output)
#        output.close()

        output = open("{0}/counts_data_2016_{1}_SR.tex".format(tabledir,tag),"w")
        printHeader(output)
        startMergedCountsTable(output,"2016")
        for region in allSR16:
            output.write(getMergedCountsLine(region,"2016",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

        output = open("{0}/counts_data_2017and2018_{1}_SR.tex".format(tabledir,tag),"w")
        printHeader(output)
        startMergedCountsTable(output,"2017-18")
        for region in allSR1718:
            output.write(getMergedCountsLine(region,"2017and2018",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

#        output = open("{0}/counts_data_{1}_VR.tex".format(tabledir,tag),"w")
#        printHeader(output)
#        startMergedCountsTable(output)
#        for region in regionsNicelyOrderedVR:
#            output.write(getMergedCountsLine(region))
#        printFooter(output)
#        output.close()

        output = open("{0}/counts_data_2016_{1}_VR.tex".format(tabledir,tag),"w")
        printHeader(output)
        startMergedCountsTable(output,"2016")
        for region in allVR16:
            output.write(getMergedCountsLine(region,"2016",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

        output = open("{0}/counts_data_2017and2018_{1}_VR.tex".format(tabledir,tag),"w")
        printHeader(output)
        startMergedCountsTable(output,"2017-18")
        for region in allVR1718:
            output.write(getMergedCountsLine(region,"2017and2018",D16,eD16,sD16,D1718,eD1718,sD1718,S1718,eS1718))
        printFooter(output)
        output.close()

    output = open("{0}/regions_data_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineData(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


    output = open("{0}/regions_data_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineData(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_data_{1}_VR_combinedSyst.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineData_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


    output = open("{0}/regions_data_{1}_SR_combinedSyst.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableData(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineData_Combined(region,D16,eD16,sD16,D1718,eD1718,sD1718))
    printFooter(output)
    output.close()


if doMC and printTables:
    output = open("{0}/regions_mc_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_{1}_VR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedVR:
        output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_mc_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

    output = open("{0}/regions_stc_mc_{1}_SR.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedRegionTableMCSTC(output)
    for region in regionsNicelyOrderedSR:
        output.write(getMergedLineMCSTC(region,M16,eM16,sM16,M1718,eM1718,sM1718))
    printFooter(output)
    output.close()

fs_regions = [cat + " " + nj + " " + pt for cat in ["P","P3","P4","M"] for nj in ["23","4"] for pt in ["hi","lo"]]
fs_regions += ["L 23","L 4"]

output = open("{0}/fshorts_data_{1}.tex".format(tabledir,tag),"w")
printHeader(output)
startMergedFshortTableData(output)
for fs_region in fs_regions:
    output.write(getMergedFSLineData(fs_region,D16f,eD16f,sD16f,D1718f,eD1718f,sD1718f))
printFooter(output)
output.close()

if doMC:
    output = open("{0}/fshorts_mc_{1}.tex".format(tabledir,tag),"w")
    printHeader(output)
    startMergedFshortTableMC(output)
    for fs_region in fs_regions:
        output.write(getMergedFSLineMC(fs_region,M16f,eM16f,sM16f,M1718f,eM1718f,sM1718f))
    printFooter(output)
    output.close()


print "Done test; still need to fix makePlotSigErr and MRvsSR table"
exit(1)

output = open("{0}/MRvsSR_{1}.tex".format(tabledir,tag),"w")
printHeader(output)
startMRvsSRTable(output)
for region in regionsNicelyOrderedSR:
    output.write(getMRvsSRLine(region))
printFooter(output)
output.close()


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


# Deprecated functions

def getCountsOld(f):
    h_LL_MR_23 = f.Get("h_LL_MR_23")
    h_LM_MR_23 = f.Get("h_LM_MR_23")
    h_LLM_MR_23 = f.Get("h_LLM_MR_23")
    h_LH_MR_23 = f.Get("h_LH_MR_23")
    h_HL_MR_4 = f.Get("h_HL_MR_4")
    h_HLM_MR_4 = f.Get("h_HLM_MR_4")
    h_HM_MR_4 = f.Get("h_HM_MR_4")
    h_HH_MR_4 = f.Get("h_HH_MR_4")
    h_LL_MR_23_hi = f.Get("h_LL_MR_23_hi")
    h_LLM_MR_23_hi = f.Get("h_LLM_MR_23_hi")
    h_LM_MR_23_hi = f.Get("h_LM_MR_23_hi")
    h_LH_MR_23_hi = f.Get("h_LH_MR_23_hi")
    h_HL_MR_4_hi = f.Get("h_HL_MR_4_hi")
    h_HLM_MR_4_hi = f.Get("h_HLM_MR_4_hi")
    h_HM_MR_4_hi = f.Get("h_HM_MR_4_hi")
    h_HH_MR_4_hi = f.Get("h_HH_MR_4_hi")
    h_LL_MR_23_lo = f.Get("h_LL_MR_23_lo")
    h_LLM_MR_23_lo = f.Get("h_LLM_MR_23_lo")
    h_LM_MR_23_lo = f.Get("h_LM_MR_23_lo")
    h_LH_MR_23_lo = f.Get("h_LH_MR_23_lo")
    h_HL_MR_4_lo = f.Get("h_HL_MR_4_lo")
    h_HLM_MR_4_lo = f.Get("h_HLM_MR_4_lo")
    h_HM_MR_4_lo = f.Get("h_HM_MR_4_lo")
    h_HH_MR_4_lo = f.Get("h_HH_MR_4_lo")

    h_LL_VR_23 = f.Get("h_LL_VR_23")
    h_LM_VR_23 = f.Get("h_LM_VR_23")
    h_LLM_VR_23 = f.Get("h_LLM_VR_23")
    h_LH_VR_23 = f.Get("h_LH_VR_23")
    h_HL_VR_4 = f.Get("h_HL_VR_4")
    h_HLM_VR_4 = f.Get("h_HLM_VR_4")
    h_HM_VR_4 = f.Get("h_HM_VR_4")
    h_HH_VR_4 = f.Get("h_HH_VR_4")
    h_LL_VR_23_hi = f.Get("h_LL_VR_23_hi")
    h_LLM_VR_23_hi = f.Get("h_LLM_VR_23_hi")
    h_LM_VR_23_hi = f.Get("h_LM_VR_23_hi")
    h_LH_VR_23_hi = f.Get("h_LH_VR_23_hi")
    h_HL_VR_4_hi = f.Get("h_HL_VR_4_hi")
    h_HLM_VR_4_hi = f.Get("h_HLM_VR_4_hi")
    h_HM_VR_4_hi = f.Get("h_HM_VR_4_hi")
    h_HH_VR_4_hi = f.Get("h_HH_VR_4_hi")
    h_LL_VR_23_lo = f.Get("h_LL_VR_23_lo")
    h_LLM_VR_23_lo = f.Get("h_LLM_VR_23_lo")
    h_LM_VR_23_lo = f.Get("h_LM_VR_23_lo")
    h_LH_VR_23_lo = f.Get("h_LH_VR_23_lo")
    h_HL_VR_4_lo = f.Get("h_HL_VR_4_lo")
    h_HLM_VR_4_lo = f.Get("h_HLM_VR_4_lo")
    h_HM_VR_4_lo = f.Get("h_HM_VR_4_lo")
    h_HH_VR_4_lo = f.Get("h_HH_VR_4_lo")

    h_LL_SR_23 = f.Get("h_LL_SR_23")
    h_LLM_SR_23 = f.Get("h_LLM_SR_23")
    h_LM_SR_23 = f.Get("h_LM_SR_23")
    h_LH_SR_23 = f.Get("h_LH_SR_23")
    h_HL_SR_4 = f.Get("h_HL_SR_4")
    h_HLM_SR_4 = f.Get("h_HLM_SR_4")
    h_HM_SR_4 = f.Get("h_HM_SR_4")
    h_HH_SR_4 = f.Get("h_HH_SR_4")
    h_LL_SR_23_hi = f.Get("h_LL_SR_23_hi")
    h_LLM_SR_23_hi = f.Get("h_LLM_SR_23_hi")
    h_LM_SR_23_hi = f.Get("h_LM_SR_23_hi")
    h_LH_SR_23_hi = f.Get("h_LH_SR_23_hi")
    h_HL_SR_4_hi = f.Get("h_HL_SR_4_hi")
    h_HLM_SR_4_hi = f.Get("h_HLM_SR_4_hi")
    h_HM_SR_4_hi = f.Get("h_HM_SR_4_hi")
    h_HH_SR_4_hi = f.Get("h_HH_SR_4_hi")
    h_LL_SR_23_lo = f.Get("h_LL_SR_23_lo")
    h_LLM_SR_23_lo = f.Get("h_LLM_SR_23_lo")
    h_LM_SR_23_lo = f.Get("h_LM_SR_23_lo")
    h_LH_SR_23_lo = f.Get("h_LH_SR_23_lo")
    h_HL_SR_4_lo = f.Get("h_HL_SR_4_lo")
    h_HLM_SR_4_lo = f.Get("h_HLM_SR_4_lo")
    h_HM_SR_4_lo = f.Get("h_HM_SR_4_lo")
    h_HH_SR_4_lo = f.Get("h_HH_SR_4_lo")

    h_2STC_VR = f.Get("h_2STC_VR")
    h_2STC_SR = f.Get("h_2STC_SR")

    # Systs
    h_LL_VR_23_ns = f.Get("h_LL_VR_23_nonclosure_systematic")
    h_LM_VR_23_ns = f.Get("h_LM_VR_23_nonclosure_systematic")
    h_LLM_VR_23_ns = f.Get("h_LLM_VR_23_nonclosure_systematic")
    h_LH_VR_23_ns = f.Get("h_LH_VR_23_nonclosure_systematic")
    h_HL_VR_4_ns = f.Get("h_HL_VR_4_nonclosure_systematic")
    h_HLM_VR_4_ns = f.Get("h_HLM_VR_4_nonclosure_systematic")
    h_HM_VR_4_ns = f.Get("h_HM_VR_4_nonclosure_systematic")
    h_HH_VR_4_ns = f.Get("h_HH_VR_4_nonclosure_systematic")
    h_LL_VR_23_hi_ns = f.Get("h_LL_VR_23_hi_nonclosure_systematic")
    h_LLM_VR_23_hi_ns = f.Get("h_LLM_VR_23_hi_nonclosure_systematic")
    h_LM_VR_23_hi_ns = f.Get("h_LM_VR_23_hi_nonclosure_systematic")
    h_LH_VR_23_hi_ns = f.Get("h_LH_VR_23_hi_nonclosure_systematic")
    h_HL_VR_4_hi_ns = f.Get("h_HL_VR_4_hi_nonclosure_systematic")
    h_HLM_VR_4_hi_ns = f.Get("h_HLM_VR_4_hi_nonclosure_systematic")
    h_HM_VR_4_hi_ns = f.Get("h_HM_VR_4_hi_nonclosure_systematic")
    h_HH_VR_4_hi_ns = f.Get("h_HH_VR_4_hi_nonclosure_systematic")
    h_LL_VR_23_lo_ns = f.Get("h_LL_VR_23_lo_nonclosure_systematic")
    h_LLM_VR_23_lo_ns = f.Get("h_LLM_VR_23_lo_nonclosure_systematic")
    h_LM_VR_23_lo_ns = f.Get("h_LM_VR_23_lo_nonclosure_systematic")
    h_LH_VR_23_lo_ns = f.Get("h_LH_VR_23_lo_nonclosure_systematic")
    h_HL_VR_4_lo_ns = f.Get("h_HL_VR_4_lo_nonclosure_systematic")
    h_HLM_VR_4_lo_ns = f.Get("h_HLM_VR_4_lo_nonclosure_systematic")
    h_HM_VR_4_lo_ns = f.Get("h_HM_VR_4_lo_nonclosure_systematic")
    h_HH_VR_4_lo_ns = f.Get("h_HH_VR_4_lo_nonclosure_systematic")
    # VR hists are used for SR as well

    h_2STC_VR_ns = f.Get("h_2STC_VR_nonclosure_systematic")


    vals = {}
    errs = {}
    systs = {}

    # 2STC
    if h_2STC_VR != None:
        vals["2 VR STC"] = h_2STC_VR.GetBinContent(1,3)
        vals["2 VR pre"] = h_2STC_VR.GetBinContent(1,2)
        vals["2 VR obs"] = h_2STC_VR.GetBinContent(1,1)
        vals["2 SR STC"] = h_2STC_SR.GetBinContent(1,3)
        vals["2 SR pre"] = h_2STC_SR.GetBinContent(1,2)
        vals["2 SR obs"] = h_2STC_SR.GetBinContent(1,1)

        errs["2 VR STC"] = h_2STC_VR.GetBinError(1,3)
        errs["2 VR pre"] = h_2STC_VR.GetBinError(1,2)
        errs["2 VR obs"] = h_2STC_VR.GetBinError(1,1)
        errs["2 SR STC"] = h_2STC_SR.GetBinError(1,3)
        errs["2 SR pre"] = h_2STC_SR.GetBinError(1,2)
        errs["2 SR obs"] = h_2STC_SR.GetBinError(1,1)


    # MR vals
    vals["P LL MR STC"] = h_LL_MR_23.GetBinContent(1,3)
    vals["P LL MR pre"] = h_LL_MR_23.GetBinContent(1,2)
    vals["P LL MR obs"] = h_LL_MR_23.GetBinContent(1,1)
    vals["P3 LL MR STC"] = h_LL_MR_23.GetBinContent(2,3)
    vals["P3 LL MR pre"] = h_LL_MR_23.GetBinContent(2,2)
    vals["P3 LL MR obs"] = h_LL_MR_23.GetBinContent(2,1)
    vals["P4 LL MR STC"] = h_LL_MR_23.GetBinContent(3,3)
    vals["P4 LL MR pre"] = h_LL_MR_23.GetBinContent(3,2)
    vals["P4 LL MR obs"] = h_LL_MR_23.GetBinContent(3,1)
    vals["M LL MR STC"] = h_LL_MR_23.GetBinContent(4,3)
    vals["M LL MR pre"] = h_LL_MR_23.GetBinContent(4,2)
    vals["M LL MR obs"] = h_LL_MR_23.GetBinContent(4,1)
    vals["L LL MR STC"] = h_LL_MR_23.GetBinContent(5,3)
    vals["L LL MR pre"] = h_LL_MR_23.GetBinContent(5,2)
    vals["L LL MR obs"] = h_LL_MR_23.GetBinContent(5,1)

    vals["P LLM MR STC"] = h_LLM_MR_23.GetBinContent(1,3)
    vals["P LLM MR pre"] = h_LLM_MR_23.GetBinContent(1,2)
    vals["P LLM MR obs"] = h_LLM_MR_23.GetBinContent(1,1)
    vals["P3 LLM MR STC"] = h_LLM_MR_23.GetBinContent(2,3)
    vals["P3 LLM MR pre"] = h_LLM_MR_23.GetBinContent(2,2)
    vals["P3 LLM MR obs"] = h_LLM_MR_23.GetBinContent(2,1)
    vals["P4 LLM MR STC"] = h_LLM_MR_23.GetBinContent(3,3)
    vals["P4 LLM MR pre"] = h_LLM_MR_23.GetBinContent(3,2)
    vals["P4 LLM MR obs"] = h_LLM_MR_23.GetBinContent(3,1)
    vals["M LLM MR STC"] = h_LLM_MR_23.GetBinContent(4,3)
    vals["M LLM MR pre"] = h_LLM_MR_23.GetBinContent(4,2)
    vals["M LLM MR obs"] = h_LLM_MR_23.GetBinContent(4,1)
    vals["L LLM MR STC"] = h_LLM_MR_23.GetBinContent(5,3)
    vals["L LLM MR pre"] = h_LLM_MR_23.GetBinContent(5,2)
    vals["L LLM MR obs"] = h_LLM_MR_23.GetBinContent(5,1)

    vals["P LM MR STC"] = h_LM_MR_23.GetBinContent(1,3)
    vals["P LM MR pre"] = h_LM_MR_23.GetBinContent(1,2)
    vals["P LM MR obs"] = h_LM_MR_23.GetBinContent(1,1)
    vals["P3 LM MR STC"] = h_LM_MR_23.GetBinContent(2,3)
    vals["P3 LM MR pre"] = h_LM_MR_23.GetBinContent(2,2)
    vals["P3 LM MR obs"] = h_LM_MR_23.GetBinContent(2,1)
    vals["P4 LM MR STC"] = h_LM_MR_23.GetBinContent(3,3)
    vals["P4 LM MR pre"] = h_LM_MR_23.GetBinContent(3,2)
    vals["P4 LM MR obs"] = h_LM_MR_23.GetBinContent(3,1)
    vals["M LM MR STC"] = h_LM_MR_23.GetBinContent(4,3)
    vals["M LM MR pre"] = h_LM_MR_23.GetBinContent(4,2)
    vals["M LM MR obs"] = h_LM_MR_23.GetBinContent(4,1)
    vals["L LM MR STC"] = h_LM_MR_23.GetBinContent(5,3)
    vals["L LM MR pre"] = h_LM_MR_23.GetBinContent(5,2)
    vals["L LM MR obs"] = h_LM_MR_23.GetBinContent(5,1)

    vals["P LH MR STC"] = h_LH_MR_23.GetBinContent(1,3)
    vals["P LH MR pre"] = h_LH_MR_23.GetBinContent(1,2)
    vals["P LH MR obs"] = h_LH_MR_23.GetBinContent(1,1)
    vals["P3 LH MR STC"] = h_LH_MR_23.GetBinContent(2,3)
    vals["P3 LH MR pre"] = h_LH_MR_23.GetBinContent(2,2)
    vals["P3 LH MR obs"] = h_LH_MR_23.GetBinContent(2,1)
    vals["P4 LH MR STC"] = h_LH_MR_23.GetBinContent(3,3)
    vals["P4 LH MR pre"] = h_LH_MR_23.GetBinContent(3,2)
    vals["P4 LH MR obs"] = h_LH_MR_23.GetBinContent(3,1)
    vals["M LH MR STC"] = h_LH_MR_23.GetBinContent(4,3)
    vals["M LH MR pre"] = h_LH_MR_23.GetBinContent(4,2)
    vals["M LH MR obs"] = h_LH_MR_23.GetBinContent(4,1)
    vals["L LH MR STC"] = h_LH_MR_23.GetBinContent(5,3)
    vals["L LH MR pre"] = h_LH_MR_23.GetBinContent(5,2)
    vals["L LH MR obs"] = h_LH_MR_23.GetBinContent(5,1)

    vals["P HL MR STC"] = h_HL_MR_4.GetBinContent(1,3)
    vals["P HL MR pre"] = h_HL_MR_4.GetBinContent(1,2)
    vals["P HL MR obs"] = h_HL_MR_4.GetBinContent(1,1)
    vals["P3 HL MR STC"] = h_HL_MR_4.GetBinContent(2,3)
    vals["P3 HL MR pre"] = h_HL_MR_4.GetBinContent(2,2)
    vals["P3 HL MR obs"] = h_HL_MR_4.GetBinContent(2,1)
    vals["P4 HL MR STC"] = h_HL_MR_4.GetBinContent(3,3)
    vals["P4 HL MR pre"] = h_HL_MR_4.GetBinContent(3,2)
    vals["P4 HL MR obs"] = h_HL_MR_4.GetBinContent(3,1)
    vals["M HL MR STC"] = h_HL_MR_4.GetBinContent(4,3)
    vals["M HL MR pre"] = h_HL_MR_4.GetBinContent(4,2)
    vals["M HL MR obs"] = h_HL_MR_4.GetBinContent(4,1)
    vals["L HL MR STC"] = h_HL_MR_4.GetBinContent(5,3)
    vals["L HL MR pre"] = h_HL_MR_4.GetBinContent(5,2)
    vals["L HL MR obs"] = h_HL_MR_4.GetBinContent(5,1)

    vals["P HLM MR STC"] = h_HLM_MR_4.GetBinContent(1,3)
    vals["P HLM MR pre"] = h_HLM_MR_4.GetBinContent(1,2)
    vals["P HLM MR obs"] = h_HLM_MR_4.GetBinContent(1,1)
    vals["P3 HLM MR STC"] = h_HLM_MR_4.GetBinContent(2,3)
    vals["P3 HLM MR pre"] = h_HLM_MR_4.GetBinContent(2,2)
    vals["P3 HLM MR obs"] = h_HLM_MR_4.GetBinContent(2,1)
    vals["P4 HLM MR STC"] = h_HLM_MR_4.GetBinContent(3,3)
    vals["P4 HLM MR pre"] = h_HLM_MR_4.GetBinContent(3,2)
    vals["P4 HLM MR obs"] = h_HLM_MR_4.GetBinContent(3,1)
    vals["M HLM MR STC"] = h_HLM_MR_4.GetBinContent(4,3)
    vals["M HLM MR pre"] = h_HLM_MR_4.GetBinContent(4,2)
    vals["M HLM MR obs"] = h_HLM_MR_4.GetBinContent(4,1)
    vals["L HLM MR STC"] = h_HLM_MR_4.GetBinContent(5,3)
    vals["L HLM MR pre"] = h_HLM_MR_4.GetBinContent(5,2)
    vals["L HLM MR obs"] = h_HLM_MR_4.GetBinContent(5,1)

    vals["P HM MR STC"] = h_HM_MR_4.GetBinContent(1,3)
    vals["P HM MR pre"] = h_HM_MR_4.GetBinContent(1,2)
    vals["P HM MR obs"] = h_HM_MR_4.GetBinContent(1,1)
    vals["P3 HM MR STC"] = h_HM_MR_4.GetBinContent(2,3)
    vals["P3 HM MR pre"] = h_HM_MR_4.GetBinContent(2,2)
    vals["P3 HM MR obs"] = h_HM_MR_4.GetBinContent(2,1)
    vals["P4 HM MR STC"] = h_HM_MR_4.GetBinContent(3,3)
    vals["P4 HM MR pre"] = h_HM_MR_4.GetBinContent(3,2)
    vals["P4 HM MR obs"] = h_HM_MR_4.GetBinContent(3,1)
    vals["M HM MR STC"] = h_HM_MR_4.GetBinContent(4,3)
    vals["M HM MR pre"] = h_HM_MR_4.GetBinContent(4,2)
    vals["M HM MR obs"] = h_HM_MR_4.GetBinContent(4,1)
    vals["L HM MR STC"] = h_HM_MR_4.GetBinContent(5,3)
    vals["L HM MR pre"] = h_HM_MR_4.GetBinContent(5,2)
    vals["L HM MR obs"] = h_HM_MR_4.GetBinContent(5,1)

    vals["P HH MR STC"] = h_HH_MR_4.GetBinContent(1,3)
    vals["P HH MR pre"] = h_HH_MR_4.GetBinContent(1,2)
    vals["P HH MR obs"] = h_HH_MR_4.GetBinContent(1,1)
    vals["P3 HH MR STC"] = h_HH_MR_4.GetBinContent(2,3)
    vals["P3 HH MR pre"] = h_HH_MR_4.GetBinContent(2,2)
    vals["P3 HH MR obs"] = h_HH_MR_4.GetBinContent(2,1)
    vals["P4 HH MR STC"] = h_HH_MR_4.GetBinContent(3,3)
    vals["P4 HH MR pre"] = h_HH_MR_4.GetBinContent(3,2)
    vals["P4 HH MR obs"] = h_HH_MR_4.GetBinContent(3,1)
    vals["M HH MR STC"] = h_HH_MR_4.GetBinContent(4,3)
    vals["M HH MR pre"] = h_HH_MR_4.GetBinContent(4,2)
    vals["M HH MR obs"] = h_HH_MR_4.GetBinContent(4,1)
    vals["L HH MR STC"] = h_HH_MR_4.GetBinContent(5,3)
    vals["L HH MR pre"] = h_HH_MR_4.GetBinContent(5,2)
    vals["L HH MR obs"] = h_HH_MR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(1,3)
    vals["P LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(1,2)
    vals["P LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(1,1)
    vals["P3 LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(2,3)
    vals["P3 LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(2,2)
    vals["P3 LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(2,1)
    vals["P4 LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(3,3)
    vals["P4 LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(3,2)
    vals["P4 LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(3,1)
    vals["M LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(4,3)
    vals["M LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(4,2)
    vals["M LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(4,1)
    vals["L LL MR hi STC"] = h_LL_MR_23_hi.GetBinContent(5,3)
    vals["L LL MR hi pre"] = h_LL_MR_23_hi.GetBinContent(5,2)
    vals["L LL MR hi obs"] = h_LL_MR_23_hi.GetBinContent(5,1)

    vals["P LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(1,3)
    vals["P LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(1,2)
    vals["P LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(1,1)
    vals["P3 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(2,3)
    vals["P3 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(2,2)
    vals["P3 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(2,1)
    vals["P4 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(3,3)
    vals["P4 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(3,2)
    vals["P4 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(3,1)
    vals["M LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(4,3)
    vals["M LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(4,2)
    vals["M LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(4,1)
    vals["L LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinContent(5,3)
    vals["L LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinContent(5,2)
    vals["L LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinContent(5,1)

    vals["P LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(1,3)
    vals["P LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(1,2)
    vals["P LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(1,1)
    vals["P3 LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(2,3)
    vals["P3 LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(2,2)
    vals["P3 LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(2,1)
    vals["P4 LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(3,3)
    vals["P4 LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(3,2)
    vals["P4 LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(3,1)
    vals["M LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(4,3)
    vals["M LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(4,2)
    vals["M LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(4,1)
    vals["L LM MR hi STC"] = h_LM_MR_23_hi.GetBinContent(5,3)
    vals["L LM MR hi pre"] = h_LM_MR_23_hi.GetBinContent(5,2)
    vals["L LM MR hi obs"] = h_LM_MR_23_hi.GetBinContent(5,1)

    vals["P LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(1,3)
    vals["P LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(1,2)
    vals["P LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(1,1)
    vals["P3 LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(2,3)
    vals["P3 LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(2,2)
    vals["P3 LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(2,1)
    vals["P4 LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(3,3)
    vals["P4 LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(3,2)
    vals["P4 LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(3,1)
    vals["M LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(4,3)
    vals["M LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(4,2)
    vals["M LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(4,1)
    vals["L LH MR hi STC"] = h_LH_MR_23_hi.GetBinContent(5,3)
    vals["L LH MR hi pre"] = h_LH_MR_23_hi.GetBinContent(5,2)
    vals["L LH MR hi obs"] = h_LH_MR_23_hi.GetBinContent(5,1)

    vals["P HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(1,3)
    vals["P HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(1,2)
    vals["P HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(1,1)
    vals["P3 HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(2,3)
    vals["P3 HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(2,2)
    vals["P3 HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(2,1)
    vals["P4 HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(3,3)
    vals["P4 HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(3,2)
    vals["P4 HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(3,1)
    vals["M HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(4,3)
    vals["M HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(4,2)
    vals["M HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(4,1)
    vals["L HL MR hi STC"] = h_HL_MR_4_hi.GetBinContent(5,3)
    vals["L HL MR hi pre"] = h_HL_MR_4_hi.GetBinContent(5,2)
    vals["L HL MR hi obs"] = h_HL_MR_4_hi.GetBinContent(5,1)

    vals["P HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(1,3)
    vals["P HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(1,2)
    vals["P HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(1,1)
    vals["P3 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(2,3)
    vals["P3 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(2,2)
    vals["P3 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(2,1)
    vals["P4 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(3,3)
    vals["P4 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(3,2)
    vals["P4 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(3,1)
    vals["M HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(4,3)
    vals["M HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(4,2)
    vals["M HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(4,1)
    vals["L HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinContent(5,3)
    vals["L HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinContent(5,2)
    vals["L HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinContent(5,1)

    vals["P HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(1,3)
    vals["P HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(1,2)
    vals["P HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(1,1)
    vals["P3 HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(2,3)
    vals["P3 HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(2,2)
    vals["P3 HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(2,1)
    vals["P4 HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(3,3)
    vals["P4 HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(3,2)
    vals["P4 HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(3,1)
    vals["M HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(4,3)
    vals["M HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(4,2)
    vals["M HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(4,1)
    vals["L HM MR hi STC"] = h_HM_MR_4_hi.GetBinContent(5,3)
    vals["L HM MR hi pre"] = h_HM_MR_4_hi.GetBinContent(5,2)
    vals["L HM MR hi obs"] = h_HM_MR_4_hi.GetBinContent(5,1)

    vals["P HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(1,3)
    vals["P HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(1,2)
    vals["P HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(1,1)
    vals["P3 HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(2,3)
    vals["P3 HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(2,2)
    vals["P3 HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(2,1)
    vals["P4 HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(3,3)
    vals["P4 HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(3,2)
    vals["P4 HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(3,1)
    vals["M HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(4,3)
    vals["M HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(4,2)
    vals["M HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(4,1)
    vals["L HH MR hi STC"] = h_HH_MR_4_hi.GetBinContent(5,3)
    vals["L HH MR hi pre"] = h_HH_MR_4_hi.GetBinContent(5,2)
    vals["L HH MR hi obs"] = h_HH_MR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(1,3)
    vals["P LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(1,2)
    vals["P LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(1,1)
    vals["P3 LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(2,3)
    vals["P3 LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(2,2)
    vals["P3 LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(2,1)
    vals["P4 LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(3,3)
    vals["P4 LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(3,2)
    vals["P4 LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(3,1)
    vals["M LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(4,3)
    vals["M LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(4,2)
    vals["M LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(4,1)
    vals["L LL MR lo STC"] = h_LL_MR_23_lo.GetBinContent(5,3)
    vals["L LL MR lo pre"] = h_LL_MR_23_lo.GetBinContent(5,2)
    vals["L LL MR lo obs"] = h_LL_MR_23_lo.GetBinContent(5,1)

    vals["P LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(1,3)
    vals["P LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(1,2)
    vals["P LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(1,1)
    vals["P3 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(2,3)
    vals["P3 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(2,2)
    vals["P3 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(2,1)
    vals["P4 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(3,3)
    vals["P4 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(3,2)
    vals["P4 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(3,1)
    vals["M LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(4,3)
    vals["M LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(4,2)
    vals["M LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(4,1)
    vals["L LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinContent(5,3)
    vals["L LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinContent(5,2)
    vals["L LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinContent(5,1)

    vals["P LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(1,3)
    vals["P LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(1,2)
    vals["P LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(1,1)
    vals["P3 LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(2,3)
    vals["P3 LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(2,2)
    vals["P3 LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(2,1)
    vals["P4 LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(3,3)
    vals["P4 LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(3,2)
    vals["P4 LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(3,1)
    vals["M LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(4,3)
    vals["M LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(4,2)
    vals["M LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(4,1)
    vals["L LM MR lo STC"] = h_LM_MR_23_lo.GetBinContent(5,3)
    vals["L LM MR lo pre"] = h_LM_MR_23_lo.GetBinContent(5,2)
    vals["L LM MR lo obs"] = h_LM_MR_23_lo.GetBinContent(5,1)

    vals["P LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(1,3)
    vals["P LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(1,2)
    vals["P LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(1,1)
    vals["P3 LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(2,3)
    vals["P3 LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(2,2)
    vals["P3 LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(2,1)
    vals["P4 LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(3,3)
    vals["P4 LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(3,2)
    vals["P4 LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(3,1)
    vals["M LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(4,3)
    vals["M LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(4,2)
    vals["M LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(4,1)
    vals["L LH MR lo STC"] = h_LH_MR_23_lo.GetBinContent(5,3)
    vals["L LH MR lo pre"] = h_LH_MR_23_lo.GetBinContent(5,2)
    vals["L LH MR lo obs"] = h_LH_MR_23_lo.GetBinContent(5,1)

    vals["P HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(1,3)
    vals["P HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(1,2)
    vals["P HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(1,1)
    vals["P3 HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(2,3)
    vals["P3 HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(2,2)
    vals["P3 HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(2,1)
    vals["P4 HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(3,3)
    vals["P4 HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(3,2)
    vals["P4 HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(3,1)
    vals["M HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(4,3)
    vals["M HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(4,2)
    vals["M HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(4,1)
    vals["L HL MR lo STC"] = h_HL_MR_4_lo.GetBinContent(5,3)
    vals["L HL MR lo pre"] = h_HL_MR_4_lo.GetBinContent(5,2)
    vals["L HL MR lo obs"] = h_HL_MR_4_lo.GetBinContent(5,1)

    vals["P HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(1,3)
    vals["P HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(1,2)
    vals["P HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(1,1)
    vals["P3 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(2,3)
    vals["P3 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(2,2)
    vals["P3 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(2,1)
    vals["P4 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(3,3)
    vals["P4 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(3,2)
    vals["P4 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(3,1)
    vals["M HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(4,3)
    vals["M HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(4,2)
    vals["M HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(4,1)
    vals["L HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinContent(5,3)
    vals["L HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinContent(5,2)
    vals["L HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinContent(5,1)

    vals["P HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(1,3)
    vals["P HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(1,2)
    vals["P HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(1,1)
    vals["P3 HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(2,3)
    vals["P3 HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(2,2)
    vals["P3 HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(2,1)
    vals["P4 HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(3,3)
    vals["P4 HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(3,2)
    vals["P4 HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(3,1)
    vals["M HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(4,3)
    vals["M HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(4,2)
    vals["M HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(4,1)
    vals["L HM MR lo STC"] = h_HM_MR_4_lo.GetBinContent(5,3)
    vals["L HM MR lo pre"] = h_HM_MR_4_lo.GetBinContent(5,2)
    vals["L HM MR lo obs"] = h_HM_MR_4_lo.GetBinContent(5,1)

    vals["P HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(1,3)
    vals["P HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(1,2)
    vals["P HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(1,1)
    vals["P3 HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(2,3)
    vals["P3 HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(2,2)
    vals["P3 HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(2,1)
    vals["P4 HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(3,3)
    vals["P4 HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(3,2)
    vals["P4 HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(3,1)
    vals["M HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(4,3)
    vals["M HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(4,2)
    vals["M HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(4,1)
    vals["L HH MR lo STC"] = h_HH_MR_4_lo.GetBinContent(5,3)
    vals["L HH MR lo pre"] = h_HH_MR_4_lo.GetBinContent(5,2)
    vals["L HH MR lo obs"] = h_HH_MR_4_lo.GetBinContent(5,1)

    # MR Errors
    errs["P LL MR STC"] = h_LL_MR_23.GetBinError(1,3)
    errs["P LL MR pre"] = h_LL_MR_23.GetBinError(1,2)
    errs["P LL MR obs"] = getAsymmetricErrors(vals["P LL MR obs"])
    errs["P3 LL MR STC"] = h_LL_MR_23.GetBinError(2,3)
    errs["P3 LL MR pre"] = h_LL_MR_23.GetBinError(2,2)
    errs["P3 LL MR obs"] = getAsymmetricErrors(vals["P3 LL MR obs"])
    errs["P4 LL MR STC"] = h_LL_MR_23.GetBinError(3,3)
    errs["P4 LL MR pre"] = h_LL_MR_23.GetBinError(3,2)
    errs["P4 LL MR obs"] = getAsymmetricErrors(vals["P4 LL MR obs"])
    errs["M LL MR STC"] = h_LL_MR_23.GetBinError(4,3)
    errs["M LL MR pre"] = h_LL_MR_23.GetBinError(4,2)
    errs["M LL MR obs"] = getAsymmetricErrors(vals["M LL MR obs"])
    errs["L LL MR STC"] = h_LL_MR_23.GetBinError(5,3)
    errs["L LL MR pre"] = h_LL_MR_23.GetBinError(5,2)
    errs["L LL MR obs"] = getAsymmetricErrors(vals["M LL MR obs"])

    errs["P LLM MR STC"] = h_LLM_MR_23.GetBinError(1,3)
    errs["P LLM MR pre"] = h_LLM_MR_23.GetBinError(1,2)
    errs["P LLM MR obs"] = getAsymmetricErrors(vals["P LLM MR obs"])
    errs["P3 LLM MR STC"] = h_LLM_MR_23.GetBinError(2,3)
    errs["P3 LLM MR pre"] = h_LLM_MR_23.GetBinError(2,2)
    errs["P3 LLM MR obs"] = getAsymmetricErrors(vals["P3 LLM MR obs"])
    errs["P4 LLM MR STC"] = h_LLM_MR_23.GetBinError(3,3)
    errs["P4 LLM MR pre"] = h_LLM_MR_23.GetBinError(3,2)
    errs["P4 LLM MR obs"] = getAsymmetricErrors(vals["P4 LLM MR obs"])
    errs["M LLM MR STC"] = h_LLM_MR_23.GetBinError(4,3)
    errs["M LLM MR pre"] = h_LLM_MR_23.GetBinError(4,2)
    errs["M LLM MR obs"] = h_LLM_MR_23.GetBinError(4,1)
    errs["L LLM MR STC"] = h_LLM_MR_23.GetBinError(5,3)
    errs["L LLM MR pre"] = h_LLM_MR_23.GetBinError(5,2)
    errs["L LLM MR obs"] = h_LLM_MR_23.GetBinError(5,1)

    errs["P LM MR STC"] = h_LM_MR_23.GetBinError(1,3)
    errs["P LM MR pre"] = h_LM_MR_23.GetBinError(1,2)
    errs["P LM MR obs"] = h_LM_MR_23.GetBinError(1,1)
    errs["P3 LM MR STC"] = h_LM_MR_23.GetBinError(2,3)
    errs["P3 LM MR pre"] = h_LM_MR_23.GetBinError(2,2)
    errs["P3 LM MR obs"] = h_LM_MR_23.GetBinError(2,1)
    errs["P4 LM MR STC"] = h_LM_MR_23.GetBinError(3,3)
    errs["P4 LM MR pre"] = h_LM_MR_23.GetBinError(3,2)
    errs["P4 LM MR obs"] = h_LM_MR_23.GetBinError(3,1)
    errs["M LM MR STC"] = h_LM_MR_23.GetBinError(4,3)
    errs["M LM MR pre"] = h_LM_MR_23.GetBinError(4,2)
    errs["M LM MR obs"] = h_LM_MR_23.GetBinError(4,1)
    errs["L LM MR STC"] = h_LM_MR_23.GetBinError(5,3)
    errs["L LM MR pre"] = h_LM_MR_23.GetBinError(5,2)
    errs["L LM MR obs"] = h_LM_MR_23.GetBinError(5,1)

    errs["P LH MR STC"] = h_LH_MR_23.GetBinError(1,3)
    errs["P LH MR pre"] = h_LH_MR_23.GetBinError(1,2)
    errs["P LH MR obs"] = h_LH_MR_23.GetBinError(1,1)
    errs["P3 LH MR STC"] = h_LH_MR_23.GetBinError(2,3)
    errs["P3 LH MR pre"] = h_LH_MR_23.GetBinError(2,2)
    errs["P3 LH MR obs"] = h_LH_MR_23.GetBinError(2,1)
    errs["P4 LH MR STC"] = h_LH_MR_23.GetBinError(3,3)
    errs["P4 LH MR pre"] = h_LH_MR_23.GetBinError(3,2)
    errs["P4 LH MR obs"] = h_LH_MR_23.GetBinError(3,1)
    errs["M LH MR STC"] = h_LH_MR_23.GetBinError(4,3)
    errs["M LH MR pre"] = h_LH_MR_23.GetBinError(4,2)
    errs["M LH MR obs"] = h_LH_MR_23.GetBinError(4,1)
    errs["L LH MR STC"] = h_LH_MR_23.GetBinError(5,3)
    errs["L LH MR pre"] = h_LH_MR_23.GetBinError(5,2)
    errs["L LH MR obs"] = h_LH_MR_23.GetBinError(5,1)

    errs["P HL MR STC"] = h_HL_MR_4.GetBinError(1,3)
    errs["P HL MR pre"] = h_HL_MR_4.GetBinError(1,2)
    errs["P HL MR obs"] = h_HL_MR_4.GetBinError(1,1)
    errs["P3 HL MR STC"] = h_HL_MR_4.GetBinError(2,3)
    errs["P3 HL MR pre"] = h_HL_MR_4.GetBinError(2,2)
    errs["P3 HL MR obs"] = h_HL_MR_4.GetBinError(2,1)
    errs["P4 HL MR STC"] = h_HL_MR_4.GetBinError(3,3)
    errs["P4 HL MR pre"] = h_HL_MR_4.GetBinError(3,2)
    errs["P4 HL MR obs"] = h_HL_MR_4.GetBinError(3,1)
    errs["M HL MR STC"] = h_HL_MR_4.GetBinError(4,3)
    errs["M HL MR pre"] = h_HL_MR_4.GetBinError(4,2)
    errs["M HL MR obs"] = h_HL_MR_4.GetBinError(4,1)
    errs["L HL MR STC"] = h_HL_MR_4.GetBinError(5,3)
    errs["L HL MR pre"] = h_HL_MR_4.GetBinError(5,2)
    errs["L HL MR obs"] = h_HL_MR_4.GetBinError(5,1)

    errs["P HLM MR STC"] = h_HLM_MR_4.GetBinError(1,3)
    errs["P HLM MR pre"] = h_HLM_MR_4.GetBinError(1,2)
    errs["P HLM MR obs"] = h_HLM_MR_4.GetBinError(1,1)
    errs["P3 HLM MR STC"] = h_HLM_MR_4.GetBinError(2,3)
    errs["P3 HLM MR pre"] = h_HLM_MR_4.GetBinError(2,2)
    errs["P3 HLM MR obs"] = h_HLM_MR_4.GetBinError(2,1)
    errs["P4 HLM MR STC"] = h_HLM_MR_4.GetBinError(3,3)
    errs["P4 HLM MR pre"] = h_HLM_MR_4.GetBinError(3,2)
    errs["P4 HLM MR obs"] = h_HLM_MR_4.GetBinError(3,1)
    errs["M HLM MR STC"] = h_HLM_MR_4.GetBinError(4,3)
    errs["M HLM MR pre"] = h_HLM_MR_4.GetBinError(4,2)
    errs["M HLM MR obs"] = h_HLM_MR_4.GetBinError(4,1)
    errs["L HLM MR STC"] = h_HLM_MR_4.GetBinError(5,3)
    errs["L HLM MR pre"] = h_HLM_MR_4.GetBinError(5,2)
    errs["L HLM MR obs"] = h_HLM_MR_4.GetBinError(5,1)

    errs["P HM MR STC"] = h_HM_MR_4.GetBinError(1,3)
    errs["P HM MR pre"] = h_HM_MR_4.GetBinError(1,2)
    errs["P HM MR obs"] = h_HM_MR_4.GetBinError(1,1)
    errs["P3 HM MR STC"] = h_HM_MR_4.GetBinError(2,3)
    errs["P3 HM MR pre"] = h_HM_MR_4.GetBinError(2,2)
    errs["P3 HM MR obs"] = h_HM_MR_4.GetBinError(2,1)
    errs["P4 HM MR STC"] = h_HM_MR_4.GetBinError(3,3)
    errs["P4 HM MR pre"] = h_HM_MR_4.GetBinError(3,2)
    errs["P4 HM MR obs"] = h_HM_MR_4.GetBinError(3,1)
    errs["M HM MR STC"] = h_HM_MR_4.GetBinError(4,3)
    errs["M HM MR pre"] = h_HM_MR_4.GetBinError(4,2)
    errs["M HM MR obs"] = h_HM_MR_4.GetBinError(4,1)
    errs["L HM MR STC"] = h_HM_MR_4.GetBinError(5,3)
    errs["L HM MR pre"] = h_HM_MR_4.GetBinError(5,2)
    errs["L HM MR obs"] = h_HM_MR_4.GetBinError(5,1)

    errs["P HH MR STC"] = h_HH_MR_4.GetBinError(1,3)
    errs["P HH MR pre"] = h_HH_MR_4.GetBinError(1,2)
    errs["P HH MR obs"] = h_HH_MR_4.GetBinError(1,1)
    errs["P3 HH MR STC"] = h_HH_MR_4.GetBinError(2,3)
    errs["P3 HH MR pre"] = h_HH_MR_4.GetBinError(2,2)
    errs["P3 HH MR obs"] = h_HH_MR_4.GetBinError(2,1)
    errs["P4 HH MR STC"] = h_HH_MR_4.GetBinError(3,3)
    errs["P4 HH MR pre"] = h_HH_MR_4.GetBinError(3,2)
    errs["P4 HH MR obs"] = h_HH_MR_4.GetBinError(3,1)
    errs["M HH MR STC"] = h_HH_MR_4.GetBinError(4,3)
    errs["M HH MR pre"] = h_HH_MR_4.GetBinError(4,2)
    errs["M HH MR obs"] = h_HH_MR_4.GetBinError(4,1)
    errs["L HH MR STC"] = h_HH_MR_4.GetBinError(5,3)
    errs["L HH MR pre"] = h_HH_MR_4.GetBinError(5,2)
    errs["L HH MR obs"] = h_HH_MR_4.GetBinError(5,1)

    # hi pt
    errs["P LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(1,3)
    errs["P LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(1,2)
    errs["P LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(1,1)
    errs["P3 LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(2,3)
    errs["P3 LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(2,2)
    errs["P3 LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(2,1)
    errs["P4 LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(3,3)
    errs["P4 LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(3,2)
    errs["P4 LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(3,1)
    errs["M LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(4,3)
    errs["M LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(4,2)
    errs["M LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(4,1)
    errs["L LL MR hi STC"] = h_LL_MR_23_hi.GetBinError(5,3)
    errs["L LL MR hi pre"] = h_LL_MR_23_hi.GetBinError(5,2)
    errs["L LL MR hi obs"] = h_LL_MR_23_hi.GetBinError(5,1)

    errs["P LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(1,3)
    errs["P LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(1,2)
    errs["P LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(1,1)
    errs["P3 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(2,3)
    errs["P3 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(2,2)
    errs["P3 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(2,1)
    errs["P4 LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(3,3)
    errs["P4 LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(3,2)
    errs["P4 LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(3,1)
    errs["M LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(4,3)
    errs["M LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(4,2)
    errs["M LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(4,1)
    errs["L LLM MR hi STC"] = h_LLM_MR_23_hi.GetBinError(5,3)
    errs["L LLM MR hi pre"] = h_LLM_MR_23_hi.GetBinError(5,2)
    errs["L LLM MR hi obs"] = h_LLM_MR_23_hi.GetBinError(5,1)

    errs["P LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(1,3)
    errs["P LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(1,2)
    errs["P LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(1,1)
    errs["P3 LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(2,3)
    errs["P3 LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(2,2)
    errs["P3 LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(2,1)
    errs["P4 LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(3,3)
    errs["P4 LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(3,2)
    errs["P4 LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(3,1)
    errs["M LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(4,3)
    errs["M LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(4,2)
    errs["M LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(4,1)
    errs["L LM MR hi STC"] = h_LM_MR_23_hi.GetBinError(5,3)
    errs["L LM MR hi pre"] = h_LM_MR_23_hi.GetBinError(5,2)
    errs["L LM MR hi obs"] = h_LM_MR_23_hi.GetBinError(5,1)

    errs["P LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(1,3)
    errs["P LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(1,2)
    errs["P LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(1,1)
    errs["P3 LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(2,3)
    errs["P3 LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(2,2)
    errs["P3 LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(2,1)
    errs["P4 LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(3,3)
    errs["P4 LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(3,2)
    errs["P4 LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(3,1)
    errs["M LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(4,3)
    errs["M LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(4,2)
    errs["M LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(4,1)
    errs["L LH MR hi STC"] = h_LH_MR_23_hi.GetBinError(5,3)
    errs["L LH MR hi pre"] = h_LH_MR_23_hi.GetBinError(5,2)
    errs["L LH MR hi obs"] = h_LH_MR_23_hi.GetBinError(5,1)

    errs["P HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(1,3)
    errs["P HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(1,2)
    errs["P HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(1,1)
    errs["P3 HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(2,3)
    errs["P3 HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(2,2)
    errs["P3 HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(2,1)
    errs["P4 HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(3,3)
    errs["P4 HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(3,2)
    errs["P4 HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(3,1)
    errs["M HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(4,3)
    errs["M HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(4,2)
    errs["M HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(4,1)
    errs["L HL MR hi STC"] = h_HL_MR_4_hi.GetBinError(5,3)
    errs["L HL MR hi pre"] = h_HL_MR_4_hi.GetBinError(5,2)
    errs["L HL MR hi obs"] = h_HL_MR_4_hi.GetBinError(5,1)

    errs["P HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(1,3)
    errs["P HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(1,2)
    errs["P HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(1,1)
    errs["P3 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(2,3)
    errs["P3 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(2,2)
    errs["P3 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(2,1)
    errs["P4 HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(3,3)
    errs["P4 HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(3,2)
    errs["P4 HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(3,1)
    errs["M HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(4,3)
    errs["M HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(4,2)
    errs["M HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(4,1)
    errs["L HLM MR hi STC"] = h_HLM_MR_4_hi.GetBinError(5,3)
    errs["L HLM MR hi pre"] = h_HLM_MR_4_hi.GetBinError(5,2)
    errs["L HLM MR hi obs"] = h_HLM_MR_4_hi.GetBinError(5,1)

    errs["P HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(1,3)
    errs["P HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(1,2)
    errs["P HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(1,1)
    errs["P3 HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(2,3)
    errs["P3 HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(2,2)
    errs["P3 HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(2,1)
    errs["P4 HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(3,3)
    errs["P4 HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(3,2)
    errs["P4 HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(3,1)
    errs["M HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(4,3)
    errs["M HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(4,2)
    errs["M HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(4,1)
    errs["L HM MR hi STC"] = h_HM_MR_4_hi.GetBinError(5,3)
    errs["L HM MR hi pre"] = h_HM_MR_4_hi.GetBinError(5,2)
    errs["L HM MR hi obs"] = h_HM_MR_4_hi.GetBinError(5,1)

    errs["P HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(1,3)
    errs["P HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(1,2)
    errs["P HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(1,1)
    errs["P3 HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(2,3)
    errs["P3 HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(2,2)
    errs["P3 HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(2,1)
    errs["P4 HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(3,3)
    errs["P4 HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(3,2)
    errs["P4 HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(3,1)
    errs["M HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(4,3)
    errs["M HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(4,2)
    errs["M HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(4,1)
    errs["L HH MR hi STC"] = h_HH_MR_4_hi.GetBinError(5,3)
    errs["L HH MR hi pre"] = h_HH_MR_4_hi.GetBinError(5,2)
    errs["L HH MR hi obs"] = h_HH_MR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(1,3)
    errs["P LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(1,2)
    errs["P LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(1,1)
    errs["P3 LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(2,3)
    errs["P3 LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(2,2)
    errs["P3 LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(2,1)
    errs["P4 LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(3,3)
    errs["P4 LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(3,2)
    errs["P4 LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(3,1)
    errs["M LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(4,3)
    errs["M LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(4,2)
    errs["M LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(4,1)
    errs["L LL MR lo STC"] = h_LL_MR_23_lo.GetBinError(5,3)
    errs["L LL MR lo pre"] = h_LL_MR_23_lo.GetBinError(5,2)
    errs["L LL MR lo obs"] = h_LL_MR_23_lo.GetBinError(5,1)

    errs["P LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(1,3)
    errs["P LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(1,2)
    errs["P LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(1,1)
    errs["P3 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(2,3)
    errs["P3 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(2,2)
    errs["P3 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(2,1)
    errs["P4 LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(3,3)
    errs["P4 LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(3,2)
    errs["P4 LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(3,1)
    errs["M LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(4,3)
    errs["M LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(4,2)
    errs["M LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(4,1)
    errs["L LLM MR lo STC"] = h_LLM_MR_23_lo.GetBinError(5,3)
    errs["L LLM MR lo pre"] = h_LLM_MR_23_lo.GetBinError(5,2)
    errs["L LLM MR lo obs"] = h_LLM_MR_23_lo.GetBinError(5,1)

    errs["P LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(1,3)
    errs["P LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(1,2)
    errs["P LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(1,1)
    errs["P3 LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(2,3)
    errs["P3 LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(2,2)
    errs["P3 LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(2,1)
    errs["P4 LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(3,3)
    errs["P4 LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(3,2)
    errs["P4 LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(3,1)
    errs["M LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(4,3)
    errs["M LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(4,2)
    errs["M LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(4,1)
    errs["L LM MR lo STC"] = h_LM_MR_23_lo.GetBinError(5,3)
    errs["L LM MR lo pre"] = h_LM_MR_23_lo.GetBinError(5,2)
    errs["L LM MR lo obs"] = h_LM_MR_23_lo.GetBinError(5,1)

    errs["P LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(1,3)
    errs["P LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(1,2)
    errs["P LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(1,1)
    errs["P3 LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(2,3)
    errs["P3 LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(2,2)
    errs["P3 LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(2,1)
    errs["P4 LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(3,3)
    errs["P4 LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(3,2)
    errs["P4 LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(3,1)
    errs["M LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(4,3)
    errs["M LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(4,2)
    errs["M LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(4,1)
    errs["L LH MR lo STC"] = h_LH_MR_23_lo.GetBinError(5,3)
    errs["L LH MR lo pre"] = h_LH_MR_23_lo.GetBinError(5,2)
    errs["L LH MR lo obs"] = h_LH_MR_23_lo.GetBinError(5,1)

    errs["P HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(1,3)
    errs["P HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(1,2)
    errs["P HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(1,1)
    errs["P3 HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(2,3)
    errs["P3 HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(2,2)
    errs["P3 HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(2,1)
    errs["P4 HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(3,3)
    errs["P4 HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(3,2)
    errs["P4 HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(3,1)
    errs["M HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(4,3)
    errs["M HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(4,2)
    errs["M HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(4,1)
    errs["L HL MR lo STC"] = h_HL_MR_4_lo.GetBinError(5,3)
    errs["L HL MR lo pre"] = h_HL_MR_4_lo.GetBinError(5,2)
    errs["L HL MR lo obs"] = h_HL_MR_4_lo.GetBinError(5,1)

    errs["P HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(1,3)
    errs["P HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(1,2)
    errs["P HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(1,1)
    errs["P3 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(2,3)
    errs["P3 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(2,2)
    errs["P3 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(2,1)
    errs["P4 HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(3,3)
    errs["P4 HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(3,2)
    errs["P4 HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(3,1)
    errs["M HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(4,3)
    errs["M HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(4,2)
    errs["M HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(4,1)
    errs["L HLM MR lo STC"] = h_HLM_MR_4_lo.GetBinError(5,3)
    errs["L HLM MR lo pre"] = h_HLM_MR_4_lo.GetBinError(5,2)
    errs["L HLM MR lo obs"] = h_HLM_MR_4_lo.GetBinError(5,1)

    errs["P HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(1,3)
    errs["P HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(1,2)
    errs["P HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(1,1)
    errs["P3 HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(2,3)
    errs["P3 HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(2,2)
    errs["P3 HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(2,1)
    errs["P4 HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(3,3)
    errs["P4 HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(3,2)
    errs["P4 HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(3,1)
    errs["M HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(4,3)
    errs["M HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(4,2)
    errs["M HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(4,1)
    errs["L HM MR lo STC"] = h_HM_MR_4_lo.GetBinError(5,3)
    errs["L HM MR lo pre"] = h_HM_MR_4_lo.GetBinError(5,2)
    errs["L HM MR lo obs"] = h_HM_MR_4_lo.GetBinError(5,1)

    errs["P HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(1,3)
    errs["P HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(1,2)
    errs["P HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(1,1)
    errs["P3 HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(2,3)
    errs["P3 HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(2,2)
    errs["P3 HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(2,1)
    errs["P4 HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(3,3)
    errs["P4 HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(3,2)
    errs["P4 HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(3,1)
    errs["M HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(4,3)
    errs["M HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(4,2)
    errs["M HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(4,1)
    errs["L HH MR lo STC"] = h_HH_MR_4_lo.GetBinError(5,3)
    errs["L HH MR lo pre"] = h_HH_MR_4_lo.GetBinError(5,2)
    errs["L HH MR lo obs"] = h_HH_MR_4_lo.GetBinError(5,1)

    # VR vals
    vals["P LL VR STC"] = h_LL_VR_23.GetBinContent(1,3)
    vals["P LL VR pre"] = h_LL_VR_23.GetBinContent(1,2)
    vals["P LL VR obs"] = h_LL_VR_23.GetBinContent(1,1)
    vals["P3 LL VR STC"] = h_LL_VR_23.GetBinContent(2,3)
    vals["P3 LL VR pre"] = h_LL_VR_23.GetBinContent(2,2)
    vals["P3 LL VR obs"] = h_LL_VR_23.GetBinContent(2,1)
    vals["P4 LL VR STC"] = h_LL_VR_23.GetBinContent(3,3)
    vals["P4 LL VR pre"] = h_LL_VR_23.GetBinContent(3,2)
    vals["P4 LL VR obs"] = h_LL_VR_23.GetBinContent(3,1)
    vals["M LL VR STC"] = h_LL_VR_23.GetBinContent(4,3)
    vals["M LL VR pre"] = h_LL_VR_23.GetBinContent(4,2)
    vals["M LL VR obs"] = h_LL_VR_23.GetBinContent(4,1)
    vals["L LL VR STC"] = h_LL_VR_23.GetBinContent(5,3)
    vals["L LL VR pre"] = h_LL_VR_23.GetBinContent(5,2)
    vals["L LL VR obs"] = h_LL_VR_23.GetBinContent(5,1)

    vals["P LLM VR STC"] = h_LLM_VR_23.GetBinContent(1,3)
    vals["P LLM VR pre"] = h_LLM_VR_23.GetBinContent(1,2)
    vals["P LLM VR obs"] = h_LLM_VR_23.GetBinContent(1,1)
    vals["P3 LLM VR STC"] = h_LLM_VR_23.GetBinContent(2,3)
    vals["P3 LLM VR pre"] = h_LLM_VR_23.GetBinContent(2,2)
    vals["P3 LLM VR obs"] = h_LLM_VR_23.GetBinContent(2,1)
    vals["P4 LLM VR STC"] = h_LLM_VR_23.GetBinContent(3,3)
    vals["P4 LLM VR pre"] = h_LLM_VR_23.GetBinContent(3,2)
    vals["P4 LLM VR obs"] = h_LLM_VR_23.GetBinContent(3,1)
    vals["M LLM VR STC"] = h_LLM_VR_23.GetBinContent(4,3)
    vals["M LLM VR pre"] = h_LLM_VR_23.GetBinContent(4,2)
    vals["M LLM VR obs"] = h_LLM_VR_23.GetBinContent(4,1)
    vals["L LLM VR STC"] = h_LLM_VR_23.GetBinContent(5,3)
    vals["L LLM VR pre"] = h_LLM_VR_23.GetBinContent(5,2)
    vals["L LLM VR obs"] = h_LLM_VR_23.GetBinContent(5,1)

    vals["P LM VR STC"] = h_LM_VR_23.GetBinContent(1,3)
    vals["P LM VR pre"] = h_LM_VR_23.GetBinContent(1,2)
    vals["P LM VR obs"] = h_LM_VR_23.GetBinContent(1,1)
    vals["P3 LM VR STC"] = h_LM_VR_23.GetBinContent(2,3)
    vals["P3 LM VR pre"] = h_LM_VR_23.GetBinContent(2,2)
    vals["P3 LM VR obs"] = h_LM_VR_23.GetBinContent(2,1)
    vals["P4 LM VR STC"] = h_LM_VR_23.GetBinContent(3,3)
    vals["P4 LM VR pre"] = h_LM_VR_23.GetBinContent(3,2)
    vals["P4 LM VR obs"] = h_LM_VR_23.GetBinContent(3,1)
    vals["M LM VR STC"] = h_LM_VR_23.GetBinContent(4,3)
    vals["M LM VR pre"] = h_LM_VR_23.GetBinContent(4,2)
    vals["M LM VR obs"] = h_LM_VR_23.GetBinContent(4,1)
    vals["L LM VR STC"] = h_LM_VR_23.GetBinContent(5,3)
    vals["L LM VR pre"] = h_LM_VR_23.GetBinContent(5,2)
    vals["L LM VR obs"] = h_LM_VR_23.GetBinContent(5,1)

    vals["P LH VR STC"] = h_LH_VR_23.GetBinContent(1,3)
    vals["P LH VR pre"] = h_LH_VR_23.GetBinContent(1,2)
    vals["P LH VR obs"] = h_LH_VR_23.GetBinContent(1,1)
    vals["P3 LH VR STC"] = h_LH_VR_23.GetBinContent(2,3)
    vals["P3 LH VR pre"] = h_LH_VR_23.GetBinContent(2,2)
    vals["P3 LH VR obs"] = h_LH_VR_23.GetBinContent(2,1)
    vals["P4 LH VR STC"] = h_LH_VR_23.GetBinContent(3,3)
    vals["P4 LH VR pre"] = h_LH_VR_23.GetBinContent(3,2)
    vals["P4 LH VR obs"] = h_LH_VR_23.GetBinContent(3,1)
    vals["M LH VR STC"] = h_LH_VR_23.GetBinContent(4,3)
    vals["M LH VR pre"] = h_LH_VR_23.GetBinContent(4,2)
    vals["M LH VR obs"] = h_LH_VR_23.GetBinContent(4,1)
    vals["L LH VR STC"] = h_LH_VR_23.GetBinContent(5,3)
    vals["L LH VR pre"] = h_LH_VR_23.GetBinContent(5,2)
    vals["L LH VR obs"] = h_LH_VR_23.GetBinContent(5,1)

    vals["P HL VR STC"] = h_HL_VR_4.GetBinContent(1,3)
    vals["P HL VR pre"] = h_HL_VR_4.GetBinContent(1,2)
    vals["P HL VR obs"] = h_HL_VR_4.GetBinContent(1,1)
    vals["P3 HL VR STC"] = h_HL_VR_4.GetBinContent(2,3)
    vals["P3 HL VR pre"] = h_HL_VR_4.GetBinContent(2,2)
    vals["P3 HL VR obs"] = h_HL_VR_4.GetBinContent(2,1)
    vals["P4 HL VR STC"] = h_HL_VR_4.GetBinContent(3,3)
    vals["P4 HL VR pre"] = h_HL_VR_4.GetBinContent(3,2)
    vals["P4 HL VR obs"] = h_HL_VR_4.GetBinContent(3,1)
    vals["M HL VR STC"] = h_HL_VR_4.GetBinContent(4,3)
    vals["M HL VR pre"] = h_HL_VR_4.GetBinContent(4,2)
    vals["M HL VR obs"] = h_HL_VR_4.GetBinContent(4,1)
    vals["L HL VR STC"] = h_HL_VR_4.GetBinContent(5,3)
    vals["L HL VR pre"] = h_HL_VR_4.GetBinContent(5,2)
    vals["L HL VR obs"] = h_HL_VR_4.GetBinContent(5,1)

    vals["P HLM VR STC"] = h_HLM_VR_4.GetBinContent(1,3)
    vals["P HLM VR pre"] = h_HLM_VR_4.GetBinContent(1,2)
    vals["P HLM VR obs"] = h_HLM_VR_4.GetBinContent(1,1)
    vals["P3 HLM VR STC"] = h_HLM_VR_4.GetBinContent(2,3)
    vals["P3 HLM VR pre"] = h_HLM_VR_4.GetBinContent(2,2)
    vals["P3 HLM VR obs"] = h_HLM_VR_4.GetBinContent(2,1)
    vals["P4 HLM VR STC"] = h_HLM_VR_4.GetBinContent(3,3)
    vals["P4 HLM VR pre"] = h_HLM_VR_4.GetBinContent(3,2)
    vals["P4 HLM VR obs"] = h_HLM_VR_4.GetBinContent(3,1)
    vals["M HLM VR STC"] = h_HLM_VR_4.GetBinContent(4,3)
    vals["M HLM VR pre"] = h_HLM_VR_4.GetBinContent(4,2)
    vals["M HLM VR obs"] = h_HLM_VR_4.GetBinContent(4,1)
    vals["L HLM VR STC"] = h_HLM_VR_4.GetBinContent(5,3)
    vals["L HLM VR pre"] = h_HLM_VR_4.GetBinContent(5,2)
    vals["L HLM VR obs"] = h_HLM_VR_4.GetBinContent(5,1)

    vals["P HM VR STC"] = h_HM_VR_4.GetBinContent(1,3)
    vals["P HM VR pre"] = h_HM_VR_4.GetBinContent(1,2)
    vals["P HM VR obs"] = h_HM_VR_4.GetBinContent(1,1)
    vals["P3 HM VR STC"] = h_HM_VR_4.GetBinContent(2,3)
    vals["P3 HM VR pre"] = h_HM_VR_4.GetBinContent(2,2)
    vals["P3 HM VR obs"] = h_HM_VR_4.GetBinContent(2,1)
    vals["P4 HM VR STC"] = h_HM_VR_4.GetBinContent(3,3)
    vals["P4 HM VR pre"] = h_HM_VR_4.GetBinContent(3,2)
    vals["P4 HM VR obs"] = h_HM_VR_4.GetBinContent(3,1)
    vals["M HM VR STC"] = h_HM_VR_4.GetBinContent(4,3)
    vals["M HM VR pre"] = h_HM_VR_4.GetBinContent(4,2)
    vals["M HM VR obs"] = h_HM_VR_4.GetBinContent(4,1)
    vals["L HM VR STC"] = h_HM_VR_4.GetBinContent(5,3)
    vals["L HM VR pre"] = h_HM_VR_4.GetBinContent(5,2)
    vals["L HM VR obs"] = h_HM_VR_4.GetBinContent(5,1)

    vals["P HH VR STC"] = h_HH_VR_4.GetBinContent(1,3)
    vals["P HH VR pre"] = h_HH_VR_4.GetBinContent(1,2)
    vals["P HH VR obs"] = h_HH_VR_4.GetBinContent(1,1)
    vals["P3 HH VR STC"] = h_HH_VR_4.GetBinContent(2,3)
    vals["P3 HH VR pre"] = h_HH_VR_4.GetBinContent(2,2)
    vals["P3 HH VR obs"] = h_HH_VR_4.GetBinContent(2,1)
    vals["P4 HH VR STC"] = h_HH_VR_4.GetBinContent(3,3)
    vals["P4 HH VR pre"] = h_HH_VR_4.GetBinContent(3,2)
    vals["P4 HH VR obs"] = h_HH_VR_4.GetBinContent(3,1)
    vals["M HH VR STC"] = h_HH_VR_4.GetBinContent(4,3)
    vals["M HH VR pre"] = h_HH_VR_4.GetBinContent(4,2)
    vals["M HH VR obs"] = h_HH_VR_4.GetBinContent(4,1)
    vals["L HH VR STC"] = h_HH_VR_4.GetBinContent(5,3)
    vals["L HH VR pre"] = h_HH_VR_4.GetBinContent(5,2)
    vals["L HH VR obs"] = h_HH_VR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(1,3)
    vals["P LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(1,2)
    vals["P LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(1,1)
    vals["P3 LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(2,3)
    vals["P3 LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(2,2)
    vals["P3 LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(2,1)
    vals["P4 LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(3,3)
    vals["P4 LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(3,2)
    vals["P4 LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(3,1)
    vals["M LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(4,3)
    vals["M LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(4,2)
    vals["M LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(4,1)
    vals["L LL VR hi STC"] = h_LL_VR_23_hi.GetBinContent(5,3)
    vals["L LL VR hi pre"] = h_LL_VR_23_hi.GetBinContent(5,2)
    vals["L LL VR hi obs"] = h_LL_VR_23_hi.GetBinContent(5,1)

    vals["P LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(1,3)
    vals["P LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(1,2)
    vals["P LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(1,1)
    vals["P3 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(2,3)
    vals["P3 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(2,2)
    vals["P3 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(2,1)
    vals["P4 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(3,3)
    vals["P4 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(3,2)
    vals["P4 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(3,1)
    vals["M LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(4,3)
    vals["M LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(4,2)
    vals["M LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(4,1)
    vals["L LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinContent(5,3)
    vals["L LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinContent(5,2)
    vals["L LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinContent(5,1)

    vals["P LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(1,3)
    vals["P LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(1,2)
    vals["P LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(1,1)
    vals["P3 LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(2,3)
    vals["P3 LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(2,2)
    vals["P3 LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(2,1)
    vals["P4 LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(3,3)
    vals["P4 LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(3,2)
    vals["P4 LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(3,1)
    vals["M LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(4,3)
    vals["M LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(4,2)
    vals["M LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(4,1)
    vals["L LM VR hi STC"] = h_LM_VR_23_hi.GetBinContent(5,3)
    vals["L LM VR hi pre"] = h_LM_VR_23_hi.GetBinContent(5,2)
    vals["L LM VR hi obs"] = h_LM_VR_23_hi.GetBinContent(5,1)

    vals["P LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(1,3)
    vals["P LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(1,2)
    vals["P LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(1,1)
    vals["P3 LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(2,3)
    vals["P3 LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(2,2)
    vals["P3 LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(2,1)
    vals["P4 LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(3,3)
    vals["P4 LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(3,2)
    vals["P4 LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(3,1)
    vals["M LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(4,3)
    vals["M LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(4,2)
    vals["M LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(4,1)
    vals["L LH VR hi STC"] = h_LH_VR_23_hi.GetBinContent(5,3)
    vals["L LH VR hi pre"] = h_LH_VR_23_hi.GetBinContent(5,2)
    vals["L LH VR hi obs"] = h_LH_VR_23_hi.GetBinContent(5,1)

    vals["P HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(1,3)
    vals["P HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(1,2)
    vals["P HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(1,1)
    vals["P3 HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(2,3)
    vals["P3 HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(2,2)
    vals["P3 HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(2,1)
    vals["P4 HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(3,3)
    vals["P4 HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(3,2)
    vals["P4 HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(3,1)
    vals["M HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(4,3)
    vals["M HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(4,2)
    vals["M HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(4,1)
    vals["L HL VR hi STC"] = h_HL_VR_4_hi.GetBinContent(5,3)
    vals["L HL VR hi pre"] = h_HL_VR_4_hi.GetBinContent(5,2)
    vals["L HL VR hi obs"] = h_HL_VR_4_hi.GetBinContent(5,1)

    vals["P HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(1,3)
    vals["P HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(1,2)
    vals["P HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(1,1)
    vals["P3 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(2,3)
    vals["P3 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(2,2)
    vals["P3 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(2,1)
    vals["P4 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(3,3)
    vals["P4 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(3,2)
    vals["P4 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(3,1)
    vals["M HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(4,3)
    vals["M HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(4,2)
    vals["M HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(4,1)
    vals["L HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinContent(5,3)
    vals["L HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinContent(5,2)
    vals["L HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinContent(5,1)

    vals["P HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(1,3)
    vals["P HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(1,2)
    vals["P HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(1,1)
    vals["P3 HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(2,3)
    vals["P3 HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(2,2)
    vals["P3 HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(2,1)
    vals["P4 HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(3,3)
    vals["P4 HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(3,2)
    vals["P4 HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(3,1)
    vals["M HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(4,3)
    vals["M HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(4,2)
    vals["M HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(4,1)
    vals["L HM VR hi STC"] = h_HM_VR_4_hi.GetBinContent(5,3)
    vals["L HM VR hi pre"] = h_HM_VR_4_hi.GetBinContent(5,2)
    vals["L HM VR hi obs"] = h_HM_VR_4_hi.GetBinContent(5,1)

    vals["P HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(1,3)
    vals["P HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(1,2)
    vals["P HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(1,1)
    vals["P3 HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(2,3)
    vals["P3 HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(2,2)
    vals["P3 HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(2,1)
    vals["P4 HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(3,3)
    vals["P4 HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(3,2)
    vals["P4 HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(3,1)
    vals["M HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(4,3)
    vals["M HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(4,2)
    vals["M HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(4,1)
    vals["L HH VR hi STC"] = h_HH_VR_4_hi.GetBinContent(5,3)
    vals["L HH VR hi pre"] = h_HH_VR_4_hi.GetBinContent(5,2)
    vals["L HH VR hi obs"] = h_HH_VR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(1,3)
    vals["P LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(1,2)
    vals["P LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(1,1)
    vals["P3 LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(2,3)
    vals["P3 LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(2,2)
    vals["P3 LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(2,1)
    vals["P4 LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(3,3)
    vals["P4 LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(3,2)
    vals["P4 LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(3,1)
    vals["M LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(4,3)
    vals["M LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(4,2)
    vals["M LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(4,1)
    vals["L LL VR lo STC"] = h_LL_VR_23_lo.GetBinContent(5,3)
    vals["L LL VR lo pre"] = h_LL_VR_23_lo.GetBinContent(5,2)
    vals["L LL VR lo obs"] = h_LL_VR_23_lo.GetBinContent(5,1)

    vals["P LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(1,3)
    vals["P LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(1,2)
    vals["P LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(1,1)
    vals["P3 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(2,3)
    vals["P3 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(2,2)
    vals["P3 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(2,1)
    vals["P4 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(3,3)
    vals["P4 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(3,2)
    vals["P4 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(3,1)
    vals["M LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(4,3)
    vals["M LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(4,2)
    vals["M LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(4,1)
    vals["L LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinContent(5,3)
    vals["L LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinContent(5,2)
    vals["L LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinContent(5,1)

    vals["P LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(1,3)
    vals["P LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(1,2)
    vals["P LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(1,1)
    vals["P3 LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(2,3)
    vals["P3 LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(2,2)
    vals["P3 LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(2,1)
    vals["P4 LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(3,3)
    vals["P4 LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(3,2)
    vals["P4 LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(3,1)
    vals["M LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(4,3)
    vals["M LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(4,2)
    vals["M LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(4,1)
    vals["L LM VR lo STC"] = h_LM_VR_23_lo.GetBinContent(5,3)
    vals["L LM VR lo pre"] = h_LM_VR_23_lo.GetBinContent(5,2)
    vals["L LM VR lo obs"] = h_LM_VR_23_lo.GetBinContent(5,1)

    vals["P LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(1,3)
    vals["P LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(1,2)
    vals["P LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(1,1)
    vals["P3 LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(2,3)
    vals["P3 LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(2,2)
    vals["P3 LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(2,1)
    vals["P4 LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(3,3)
    vals["P4 LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(3,2)
    vals["P4 LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(3,1)
    vals["M LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(4,3)
    vals["M LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(4,2)
    vals["M LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(4,1)
    vals["L LH VR lo STC"] = h_LH_VR_23_lo.GetBinContent(5,3)
    vals["L LH VR lo pre"] = h_LH_VR_23_lo.GetBinContent(5,2)
    vals["L LH VR lo obs"] = h_LH_VR_23_lo.GetBinContent(5,1)

    vals["P HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(1,3)
    vals["P HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(1,2)
    vals["P HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(1,1)
    vals["P3 HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(2,3)
    vals["P3 HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(2,2)
    vals["P3 HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(2,1)
    vals["P4 HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(3,3)
    vals["P4 HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(3,2)
    vals["P4 HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(3,1)
    vals["M HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(4,3)
    vals["M HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(4,2)
    vals["M HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(4,1)
    vals["L HL VR lo STC"] = h_HL_VR_4_lo.GetBinContent(5,3)
    vals["L HL VR lo pre"] = h_HL_VR_4_lo.GetBinContent(5,2)
    vals["L HL VR lo obs"] = h_HL_VR_4_lo.GetBinContent(5,1)

    vals["P HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(1,3)
    vals["P HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(1,2)
    vals["P HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(1,1)
    vals["P3 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(2,3)
    vals["P3 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(2,2)
    vals["P3 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(2,1)
    vals["P4 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(3,3)
    vals["P4 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(3,2)
    vals["P4 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(3,1)
    vals["M HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(4,3)
    vals["M HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(4,2)
    vals["M HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(4,1)
    vals["L HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinContent(5,3)
    vals["L HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinContent(5,2)
    vals["L HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinContent(5,1)

    vals["P HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(1,3)
    vals["P HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(1,2)
    vals["P HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(1,1)
    vals["P3 HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(2,3)
    vals["P3 HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(2,2)
    vals["P3 HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(2,1)
    vals["P4 HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(3,3)
    vals["P4 HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(3,2)
    vals["P4 HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(3,1)
    vals["M HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(4,3)
    vals["M HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(4,2)
    vals["M HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(4,1)
    vals["L HM VR lo STC"] = h_HM_VR_4_lo.GetBinContent(5,3)
    vals["L HM VR lo pre"] = h_HM_VR_4_lo.GetBinContent(5,2)
    vals["L HM VR lo obs"] = h_HM_VR_4_lo.GetBinContent(5,1)

    vals["P HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(1,3)
    vals["P HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(1,2)
    vals["P HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(1,1)
    vals["P3 HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(2,3)
    vals["P3 HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(2,2)
    vals["P3 HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(2,1)
    vals["P4 HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(3,3)
    vals["P4 HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(3,2)
    vals["P4 HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(3,1)
    vals["M HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(4,3)
    vals["M HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(4,2)
    vals["M HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(4,1)
    vals["L HH VR lo STC"] = h_HH_VR_4_lo.GetBinContent(5,3)
    vals["L HH VR lo pre"] = h_HH_VR_4_lo.GetBinContent(5,2)
    vals["L HH VR lo obs"] = h_HH_VR_4_lo.GetBinContent(5,1)

    # VR Errors
    errs["P LL VR STC"] = h_LL_VR_23.GetBinError(1,3)
    errs["P LL VR pre"] = h_LL_VR_23.GetBinError(1,2)
    errs["P LL VR obs"] = h_LL_VR_23.GetBinError(1,1)
    errs["P3 LL VR STC"] = h_LL_VR_23.GetBinError(2,3)
    errs["P3 LL VR pre"] = h_LL_VR_23.GetBinError(2,2)
    errs["P3 LL VR obs"] = h_LL_VR_23.GetBinError(2,1)
    errs["P4 LL VR STC"] = h_LL_VR_23.GetBinError(3,3)
    errs["P4 LL VR pre"] = h_LL_VR_23.GetBinError(3,2)
    errs["P4 LL VR obs"] = h_LL_VR_23.GetBinError(3,1)
    errs["M LL VR STC"] = h_LL_VR_23.GetBinError(4,3)
    errs["M LL VR pre"] = h_LL_VR_23.GetBinError(4,2)
    errs["M LL VR obs"] = h_LL_VR_23.GetBinError(4,1)
    errs["L LL VR STC"] = h_LL_VR_23.GetBinError(5,3)
    errs["L LL VR pre"] = h_LL_VR_23.GetBinError(5,2)
    errs["L LL VR obs"] = h_LL_VR_23.GetBinError(5,1)

    errs["P LLM VR STC"] = h_LLM_VR_23.GetBinError(1,3)
    errs["P LLM VR pre"] = h_LLM_VR_23.GetBinError(1,2)
    errs["P LLM VR obs"] = h_LLM_VR_23.GetBinError(1,1)
    errs["P3 LLM VR STC"] = h_LLM_VR_23.GetBinError(2,3)
    errs["P3 LLM VR pre"] = h_LLM_VR_23.GetBinError(2,2)
    errs["P3 LLM VR obs"] = h_LLM_VR_23.GetBinError(2,1)
    errs["P4 LLM VR STC"] = h_LLM_VR_23.GetBinError(3,3)
    errs["P4 LLM VR pre"] = h_LLM_VR_23.GetBinError(3,2)
    errs["P4 LLM VR obs"] = h_LLM_VR_23.GetBinError(3,1)
    errs["M LLM VR STC"] = h_LLM_VR_23.GetBinError(4,3)
    errs["M LLM VR pre"] = h_LLM_VR_23.GetBinError(4,2)
    errs["M LLM VR obs"] = h_LLM_VR_23.GetBinError(4,1)
    errs["L LLM VR STC"] = h_LLM_VR_23.GetBinError(5,3)
    errs["L LLM VR pre"] = h_LLM_VR_23.GetBinError(5,2)
    errs["L LLM VR obs"] = h_LLM_VR_23.GetBinError(5,1)

    errs["P LM VR STC"] = h_LM_VR_23.GetBinError(1,3)
    errs["P LM VR pre"] = h_LM_VR_23.GetBinError(1,2)
    errs["P LM VR obs"] = h_LM_VR_23.GetBinError(1,1)
    errs["P3 LM VR STC"] = h_LM_VR_23.GetBinError(2,3)
    errs["P3 LM VR pre"] = h_LM_VR_23.GetBinError(2,2)
    errs["P3 LM VR obs"] = h_LM_VR_23.GetBinError(2,1)
    errs["P4 LM VR STC"] = h_LM_VR_23.GetBinError(3,3)
    errs["P4 LM VR pre"] = h_LM_VR_23.GetBinError(3,2)
    errs["P4 LM VR obs"] = h_LM_VR_23.GetBinError(3,1)
    errs["M LM VR STC"] = h_LM_VR_23.GetBinError(4,3)
    errs["M LM VR pre"] = h_LM_VR_23.GetBinError(4,2)
    errs["M LM VR obs"] = h_LM_VR_23.GetBinError(4,1)
    errs["L LM VR STC"] = h_LM_VR_23.GetBinError(5,3)
    errs["L LM VR pre"] = h_LM_VR_23.GetBinError(5,2)
    errs["L LM VR obs"] = h_LM_VR_23.GetBinError(5,1)

    errs["P LH VR STC"] = h_LH_VR_23.GetBinError(1,3)
    errs["P LH VR pre"] = h_LH_VR_23.GetBinError(1,2)
    errs["P LH VR obs"] = h_LH_VR_23.GetBinError(1,1)
    errs["P3 LH VR STC"] = h_LH_VR_23.GetBinError(2,3)
    errs["P3 LH VR pre"] = h_LH_VR_23.GetBinError(2,2)
    errs["P3 LH VR obs"] = h_LH_VR_23.GetBinError(2,1)
    errs["P4 LH VR STC"] = h_LH_VR_23.GetBinError(3,3)
    errs["P4 LH VR pre"] = h_LH_VR_23.GetBinError(3,2)
    errs["P4 LH VR obs"] = h_LH_VR_23.GetBinError(3,1)
    errs["M LH VR STC"] = h_LH_VR_23.GetBinError(4,3)
    errs["M LH VR pre"] = h_LH_VR_23.GetBinError(4,2)
    errs["M LH VR obs"] = h_LH_VR_23.GetBinError(4,1)
    errs["L LH VR STC"] = h_LH_VR_23.GetBinError(5,3)
    errs["L LH VR pre"] = h_LH_VR_23.GetBinError(5,2)
    errs["L LH VR obs"] = h_LH_VR_23.GetBinError(5,1)

    errs["P HL VR STC"] = h_HL_VR_4.GetBinError(1,3)
    errs["P HL VR pre"] = h_HL_VR_4.GetBinError(1,2)
    errs["P HL VR obs"] = h_HL_VR_4.GetBinError(1,1)
    errs["P3 HL VR STC"] = h_HL_VR_4.GetBinError(2,3)
    errs["P3 HL VR pre"] = h_HL_VR_4.GetBinError(2,2)
    errs["P3 HL VR obs"] = h_HL_VR_4.GetBinError(2,1)
    errs["P4 HL VR STC"] = h_HL_VR_4.GetBinError(3,3)
    errs["P4 HL VR pre"] = h_HL_VR_4.GetBinError(3,2)
    errs["P4 HL VR obs"] = h_HL_VR_4.GetBinError(3,1)
    errs["M HL VR STC"] = h_HL_VR_4.GetBinError(4,3)
    errs["M HL VR pre"] = h_HL_VR_4.GetBinError(4,2)
    errs["M HL VR obs"] = h_HL_VR_4.GetBinError(4,1)
    errs["L HL VR STC"] = h_HL_VR_4.GetBinError(5,3)
    errs["L HL VR pre"] = h_HL_VR_4.GetBinError(5,2)
    errs["L HL VR obs"] = h_HL_VR_4.GetBinError(5,1)

    errs["P HLM VR STC"] = h_HLM_VR_4.GetBinError(1,3)
    errs["P HLM VR pre"] = h_HLM_VR_4.GetBinError(1,2)
    errs["P HLM VR obs"] = h_HLM_VR_4.GetBinError(1,1)
    errs["P3 HLM VR STC"] = h_HLM_VR_4.GetBinError(2,3)
    errs["P3 HLM VR pre"] = h_HLM_VR_4.GetBinError(2,2)
    errs["P3 HLM VR obs"] = h_HLM_VR_4.GetBinError(2,1)
    errs["P4 HLM VR STC"] = h_HLM_VR_4.GetBinError(3,3)
    errs["P4 HLM VR pre"] = h_HLM_VR_4.GetBinError(3,2)
    errs["P4 HLM VR obs"] = h_HLM_VR_4.GetBinError(3,1)
    errs["M HLM VR STC"] = h_HLM_VR_4.GetBinError(4,3)
    errs["M HLM VR pre"] = h_HLM_VR_4.GetBinError(4,2)
    errs["M HLM VR obs"] = h_HLM_VR_4.GetBinError(4,1)
    errs["L HLM VR STC"] = h_HLM_VR_4.GetBinError(5,3)
    errs["L HLM VR pre"] = h_HLM_VR_4.GetBinError(5,2)
    errs["L HLM VR obs"] = h_HLM_VR_4.GetBinError(5,1)

    errs["P HM VR STC"] = h_HM_VR_4.GetBinError(1,3)
    errs["P HM VR pre"] = h_HM_VR_4.GetBinError(1,2)
    errs["P HM VR obs"] = h_HM_VR_4.GetBinError(1,1)
    errs["P3 HM VR STC"] = h_HM_VR_4.GetBinError(2,3)
    errs["P3 HM VR pre"] = h_HM_VR_4.GetBinError(2,2)
    errs["P3 HM VR obs"] = h_HM_VR_4.GetBinError(2,1)
    errs["P4 HM VR STC"] = h_HM_VR_4.GetBinError(3,3)
    errs["P4 HM VR pre"] = h_HM_VR_4.GetBinError(3,2)
    errs["P4 HM VR obs"] = h_HM_VR_4.GetBinError(3,1)
    errs["M HM VR STC"] = h_HM_VR_4.GetBinError(4,3)
    errs["M HM VR pre"] = h_HM_VR_4.GetBinError(4,2)
    errs["M HM VR obs"] = h_HM_VR_4.GetBinError(4,1)
    errs["L HM VR STC"] = h_HM_VR_4.GetBinError(5,3)
    errs["L HM VR pre"] = h_HM_VR_4.GetBinError(5,2)
    errs["L HM VR obs"] = h_HM_VR_4.GetBinError(5,1)

    errs["P HH VR STC"] = h_HH_VR_4.GetBinError(1,3)
    errs["P HH VR pre"] = h_HH_VR_4.GetBinError(1,2)
    errs["P HH VR obs"] = h_HH_VR_4.GetBinError(1,1)
    errs["P3 HH VR STC"] = h_HH_VR_4.GetBinError(2,3)
    errs["P3 HH VR pre"] = h_HH_VR_4.GetBinError(2,2)
    errs["P3 HH VR obs"] = h_HH_VR_4.GetBinError(2,1)
    errs["P4 HH VR STC"] = h_HH_VR_4.GetBinError(3,3)
    errs["P4 HH VR pre"] = h_HH_VR_4.GetBinError(3,2)
    errs["P4 HH VR obs"] = h_HH_VR_4.GetBinError(3,1)
    errs["M HH VR STC"] = h_HH_VR_4.GetBinError(4,3)
    errs["M HH VR pre"] = h_HH_VR_4.GetBinError(4,2)
    errs["M HH VR obs"] = h_HH_VR_4.GetBinError(4,1)
    errs["L HH VR STC"] = h_HH_VR_4.GetBinError(5,3)
    errs["L HH VR pre"] = h_HH_VR_4.GetBinError(5,2)
    errs["L HH VR obs"] = h_HH_VR_4.GetBinError(5,1)

    # hi pt
    errs["P LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(1,3)
    errs["P LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(1,2)
    errs["P LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(1,1)
    errs["P3 LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(2,3)
    errs["P3 LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(2,2)
    errs["P3 LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(2,1)
    errs["P4 LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(3,3)
    errs["P4 LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(3,2)
    errs["P4 LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(3,1)
    errs["M LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(4,3)
    errs["M LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(4,2)
    errs["M LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(4,1)
    errs["L LL VR hi STC"] = h_LL_VR_23_hi.GetBinError(5,3)
    errs["L LL VR hi pre"] = h_LL_VR_23_hi.GetBinError(5,2)
    errs["L LL VR hi obs"] = h_LL_VR_23_hi.GetBinError(5,1)

    errs["P LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(1,3)
    errs["P LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(1,2)
    errs["P LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(1,1)
    errs["P3 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(2,3)
    errs["P3 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(2,2)
    errs["P3 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(2,1)
    errs["P4 LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(3,3)
    errs["P4 LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(3,2)
    errs["P4 LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(3,1)
    errs["M LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(4,3)
    errs["M LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(4,2)
    errs["M LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(4,1)
    errs["L LLM VR hi STC"] = h_LLM_VR_23_hi.GetBinError(5,3)
    errs["L LLM VR hi pre"] = h_LLM_VR_23_hi.GetBinError(5,2)
    errs["L LLM VR hi obs"] = h_LLM_VR_23_hi.GetBinError(5,1)

    errs["P LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(1,3)
    errs["P LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(1,2)
    errs["P LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(1,1)
    errs["P3 LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(2,3)
    errs["P3 LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(2,2)
    errs["P3 LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(2,1)
    errs["P4 LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(3,3)
    errs["P4 LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(3,2)
    errs["P4 LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(3,1)
    errs["M LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(4,3)
    errs["M LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(4,2)
    errs["M LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(4,1)
    errs["L LM VR hi STC"] = h_LM_VR_23_hi.GetBinError(5,3)
    errs["L LM VR hi pre"] = h_LM_VR_23_hi.GetBinError(5,2)
    errs["L LM VR hi obs"] = h_LM_VR_23_hi.GetBinError(5,1)

    errs["P LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(1,3)
    errs["P LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(1,2)
    errs["P LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(1,1)
    errs["P3 LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(2,3)
    errs["P3 LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(2,2)
    errs["P3 LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(2,1)
    errs["P4 LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(3,3)
    errs["P4 LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(3,2)
    errs["P4 LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(3,1)
    errs["M LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(4,3)
    errs["M LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(4,2)
    errs["M LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(4,1)
    errs["L LH VR hi STC"] = h_LH_VR_23_hi.GetBinError(5,3)
    errs["L LH VR hi pre"] = h_LH_VR_23_hi.GetBinError(5,2)
    errs["L LH VR hi obs"] = h_LH_VR_23_hi.GetBinError(5,1)

    errs["P HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(1,3)
    errs["P HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(1,2)
    errs["P HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(1,1)
    errs["P3 HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(2,3)
    errs["P3 HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(2,2)
    errs["P3 HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(2,1)
    errs["P4 HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(3,3)
    errs["P4 HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(3,2)
    errs["P4 HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(3,1)
    errs["M HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(4,3)
    errs["M HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(4,2)
    errs["M HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(4,1)
    errs["L HL VR hi STC"] = h_HL_VR_4_hi.GetBinError(5,3)
    errs["L HL VR hi pre"] = h_HL_VR_4_hi.GetBinError(5,2)
    errs["L HL VR hi obs"] = h_HL_VR_4_hi.GetBinError(5,1)

    errs["P HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(1,3)
    errs["P HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(1,2)
    errs["P HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(1,1)
    errs["P3 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(2,3)
    errs["P3 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(2,2)
    errs["P3 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(2,1)
    errs["P4 HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(3,3)
    errs["P4 HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(3,2)
    errs["P4 HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(3,1)
    errs["M HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(4,3)
    errs["M HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(4,2)
    errs["M HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(4,1)
    errs["L HLM VR hi STC"] = h_HLM_VR_4_hi.GetBinError(5,3)
    errs["L HLM VR hi pre"] = h_HLM_VR_4_hi.GetBinError(5,2)
    errs["L HLM VR hi obs"] = h_HLM_VR_4_hi.GetBinError(5,1)

    errs["P HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(1,3)
    errs["P HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(1,2)
    errs["P HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(1,1)
    errs["P3 HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(2,3)
    errs["P3 HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(2,2)
    errs["P3 HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(2,1)
    errs["P4 HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(3,3)
    errs["P4 HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(3,2)
    errs["P4 HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(3,1)
    errs["M HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(4,3)
    errs["M HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(4,2)
    errs["M HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(4,1)
    errs["L HM VR hi STC"] = h_HM_VR_4_hi.GetBinError(5,3)
    errs["L HM VR hi pre"] = h_HM_VR_4_hi.GetBinError(5,2)
    errs["L HM VR hi obs"] = h_HM_VR_4_hi.GetBinError(5,1)

    errs["P HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(1,3)
    errs["P HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(1,2)
    errs["P HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(1,1)
    errs["P3 HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(2,3)
    errs["P3 HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(2,2)
    errs["P3 HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(2,1)
    errs["P4 HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(3,3)
    errs["P4 HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(3,2)
    errs["P4 HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(3,1)
    errs["M HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(4,3)
    errs["M HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(4,2)
    errs["M HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(4,1)
    errs["L HH VR hi STC"] = h_HH_VR_4_hi.GetBinError(5,3)
    errs["L HH VR hi pre"] = h_HH_VR_4_hi.GetBinError(5,2)
    errs["L HH VR hi obs"] = h_HH_VR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(1,3)
    errs["P LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(1,2)
    errs["P LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(1,1)
    errs["P3 LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(2,3)
    errs["P3 LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(2,2)
    errs["P3 LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(2,1)
    errs["P4 LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(3,3)
    errs["P4 LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(3,2)
    errs["P4 LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(3,1)
    errs["M LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(4,3)
    errs["M LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(4,2)
    errs["M LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(4,1)
    errs["L LL VR lo STC"] = h_LL_VR_23_lo.GetBinError(5,3)
    errs["L LL VR lo pre"] = h_LL_VR_23_lo.GetBinError(5,2)
    errs["L LL VR lo obs"] = h_LL_VR_23_lo.GetBinError(5,1)

    errs["P LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(1,3)
    errs["P LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(1,2)
    errs["P LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(1,1)
    errs["P3 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(2,3)
    errs["P3 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(2,2)
    errs["P3 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(2,1)
    errs["P4 LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(3,3)
    errs["P4 LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(3,2)
    errs["P4 LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(3,1)
    errs["M LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(4,3)
    errs["M LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(4,2)
    errs["M LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(4,1)
    errs["L LLM VR lo STC"] = h_LLM_VR_23_lo.GetBinError(5,3)
    errs["L LLM VR lo pre"] = h_LLM_VR_23_lo.GetBinError(5,2)
    errs["L LLM VR lo obs"] = h_LLM_VR_23_lo.GetBinError(5,1)

    errs["P LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(1,3)
    errs["P LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(1,2)
    errs["P LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(1,1)
    errs["P3 LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(2,3)
    errs["P3 LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(2,2)
    errs["P3 LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(2,1)
    errs["P4 LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(3,3)
    errs["P4 LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(3,2)
    errs["P4 LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(3,1)
    errs["M LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(4,3)
    errs["M LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(4,2)
    errs["M LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(4,1)
    errs["L LM VR lo STC"] = h_LM_VR_23_lo.GetBinError(5,3)
    errs["L LM VR lo pre"] = h_LM_VR_23_lo.GetBinError(5,2)
    errs["L LM VR lo obs"] = h_LM_VR_23_lo.GetBinError(5,1)

    errs["P LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(1,3)
    errs["P LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(1,2)
    errs["P LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(1,1)
    errs["P3 LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(2,3)
    errs["P3 LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(2,2)
    errs["P3 LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(2,1)
    errs["P4 LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(3,3)
    errs["P4 LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(3,2)
    errs["P4 LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(3,1)
    errs["M LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(4,3)
    errs["M LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(4,2)
    errs["M LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(4,1)
    errs["L LH VR lo STC"] = h_LH_VR_23_lo.GetBinError(5,3)
    errs["L LH VR lo pre"] = h_LH_VR_23_lo.GetBinError(5,2)
    errs["L LH VR lo obs"] = h_LH_VR_23_lo.GetBinError(5,1)

    errs["P HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(1,3)
    errs["P HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(1,2)
    errs["P HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(1,1)
    errs["P3 HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(2,3)
    errs["P3 HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(2,2)
    errs["P3 HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(2,1)
    errs["P4 HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(3,3)
    errs["P4 HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(3,2)
    errs["P4 HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(3,1)
    errs["M HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(4,3)
    errs["M HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(4,2)
    errs["M HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(4,1)
    errs["L HL VR lo STC"] = h_HL_VR_4_lo.GetBinError(5,3)
    errs["L HL VR lo pre"] = h_HL_VR_4_lo.GetBinError(5,2)
    errs["L HL VR lo obs"] = h_HL_VR_4_lo.GetBinError(5,1)

    errs["P HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(1,3)
    errs["P HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(1,2)
    errs["P HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(1,1)
    errs["P3 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(2,3)
    errs["P3 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(2,2)
    errs["P3 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(2,1)
    errs["P4 HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(3,3)
    errs["P4 HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(3,2)
    errs["P4 HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(3,1)
    errs["M HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(4,3)
    errs["M HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(4,2)
    errs["M HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(4,1)
    errs["L HLM VR lo STC"] = h_HLM_VR_4_lo.GetBinError(5,3)
    errs["L HLM VR lo pre"] = h_HLM_VR_4_lo.GetBinError(5,2)
    errs["L HLM VR lo obs"] = h_HLM_VR_4_lo.GetBinError(5,1)

    errs["P HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(1,3)
    errs["P HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(1,2)
    errs["P HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(1,1)
    errs["P3 HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(2,3)
    errs["P3 HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(2,2)
    errs["P3 HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(2,1)
    errs["P4 HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(3,3)
    errs["P4 HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(3,2)
    errs["P4 HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(3,1)
    errs["M HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(4,3)
    errs["M HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(4,2)
    errs["M HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(4,1)
    errs["L HM VR lo STC"] = h_HM_VR_4_lo.GetBinError(5,3)
    errs["L HM VR lo pre"] = h_HM_VR_4_lo.GetBinError(5,2)
    errs["L HM VR lo obs"] = h_HM_VR_4_lo.GetBinError(5,1)

    errs["P HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(1,3)
    errs["P HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(1,2)
    errs["P HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(1,1)
    errs["P3 HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(2,3)
    errs["P3 HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(2,2)
    errs["P3 HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(2,1)
    errs["P4 HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(3,3)
    errs["P4 HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(3,2)
    errs["P4 HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(3,1)
    errs["M HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(4,3)
    errs["M HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(4,2)
    errs["M HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(4,1)
    errs["L HH VR lo STC"] = h_HH_VR_4_lo.GetBinError(5,3)
    errs["L HH VR lo pre"] = h_HH_VR_4_lo.GetBinError(5,2)
    errs["L HH VR lo obs"] = h_HH_VR_4_lo.GetBinError(5,1)

    # VR systs
    if h_LL_VR_23_ns != None:
        systs["P LL VR"] = h_LL_VR_23_ns.GetBinContent(1)
        systs["P3 LL VR"] = h_LL_VR_23_ns.GetBinContent(2)
        systs["P4 LL VR"] = h_LL_VR_23_ns.GetBinContent(3)
        systs["M LL VR"] = h_LL_VR_23_ns.GetBinContent(4)
        systs["L LL VR"] = h_LL_VR_23_ns.GetBinContent(5)
        systs["P LLM VR"] = h_LLM_VR_23_ns.GetBinContent(1)
        systs["P3 LLM VR"] = h_LLM_VR_23_ns.GetBinContent(2)
        systs["P4 LLM VR"] = h_LLM_VR_23_ns.GetBinContent(3)
        systs["M LLM VR"] = h_LLM_VR_23_ns.GetBinContent(4)
        systs["L LLM VR"] = h_LLM_VR_23_ns.GetBinContent(5)
        systs["P LM VR"] = h_LM_VR_23_ns.GetBinContent(1)
        systs["P3 LM VR"] = h_LM_VR_23_ns.GetBinContent(2)
        systs["P4 LM VR"] = h_LM_VR_23_ns.GetBinContent(3)
        systs["M LM VR"] = h_LM_VR_23_ns.GetBinContent(4)
        systs["L LM VR"] = h_LM_VR_23_ns.GetBinContent(5)
        systs["P LH VR"] = h_LH_VR_23_ns.GetBinContent(1)
        systs["P3 LH VR"] = h_LH_VR_23_ns.GetBinContent(2)
        systs["P4 LH VR"] = h_LH_VR_23_ns.GetBinContent(3)
        systs["M LH VR"] = h_LH_VR_23_ns.GetBinContent(4)
        systs["L LH VR"] = h_LH_VR_23_ns.GetBinContent(5)
        systs["P HL VR"] = h_HL_VR_4_ns.GetBinContent(1)
        systs["P3 HL VR"] = h_HL_VR_4_ns.GetBinContent(2)
        systs["P4 HL VR"] = h_HL_VR_4_ns.GetBinContent(3)
        systs["M HL VR"] = h_HL_VR_4_ns.GetBinContent(4)
        systs["L HL VR"] = h_HL_VR_4_ns.GetBinContent(5)
        systs["P HLM VR"] = h_HLM_VR_4_ns.GetBinContent(1)
        systs["P3 HLM VR"] = h_HLM_VR_4_ns.GetBinContent(2)
        systs["P4 HLM VR"] = h_HLM_VR_4_ns.GetBinContent(3)
        systs["M HLM VR"] = h_HLM_VR_4_ns.GetBinContent(4)
        systs["L HLM VR"] = h_HLM_VR_4_ns.GetBinContent(5)
        systs["P HM VR"] = h_HM_VR_4_ns.GetBinContent(1)
        systs["P3 HM VR"] = h_HM_VR_4_ns.GetBinContent(2)
        systs["P4 HM VR"] = h_HM_VR_4_ns.GetBinContent(3)
        systs["M HM VR"] = h_HM_VR_4_ns.GetBinContent(4)
        systs["L HM VR"] = h_HM_VR_4_ns.GetBinContent(5)
        systs["P HH VR"] = h_HH_VR_4_ns.GetBinContent(1)
        systs["P3 HH VR"] = h_HH_VR_4_ns.GetBinContent(2)
        systs["P4 HH VR"] = h_HH_VR_4_ns.GetBinContent(3)
        systs["M HH VR"] = h_HH_VR_4_ns.GetBinContent(4)
        systs["L HH VR"] = h_HH_VR_4_ns.GetBinContent(5)
        systs["P LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(3)
        systs["M LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(4)
        systs["L LL VR hi"] = h_LL_VR_23_hi_ns.GetBinContent(5)
        systs["P LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(3)
        systs["M LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(4)
        systs["L LLM VR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(5)
        systs["P LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(3)
        systs["M LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(4)
        systs["L LM VR hi"] = h_LM_VR_23_hi_ns.GetBinContent(5)
        systs["P LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(3)
        systs["M LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(4)
        systs["L LH VR hi"] = h_LH_VR_23_hi_ns.GetBinContent(5)
        systs["P HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(3)
        systs["M HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(4)
        systs["L HL VR hi"] = h_HL_VR_4_hi_ns.GetBinContent(5)
        systs["P HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(3)
        systs["M HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(4)
        systs["L HLM VR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(5)
        systs["P HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(3)
        systs["M HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(4)
        systs["L HM VR hi"] = h_HM_VR_4_hi_ns.GetBinContent(5)
        systs["P HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(3)
        systs["M HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(4)
        systs["L HH VR hi"] = h_HH_VR_4_hi_ns.GetBinContent(5)
        systs["P LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(3)
        systs["M LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(4)
        systs["L LL VR lo"] = h_LL_VR_23_lo_ns.GetBinContent(5)
        systs["P LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(3)
        systs["M LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(4)
        systs["L LLM VR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(5)
        systs["P LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(3)
        systs["M LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(4)
        systs["L LM VR lo"] = h_LM_VR_23_lo_ns.GetBinContent(5)
        systs["P LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(3)
        systs["M LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(4)
        systs["L LH VR lo"] = h_LH_VR_23_lo_ns.GetBinContent(5)
        systs["P HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(3)
        systs["M HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(4)
        systs["L HL VR lo"] = h_HL_VR_4_lo_ns.GetBinContent(5)
        systs["P HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(3)
        systs["M HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(4)
        systs["L HLM VR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(5)
        systs["P HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(3)
        systs["M HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(4)
        systs["L HM VR lo"] = h_HM_VR_4_lo_ns.GetBinContent(5)
        systs["P HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(3)
        systs["M HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(4)
        systs["L HH VR lo"] = h_HH_VR_4_lo_ns.GetBinContent(5)

    # SR vals
    vals["P LL SR STC"] = h_LL_SR_23.GetBinContent(1,3)
    vals["P LL SR pre"] = h_LL_SR_23.GetBinContent(1,2)
    vals["P LL SR obs"] = h_LL_SR_23.GetBinContent(1,1)
    vals["P3 LL SR STC"] = h_LL_SR_23.GetBinContent(2,3)
    vals["P3 LL SR pre"] = h_LL_SR_23.GetBinContent(2,2)
    vals["P3 LL SR obs"] = h_LL_SR_23.GetBinContent(2,1)
    vals["P4 LL SR STC"] = h_LL_SR_23.GetBinContent(3,3)
    vals["P4 LL SR pre"] = h_LL_SR_23.GetBinContent(3,2)
    vals["P4 LL SR obs"] = h_LL_SR_23.GetBinContent(3,1)
    vals["M LL SR STC"] = h_LL_SR_23.GetBinContent(4,3)
    vals["M LL SR pre"] = h_LL_SR_23.GetBinContent(4,2)
    vals["M LL SR obs"] = h_LL_SR_23.GetBinContent(4,1)
    vals["L LL SR STC"] = h_LL_SR_23.GetBinContent(5,3)
    vals["L LL SR pre"] = h_LL_SR_23.GetBinContent(5,2)
    vals["L LL SR obs"] = h_LL_SR_23.GetBinContent(5,1)

    vals["P LLM SR STC"] = h_LLM_SR_23.GetBinContent(1,3)
    vals["P LLM SR pre"] = h_LLM_SR_23.GetBinContent(1,2)
    vals["P LLM SR obs"] = h_LLM_SR_23.GetBinContent(1,1)
    vals["P3 LLM SR STC"] = h_LLM_SR_23.GetBinContent(2,3)
    vals["P3 LLM SR pre"] = h_LLM_SR_23.GetBinContent(2,2)
    vals["P3 LLM SR obs"] = h_LLM_SR_23.GetBinContent(2,1)
    vals["P4 LLM SR STC"] = h_LLM_SR_23.GetBinContent(3,3)
    vals["P4 LLM SR pre"] = h_LLM_SR_23.GetBinContent(3,2)
    vals["P4 LLM SR obs"] = h_LLM_SR_23.GetBinContent(3,1)
    vals["M LLM SR STC"] = h_LLM_SR_23.GetBinContent(4,3)
    vals["M LLM SR pre"] = h_LLM_SR_23.GetBinContent(4,2)
    vals["M LLM SR obs"] = h_LLM_SR_23.GetBinContent(4,1)
    vals["L LLM SR STC"] = h_LLM_SR_23.GetBinContent(5,3)
    vals["L LLM SR pre"] = h_LLM_SR_23.GetBinContent(5,2)
    vals["L LLM SR obs"] = h_LLM_SR_23.GetBinContent(5,1)

    vals["P LM SR STC"] = h_LM_SR_23.GetBinContent(1,3)
    vals["P LM SR pre"] = h_LM_SR_23.GetBinContent(1,2)
    vals["P LM SR obs"] = h_LM_SR_23.GetBinContent(1,1)
    vals["P3 LM SR STC"] = h_LM_SR_23.GetBinContent(2,3)
    vals["P3 LM SR pre"] = h_LM_SR_23.GetBinContent(2,2)
    vals["P3 LM SR obs"] = h_LM_SR_23.GetBinContent(2,1)
    vals["P4 LM SR STC"] = h_LM_SR_23.GetBinContent(3,3)
    vals["P4 LM SR pre"] = h_LM_SR_23.GetBinContent(3,2)
    vals["P4 LM SR obs"] = h_LM_SR_23.GetBinContent(3,1)
    vals["M LM SR STC"] = h_LM_SR_23.GetBinContent(4,3)
    vals["M LM SR pre"] = h_LM_SR_23.GetBinContent(4,2)
    vals["M LM SR obs"] = h_LM_SR_23.GetBinContent(4,1)
    vals["L LM SR STC"] = h_LM_SR_23.GetBinContent(5,3)
    vals["L LM SR pre"] = h_LM_SR_23.GetBinContent(5,2)
    vals["L LM SR obs"] = h_LM_SR_23.GetBinContent(5,1)

    vals["P LH SR STC"] = h_LH_SR_23.GetBinContent(1,3)
    vals["P LH SR pre"] = h_LH_SR_23.GetBinContent(1,2)
    vals["P LH SR obs"] = h_LH_SR_23.GetBinContent(1,1)
    vals["P3 LH SR STC"] = h_LH_SR_23.GetBinContent(2,3)
    vals["P3 LH SR pre"] = h_LH_SR_23.GetBinContent(2,2)
    vals["P3 LH SR obs"] = h_LH_SR_23.GetBinContent(2,1)
    vals["P4 LH SR STC"] = h_LH_SR_23.GetBinContent(3,3)
    vals["P4 LH SR pre"] = h_LH_SR_23.GetBinContent(3,2)
    vals["P4 LH SR obs"] = h_LH_SR_23.GetBinContent(3,1)
    vals["M LH SR STC"] = h_LH_SR_23.GetBinContent(4,3)
    vals["M LH SR pre"] = h_LH_SR_23.GetBinContent(4,2)
    vals["M LH SR obs"] = h_LH_SR_23.GetBinContent(4,1)
    vals["L LH SR STC"] = h_LH_SR_23.GetBinContent(5,3)
    vals["L LH SR pre"] = h_LH_SR_23.GetBinContent(5,2)
    vals["L LH SR obs"] = h_LH_SR_23.GetBinContent(5,1)

    vals["P HL SR STC"] = h_HL_SR_4.GetBinContent(1,3)
    vals["P HL SR pre"] = h_HL_SR_4.GetBinContent(1,2)
    vals["P HL SR obs"] = h_HL_SR_4.GetBinContent(1,1)
    vals["P3 HL SR STC"] = h_HL_SR_4.GetBinContent(2,3)
    vals["P3 HL SR pre"] = h_HL_SR_4.GetBinContent(2,2)
    vals["P3 HL SR obs"] = h_HL_SR_4.GetBinContent(2,1)
    vals["P4 HL SR STC"] = h_HL_SR_4.GetBinContent(3,3)
    vals["P4 HL SR pre"] = h_HL_SR_4.GetBinContent(3,2)
    vals["P4 HL SR obs"] = h_HL_SR_4.GetBinContent(3,1)
    vals["M HL SR STC"] = h_HL_SR_4.GetBinContent(4,3)
    vals["M HL SR pre"] = h_HL_SR_4.GetBinContent(4,2)
    vals["M HL SR obs"] = h_HL_SR_4.GetBinContent(4,1)
    vals["L HL SR STC"] = h_HL_SR_4.GetBinContent(5,3)
    vals["L HL SR pre"] = h_HL_SR_4.GetBinContent(5,2)
    vals["L HL SR obs"] = h_HL_SR_4.GetBinContent(5,1)

    vals["P HLM SR STC"] = h_HLM_SR_4.GetBinContent(1,3)
    vals["P HLM SR pre"] = h_HLM_SR_4.GetBinContent(1,2)
    vals["P HLM SR obs"] = h_HLM_SR_4.GetBinContent(1,1)
    vals["P3 HLM SR STC"] = h_HLM_SR_4.GetBinContent(2,3)
    vals["P3 HLM SR pre"] = h_HLM_SR_4.GetBinContent(2,2)
    vals["P3 HLM SR obs"] = h_HLM_SR_4.GetBinContent(2,1)
    vals["P4 HLM SR STC"] = h_HLM_SR_4.GetBinContent(3,3)
    vals["P4 HLM SR pre"] = h_HLM_SR_4.GetBinContent(3,2)
    vals["P4 HLM SR obs"] = h_HLM_SR_4.GetBinContent(3,1)
    vals["M HLM SR STC"] = h_HLM_SR_4.GetBinContent(4,3)
    vals["M HLM SR pre"] = h_HLM_SR_4.GetBinContent(4,2)
    vals["M HLM SR obs"] = h_HLM_SR_4.GetBinContent(4,1)
    vals["L HLM SR STC"] = h_HLM_SR_4.GetBinContent(5,3)
    vals["L HLM SR pre"] = h_HLM_SR_4.GetBinContent(5,2)
    vals["L HLM SR obs"] = h_HLM_SR_4.GetBinContent(5,1)

    vals["P HM SR STC"] = h_HM_SR_4.GetBinContent(1,3)
    vals["P HM SR pre"] = h_HM_SR_4.GetBinContent(1,2)
    vals["P HM SR obs"] = h_HM_SR_4.GetBinContent(1,1)
    vals["P3 HM SR STC"] = h_HM_SR_4.GetBinContent(2,3)
    vals["P3 HM SR pre"] = h_HM_SR_4.GetBinContent(2,2)
    vals["P3 HM SR obs"] = h_HM_SR_4.GetBinContent(2,1)
    vals["P4 HM SR STC"] = h_HM_SR_4.GetBinContent(3,3)
    vals["P4 HM SR pre"] = h_HM_SR_4.GetBinContent(3,2)
    vals["P4 HM SR obs"] = h_HM_SR_4.GetBinContent(3,1)
    vals["M HM SR STC"] = h_HM_SR_4.GetBinContent(4,3)
    vals["M HM SR pre"] = h_HM_SR_4.GetBinContent(4,2)
    vals["M HM SR obs"] = h_HM_SR_4.GetBinContent(4,1)
    vals["L HM SR STC"] = h_HM_SR_4.GetBinContent(5,3)
    vals["L HM SR pre"] = h_HM_SR_4.GetBinContent(5,2)
    vals["L HM SR obs"] = h_HM_SR_4.GetBinContent(5,1)

    vals["P HH SR STC"] = h_HH_SR_4.GetBinContent(1,3)
    vals["P HH SR pre"] = h_HH_SR_4.GetBinContent(1,2)
    vals["P HH SR obs"] = h_HH_SR_4.GetBinContent(1,1)
    vals["P3 HH SR STC"] = h_HH_SR_4.GetBinContent(2,3)
    vals["P3 HH SR pre"] = h_HH_SR_4.GetBinContent(2,2)
    vals["P3 HH SR obs"] = h_HH_SR_4.GetBinContent(2,1)
    vals["P4 HH SR STC"] = h_HH_SR_4.GetBinContent(3,3)
    vals["P4 HH SR pre"] = h_HH_SR_4.GetBinContent(3,2)
    vals["P4 HH SR obs"] = h_HH_SR_4.GetBinContent(3,1)
    vals["M HH SR STC"] = h_HH_SR_4.GetBinContent(4,3)
    vals["M HH SR pre"] = h_HH_SR_4.GetBinContent(4,2)
    vals["M HH SR obs"] = h_HH_SR_4.GetBinContent(4,1)
    vals["L HH SR STC"] = h_HH_SR_4.GetBinContent(5,3)
    vals["L HH SR pre"] = h_HH_SR_4.GetBinContent(5,2)
    vals["L HH SR obs"] = h_HH_SR_4.GetBinContent(5,1)

    # hi pt
    vals["P LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(1,3)
    vals["P LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(1,2)
    vals["P LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(1,1)
    vals["P3 LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(2,3)
    vals["P3 LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(2,2)
    vals["P3 LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(2,1)
    vals["P4 LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(3,3)
    vals["P4 LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(3,2)
    vals["P4 LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(3,1)
    vals["M LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(4,3)
    vals["M LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(4,2)
    vals["M LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(4,1)
    vals["L LL SR hi STC"] = h_LL_SR_23_hi.GetBinContent(5,3)
    vals["L LL SR hi pre"] = h_LL_SR_23_hi.GetBinContent(5,2)
    vals["L LL SR hi obs"] = h_LL_SR_23_hi.GetBinContent(5,1)

    vals["P LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(1,3)
    vals["P LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(1,2)
    vals["P LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(1,1)
    vals["P3 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(2,3)
    vals["P3 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(2,2)
    vals["P3 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(2,1)
    vals["P4 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(3,3)
    vals["P4 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(3,2)
    vals["P4 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(3,1)
    vals["M LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(4,3)
    vals["M LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(4,2)
    vals["M LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(4,1)
    vals["L LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinContent(5,3)
    vals["L LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinContent(5,2)
    vals["L LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinContent(5,1)

    vals["P LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(1,3)
    vals["P LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(1,2)
    vals["P LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(1,1)
    vals["P3 LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(2,3)
    vals["P3 LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(2,2)
    vals["P3 LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(2,1)
    vals["P4 LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(3,3)
    vals["P4 LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(3,2)
    vals["P4 LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(3,1)
    vals["M LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(4,3)
    vals["M LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(4,2)
    vals["M LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(4,1)
    vals["L LM SR hi STC"] = h_LM_SR_23_hi.GetBinContent(5,3)
    vals["L LM SR hi pre"] = h_LM_SR_23_hi.GetBinContent(5,2)
    vals["L LM SR hi obs"] = h_LM_SR_23_hi.GetBinContent(5,1)

    vals["P LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(1,3)
    vals["P LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(1,2)
    vals["P LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(1,1)
    vals["P3 LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(2,3)
    vals["P3 LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(2,2)
    vals["P3 LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(2,1)
    vals["P4 LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(3,3)
    vals["P4 LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(3,2)
    vals["P4 LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(3,1)
    vals["M LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(4,3)
    vals["M LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(4,2)
    vals["M LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(4,1)
    vals["L LH SR hi STC"] = h_LH_SR_23_hi.GetBinContent(5,3)
    vals["L LH SR hi pre"] = h_LH_SR_23_hi.GetBinContent(5,2)
    vals["L LH SR hi obs"] = h_LH_SR_23_hi.GetBinContent(5,1)

    vals["P HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(1,3)
    vals["P HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(1,2)
    vals["P HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(1,1)
    vals["P3 HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(2,3)
    vals["P3 HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(2,2)
    vals["P3 HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(2,1)
    vals["P4 HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(3,3)
    vals["P4 HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(3,2)
    vals["P4 HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(3,1)
    vals["M HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(4,3)
    vals["M HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(4,2)
    vals["M HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(4,1)
    vals["L HL SR hi STC"] = h_HL_SR_4_hi.GetBinContent(5,3)
    vals["L HL SR hi pre"] = h_HL_SR_4_hi.GetBinContent(5,2)
    vals["L HL SR hi obs"] = h_HL_SR_4_hi.GetBinContent(5,1)

    vals["P HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(1,3)
    vals["P HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(1,2)
    vals["P HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(1,1)
    vals["P3 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(2,3)
    vals["P3 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(2,2)
    vals["P3 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(2,1)
    vals["P4 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(3,3)
    vals["P4 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(3,2)
    vals["P4 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(3,1)
    vals["M HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(4,3)
    vals["M HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(4,2)
    vals["M HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(4,1)
    vals["L HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinContent(5,3)
    vals["L HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinContent(5,2)
    vals["L HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinContent(5,1)

    vals["P HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(1,3)
    vals["P HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(1,2)
    vals["P HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(1,1)
    vals["P3 HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(2,3)
    vals["P3 HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(2,2)
    vals["P3 HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(2,1)
    vals["P4 HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(3,3)
    vals["P4 HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(3,2)
    vals["P4 HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(3,1)
    vals["M HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(4,3)
    vals["M HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(4,2)
    vals["M HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(4,1)
    vals["L HM SR hi STC"] = h_HM_SR_4_hi.GetBinContent(5,3)
    vals["L HM SR hi pre"] = h_HM_SR_4_hi.GetBinContent(5,2)
    vals["L HM SR hi obs"] = h_HM_SR_4_hi.GetBinContent(5,1)

    vals["P HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(1,3)
    vals["P HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(1,2)
    vals["P HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(1,1)
    vals["P3 HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(2,3)
    vals["P3 HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(2,2)
    vals["P3 HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(2,1)
    vals["P4 HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(3,3)
    vals["P4 HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(3,2)
    vals["P4 HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(3,1)
    vals["M HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(4,3)
    vals["M HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(4,2)
    vals["M HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(4,1)
    vals["L HH SR hi STC"] = h_HH_SR_4_hi.GetBinContent(5,3)
    vals["L HH SR hi pre"] = h_HH_SR_4_hi.GetBinContent(5,2)
    vals["L HH SR hi obs"] = h_HH_SR_4_hi.GetBinContent(5,1)

    # lo pt
    vals["P LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(1,3)
    vals["P LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(1,2)
    vals["P LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(1,1)
    vals["P3 LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(2,3)
    vals["P3 LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(2,2)
    vals["P3 LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(2,1)
    vals["P4 LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(3,3)
    vals["P4 LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(3,2)
    vals["P4 LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(3,1)
    vals["M LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(4,3)
    vals["M LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(4,2)
    vals["M LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(4,1)
    vals["L LL SR lo STC"] = h_LL_SR_23_lo.GetBinContent(5,3)
    vals["L LL SR lo pre"] = h_LL_SR_23_lo.GetBinContent(5,2)
    vals["L LL SR lo obs"] = h_LL_SR_23_lo.GetBinContent(5,1)

    vals["P LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(1,3)
    vals["P LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(1,2)
    vals["P LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(1,1)
    vals["P3 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(2,3)
    vals["P3 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(2,2)
    vals["P3 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(2,1)
    vals["P4 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(3,3)
    vals["P4 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(3,2)
    vals["P4 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(3,1)
    vals["M LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(4,3)
    vals["M LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(4,2)
    vals["M LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(4,1)
    vals["L LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinContent(5,3)
    vals["L LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinContent(5,2)
    vals["L LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinContent(5,1)

    vals["P LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(1,3)
    vals["P LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(1,2)
    vals["P LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(1,1)
    vals["P3 LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(2,3)
    vals["P3 LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(2,2)
    vals["P3 LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(2,1)
    vals["P4 LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(3,3)
    vals["P4 LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(3,2)
    vals["P4 LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(3,1)
    vals["M LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(4,3)
    vals["M LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(4,2)
    vals["M LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(4,1)
    vals["L LM SR lo STC"] = h_LM_SR_23_lo.GetBinContent(5,3)
    vals["L LM SR lo pre"] = h_LM_SR_23_lo.GetBinContent(5,2)
    vals["L LM SR lo obs"] = h_LM_SR_23_lo.GetBinContent(5,1)

    vals["P LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(1,3)
    vals["P LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(1,2)
    vals["P LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(1,1)
    vals["P3 LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(2,3)
    vals["P3 LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(2,2)
    vals["P3 LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(2,1)
    vals["P4 LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(3,3)
    vals["P4 LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(3,2)
    vals["P4 LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(3,1)
    vals["M LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(4,3)
    vals["M LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(4,2)
    vals["M LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(4,1)
    vals["L LH SR lo STC"] = h_LH_SR_23_lo.GetBinContent(5,3)
    vals["L LH SR lo pre"] = h_LH_SR_23_lo.GetBinContent(5,2)
    vals["L LH SR lo obs"] = h_LH_SR_23_lo.GetBinContent(5,1)

    vals["P HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(1,3)
    vals["P HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(1,2)
    vals["P HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(1,1)
    vals["P3 HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(2,3)
    vals["P3 HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(2,2)
    vals["P3 HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(2,1)
    vals["P4 HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(3,3)
    vals["P4 HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(3,2)
    vals["P4 HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(3,1)
    vals["M HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(4,3)
    vals["M HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(4,2)
    vals["M HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(4,1)
    vals["L HL SR lo STC"] = h_HL_SR_4_lo.GetBinContent(5,3)
    vals["L HL SR lo pre"] = h_HL_SR_4_lo.GetBinContent(5,2)
    vals["L HL SR lo obs"] = h_HL_SR_4_lo.GetBinContent(5,1)

    vals["P HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(1,3)
    vals["P HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(1,2)
    vals["P HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(1,1)
    vals["P3 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(2,3)
    vals["P3 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(2,2)
    vals["P3 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(2,1)
    vals["P4 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(3,3)
    vals["P4 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(3,2)
    vals["P4 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(3,1)
    vals["M HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(4,3)
    vals["M HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(4,2)
    vals["M HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(4,1)
    vals["L HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinContent(5,3)
    vals["L HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinContent(5,2)
    vals["L HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinContent(5,1)

    vals["P HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(1,3)
    vals["P HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(1,2)
    vals["P HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(1,1)
    vals["P3 HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(2,3)
    vals["P3 HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(2,2)
    vals["P3 HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(2,1)
    vals["P4 HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(3,3)
    vals["P4 HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(3,2)
    vals["P4 HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(3,1)
    vals["M HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(4,3)
    vals["M HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(4,2)
    vals["M HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(4,1)
    vals["L HM SR lo STC"] = h_HM_SR_4_lo.GetBinContent(5,3)
    vals["L HM SR lo pre"] = h_HM_SR_4_lo.GetBinContent(5,2)
    vals["L HM SR lo obs"] = h_HM_SR_4_lo.GetBinContent(5,1)

    vals["P HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(1,3)
    vals["P HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(1,2)
    vals["P HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(1,1)
    vals["P3 HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(2,3)
    vals["P3 HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(2,2)
    vals["P3 HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(2,1)
    vals["P4 HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(3,3)
    vals["P4 HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(3,2)
    vals["P4 HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(3,1)
    vals["M HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(4,3)
    vals["M HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(4,2)
    vals["M HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(4,1)
    vals["L HH SR lo STC"] = h_HH_SR_4_lo.GetBinContent(5,3)
    vals["L HH SR lo pre"] = h_HH_SR_4_lo.GetBinContent(5,2)
    vals["L HH SR lo obs"] = h_HH_SR_4_lo.GetBinContent(5,1)

    # SR Errors
    errs["P LL SR STC"] = h_LL_SR_23.GetBinError(1,3)
    errs["P LL SR pre"] = h_LL_SR_23.GetBinError(1,2)
    errs["P LL SR obs"] = h_LL_SR_23.GetBinError(1,1)
    errs["P3 LL SR STC"] = h_LL_SR_23.GetBinError(2,3)
    errs["P3 LL SR pre"] = h_LL_SR_23.GetBinError(2,2)
    errs["P3 LL SR obs"] = h_LL_SR_23.GetBinError(2,1)
    errs["P4 LL SR STC"] = h_LL_SR_23.GetBinError(3,3)
    errs["P4 LL SR pre"] = h_LL_SR_23.GetBinError(3,2)
    errs["P4 LL SR obs"] = h_LL_SR_23.GetBinError(3,1)
    errs["M LL SR STC"] = h_LL_SR_23.GetBinError(4,3)
    errs["M LL SR pre"] = h_LL_SR_23.GetBinError(4,2)
    errs["M LL SR obs"] = h_LL_SR_23.GetBinError(4,1)
    errs["L LL SR STC"] = h_LL_SR_23.GetBinError(5,3)
    errs["L LL SR pre"] = h_LL_SR_23.GetBinError(5,2)
    errs["L LL SR obs"] = h_LL_SR_23.GetBinError(5,1)

    errs["P LLM SR STC"] = h_LLM_SR_23.GetBinError(1,3)
    errs["P LLM SR pre"] = h_LLM_SR_23.GetBinError(1,2)
    errs["P LLM SR obs"] = h_LLM_SR_23.GetBinError(1,1)
    errs["P3 LLM SR STC"] = h_LLM_SR_23.GetBinError(2,3)
    errs["P3 LLM SR pre"] = h_LLM_SR_23.GetBinError(2,2)
    errs["P3 LLM SR obs"] = h_LLM_SR_23.GetBinError(2,1)
    errs["P4 LLM SR STC"] = h_LLM_SR_23.GetBinError(3,3)
    errs["P4 LLM SR pre"] = h_LLM_SR_23.GetBinError(3,2)
    errs["P4 LLM SR obs"] = h_LLM_SR_23.GetBinError(3,1)
    errs["M LLM SR STC"] = h_LLM_SR_23.GetBinError(4,3)
    errs["M LLM SR pre"] = h_LLM_SR_23.GetBinError(4,2)
    errs["M LLM SR obs"] = h_LLM_SR_23.GetBinError(4,1)
    errs["L LLM SR STC"] = h_LLM_SR_23.GetBinError(5,3)
    errs["L LLM SR pre"] = h_LLM_SR_23.GetBinError(5,2)
    errs["L LLM SR obs"] = h_LLM_SR_23.GetBinError(5,1)

    errs["P LM SR STC"] = h_LM_SR_23.GetBinError(1,3)
    errs["P LM SR pre"] = h_LM_SR_23.GetBinError(1,2)
    errs["P LM SR obs"] = h_LM_SR_23.GetBinError(1,1)
    errs["P3 LM SR STC"] = h_LM_SR_23.GetBinError(2,3)
    errs["P3 LM SR pre"] = h_LM_SR_23.GetBinError(2,2)
    errs["P3 LM SR obs"] = h_LM_SR_23.GetBinError(2,1)
    errs["P4 LM SR STC"] = h_LM_SR_23.GetBinError(3,3)
    errs["P4 LM SR pre"] = h_LM_SR_23.GetBinError(3,2)
    errs["P4 LM SR obs"] = h_LM_SR_23.GetBinError(3,1)
    errs["M LM SR STC"] = h_LM_SR_23.GetBinError(4,3)
    errs["M LM SR pre"] = h_LM_SR_23.GetBinError(4,2)
    errs["M LM SR obs"] = h_LM_SR_23.GetBinError(4,1)
    errs["L LM SR STC"] = h_LM_SR_23.GetBinError(5,3)
    errs["L LM SR pre"] = h_LM_SR_23.GetBinError(5,2)
    errs["L LM SR obs"] = h_LM_SR_23.GetBinError(5,1)

    errs["P LH SR STC"] = h_LH_SR_23.GetBinError(1,3)
    errs["P LH SR pre"] = h_LH_SR_23.GetBinError(1,2)
    errs["P LH SR obs"] = h_LH_SR_23.GetBinError(1,1)
    errs["P3 LH SR STC"] = h_LH_SR_23.GetBinError(2,3)
    errs["P3 LH SR pre"] = h_LH_SR_23.GetBinError(2,2)
    errs["P3 LH SR obs"] = h_LH_SR_23.GetBinError(2,1)
    errs["P4 LH SR STC"] = h_LH_SR_23.GetBinError(3,3)
    errs["P4 LH SR pre"] = h_LH_SR_23.GetBinError(3,2)
    errs["P4 LH SR obs"] = h_LH_SR_23.GetBinError(3,1)
    errs["M LH SR STC"] = h_LH_SR_23.GetBinError(4,3)
    errs["M LH SR pre"] = h_LH_SR_23.GetBinError(4,2)
    errs["M LH SR obs"] = h_LH_SR_23.GetBinError(4,1)
    errs["L LH SR STC"] = h_LH_SR_23.GetBinError(5,3)
    errs["L LH SR pre"] = h_LH_SR_23.GetBinError(5,2)
    errs["L LH SR obs"] = h_LH_SR_23.GetBinError(5,1)

    errs["P HL SR STC"] = h_HL_SR_4.GetBinError(1,3)
    errs["P HL SR pre"] = h_HL_SR_4.GetBinError(1,2)
    errs["P HL SR obs"] = h_HL_SR_4.GetBinError(1,1)
    errs["P3 HL SR STC"] = h_HL_SR_4.GetBinError(2,3)
    errs["P3 HL SR pre"] = h_HL_SR_4.GetBinError(2,2)
    errs["P3 HL SR obs"] = h_HL_SR_4.GetBinError(2,1)
    errs["P4 HL SR STC"] = h_HL_SR_4.GetBinError(3,3)
    errs["P4 HL SR pre"] = h_HL_SR_4.GetBinError(3,2)
    errs["P4 HL SR obs"] = h_HL_SR_4.GetBinError(3,1)
    errs["M HL SR STC"] = h_HL_SR_4.GetBinError(4,3)
    errs["M HL SR pre"] = h_HL_SR_4.GetBinError(4,2)
    errs["M HL SR obs"] = h_HL_SR_4.GetBinError(4,1)
    errs["L HL SR STC"] = h_HL_SR_4.GetBinError(5,3)
    errs["L HL SR pre"] = h_HL_SR_4.GetBinError(5,2)
    errs["L HL SR obs"] = h_HL_SR_4.GetBinError(5,1)

    errs["P HLM SR STC"] = h_HLM_SR_4.GetBinError(1,3)
    errs["P HLM SR pre"] = h_HLM_SR_4.GetBinError(1,2)
    errs["P HLM SR obs"] = h_HLM_SR_4.GetBinError(1,1)
    errs["P3 HLM SR STC"] = h_HLM_SR_4.GetBinError(2,3)
    errs["P3 HLM SR pre"] = h_HLM_SR_4.GetBinError(2,2)
    errs["P3 HLM SR obs"] = h_HLM_SR_4.GetBinError(2,1)
    errs["P4 HLM SR STC"] = h_HLM_SR_4.GetBinError(3,3)
    errs["P4 HLM SR pre"] = h_HLM_SR_4.GetBinError(3,2)
    errs["P4 HLM SR obs"] = h_HLM_SR_4.GetBinError(3,1)
    errs["M HLM SR STC"] = h_HLM_SR_4.GetBinError(4,3)
    errs["M HLM SR pre"] = h_HLM_SR_4.GetBinError(4,2)
    errs["M HLM SR obs"] = h_HLM_SR_4.GetBinError(4,1)
    errs["L HLM SR STC"] = h_HLM_SR_4.GetBinError(5,3)
    errs["L HLM SR pre"] = h_HLM_SR_4.GetBinError(5,2)
    errs["L HLM SR obs"] = h_HLM_SR_4.GetBinError(5,1)

    errs["P HM SR STC"] = h_HM_SR_4.GetBinError(1,3)
    errs["P HM SR pre"] = h_HM_SR_4.GetBinError(1,2)
    errs["P HM SR obs"] = h_HM_SR_4.GetBinError(1,1)
    errs["P3 HM SR STC"] = h_HM_SR_4.GetBinError(2,3)
    errs["P3 HM SR pre"] = h_HM_SR_4.GetBinError(2,2)
    errs["P3 HM SR obs"] = h_HM_SR_4.GetBinError(2,1)
    errs["P4 HM SR STC"] = h_HM_SR_4.GetBinError(3,3)
    errs["P4 HM SR pre"] = h_HM_SR_4.GetBinError(3,2)
    errs["P4 HM SR obs"] = h_HM_SR_4.GetBinError(3,1)
    errs["M HM SR STC"] = h_HM_SR_4.GetBinError(4,3)
    errs["M HM SR pre"] = h_HM_SR_4.GetBinError(4,2)
    errs["M HM SR obs"] = h_HM_SR_4.GetBinError(4,1)
    errs["L HM SR STC"] = h_HM_SR_4.GetBinError(5,3)
    errs["L HM SR pre"] = h_HM_SR_4.GetBinError(5,2)
    errs["L HM SR obs"] = h_HM_SR_4.GetBinError(5,1)

    errs["P HH SR STC"] = h_HH_SR_4.GetBinError(1,3)
    errs["P HH SR pre"] = h_HH_SR_4.GetBinError(1,2)
    errs["P HH SR obs"] = h_HH_SR_4.GetBinError(1,1)
    errs["P3 HH SR STC"] = h_HH_SR_4.GetBinError(2,3)
    errs["P3 HH SR pre"] = h_HH_SR_4.GetBinError(2,2)
    errs["P3 HH SR obs"] = h_HH_SR_4.GetBinError(2,1)
    errs["P4 HH SR STC"] = h_HH_SR_4.GetBinError(3,3)
    errs["P4 HH SR pre"] = h_HH_SR_4.GetBinError(3,2)
    errs["P4 HH SR obs"] = h_HH_SR_4.GetBinError(3,1)
    errs["M HH SR STC"] = h_HH_SR_4.GetBinError(4,3)
    errs["M HH SR pre"] = h_HH_SR_4.GetBinError(4,2)
    errs["M HH SR obs"] = h_HH_SR_4.GetBinError(4,1)
    errs["L HH SR STC"] = h_HH_SR_4.GetBinError(5,3)
    errs["L HH SR pre"] = h_HH_SR_4.GetBinError(5,2)
    errs["L HH SR obs"] = h_HH_SR_4.GetBinError(5,1)

    # hi pt
    errs["P LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(1,3)
    errs["P LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(1,2)
    errs["P LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(1,1)
    errs["P3 LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(2,3)
    errs["P3 LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(2,2)
    errs["P3 LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(2,1)
    errs["P4 LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(3,3)
    errs["P4 LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(3,2)
    errs["P4 LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(3,1)
    errs["M LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(4,3)
    errs["M LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(4,2)
    errs["M LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(4,1)
    errs["L LL SR hi STC"] = h_LL_SR_23_hi.GetBinError(5,3)
    errs["L LL SR hi pre"] = h_LL_SR_23_hi.GetBinError(5,2)
    errs["L LL SR hi obs"] = h_LL_SR_23_hi.GetBinError(5,1)

    errs["P LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(1,3)
    errs["P LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(1,2)
    errs["P LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(1,1)
    errs["P3 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(2,3)
    errs["P3 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(2,2)
    errs["P3 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(2,1)
    errs["P4 LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(3,3)
    errs["P4 LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(3,2)
    errs["P4 LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(3,1)
    errs["M LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(4,3)
    errs["M LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(4,2)
    errs["M LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(4,1)
    errs["L LLM SR hi STC"] = h_LLM_SR_23_hi.GetBinError(5,3)
    errs["L LLM SR hi pre"] = h_LLM_SR_23_hi.GetBinError(5,2)
    errs["L LLM SR hi obs"] = h_LLM_SR_23_hi.GetBinError(5,1)

    errs["P LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(1,3)
    errs["P LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(1,2)
    errs["P LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(1,1)
    errs["P3 LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(2,3)
    errs["P3 LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(2,2)
    errs["P3 LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(2,1)
    errs["P4 LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(3,3)
    errs["P4 LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(3,2)
    errs["P4 LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(3,1)
    errs["M LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(4,3)
    errs["M LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(4,2)
    errs["M LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(4,1)
    errs["L LM SR hi STC"] = h_LM_SR_23_hi.GetBinError(5,3)
    errs["L LM SR hi pre"] = h_LM_SR_23_hi.GetBinError(5,2)
    errs["L LM SR hi obs"] = h_LM_SR_23_hi.GetBinError(5,1)

    errs["P LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(1,3)
    errs["P LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(1,2)
    errs["P LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(1,1)
    errs["P3 LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(2,3)
    errs["P3 LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(2,2)
    errs["P3 LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(2,1)
    errs["P4 LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(3,3)
    errs["P4 LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(3,2)
    errs["P4 LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(3,1)
    errs["M LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(4,3)
    errs["M LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(4,2)
    errs["M LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(4,1)
    errs["L LH SR hi STC"] = h_LH_SR_23_hi.GetBinError(5,3)
    errs["L LH SR hi pre"] = h_LH_SR_23_hi.GetBinError(5,2)
    errs["L LH SR hi obs"] = h_LH_SR_23_hi.GetBinError(5,1)

    errs["P HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(1,3)
    errs["P HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(1,2)
    errs["P HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(1,1)
    errs["P3 HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(2,3)
    errs["P3 HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(2,2)
    errs["P3 HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(2,1)
    errs["P4 HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(3,3)
    errs["P4 HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(3,2)
    errs["P4 HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(3,1)
    errs["M HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(4,3)
    errs["M HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(4,2)
    errs["M HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(4,1)
    errs["L HL SR hi STC"] = h_HL_SR_4_hi.GetBinError(5,3)
    errs["L HL SR hi pre"] = h_HL_SR_4_hi.GetBinError(5,2)
    errs["L HL SR hi obs"] = h_HL_SR_4_hi.GetBinError(5,1)

    errs["P HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(1,3)
    errs["P HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(1,2)
    errs["P HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(1,1)
    errs["P3 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(2,3)
    errs["P3 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(2,2)
    errs["P3 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(2,1)
    errs["P4 HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(3,3)
    errs["P4 HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(3,2)
    errs["P4 HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(3,1)
    errs["M HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(4,3)
    errs["M HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(4,2)
    errs["M HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(4,1)
    errs["L HLM SR hi STC"] = h_HLM_SR_4_hi.GetBinError(5,3)
    errs["L HLM SR hi pre"] = h_HLM_SR_4_hi.GetBinError(5,2)
    errs["L HLM SR hi obs"] = h_HLM_SR_4_hi.GetBinError(5,1)

    errs["P HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(1,3)
    errs["P HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(1,2)
    errs["P HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(1,1)
    errs["P3 HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(2,3)
    errs["P3 HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(2,2)
    errs["P3 HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(2,1)
    errs["P4 HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(3,3)
    errs["P4 HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(3,2)
    errs["P4 HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(3,1)
    errs["M HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(4,3)
    errs["M HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(4,2)
    errs["M HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(4,1)
    errs["L HM SR hi STC"] = h_HM_SR_4_hi.GetBinError(5,3)
    errs["L HM SR hi pre"] = h_HM_SR_4_hi.GetBinError(5,2)
    errs["L HM SR hi obs"] = h_HM_SR_4_hi.GetBinError(5,1)

    errs["P HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(1,3)
    errs["P HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(1,2)
    errs["P HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(1,1)
    errs["P3 HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(2,3)
    errs["P3 HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(2,2)
    errs["P3 HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(2,1)
    errs["P4 HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(3,3)
    errs["P4 HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(3,2)
    errs["P4 HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(3,1)
    errs["M HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(4,3)
    errs["M HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(4,2)
    errs["M HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(4,1)
    errs["L HH SR hi STC"] = h_HH_SR_4_hi.GetBinError(5,3)
    errs["L HH SR hi pre"] = h_HH_SR_4_hi.GetBinError(5,2)
    errs["L HH SR hi obs"] = h_HH_SR_4_hi.GetBinError(5,1)

    # lo pt
    errs["P LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(1,3)
    errs["P LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(1,2)
    errs["P LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(1,1)
    errs["P3 LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(2,3)
    errs["P3 LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(2,2)
    errs["P3 LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(2,1)
    errs["P4 LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(3,3)
    errs["P4 LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(3,2)
    errs["P4 LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(3,1)
    errs["M LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(4,3)
    errs["M LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(4,2)
    errs["M LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(4,1)
    errs["L LL SR lo STC"] = h_LL_SR_23_lo.GetBinError(5,3)
    errs["L LL SR lo pre"] = h_LL_SR_23_lo.GetBinError(5,2)
    errs["L LL SR lo obs"] = h_LL_SR_23_lo.GetBinError(5,1)

    errs["P LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(1,3)
    errs["P LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(1,2)
    errs["P LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(1,1)
    errs["P3 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(2,3)
    errs["P3 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(2,2)
    errs["P3 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(2,1)
    errs["P4 LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(3,3)
    errs["P4 LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(3,2)
    errs["P4 LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(3,1)
    errs["M LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(4,3)
    errs["M LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(4,2)
    errs["M LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(4,1)
    errs["L LLM SR lo STC"] = h_LLM_SR_23_lo.GetBinError(5,3)
    errs["L LLM SR lo pre"] = h_LLM_SR_23_lo.GetBinError(5,2)
    errs["L LLM SR lo obs"] = h_LLM_SR_23_lo.GetBinError(5,1)

    errs["P LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(1,3)
    errs["P LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(1,2)
    errs["P LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(1,1)
    errs["P3 LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(2,3)
    errs["P3 LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(2,2)
    errs["P3 LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(2,1)
    errs["P4 LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(3,3)
    errs["P4 LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(3,2)
    errs["P4 LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(3,1)
    errs["M LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(4,3)
    errs["M LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(4,2)
    errs["M LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(4,1)
    errs["L LM SR lo STC"] = h_LM_SR_23_lo.GetBinError(5,3)
    errs["L LM SR lo pre"] = h_LM_SR_23_lo.GetBinError(5,2)
    errs["L LM SR lo obs"] = h_LM_SR_23_lo.GetBinError(5,1)

    errs["P LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(1,3)
    errs["P LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(1,2)
    errs["P LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(1,1)
    errs["P3 LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(2,3)
    errs["P3 LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(2,2)
    errs["P3 LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(2,1)
    errs["P4 LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(3,3)
    errs["P4 LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(3,2)
    errs["P4 LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(3,1)
    errs["M LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(4,3)
    errs["M LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(4,2)
    errs["M LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(4,1)
    errs["L LH SR lo STC"] = h_LH_SR_23_lo.GetBinError(5,3)
    errs["L LH SR lo pre"] = h_LH_SR_23_lo.GetBinError(5,2)
    errs["L LH SR lo obs"] = h_LH_SR_23_lo.GetBinError(5,1)

    errs["P HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(1,3)
    errs["P HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(1,2)
    errs["P HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(1,1)
    errs["P3 HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(2,3)
    errs["P3 HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(2,2)
    errs["P3 HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(2,1)
    errs["P4 HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(3,3)
    errs["P4 HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(3,2)
    errs["P4 HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(3,1)
    errs["M HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(4,3)
    errs["M HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(4,2)
    errs["M HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(4,1)
    errs["L HL SR lo STC"] = h_HL_SR_4_lo.GetBinError(5,3)
    errs["L HL SR lo pre"] = h_HL_SR_4_lo.GetBinError(5,2)
    errs["L HL SR lo obs"] = h_HL_SR_4_lo.GetBinError(5,1)

    errs["P HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(1,3)
    errs["P HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(1,2)
    errs["P HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(1,1)
    errs["P3 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(2,3)
    errs["P3 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(2,2)
    errs["P3 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(2,1)
    errs["P4 HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(3,3)
    errs["P4 HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(3,2)
    errs["P4 HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(3,1)
    errs["M HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(4,3)
    errs["M HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(4,2)
    errs["M HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(4,1)
    errs["L HLM SR lo STC"] = h_HLM_SR_4_lo.GetBinError(5,3)
    errs["L HLM SR lo pre"] = h_HLM_SR_4_lo.GetBinError(5,2)
    errs["L HLM SR lo obs"] = h_HLM_SR_4_lo.GetBinError(5,1)

    errs["P HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(1,3)
    errs["P HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(1,2)
    errs["P HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(1,1)
    errs["P3 HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(2,3)
    errs["P3 HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(2,2)
    errs["P3 HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(2,1)
    errs["P4 HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(3,3)
    errs["P4 HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(3,2)
    errs["P4 HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(3,1)
    errs["M HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(4,3)
    errs["M HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(4,2)
    errs["M HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(4,1)
    errs["L HM SR lo STC"] = h_HM_SR_4_lo.GetBinError(5,3)
    errs["L HM SR lo pre"] = h_HM_SR_4_lo.GetBinError(5,2)
    errs["L HM SR lo obs"] = h_HM_SR_4_lo.GetBinError(5,1)

    errs["P HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(1,3)
    errs["P HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(1,2)
    errs["P HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(1,1)
    errs["P3 HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(2,3)
    errs["P3 HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(2,2)
    errs["P3 HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(2,1)
    errs["P4 HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(3,3)
    errs["P4 HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(3,2)
    errs["P4 HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(3,1)
    errs["M HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(4,3)
    errs["M HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(4,2)
    errs["M HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(4,1)
    errs["L HH SR lo STC"] = h_HH_SR_4_lo.GetBinError(5,3)
    errs["L HH SR lo pre"] = h_HH_SR_4_lo.GetBinError(5,2)
    errs["L HH SR lo obs"] = h_HH_SR_4_lo.GetBinError(5,1)

    # SR systs; The systs here are based on VR, so the VR in the hist names is intentional, not a bug. 
    # (These percentage values are all the same as for the VR equivalents, above.)
    if h_LL_VR_23_ns != None:
        systs["P LL SR"] = h_LL_VR_23_ns.GetBinContent(1)
        systs["P3 LL SR"] = h_LL_VR_23_ns.GetBinContent(2)
        systs["P4 LL SR"] = h_LL_VR_23_ns.GetBinContent(3)
        systs["M LL SR"] = h_LL_VR_23_ns.GetBinContent(4)
        systs["L LL SR"] = h_LL_VR_23_ns.GetBinContent(5)
        systs["P LLM SR"] = h_LLM_VR_23_ns.GetBinContent(1)
        systs["P3 LLM SR"] = h_LLM_VR_23_ns.GetBinContent(2)
        systs["P4 LLM SR"] = h_LLM_VR_23_ns.GetBinContent(3)
        systs["M LLM SR"] = h_LLM_VR_23_ns.GetBinContent(4)
        systs["L LLM SR"] = h_LLM_VR_23_ns.GetBinContent(5)
        systs["P LM SR"] = h_LM_VR_23_ns.GetBinContent(1)
        systs["P3 LM SR"] = h_LM_VR_23_ns.GetBinContent(2)
        systs["P4 LM SR"] = h_LM_VR_23_ns.GetBinContent(3)
        systs["M LM SR"] = h_LM_VR_23_ns.GetBinContent(4)
        systs["L LM SR"] = h_LM_VR_23_ns.GetBinContent(5)
        systs["P LH SR"] = h_LH_VR_23_ns.GetBinContent(1)
        systs["P3 LH SR"] = h_LH_VR_23_ns.GetBinContent(2)
        systs["P4 LH SR"] = h_LH_VR_23_ns.GetBinContent(3)
        systs["M LH SR"] = h_LH_VR_23_ns.GetBinContent(4)
        systs["L LH SR"] = h_LH_VR_23_ns.GetBinContent(5)
        systs["P HL SR"] = h_HL_VR_4_ns.GetBinContent(1)
        systs["P3 HL SR"] = h_HL_VR_4_ns.GetBinContent(2)
        systs["P4 HL SR"] = h_HL_VR_4_ns.GetBinContent(3)
        systs["M HL SR"] = h_HL_VR_4_ns.GetBinContent(4)
        systs["L HL SR"] = h_HL_VR_4_ns.GetBinContent(5)
        systs["P HLM SR"] = h_HLM_VR_4_ns.GetBinContent(1)
        systs["P3 HLM SR"] = h_HLM_VR_4_ns.GetBinContent(2)
        systs["P4 HLM SR"] = h_HLM_VR_4_ns.GetBinContent(3)
        systs["M HLM SR"] = h_HLM_VR_4_ns.GetBinContent(4)
        systs["L HLM SR"] = h_HLM_VR_4_ns.GetBinContent(5)
        systs["P HM SR"] = h_HM_VR_4_ns.GetBinContent(1)
        systs["P3 HM SR"] = h_HM_VR_4_ns.GetBinContent(2)
        systs["P4 HM SR"] = h_HM_VR_4_ns.GetBinContent(3)
        systs["M HM SR"] = h_HM_VR_4_ns.GetBinContent(4)
        systs["L HM SR"] = h_HM_VR_4_ns.GetBinContent(5)
        systs["P HH SR"] = h_HH_VR_4_ns.GetBinContent(1)
        systs["P3 HH SR"] = h_HH_VR_4_ns.GetBinContent(2)
        systs["P4 HH SR"] = h_HH_VR_4_ns.GetBinContent(3)
        systs["M HH SR"] = h_HH_VR_4_ns.GetBinContent(4)
        systs["L HH SR"] = h_HH_VR_4_ns.GetBinContent(5)
        systs["P LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(3)
        systs["M LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(4)
        systs["L LL SR hi"] = h_LL_VR_23_hi_ns.GetBinContent(5)
        systs["P LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(3)
        systs["M LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(4)
        systs["L LLM SR hi"] = h_LLM_VR_23_hi_ns.GetBinContent(5)
        systs["P LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(3)
        systs["M LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(4)
        systs["L LM SR hi"] = h_LM_VR_23_hi_ns.GetBinContent(5)
        systs["P LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(1)
        systs["P3 LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(2)
        systs["P4 LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(3)
        systs["M LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(4)
        systs["L LH SR hi"] = h_LH_VR_23_hi_ns.GetBinContent(5)
        systs["P HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(3)
        systs["M HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(4)
        systs["L HL SR hi"] = h_HL_VR_4_hi_ns.GetBinContent(5)
        systs["P HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(3)
        systs["M HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(4)
        systs["L HLM SR hi"] = h_HLM_VR_4_hi_ns.GetBinContent(5)
        systs["P HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(3)
        systs["M HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(4)
        systs["L HM SR hi"] = h_HM_VR_4_hi_ns.GetBinContent(5)
        systs["P HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(1)
        systs["P3 HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(2)
        systs["P4 HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(3)
        systs["M HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(4)
        systs["L HH SR hi"] = h_HH_VR_4_hi_ns.GetBinContent(5)
        systs["P LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(3)
        systs["M LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(4)
        systs["L LL SR lo"] = h_LL_VR_23_lo_ns.GetBinContent(5)
        systs["P LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(3)
        systs["M LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(4)
        systs["L LLM SR lo"] = h_LLM_VR_23_lo_ns.GetBinContent(5)
        systs["P LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(3)
        systs["M LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(4)
        systs["L LM SR lo"] = h_LM_VR_23_lo_ns.GetBinContent(5)
        systs["P LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(1)
        systs["P3 LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(2)
        systs["P4 LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(3)
        systs["M LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(4)
        systs["L LH SR lo"] = h_LH_VR_23_lo_ns.GetBinContent(5)
        systs["P HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(3)
        systs["M HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(4)
        systs["L HL SR lo"] = h_HL_VR_4_lo_ns.GetBinContent(5)
        systs["P HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(3)
        systs["M HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(4)
        systs["L HLM SR lo"] = h_HLM_VR_4_lo_ns.GetBinContent(5)
        systs["P HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(3)
        systs["M HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(4)
        systs["L HM SR lo"] = h_HM_VR_4_lo_ns.GetBinContent(5)
        systs["P HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(1)
        systs["P3 HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(2)
        systs["P4 HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(3)
        systs["M HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(4)
        systs["L HH SR lo"] = h_HH_VR_4_lo_ns.GetBinContent(5)

    return vals,errs,systs

def makePlot(regions,vals,errs,systs,desc): 
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Normalized Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    ptindices = ["dummy0"]
    tag_index = 1
    for region in regions:
        ptindices.append(0 if region.find("lo") >= 0 else 1)
        hpred.GetXaxis().SetBinLabel(tag_index,region)
        pred = vals[region+" pre"]
        perr = errs[region+" pre"]
        obs = vals[region+" obs"]
        oerr = 1.83 if obs == 0 else errs[region+" obs"]
        if obs == 0 and verbose:
            print "In region",region,"of",desc,", observation was 0, so setting oerr to 1.83"
        if pred > 0:
            hpred.SetBinContent(tag_index,1) # Set pred to 1 and rescale error below
            hpred.SetBinError(tag_index,perr/pred)
            hobs.SetBinContent(tag_index,obs/pred)
            hobs.SetBinError(tag_index,oerr/pred)
        else: # if prediction is 0, just plot the raw counts
            hpred.SetBinContent(tag_index,0)
            hpred.SetBinError(tag_index,perr)
            hobs.SetBinContent(tag_index,obs)
            hobs.SetBinError(tag_index,oerr)
        tag_index += 1    
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(3)
    hobs.SetLineColor(ROOT.kRed)
    hpred.SetLineColor(ROOT.kBlack)
    hpred_nofill = hpred.Clone(hpred.GetName()+"nofill")
    hpred_syst = hpred.Clone(hpred.GetName()+"_syst")
    for bin in range(1,hpred_syst.GetNbinsX()+1):
        hpred_syst.SetBinError(bin,sqrt(hpred.GetBinError(bin)**2+nonclosure_error[ptindices[bin]]**2)) # nonclosure error is syst/sqrt(prediction)...kinda weird to normalize here
    hpred.SetFillColor(ROOT.kGray)
    hpred_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction, with f_{short} Stat & Syst")
    tl.AddEntry(hpred_syst,"Prediction, with Non-closure Syst")
    hpred_syst.Draw("E2")
    hpred.Draw("E2 same")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    simplecanvas.SaveAs("{}/{}.{}".format(plotdir,desc.replace(" ","_"),format))

def makePlotRawOld(regions,vals,errs,systs,desc,rescale=1.0): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    ratiocanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hobs=ROOT.TH1D(desc,desc+";;Short Track Counts",nregions,0,nregions)
    hobs.SetLineWidth(3)
    hpred=hobs.Clone(hobs.GetName()+"_prediction")
    perrs = []
    oerrs = []
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred.GetXaxis().SetBinLabel(bin_index,region)
        pred = vals[region+" pre"]*rescale
        perr = errs[region+" pre"]*rescale
        obs = vals[region+" obs"]
        oerr = errs[region+" obs"]
        hpred.SetBinContent(bin_index,pred)
        perrs.append(perr)
        hobs.SetBinContent(bin_index,obs)
        oerrs.append(oerr)
    gpred = getPoissonGraph( hpred, perrs )
    gobs = getPoissonGraph( hobs, oerrs )
    hpred.GetXaxis().LabelsOption("v")
    hpred.GetXaxis().SetTitleOffset(4.8)
    hpred.SetMinimum(-0.001)
    hpred.SetMaximum(2.0*max(hpred.GetMaximum(),hobs.GetMaximum()))
    hobs.SetLineColor(ROOT.kRed)
    hpred.SetLineColor(ROOT.kBlack)
    hpred_nofill = hpred.Clone(hpred.GetName()+"nofill")
    hpred_syst = hpred.Clone(hpred.GetName()+"_syst")
    for index,region in enumerate(regions):
        bin_index = index+1
        hpred_syst.SetBinError(bin_index,sqrt(hpred_syst.GetBinError(bin_index)**2+(systs[region]*hpred_syst.GetBinContent(bin_index))**2))
    hpred.SetFillColor(ROOT.kGray)
    hpred_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hobs,"Observation")
    tl.AddEntry(hpred,"Prediction, with f_{short} Stat & Syst")
    tl.AddEntry(hpred_syst,"Prediction, with Non-Closure Systematic")
    pads[0].cd()
    hpred_syst.Draw("E2")
    hpred.Draw("E2 same")
    hpred_nofill.Draw("hist same")
    hobs.Draw("same E0")
    tl.Draw()
    hratio = hobs.Clone("ratio"+desc)
    hratio.SetTitle("")
    hratio.SetLineColor(ROOT.kRed)
    hratio.SetFillColor(0)
    hratio.GetYaxis().SetLabelSize(hpred_syst.GetYaxis().GetLabelSize()*.83/.16/2)
    hratio.GetYaxis().SetTitleSize(hpred_syst.GetYaxis().GetTitleSize()*.83/.16/2)
    hratio.GetYaxis().SetTitleOffset(0.35)
    hratio.GetYaxis().SetTitle("Obs/Pred")
    hratio.Divide(hpred)
    for bin in range(1,hratio.GetNbinsX()+1):
        hratio.SetBinError(bin,hobs.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
    pads[1].cd()
    h1 = hratio.Clone(region+"1")
    hprojection = ROOT.TH1D("h_projection_"+desc,"Distribution of Obs/Pred Ratios;Obs/Pred;Region Count",12,0,3)
    hprojection.SetLineWidth(3)
    hprojection.SetLineColor(ROOT.kBlack)
    maxval = 0
    for bin in range(1,hratio.GetNbinsX()+1):
        h1.SetBinContent(bin,1)
        h1.GetXaxis().SetBinLabel(bin,"")
        h1.SetBinError(bin,0.0)
        ratio_content = hratio.GetBinContent(bin)
        maxval = max(ratio_content,maxval)
        if ratio_content >= 3: ratio_content = 2.99
        if ratio_content == 0: continue
        hprojection.Fill(ratio_content)
    h1_err = h1.Clone(region+"1err")
    h1_systerr = h1.Clone(region+"1systerr")
    for bin in range(1,hratio.GetNbinsX()+1):
        h1_err.SetBinError(bin,hpred.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
        h1_systerr.SetBinError(bin,hpred_syst.GetBinError(bin)/hpred.GetBinContent(bin) if hpred.GetBinContent(bin) > 0 else 0)
    h1_err.SetFillColor(ROOT.kGray)
    h1_systerr.SetFillColor(ROOT.kGray+2)
    h1_systerr.SetMinimum(-0.01)
    h1_systerr.SetMaximum(min(round(maxval+1),5))
    h1_systerr.SetLineColor(ROOT.kBlack)
    h1_systerr.Draw("E2")
    h1_err.Draw("E2 same")
    h1.SetLineColor(ROOT.kBlack)
    h1.Draw("hist same")
    hratio.Draw("same")
    ratiocanvas.SaveAs("{}/{}_raw.{}".format(plotdir,desc.replace(" ","_"),format))
    simplecanvas.cd()
    hprojection.Draw()
    simplecanvas.SaveAs("{}/{}_ratios_zerosuppressed.{}".format(plotdir,desc.replace(" ","_"),format))

def makePlotDiscrepanciesOld(regions_sets,vals_sets,errs_sets,systs_sets,desc,onlyNonMin=False,rescale=[1.0,1.0]): # Raw means non-normalized. "rescale" multiplies prediction, to enable partial unblinding.
    simplecanvas.cd()
    tl.Clear()
    # X.001 so slightly over X.0 is not in overlow, specifically for floating point weirdness in VR, which is +/- 1.0 by construction
    maxsigma = 1.001 if desc.find("VR") >= 0 else 2.001
    nbins = 10 if desc.find("VR") >= 0 else 20
    hsigma=ROOT.TH1D(desc,"N_{Pred} - N_{Obs}, "+desc+" Regions, N_{obs} > 0;(N_{Pred}-N_{Obs})/Error;Region Count",nbins+1,-maxsigma,maxsigma)
    hsigma.SetLineWidth(3)
    print "begin mpd"
    for set_index in range(len(regions_sets)):
        regions = regions_sets[set_index]
        vals = vals_sets[set_index]
        errs = errs_sets[set_index]
        systs = systs_sets[set_index]
        for index,region in enumerate(regions):
            bin_index = index+1
            pred = vals[region+" pre"]*rescale[set_index]
            perr = errs[region+" pre"]*rescale[set_index]
#            if pred < 0.5: continue
            obs = vals[region+" obs"]
            oerr = errs[region+" obs"]
            if oerr == 0: oerr = 1.83
            perr_syst = sqrt(perr**2 + (systs[region]*pred)**2)
            delta = pred - obs
            total_err = sqrt( oerr**2 + perr_syst**2 )
            sigma = delta/total_err
            if hsigma.FindBin(sigma) == 0 or hsigma.FindBin(sigma) == hsigma.GetNbinsX()+1:
                print "Sigma =", sigma, "is overflow", desc, "Setting to fall in max bin"
                sigma = maxsigma - 0.001 if sigma > 0 else maxsigma + 0.001 # Automatic overflow bins
            hsigma.Fill(sigma)
    print "end mpd"
    hsigma.SetMinimum(0)
    hsigma.SetMaximum(1.5*hsigma.GetMaximum())
    hsigma.SetLineColor(ROOT.kBlack)
    hsigma.Draw("hist")
    tl.AddEntry(hsigma,"(N_{{Pred}}-N_{{Obs}})/#sigma, Mean = {:.2f}, Deviation = {:.2f}".format(hsigma.GetMean(),hsigma.GetStdDev()))
    tl.Draw()
    simplecanvas.SaveAs("{}/{}_sigma_zerosuppressed.{}".format(plotdir,desc.replace(" ","_"),format))


def makeSignalPlotOld(regions,vals_bg,errs_bg,list_of_vals_sig,list_of_errs_sig, systs, rescale_lumi, desc, sig_tags, sig_colors): # Don't rescale counts
    simplecanvas.cd()
    tl.Clear()
    nregions=len(regions)
    hbg=ROOT.TH1D(desc+"_counts",desc+";;Short Track Counts",nregions,0,nregions)
    hbg.SetLineWidth(3)
    hsigs = [hbg.Clone(hbg.GetName()+"_withSig_"+sig_tag) for sig_tag in sig_tags]
    for index,region in enumerate(regions):
        bin_index = index+1
        hbg.GetXaxis().SetBinLabel(bin_index,region)
        val_bg = vals_bg[region+" pre"]
        err_bg = errs_bg[region+" pre"]
        hbg.SetBinContent(bin_index,val_bg)
        hbg.SetBinError(bin_index,err_bg)
        for sig_index,sig_tag in enumerate(sig_tags):
            vals_sig = list_of_vals_sig[sig_index]
            errs_sig = list_of_errs_sig[sig_index]
            hsig = hsigs[sig_index]
            val_sig = vals_sig[region+" obs"] * rescale_lumi
            err_sig = errs_sig[region+" obs"]
            hsig.SetBinContent(bin_index,val_sig+val_bg) # Add val_bg so sig is sitting atop background, for easy error comparison
            hsig.SetBinError(bin_index,err_sig)
    hbg.GetXaxis().LabelsOption("v")
    hbg.GetXaxis().SetTitleOffset(4.8)
    hbg.SetMinimum(-0.001)
    hbg.SetMaximum(2.5*hsig.GetMaximum())
    for sig_index,hsig in enumerate(hsigs):
        hsig.SetLineColor(sig_colors[sig_index])
    hbg.SetLineColor(ROOT.kBlack)
    hbg_nofill = hbg.Clone(hbg.GetName()+"nofill")
    hbg_syst = hbg.Clone(hbg.GetName()+"_syst")
    for index,region in enumerate(regions):
        bin_index = index+1
        hbg_syst.SetBinError(bin_index,sqrt(hbg.GetBinError(bin_index)**2+(systs[region]*hbg.GetBinContent(bin_index))**2))
    hbg.SetFillColor(ROOT.kGray)
    hbg_syst.SetFillColor(ROOT.kGray+2)
    tl.AddEntry(hbg,"Predicted BG")
    tl.AddEntry(hbg_syst,"Predicted BG, with Non-closure Syst")
    hbg_syst.Draw("E2")
    hbg.Draw("E2 same")
    hbg_nofill.Draw("hist same")    
    for sig_index,hsig in enumerate(hsigs):
        hsig.Draw("same E0")
        tl.AddEntry(hsig,sig_tags[sig_index]+" + Pred BG (Signal MC Errors)")
    tl.Draw()
    simplecanvas.SaveAs("{}/{}_counts.{}".format(plotdir,desc.replace(" ","_").replace("(","").replace(")","").replace(",",""),format))

def getMergedLineDataSTCOld(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, sD16[region]*D16[region+" pre"]*rescale16, D16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f} & - & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, sD1718[region]*D1718[region+" pre"]*rescale1718, D1718[region+" obs"])

    else:
        return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" STC"], D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, sD1718[region]*D1718[region+" pre"]*rescale1718, D1718[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, sD16[region]*D16[region+" pre"]*rescale16, D16[region+" obs"])

def getMergedLineMCSTCOld(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst)& {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),

                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], sM16[region]*M16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.3f} $\pm$ {:.3f} & - & - & - \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], sM17[region]*M17[region+" pre"], M17[region+" obs"], eM17[region+" obs"])

    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst)& {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} $\pm$ {:.3f} (non-closure syst) & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], sM17[region]*M17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], sM16[region]*M16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])


def getLineData(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D18[region+" pre"], eD18[region+" pre"], D18[region+" obs"],
                                                                                                                                                                                                                                    D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

def getLineDataSTC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D18[region+" STC"], D18[region+" pre"], eD18[region+" pre"], D18[region+" obs"],
                                                                                                                                                                                                                                    D17[region+" STC"], D17[region+" pre"], eD17[region+" pre"], D17[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" STC"], D16[region+" pre"], eD16[region+" pre"], D16[region+" obs"])

def getLineMCSTC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" STC"], eM17[region+" STC"], M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" STC"], eM16[region+" STC"], M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])


def getMergedLineDataOld(region,rescale16=1.0,rescale1718=1.0): # rescale multiplies prediction, to enable partial unblinding
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # don't return 2017-2018
        return colorline+"{} & - & - & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, D16[region+" obs"])
    elif cat == "P3" or cat == "P4": # don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & - & -\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, D1718[region+" obs"])
    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.0f} & {:.3f} $\pm$ {:.3f} & {:.0f}\\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    D1718[region+" pre"]*rescale1718, eD1718[region+" pre"]*rescale1718, D1718[region+" obs"],
                                                                                                                                                                                                                                    D16[region+" pre"]*rescale16, eD16[region+" pre"]*rescale16, D16[region+" obs"])

def getMergedLineMC(region):
    if region[0] == "P":
        colorline = "\\rowcolor{green!25}"
    elif region[0] == "M":
        colorline = "\\rowcolor{blue!25}"
    else:
        colorline = "\\rowcolor{red!25}"
    cat = region[0:2]
    if cat == "P ": # Don't return 2017-18
        return colorline+"{} & - & - & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),

                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

    elif cat == "P3" or cat == "P4": # Don't return 2016
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & - & - \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"])
    else:
        return colorline+"{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} \\\\ \n".format(region.replace(" VR","").replace(" SR",""),
                                                                                                                                                                                                                                    M17[region+" pre"], eM17[region+" pre"], M17[region+" obs"], eM17[region+" obs"],
                                  M16[region+" pre"], eM16[region+" pre"], M16[region+" obs"], eM16[region+" obs"])

def getFSLineDataOld(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,D18f[cat],eD18f[cat],D17f[cat],eD17f[cat],D16f[cat],eD16f[cat])

def getFSLineMCOld(cat):
    return "{} & {:.3f} $\pm$ {:.3f} & {:.3f} $\pm$ {:.3f}\\\\ \n".format(cat,M17f[cat],eM17f[cat],M16f[cat],eM16f[cat])

def getMergedFSLineDataOld(cat):
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return "{} & - & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D16f[cat],eD16f[cat],sD16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,D1718f[cat],eD1718f[cat],sD1718f[cat])
    else:
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,D1718f[cat],eD1718f[cat],sD1718f[cat],D16f[cat],eD16f[cat],sD16f[cat])

def getMergedFSLineMCOld(cat):
    subcat = cat[0:2]
    if subcat == "P ": # Don't return 2017-18
        return "{} & - & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst)\\\\ \n".format(cat,M16f[cat],eM16f[cat],sM16f[cat])
    elif subcat == "P3" or subcat == "P4": # don't return 2016
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & -\\\\ \n".format(cat,M17f[cat],eM17f[cat],sM17f[cat])
    else:
        return "{} & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) & {:.3f} $\pm$ {:.3f} (stat) $\pm$ {:.3f} (syst) \\\\ \n".format(cat,M17f[cat],eM17f[cat],sM17f[cat],M16f[cat],eM16f[cat],sM16f[cat])
