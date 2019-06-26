import sys, os
sys.path.append("ProjectMetis/scripts")
import dis_client
sys.path.append("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/babymaker/batchsubmit/NEW_WORKFLOW")
from samples_2016 import samples_2016
from samples_2017 import samples_2017
from samples_2018 import samples_2018

categories = ["wjets", "dyjetsll", "zinv", "ttbar", "gjets", "singletop", "ttv", "tttt", "diboson", "qcd_ht"]


# samples = samples_2016
# campaign = "RunIISummer16MiniAODv3"
# common = "RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3[_ext*]-v*/MINIAODSIM"

# samples = samples_2017
# campaign = "RunIIFall17MiniAODv2"
# common = "RunIIFall17MiniAODv2-PU2017_12Apr2018[_new_pmx]_94X_mc2017_realistic_v14[_ext*]-v*/MINIAODSIM"

samples = samples_2018
campaign = "RunIIAutumn18MiniAOD"
common = "RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15[_ext*]-v*/MINIAODSIM"


outdir = "/home/users/bemarsh/public_html/mt2/samples_tex/"
os.system("mkdir -p " + outdir)
outfile = os.path.join(outdir, "samples_"+campaign+".txt")

fout = open(outfile ,'w')

fout.write(r"""
\begin{table}[htp]
\caption{MiniAOD Standard Model samples from the CAMPAIGN production. \label{tab:mcbkg_CAMPAIGN}}
\centering
\scriptsize
\makebox[\textwidth][c]{%                                                                                                                                                                 
\begin{tabular}{l|c|c|c}
\hline
Sample & Cross Section & K-factor & Eff. lumi \\
& [pb] & & [/fb]\\
\hline
""".replace("CAMPAIGN", campaign))

def getShortDS(ds):
    return ds.split("/")[1]

dsinfo = {}
for cat in categories:
    for ds in samples[cat]:
        short = getShortDS(ds)
        if short not in dsinfo:
            dsinfo[short] = {"nevents":0, "xsec":0, "kfact":0}
        try:
            dsinfo[short]["nevents"] += dis_client.query(ds, "snt")["response"]["payload"][0]["nevents_out"]
        except IndexError:
            continue
        dsinfo[short]["xsec"]  = samples[cat][ds][0] * samples[cat][ds][2]
        dsinfo[short]["kfact"] = samples[cat][ds][1]

already_done = []
for cat in categories:
    for ds in sorted(samples[cat].keys(), key=lambda x:dsinfo[getShortDS(x)]["xsec"], reverse=True):

        ds_short = ds.split("/")[1]
        if ds_short in already_done:
            continue
        already_done.append(ds_short)

        xsec = dsinfo[ds_short]["xsec"]
        kfact = dsinfo[ds_short]["kfact"]
        nevents = dsinfo[ds_short]["nevents"]

        if xsec==0:
            continue

        eff_lumi = nevents / (xsec*kfact) / 1000.0

        if xsec < 0.01:
            xsec = round(xsec, 6)
        elif xsec < 0.1:
            xsec = round(xsec, 5)
        elif xsec < 1:
            xsec = round(xsec, 4)
        elif xsec < 10:
            xsec = round(xsec, 3)
        elif xsec < 100:
            xsec = round(xsec, 2)
        elif xsec < 1000:
            xsec = round(xsec, 1)
        else:
            xsec = int(round(xsec))

        if eff_lumi < 1:
            eff_lumi = round(eff_lumi, 2)
        elif eff_lumi < 20:
            eff_lumi = round(eff_lumi, 1)
        else:
            eff_lumi = int(round(eff_lumi))

        fout.write("/{0}/$\\dagger$ & {1} & {2} & {3} \\\\\n".format(ds_short, xsec, kfact, eff_lumi).replace("_","\\_"))

    fout.write("\\hline\n")

fout.write("$\\dagger$ = {0} & & & \\\\\n".format(common).replace("_","\\_"))
fout.write(r"""\hline
\end{tabular}}
\end{table}
""")

