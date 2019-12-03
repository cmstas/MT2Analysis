import os
import glob
import ROOT as r

cuts = []
cuts.append(           r.TCut("Flag_goodVertices && Flag_HBHENoiseFilter && Flag_HBHENoiseIsoFilter && Flag_EcalDeadCellTriggerPrimitiveFilter && Flag_badMuonFilter && Flag_ecalBadCalibFilterUpdate && ((ht>250 && met_pt>250) || (ht>1200 && met_pt>30))"))
cuts.append(cuts[-1] + r.TCut("nJet30>=1"))
cuts.append(cuts[-1] + r.TCut("nJet30==1 || mt2>200"))
cuts.append(cuts[-1] + r.TCut("deltaPhiMin>0.3"))
cuts.append(cuts[-1] + r.TCut("diffMetMht/met_pt<0.5"))
cuts.append(cuts[-1] + r.TCut("nMuons10+nElectrons10+nPFLep5LowMT+nPFHad10LowMT==0"))

names = [
    r"Before selection",
    r"Trigger selection",
    r"At least one jet",
    r"$M_\mathrm{T2} > 200$ GeV (if $N_\mathrm{j}\geq2$)",
    r"$\Delta\phi_\mathrm{min} > 0.3$",
    r"$|\vec{p}_\mathrm{T}^\mathrm{miss}-|\vec{H}_\mathrm{T}^\mathrm{miss}|/|p_\mathrm{T}^\mathrm{miss} < 0.5$",
    r"Lepton veto",
]

# extra HT,NJ,NB cuts
extracuts = {
    "T1tttt_2200_0" : (1200, 7, 2),    
    "T1tttt_1800_1300" : ((575,1200), 7, 2),    
    "T1bbbb_2200_0" : (1200, 4, 2),    
    "T1bbbb_1700_1400" : ((450,1200), 4, 2),    
    "T1qqqq_2100_0" : (1200, 4, 0),    
    "T1qqqq_1500_1200" : ((575,1200), 4, 0),    
    "T2tt_1200_0" : (1200, 4, 2),
    "T2tt_800_550" : ((575,1200), 4, 2),
    "T2bb_1300_0" : (1200, -4, 1),
    "T2bb_800_650" : ((450,1200), -4, 1),
    "T2qq_1800_0" : (1200, -4, 0),
    "T2qq_1100_900" : ((575,1200), -4, 0),
}

indir = "/nfs-7/userdata/mt2/unskimmed_signal_massskim/2017/"
xsecfile = r.TFile("../limits/SignalScan/xsec_susy_13tev_run2.root")

dcdir = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-17_FullRunII_withDYContam"
dcdir2 = "/home/users/bemarsh/analysis/mt2/current/MT2Analysis/scripts/CardMaker/cards_V00-10-17_FullRunII"

