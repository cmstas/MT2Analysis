import ROOT as r
import os,sys
sys.path.append("../../CardMaker/")
from Datacard import Datacard
import glob
import cPickle as pickle

if len(sys.argv)<2:
    print "usage: {0} <indir>".format(sys.argv[0])
    exit(1)


indir = sys.argv[1]
POINT = indir.strip("/").split("/")[-1]
MODEL = POINT.split("_")[0]
M1 = POINT.split("_")[1]
M2 = POINT.split("_")[2]

full_limit_dir = "/hadoop/cms/store/user/bemarsh/combine/limits/{0}_V00-10-18_FullRunII/".format(MODEL)
# full_limit_dir = "/hadoop/cms/store/user/bemarsh/combine/limits/{0}_V00-10-17_FullRunII_withDYContam/".format(MODEL)
dc_dir = "../../CardMaker/cards_V00-10-18_FullRunII/"
# dc_dir = "../../CardMaker/cards_V00-10-17_FullRunII_withDYContam/"
postfit_file = r.TFile("../../../limits/postfit/V00-10-18_FullRunII/fitDiagnostics_all.root")
# postfit_file = r.TFile("../../../limits/postfit/V00-10-17_FullRunII/fitDiagnostics_all.root")
chnames = [k.GetName() for k in postfit_file.Get("shapes_fit_b").GetListOfKeys()]

trs = {}
for fname in glob.glob(os.path.join(indir, "log_*.txt")):
    tr = fname.split("log_")[-1].split(".txt")[0]
    obs = 99999
    exp = 99999
    with open(fname) as fid:
        for line in fid:
            if line.startswith("Observed"):
                obs = float(line.strip().split()[-1])
            if line.startswith("Expected 50"):
                exp = float(line.strip().split()[-1])
    trs[tr] = {"obs":obs, "exp":exp, "mt2bins":[]}

    mt2bins = []
    for ch in chnames:
        if tr not in ch:
            continue
        mt2lo = int(ch.split("_m")[-1].split("to")[0])
        mt2hi = ch.split("_m")[-1].split("to")[1]
        mt2hi = "$\infty$" if mt2hi=="Inf" else mt2hi

        h_pre = postfit_file.Get("shapes_prefit/{0}/total_background".format(ch))
        h_post = postfit_file.Get("shapes_fit_b/{0}/total_background".format(ch))
        h_obs = postfit_file.Get("shapes_prefit/{0}/data".format(ch))

        # bkg_pre = h_pre.GetBinContent(1)
        bkg_post = h_post.GetBinContent(1)
        err_post = h_post.GetBinError(1)
        # x,obs = r.Double(), r.Double()
        # h_obs.GetPoint(0, x, obs)

        sig = 0.0
        dc = open(os.path.join(dc_dir, MODEL, POINT, "datacard_{0}_{1}.txt".format(ch, POINT)))
        for line in dc:
            if line.startswith("# total signal"):
                sig = float(line.strip().split()[-1])
        dc.close()

        # open pickled datacard to compute obs/prefit background
        with open(os.path.join(dc_dir, "templates", "template_datacards.pkl"), 'rb') as f:
            dcs = pickle.load(f)
        dc = dcs[ch]
        obs = dc.GetObservation()
        bkg_pre = sum(dc.GetBackgroundRates().values())

        mt2bins.append((mt2lo,mt2hi,bkg_pre,bkg_post,err_post,int(obs),sig))  

    trs[tr]["mt2bins"] = sorted(mt2bins)
    trs[tr]["totsig"] = sum([x[6] for x in mt2bins])
    

f_limit = r.TFile(os.path.join(full_limit_dir, "limit_{0}.root".format(POINT)))
t = f_limit.Get("limit")
t.GetEntry(5)
full_obs = t.limit
t.GetEntry(2)
full_exp = t.limit
f_limit.Close()

print r"""
\begin{table}[h]
\centering
\scriptsize
\label{tab:sigrank_MODEL_M1_M2}
\caption{MODEL (M1, M2): ranking of topological regions, according to expected exclusion limit, for an integrated luminosity of 137 fb$^{-1}$. Expected exclusion limit: EXP. Observed exclusion limit: OBS.}
\makebox[\textwidth][c]{
\begin{tabular}{|r|c|c|c|c|c|c|c|c|}
\hline
Region & Exp. Limit & Obs. Limit & Signal & $M_{T2}$ & Signal & SM (pre-fit) & SM (post-fit) & Obs. \\ \hline\hline
""".replace("MODEL",MODEL).replace("M1",M1).replace("M2",M2).replace("EXP", "{0:.3f}".format(full_exp)).replace("OBS", "{0:.3f}".format(full_obs))

for tr in sorted(trs.keys(), key=lambda x:trs[x]["exp"])[:10]:
    for imt2 in range(len(trs[tr]["mt2bins"])):
        if imt2==0:                                  
            print "\multirow{{{11}}}{{*}}{{{0}}} & \multirow{{{11}}}{{*}}{{{1:.2f}}} & \multirow{{{11}}}{{*}}{{{2:.2f}}} &\
 \multirow{{{11}}}{{*}}{{{10:.2f}}} & [{3},{4}] & {9:.2f} & {5:.1f} & ${6:.1f}\pm{7:.1f}$ & {8:d} \\\\".format(tr.replace("_"," "), trs[tr]["exp"], trs[tr]["obs"], 
                                                                                                               trs[tr]["mt2bins"][imt2][0], trs[tr]["mt2bins"][imt2][1], trs[tr]["mt2bins"][imt2][2], 
                                                                                                               trs[tr]["mt2bins"][imt2][3], trs[tr]["mt2bins"][imt2][4], trs[tr]["mt2bins"][imt2][5],
                                                                                                               trs[tr]["mt2bins"][imt2][6], trs[tr]["totsig"],len(trs[tr]["mt2bins"]))
        else:
            print " & & & & [{0},{1}] & {6:.2f} & {2:.1f} & ${3:.1f}\pm{4:.1f}$ & {5:d}\\\\".format(trs[tr]["mt2bins"][imt2][0], trs[tr]["mt2bins"][imt2][1], trs[tr]["mt2bins"][imt2][2],
                                                                                                  trs[tr]["mt2bins"][imt2][3], trs[tr]["mt2bins"][imt2][4], trs[tr]["mt2bins"][imt2][5],
                                                                                                  trs[tr]["mt2bins"][imt2][6])
    print "\\hline"

print r"""
\end{tabular}
}
\end{table}
"""



