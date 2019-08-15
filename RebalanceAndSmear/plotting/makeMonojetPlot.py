import ROOT as r
import glob
import os
import pyRootPlotMaker as ppm

r.gROOT.SetBatch(1)

tag = "V00-10-16_newJER_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"
# tag = "V00-10-04_fullBinning_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"
# tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma"
# tag = "V00-10-01_31Mar2018_usedByJason_withMonojet"
isMC = False

year = "All"
lumi = 137

# year = 2017
# lumi = 41.5

# year = 2016
# lumi = 35.9

if isMC:
    dirs = ["srbaseJ", "srbaseJ0B", "srbaseJ1B", "crRSInvertDPhibaseJ", "crRSInvertDPhibaseJ0B", "crRSInvertDPhibaseJ1B"]
    hnames = ["J1pt", "htbins", "htbins_jet2pt_30_60"]
    xRangeUser=[(30,330), None, None]
    isLog=[False, True, True]
    xnames=["p_{T}(jet1)", "H_{T}", "H_{T} (30 < p_{T}(jet2) < 60)"]
else:
    dirs = ["srbaseJ", "srbaseJ0B", "srbaseJ1B", "crRSInvertDPhibaseJ", "crRSInvertDPhibaseJ0B", "crRSInvertDPhibaseJ1B"]
    hnames = ["J1pt", "htbins", "htbins_jet2pt_30_60"]
    xRangeUser=[(30,330), None, None]
    isLog=[False, True, True]
    xnames=["p_{T}(jet1)", "H_{T}", "H_{T} (30 < p_{T}(jet2) < 60)"]

dir_RS = "looper_output/{0}/{1}".format(tag, "qcd" if isMC else "data{0}".format(year))
dir_data = "../SmearLooper/output/V00-10-16_94x_{0}_noRS/".format("combined" if year=="All" else year)
# dir_data = "../SmearLooper/output/test/"
dir_ewk = "../SmearLooper/output/V00-10-16_94x_{0}_noRS/".format("combined" if year=="All" else year)

ewk_samples = ["wjets_ht", "zinv_ht"]

username = os.environ["USER"]

f_rs = r.TFile(os.path.join(dir_RS,"merged_hists.root"))
f_data = r.TFile(os.path.join(dir_data,"data_Run{0}.root".format(year) if not isMC else "qcd_ht.root"))
if not isMC:
    f_ewk = [r.TFile(os.path.join(dir_ewk,"{0}.root".format(samp))) for samp in ewk_samples]

for dir in dirs:
    outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/monojet/{2}".format(username,tag,dir)
    os.system("mkdir -p "+outdir)
    os.system("cp ~/scripts/index.php "+outdir)
    for ih, hname in enumerate(hnames):
        if "sr" in dir and "30_60" in hname:
            continue

        h_rs = f_rs.Get("{0}/h_{1}".format(dir,hname))
        h_data = f_data.Get("{0}/h_{1}".format(dir,hname))
        if not isMC:
            h_ewk = [f.Get("{0}/h_{1}".format(dir,hname)) for f in f_ewk]

        corr = 3.0 if year=="All" else 1.0
        if isMC:
            h_rs.Scale(lumi/corr)
            h_data.Scale(lumi/corr)
        else:
            [h.Scale(lumi/corr) for h in h_ewk]

        h_bkg_vec = [h_rs] if isMC else [h_rs] + h_ewk
        bkg_names = ["RS from MC"] if isMC else ["RS from Data"] + ewk_samples

        if not isMC and "sr" in dir:
            h_bkg_vec = [h_rs]
            bkg_names = ["RS from Data"]
            h_data = None

        saveAs=outdir+"/{0}_{1}_{2}.".format("mc" if isMC else "data{0}".format(year), dir, hname)
        
        for ext in ["pdf","png"]:
            ppm.plotDataMC(h_bkg_vec, bkg_names, h_data, dataTitle="QCD MC" if isMC else "Data", lumi=lumi,
                           xAxisTitle=xnames[ih], doPause=False, doMT2Colors=True, yRangeUserRatio=(0,3), cmsText="CMS",
                           xRangeUser=xRangeUser[ih], doOverflow=False, isLog=isLog[ih], saveAs=saveAs+ext)