fout = open("/home/users/bemarsh/public_html/mt2/supplementary_figures/cutflow/tables.txt", 'w')
for POINT in sorted(extracuts.keys()):
    ht, nj, nb = extracuts[POINT]
    MODEL = POINT.split("_")[0]
    M1,M2 = map(int, POINT.split("_")[1:])
    
    extras = []
    names_ = list(names)
    if isinstance(ht, tuple):
        extras.append(cuts[-1] + r.TCut("ht>={0} && ht<{1}".format(*ht)))
        names_.append("${0} \leq H_\mathrm{{T}} < {1}$ GeV".format(*ht))
    else:
        extras.append(cuts[-1] + r.TCut("ht>={0}".format(ht)))
        names_.append("$H_\mathrm{{T}} \geq {0}$ GeV".format(ht))
    if nj > 0:
        extras.append(extras[-1] + r.TCut("nJet30>={0}".format(nj)))
        names_.append("$N_\mathrm{{j}} \geq {0}$".format(nj))
    else:
        extras.append(extras[-1] + r.TCut("nJet30<{0}".format(-nj)))
        names_.append("$N_\mathrm{{j}} < {0}$".format(-nj))
    if nb != 0:
            extras.append(extras[-1] + r.TCut("nBJet20>={0}".format(nb)))
            names_.append("$N_\mathrm{{b}} \geq {0}$".format(nb))
    else:
        extras.append(extras[-1] + r.TCut("nBJet20==0"))
        names_.append("$N_\mathrm{{b}} = 0$")

    # get total signal yield
    d = dcdir
    if not os.path.exists(os.path.join(d, MODEL, POINT)):
        d = dcdir2
    totsig_ = 0.0
    for f in glob.glob(os.path.join(d, MODEL, POINT, "*.txt")):
        with open(f) as fid:
            for line in fid:
                if line.startswith("# total signal"):
                    totsig_ += float(line.strip().split(":")[-1])
                    break
    
    f = r.TFile(os.path.join(indir, POINT+".root"))
    t = f.Get("mt2")
    hname = "h_xsec_gluino" if "T1" in MODEL or "T5" in MODEL else "h_xsec_squark" if MODEL in ["T2qq"] else "h_xsec_stop"
    h = xsecfile.Get(hname)
    xsec = 1000*h.GetBinContent(h.FindBin(M1))
    if MODEL=="T2qq":
        xsec *= 0.8
    totsig = xsec * 137

    yields = [1.*t.GetEntries()]
    for cut in cuts:
        yields.append(1.*t.GetEntries(cut.GetTitle()))
    scale = totsig / yields[0]
    for cut in extras:
        yields.append(1.*t.GetEntries(cut.GetTitle()))
        
    f.Close()

    if "t" in MODEL:
        plet = "t"
        pname = "top"
    if "b" in MODEL:
        plet = "b"
        pname = "bottom"
    if "q" in MODEL:
        plet = "q"
        pname = "light"
    if "T1" in MODEL:
        header = r"$\mathrm{pp}\rightarrow \tilde{\mathrm{g}}\tilde{\mathrm{g}}$, $\tilde{\mathrm{g}}\rightarrow \mathrm{PLET}\bar{\mathrm{PLET}}\tilde{\chi}_1^0$ ($\mathrm{m}_{\tilde{\mathrm{g}}}=M1$ GeV, $\mathrm{m}_{\tilde{\chi}_1^0}=M2$ GeV)"
        caption = "Cut flow table for baseline selection and several sample additional kinematic selections for a signal model of gluino-mediated PNAME squark production with the mass of the gluino " + \
        "and the LSP equal to M1 and M2 GeV, respectively. The theory cross section for this signal is XSEC fb."
    if "T2" in MODEL:
        header = r"$\mathrm{pp}\rightarrow \tilde{\mathrm{PLET}}\tilde{\mathrm{PLET}}$, $\tilde{\mathrm{PLET}}\rightarrow \mathrm{PLET}\tilde{\chi}_1^0$ ($\mathrm{m}_{\tilde{\mathrm{PLET}}}=M1$ GeV, $\mathrm{m}_{\tilde{\chi}_1^0}=M2$ GeV)"
        caption = "Cut flow table for baseline selection and several sample additional kinematic selections for a signal model of direct PNAME squark production with the mass of the squark " + \
        "and the LSP equal to M1 and M2 GeV, respectively. The theory cross section for this signal is XSEC fb."
    header = header.replace("M1", str(M1))
    header = header.replace("M2", str(M2 if M2>0 else 1))
    header = header.replace("PLET", plet)
    caption = caption.replace("M1", str(M1))
    caption = caption.replace("M2", str(M2 if M2>0 else 1))
    caption = caption.replace("PNAME", pname)
    caption = caption.replace("XSEC", "{0:.1f}".format(xsec) if xsec >= 10 else "{0:.2f}".format(xsec))
    if MODEL=="T2qq":
        caption = caption.replace(" fb.", " fb (assuming 8-fold squark degeneracy).")


    s = r"""
\begin{table}[!ht]
\small
\caption{CAPTION}
\vspace{2mm}
\centering
\begin{tabular}{l|c|c}
\multicolumn{3}{c}{HEADER} \\ \hline
Selection & Yield & Efficiency (\%) \\ \hline
""".replace("HEADER", header).replace("CAPTION", caption)

    for name,y in zip(names_,yields):
        s += "{0} & {1:.1f} & {2:.1f} \\\\ \n".format(name, y*scale, 100*y/yields[0])
        if name.startswith("Lepton"):
            s += "\\hline \n"

    s += r"""
\end{tabular}
\end{table}
"""

    # print s
    fout.write(s)



fout.close()
