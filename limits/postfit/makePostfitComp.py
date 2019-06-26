import glob
import ROOT as r
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

f1 = r.TFile("FullRunII_17MCfor18_ttbbWeights_v3_T2qq_1300_850/fitDiagnosticstest_pulls_v2.root")
f2 = r.TFile("FullRunII_17MCfor18_ttbbWeights_v3_T2qq_1300_850/fitDiagnosticstest_pulls_dummyobs_v2.root")

cards = glob.glob("FullRunII_17MCfor18_ttbbWeights_v3_T2qq_1300_850/cards_HT250to350_j1_b0_dummyobs/datacard_*.txt")
bins = [x.split("datacard_")[-1].split("_T2qq")[0] for x in cards]
bins = sorted(bins)

results = []
for i,bin in enumerate(bins):
    chname = "ch{0}".format(i+1)

    h1 = f1.Get("shapes_fit_b/{0}_T2qq_1300_850/total_background".format(bin))
    h2 = f2.Get("shapes_fit_b/{0}/total_background".format(chname))
    
    v1 = h1.GetBinContent(1)
    v2 = h2.GetBinContent(1)

    results.append((bin, v1, v2))

# for bin,v1,v2 in sorted(results, key=lambda x:x[2]/x[1], reverse=True):
#     print "{0:40s} {1:10.3f} {2:10.3f} {3:+6.2f}%".format(bin, v1, v2, 100.0*(v2/v1-1))


ht_regs = ["HT250to450", "HT450to575", "HT575to1200", "HT1200to1500", "HT1500toInf"]
trs_vll = ["j2to3_b0","j2to3_b1","j2to3_b2","j4to6_b0","j4to6_b1","j4to6_b2","j7toInf_b0","j7toInf_b1","j7toInf_b2","j2to6_b3toInf","j7toInf_b3toInf"]
trs_mhuh = ["j2to3_b0","j2to3_b1","j2to3_b2","j4to6_b0","j4to6_b1","j4to6_b2","j2to6_b3toInf","j7to9_b0","j7to9_b1","j7to9_b2","j7to9_b3","j7to9_b4toInf","j10toInf_b0","j10toInf_b1","j10toInf_b2","j10toInf_b3","j10toInf_b4toInf"]

for ht_reg in ht_regs:
    if ht_regs in ["HT250to450", "HT450to575"]:
        trs = trs_vll
    else:
        trs = trs_mhuh

    contents = []
    for tr in trs:
        sc = []
        for bin,v1,v2 in results:
            if ht_reg in bin and tr in bin:
                sc.append((bin,v1,v2))
        sc = sorted(sc, key=lambda x:int(x[0].split("_m")[-1].split("to")[0]))
        contents.append(sc)

    NBINS = sum(len(sc) for sc in contents)
    h1 = r.TH1D("h1","",NBINS,0,NBINS)
    h2 = r.TH1D("h2","",NBINS,0,NBINS)
    hr = r.TH1D("hr","",NBINS,0,NBINS)
      
    ibin = 0
    for sc in contents:
        for bin in sc:
            ibin+=1
            print bin[0]
            h1.SetBinContent(ibin, bin[1])
            h2.SetBinContent(ibin, bin[2])
            hr.SetBinContent(ibin, bin[2]/bin[1])
            hr.SetBinError(ibin, 0)
        
    c = r.TCanvas("c","c",900,400)
    
    hr.SetMarkerStyle(20)
    hr.GetYaxis().SetRangeUser(0.8,1.2)

    hr.Draw("PE")

    line = r.TLine()
    line.SetLineStyle(2)
    line.DrawLine(0,1,NBINS,1)
    ibin = 0
    for sc in contents[:-1]:
        ibin += len(sc)
        line.DrawLine(ibin,0.8,ibin,1.2)

    text = r.TLatex()
    text.SetTextFont(42)
    text.SetTextSize(0.03)
    text.SetTextAlign(22)
    prevbin = 0
    curbin = 0
    for sc in contents:
        curbin += len(sc)
        if len(sc)>0:
            # text.DrawLatex(0.5*(prevbin+curbin), 1.1, sc[0][0].split("_m")[0])
            text.DrawLatex(0.5*(prevbin+curbin), 1.15, sc[0][0].split("_")[1])
            text.DrawLatex(0.5*(prevbin+curbin), 1.1, sc[0][0].split("_")[2])
        prevbin = curbin

    c.SaveAs("~/public_html/mt2/t2qq_study/{0}.png".format(ht_reg))
    c.SaveAs("~/public_html/mt2/t2qq_study/{0}.pdf".format(ht_reg))



