import glob
import ROOT as r
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

TAG = "V00-10-15_FullRunII_decorrHalfLepEff"
# TAG = "V00-10-15_FullRunII_decorrHalfLepEff_T2qq_700_350"
# TAG = "V00-10-15_FullRunII_decorrHalfLepEff_T2qq_950_0"

f1 = r.TFile("{0}/fitDiagnostics_all.root".format(TAG))

# cards = glob.glob("FullRunII_17MCfor18_ttbbWeights_v3_T2qq_1300_850/cards_HT250to350_j1_b0_dummyobs/datacard_*.txt")
# bins = [x.split("datacard_")[-1].split("_T2qq")[0] for x in cards]
# bins = sorted(bins)

d = f1.Get("shapes_fit_b")
bins = [k.GetName() for k in d.GetListOfKeys() if k.GetName().startswith("HT")]

results = []
for i,bin in enumerate(bins):
    # print bin
    chname = "ch{0}".format(i+1)

    hp = f1.Get("shapes_prefit/{0}/total_background".format(bin))
    hb = f1.Get("shapes_fit_b/{0}/total_background".format(bin))
    hs = f1.Get("shapes_fit_s/{0}/total_background".format(bin))
    ho = f1.Get("shapes_prefit/{0}/data".format(bin))

    cp = hp.GetBinContent(1)
    cs = hs.GetBinContent(1)
    cb = hb.GetBinContent(1)
    ep = hp.GetBinError(1)
    es = hs.GetBinError(1)
    eb = hb.GetBinError(1)

    x,y = r.Double(), r.Double()
    ho.GetPoint(0, x, y)

    # pb = 1.0 - r.Math.gaussian_cdf((y-cb)/eb)
    # ps = 1.0 - r.Math.gaussian_cdf((y-cs)/es)

    results.append((bin, cp, ep, cb, eb, cs, es, int(y)))
    
fout = open("/home/users/bemarsh/public_html/mt2/t2qq_study/nuis_pulls/prepostfit_results_{0}.txt".format(TAG), 'w')

fout.write("{0:40s} {1:19s} {2:19s} {3:19s} {4:9s}\n".format("", "         prefit", "         fit_b", "         fit_sb", "observed"))
fout.write("----------------------------------------------------------------------------------------------------------------\n")
for bin,cp, ep, cb, eb, cs, es, obs in sorted(results, key=lambda x:x[0], reverse=False):
    fout.write("{0:40s} {1:9.2f} {2:9.2f} {3:9.2f} {4:9.2f} {5:9.2f} {6:9.2f} {7:9d}\n".format(bin, cp, ep, cb, eb, cs, es, obs))

fout.close()
