import os,sys
sys.path.append("../../CardMaker/")
from Datacard import Datacard
import glob
import cPickle as pickle
import matplotlib.pyplot as plt

if len(sys.argv)<2:
    print "usage: {0} <indir>".format(sys.argv[0])
    exit(1)

indir = sys.argv[1]
POINT = indir.strip("/").split("/")[-1]
MODEL = POINT.split("_")[0]
M1 = POINT.split("_")[1]
M2 = POINT.split("_")[2]

dc_dir = "../../CardMaker/cards_V00-10-17_FullRunII_forPlotting/"
dcs = pickle.load(open(os.path.join(dc_dir, MODEL, POINT, "signal_datacards.pkl"), 'rb'))

outdir = "/home/users/bemarsh/public_html/mt2/rankings/{0}".format(POINT)
os.system("mkdir -p "+outdir)

trs = {}
for fname in glob.glob(os.path.join(indir, "log_*.txt")):
    tr = fname.split("log_")[-1].split(".txt")[0]
    obs = 9999
    exp = 9999
    em2s = 9999
    em1s = 9999
    ep1s = 9999
    ep2s = 9999
    sig = 9999
    with open(fname) as fid:
        for line in fid:
            if line.startswith("Observed"):
                obs = float(line.strip().split()[-1])
            if line.startswith("Expected 50"):
                exp = float(line.strip().split()[-1])
            if line.startswith("Expected  2.5"):
                em2s = float(line.strip().split()[-1])
            if line.startswith("Expected 16"):
                em1s = float(line.strip().split()[-1])
            if line.startswith("Expected 84"):
                ep1s = float(line.strip().split()[-1])
            if line.startswith("Expected 97"):
                ep2s = float(line.strip().split()[-1])

    with open(fname.replace("log","sig")) as fid:
        for line in fid:
            if line.startswith("Significance"):
                sig = float(line.strip().split()[-1])

    trs[tr] = {"obs":obs, "exp":exp, "em2s":em2s, "em1s":em1s, "ep1s":ep1s, "ep2s":ep2s, "sig":sig}

    estsig = 9999
    if reduce(lambda x,y:max(abs(x),abs(y)), trs[tr].values()) < 9999:
        if obs < em2s:
            estsig = -(obs-em2s)/(em2s-em1s) - 1
        elif obs < em1s:
            estsig = -(obs-em1s)/(em2s-em1s) - 1
        elif obs < exp:
            estsig = -(obs-exp)/(em1s-exp)
        elif obs < ep1s:
            estsig = (obs-exp)/(ep1s-exp)
        elif obs < ep2s:
            estsig = (obs-ep1s)/(ep2s-ep1s) + 1
        else:
            estsig = (obs-ep2s)/(ep2s-ep1s) + 1
    trs[tr]["estsig"] = estsig

    tr_cards = []
    for name in dcs:
        if tr in name:
            tr_cards.append(dcs[name])
    # merged = Datacard.MergeCards(tr_cards, "merged_"+tr)
    trs[tr]["nobs"] = int(sum(d.GetObservation() for d in tr_cards))
    trs[tr]["nsig"] = sum(d.GetSignalRate() for d in tr_cards)
    trs[tr]["pred"] = sum(sum(d.GetBackgroundRates().values()) for d in tr_cards)
    trs[tr]["unc"] = Datacard.GetTotalMergedUncertainty(tr_cards)
    

for key in ["obs","exp","sig","estsig","obsOverExp"]:
    fout = open(os.path.join(outdir, key+".txt"), 'w')
    fout.write("{0:>6s}  {1:27s} {2:>9s} {3:>9s} {4:>9s} {5:>9s} {6:>9s} {7:>9s} {8:>12s} {9:>8s}\n".format("Rank", "Region", "Obs", "Exp", "Obs/Exp", "Sig", "EstSig", "NSig", "Pred", "Nobs"))
    sortkey = lambda x:trs[x][key]
    if key == "obsOverExp":
        sortkey = lambda x:trs[x]["obs"]/trs[x]["exp"]
    for i,tr in enumerate(sorted(trs, key=sortkey)):
        fout.write("{0:6d}  {1:27s} {2:9.3f} {3:9.3f} {4:9.3f} {5:9.3f} {6:9.3f} {7:9.3f} {8:12.3f} {9:8d}\n".format(
                i+1, tr, trs[tr]["obs"], trs[tr]["exp"], trs[tr]["obs"]/trs[tr]["exp"],trs[tr]["sig"], trs[tr]["estsig"],
                trs[tr]["nsig"], trs[tr]["pred"], trs[tr]["nobs"]))

    fout.close()

sig,estsig = zip(*[(trs[tr]["sig"],trs[tr]["obs"]/trs[tr]["exp"]) for tr in trs])
plt.plot(estsig,sig,'o')
# plt.plot([-3,3],[-3,3],'--k')
# plt.gca().set_xlim(-3,3)
# plt.gca().set_ylim(-3,3)
plt.gca().set_xlim(0,3)
plt.gca().set_ylim(-3,3)
plt.show()

