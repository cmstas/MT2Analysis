import ROOT as r
import glob
import os
import pyRootPlotMaker as ppm

r.gROOT.SetBatch(1)

# tag = "V00-10-01_31Mar2018_2016JRTs_withMonojet_v2"
tag = "V00-10-01_31Mar2018_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"
# tag = "V00-10-01_31Mar2018_usedByJason_withMonojet"
isMC = False
lumi = 41.4
if isMC:
    dirs = ["srbaseJ", "srbaseJ0B", "srbaseJ1B", "crRSInvertDPhibaseJ", "crRSInvertDPhibaseJ0B", "crRSInvertDPhibaseJ1B"]
    hnames = ["J1pt", "htbins"]
    xRangeUser=[(30,330), None]
    isLog=[False, True]
    xnames=["p_{T}(jet1)", "H_{T}"]
else:
    dirs = ["crRSInvertDPhibaseJ", "crRSInvertDPhibaseJ0B", "crRSInvertDPhibaseJ1B"]
    hnames = ["J1pt", "htbins", "htbins_jet2pt_30_60"]
    xRangeUser=[(30,330), None, None]
    isLog=[False, True, True]
    xnames=["p_{T}(jet1)", "H_{T}", "H_{T} (30 < p_{T}(jet2) < 60)"]

dir_RS = "looper_output/{0}/{1}".format(tag, "qcd" if isMC else "data")
dir_data = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"
# dir_data = "../SmearLooper/output/test/"
dir_ewk = "../SmearLooper/output/V00-10-04_94x_2017_noRS"

ewk_samples = ["wjets_ht", "zinv_ht"]

username = os.environ["USER"]

f_rs = r.TFile(os.path.join(dir_RS,"merged_hists.root"))
f_data = r.TFile(os.path.join(dir_data,"data_Run2017.root" if not isMC else "qcd_ht.root"))
if not isMC:
    f_ewk = [r.TFile(os.path.join(dir_ewk,"{0}.root".format(samp))) for samp in ewk_samples]

for dir in dirs:
    outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/monojet/{2}".format(username,tag,dir)
    os.system("mkdir -p "+outdir)
    os.system("cp ~/scripts/index.php "+outdir)
    for ih, hname in enumerate(hnames):
        h_rs = f_rs.Get("{0}/h_{1}".format(dir,hname))
        h_data = f_data.Get("{0}/h_{1}".format(dir,hname))
        if not isMC:
            h_ewk = [f.Get("{0}/h_{1}".format(dir,hname)) for f in f_ewk]

        if isMC:
            h_rs.Scale(lumi)
            h_data.Scale(lumi)
        else:
            [h.Scale(lumi) for h in h_ewk]


        h_bkg_vec = [h_rs] if isMC else [h_rs] + h_ewk
        bkg_names = ["RS from MC"] if isMC else ["RS from Data"] + ewk_samples

        saveAs=outdir+"/{0}_{1}_{2}.".format("mc" if isMC else "data", dir, hname)
        
        for ext in ["pdf","png"]:
            ppm.plotDataMC(h_bkg_vec, bkg_names, h_data, dataTitle="QCD MC" if isMC else "Data", lumi=lumi,
                           xAxisTitle=xnames[ih], doPause=False, doMT2Colors=True, yRangeUserRatio=(0,3), 
                           xRangeUser=xRangeUser[ih], doOverflow=False, isLog=isLog[ih], saveAs=saveAs+ext)




