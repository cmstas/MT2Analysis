import sys, os
sys.path.append("./CardMaker")
from Datacard import Datacard
import cPickle as pickle
from math import sqrt, log10, floor

datacards = pickle.load(open("CardMaker/cards_V00-10-17_FullRunII/templates/template_datacards.pkl", 'rb'))

dcs = {"monojet":{},"VL":{},"Ll":{},"Lh":{},"Ml":{},"Mm":{},"Mh":{},"Hl":{},"Hm":{},"Hh":{},"UHl":{},"UHh":{}}
for name,dc in datacards.items():
    hlo = int(name.split("HT")[1].split("_")[0].split("to")[0].replace("Inf","-1"))
    hhi = int(name.split("HT")[1].split("_")[0].split("to")[-1].replace("Inf","-1"))
    jlo = int(name.split("_j")[1].split("_")[0].split("to")[0].replace("Inf","-1"))
    jhi = int(name.split("_j")[1].split("_")[0].split("to")[-1].replace("Inf","-1"))
    blo = int(name.split("_b")[1].split("_")[0].split("to")[0].replace("Inf","-1"))
    bhi = int(name.split("_b")[1].split("_")[0].split("to")[-1].replace("Inf","-1"))
    mlo = int(name.split("_m")[1].split("_")[0].split("to")[0].replace("Inf","-1"))
    mhi = int(name.split("_m")[1].split("_")[0].split("to")[-1].replace("Inf","-1"))

    ht_reg = ""
    if jlo == 1: ht_reg = "monojet"
    elif hlo == 250: ht_reg = "VL"
    elif hlo == 450 and jlo<7: ht_reg = "Ll"
    elif hlo == 450 and jlo>=7: ht_reg = "Lh"
    elif hlo == 575 and jhi<4:  ht_reg = "Ml"
    elif hlo == 575 and jlo<7:  ht_reg = "Mm"
    elif hlo == 575 and jlo>=7: ht_reg = "Mh"
    elif hlo == 1200 and jhi<4:  ht_reg = "Hl"
    elif hlo == 1200 and jlo<7:  ht_reg = "Hm"
    elif hlo == 1200 and jlo>=7: ht_reg = "Hh"
    elif hlo == 1500 and jlo<7:  ht_reg = "UHl"
    elif hlo == 1500 and jlo>=7: ht_reg = "UHh"
    else: raise Exception()

    hbnd = (hlo, hhi)
    jbnd = (jlo, jhi)
    bbnd = (blo, bhi)
    mbnd = (mlo, mhi)

    dcs[ht_reg][(hbnd,jbnd,bbnd,mbnd)] = dc


def GetStatSyst(dc, bkg):
    # returns (stat_up, stat_down, syst)
    uncs = dc.GetTotalUncertainty(returnDict=True, bkgMatch=bkg)
    stat_up = 0.0
    stat_dn = 0.0
    syst = 0.0
    for nuis,(up,dn) in uncs.items():
        if "stat" in nuis.lower():
            stat_up += up**2
            stat_dn += dn**2
        else:
            syst += max(up,dn)**2

    return (sqrt(stat_up), sqrt(stat_dn), sqrt(syst))


titles = {
    "monojet": r"$\njets = 1$",
    "VL": r"$250 \leq \Ht < 450$ \GeV",
    "Ll":  r"$450 \leq \Ht < 575$ \GeV, $\njets < 7$",
    "Lh":  r"$450 \leq \Ht < 575$ \GeV, $\njets \geq 7$",
    "Ml":  r"$575 \leq \Ht < 1200$ \GeV, $\njets^\mathrm{hi} < 4$",
    "Mm":  r"$575 \leq \Ht < 1200$ \GeV, $4 \leq \njets^\mathrm{hi} < 7$",
    "Mh":  r"$575 \leq \Ht < 1200$ \GeV, $\njets \geq 7$",
    "Hl":  r"$1200 \leq \Ht < 1500$ \GeV, $\njets^\mathrm{hi} < 4$",
    "Hm":  r"$1200 \leq \Ht < 1500$ \GeV, $4 \leq \njets^\mathrm{hi} < 7$",
    "Hh":  r"$1200 \leq \Ht < 1500$ \GeV, $\njets \geq 7$",
    "UHl":  r"$\Ht \geq 1500$ \GeV, $\njets < 7$",
    "UHh":  r"$\Ht \geq 1500$ \GeV, $\njets \geq 7$",
}
ht_regs = ["monojet", "VL","Ll","Lh","Ml","Mm","Mh","Hl","Hm","Hh","UHl","UHh"]
# ht_regs = ["monojet"]
yaml_data = {ht_reg:[] for ht_reg in ht_regs}
fout = open("/home/users/bemarsh/public_html/mt2/mt2_yields.txt", 'w')
for ht_reg in ht_regs:
    s = ""
    prev = (0,0,0,0)
    s += r"""
\begin{table}[!ht]
\setlength\tabcolsep{1.5mm}
\scriptsize
\centering
\caption{Predictions and observations for the NBINS search regions with TITLE. For each of the background
predictions, the first uncertainty listed is statistical (from the limited size of data control samples
and Monte Carlo samples), and the second is systematic.}
\label{tab:yieldsREGION}
\renewcommand{\arraystretch}{1.3}
\begin{tabular}{c|c||c|c|c|c|c} \hline
\multicolumn{7}{c}{""".replace("REGION", ht_reg).replace("NBINS", str(len(dcs[ht_reg]))).replace("TITLE", titles[ht_reg])

    s += titles[ht_reg] + r"} \\ \hline" + "\n"
    s += r"\njets, \nbtags & "
    s += r"$p_\text{T}^\text{jet1}$" if ht_reg=="monojet" else r"\mttwo"
    s += r" [\GeV] & Lost lepton & \znunu & Multijet & Total background & Data \\" + "\n"
    key = (lambda x:x) if ht_reg!="monojet" else (lambda x:(x[0][2],x[0][0]))
    for ((hlo,hhi),(jlo,jhi),(blo,bhi),(mlo,mhi)),dc in sorted(dcs[ht_reg].items(), key=key):
        if (jlo,jhi,blo,bhi) != prev:
            s += r"\hline"+"\n"
        
            if ht_reg != "monojet":
                nbins = len([1 for (a,b),(c,d),(e,f),_ in dcs[ht_reg].keys() if a==hlo and c==jlo and e==blo])
            else:
                nbins = len([1 for _,(c,d),(e,f),_ in dcs[ht_reg].keys() if c==jlo and e==blo])
            s += r"\multirow{" + str(nbins) + "}{*}{"
            if jlo==jhi:    s += "{0}j".format(jlo)
            elif jhi != -1: s += "{0}-{1}j".format(jlo,jhi)
            else:           s += "$\geq{0}$j".format(jlo)
            s += ", "
            if blo==bhi:    s += "{0}b".format(blo)
            elif bhi != -1: s += "{0}-{1}b".format(blo,bhi)
            else:           s += "$\geq{0}$b".format(blo)
            s += "}"

        prev = (jlo,jhi,blo,bhi)
        
        s += " & "
        if ht_reg != "monojet":
            if mhi != -1:   s += "{0}-{1}".format(mlo,mhi)
            else:           s += "$\geq{0}$".format(mlo)
        else:
            if hhi != -1:   s += "{0}-{1}".format(hlo,hhi)
            else:           s += "$\geq{0}$".format(hlo)
        s += " & "

        def GetRoundedValues(rate, uncs, unc_sig_fig=2, prec_cap=-2):
            max_unc = reduce(max, uncs)
            if max_unc == 0:
                prec = 0
            else:
                prec = int(floor(log10(max_unc))) - (unc_sig_fig - 1)
            prec = max(prec, prec_cap)
            ndec = max(0,-prec)
            rnd = lambda x: "{{0:.{0}f}}".format(ndec).format(int(round(float(x)/10**prec)) * 10**prec)

            rate = rnd(rate)
            uncs = tuple(rnd(unc) for unc in uncs)
            return rate, uncs

        zinv_stat_up, zinv_stat_dn, zinv_syst = GetStatSyst(dc, "zinv")
        llep_stat_up, llep_stat_dn, llep_syst = GetStatSyst(dc, "llep")
        qcd_stat_up, qcd_stat_dn, qcd_syst = GetStatSyst(dc, "qcd")
        tot_stat_up, tot_stat_dn, tot_syst = GetStatSyst(dc, None)
        bkg_rates = dc.GetBackgroundRates()
        zinv_rate = bkg_rates["zinv"]
        llep_rate = bkg_rates["llep"]
        qcd_rate = bkg_rates["qcd"]
        tot_rate = zinv_rate + llep_rate + qcd_rate

        # add in some scientific notation for particularly big numbers
        zinv_exp = 0
        llep_exp = 0
        # if zinv_rate > 1e4:
        #     max_unc = max(max(zinv_stat_up, zinv_stat_dn), zinv_syst)
        #     zinv_exp = int(floor(log10(max_unc))) - 1
        #     if zinv_exp > 1:
        #         zinv_rate /= 10**zinv_exp
        #         zinv_stat_up /= 10**zinv_exp
        #         zinv_stat_dn /= 10**zinv_exp
        #         zinv_syst /= 10**zinv_exp
        #     else:
        #         zinv_exp = 0
        # if llep_rate > 1e4:
        #     max_unc = max(max(llep_stat_up, llep_stat_dn), llep_syst)
        #     llep_exp = int(floor(log10(max_unc))) - 1
        #     if llep_exp > 1:
        #         llep_rate /= 10**llep_exp
        #         llep_stat_up /= 10**llep_exp
        #         llep_stat_dn /= 10**llep_exp
        #         llep_syst /= 10**llep_exp
        #     else:
        #         llep_exp = 0
            

        zinv_rate, (zinv_stat_up, zinv_stat_dn, zinv_syst) = GetRoundedValues(zinv_rate, (zinv_stat_up, zinv_stat_dn, zinv_syst))
        llep_rate, (llep_stat_up, llep_stat_dn, llep_syst) = GetRoundedValues(llep_rate, (llep_stat_up, llep_stat_dn, llep_syst))
        qcd_rate, (qcd_stat_up, qcd_stat_dn, qcd_syst) = GetRoundedValues(qcd_rate, (qcd_stat_up, qcd_stat_dn, qcd_syst))
        tot_rate, (tot_stat_up, tot_stat_dn, tot_syst) = GetRoundedValues(tot_rate, (tot_stat_up, tot_stat_dn, tot_syst))

        def getstr(rate, stat_up, stat_dn, syst, boldrate=False):
            if float(rate) < 0.01 and float(stat_up)<0.01 and float(stat_dn)<0.01 and float(syst)<0.01:
                return "$<0.01$"
            s = "$"
            if boldrate:
                s += "{\\bf " + rate + "}"
            else:
                s += rate
            if stat_up==stat_dn:
                s += r"\pm" + stat_up
            else:
                s += "^{+" + stat_up + "}_{-" + stat_dn + "}"
            s += r"\pm" + syst + "$"
            return s

        s += getstr(llep_rate, llep_stat_up, llep_stat_dn, llep_syst)
        if llep_exp > 0:
            s += r" $\times 10^{0}$".format(llep_exp)
        s += " & "
        s += getstr(zinv_rate, zinv_stat_up, zinv_stat_dn, zinv_syst)
        if zinv_exp > 0:
            s += r" $\times 10^{0}$".format(zinv_exp)
        s += " & "
        s += getstr(qcd_rate, qcd_stat_up, qcd_stat_dn, qcd_syst)
        s += " & "
        s += getstr(tot_rate, tot_stat_up, tot_stat_dn, tot_syst, boldrate=True)
        s += " & "

        s += "{\\bf " + str(int(dc.GetObservation())) + "}"

        s += "\\\\ \n"

        yaml_data[ht_reg].append({
                "bounds": ((hlo,hhi),(jlo,jhi),(blo,bhi),(mlo,mhi)),
                "zinv": (zinv_rate, (zinv_stat_up, zinv_stat_dn, zinv_syst)),
                "llep": (llep_rate, (llep_stat_up, llep_stat_dn, llep_syst)),
                "qcd": (qcd_rate, (qcd_stat_up, qcd_stat_dn, qcd_syst)),
                "total": (tot_rate, (tot_stat_up, tot_stat_dn, tot_syst)),
                "obs": str(int(dc.GetObservation())),
                })

    s += r"""
\hline
\end{tabular}
\end{table}
"""
    # print s
    fout.write(s + "\n\n")

fout.close()

yaml_dir = "/home/users/bemarsh/public_html/mt2/hepdata/fullRunII"
os.system("mkdir -p "+yaml_dir)
for ht_reg, data in yaml_data.items():
    fout = open(os.path.join(yaml_dir, "yields_mt2_{0}.yaml".format(ht_reg)), 'w')
    
    fout.write(
"""independent_variables:
- header: {name: '$N_\mathrm{j}$'}
  values:
"""
)
    for d in data[:]:
        jlo,jhi = d["bounds"][1]
        if jlo==jhi:
            fout.write("  - value: {0}\n".format(jlo))
        else:
            fout.write("  - {{low: {0}, high: {1}}}\n".format(jlo,jhi).replace("-1","-1"))

    fout.write(
"""- header: {name: '$N_\mathrm{b}$'}
  values:
"""
)
    for d in data[:]:
        blo,bhi = d["bounds"][2]
        if blo==bhi:
            fout.write("  - value: {0}\n".format(blo))
        else:
            fout.write("  - {{low: {0}, high: {1}}}\n".format(blo,bhi).replace("-1","-1"))

    fout.write(
"""- header: {name: '$M_\mathrm{T2}$', units: GEV}
  values:
"""
)
    for d in data[:]:
        mlo,mhi = d["bounds"][0 if ht_reg=="monojet" else 3]
        if mlo==mhi:
            fout.write("  - value: {0}\n".format(mlo))
        else:
            fout.write("  - {{low: {0}, high: {1}}}\n".format(mlo,mhi).replace("-1","-1"))

    fout.write(
"""dependent_variables:
- header: {name: 'Lost lepton'}
  qualifiers:
  - {name: '$\\sqrt{s}$', value: 13 TeV}
  - {name: '$\\mathcal{L}_\\mathrm{int}$', value: '137 fb$^{-1}$'}
  values:
"""
)
    for d in data[:]:
        rate,(stat_up,stat_down,syst)  = d["llep"]
        fout.write("  - value: {0}\n    errors:\n".format(rate))
        if stat_up == stat_down:
            fout.write("    - {{symerror: {0}, label: stat}}\n".format(stat_up))
        else:
            fout.write("    - {{asymerror: {{plus: {0}, minus: -{1}}}, label: stat}}\n".format(stat_up,stat_down))
        fout.write("    - {{symerror: {0}, label: syst}}\n".format(syst))

    fout.write(
"""- header: {name: '$Z\\to\\nu\\bar{\\nu}$'}
  qualifiers:
  - {name: '$\\sqrt{s}$', value: 13 TeV}
  - {name: '$\\mathcal{L}_\\mathrm{int}$', value: '137 fb$^{-1}$'}
  values:
"""
)
    for d in data[:]:
        rate,(stat_up,stat_down,syst)  = d["zinv"]
        fout.write("  - value: {0}\n    errors:\n".format(rate))
        if stat_up == stat_down:
            fout.write("    - {{symerror: {0}, label: stat}}\n".format(stat_up))
        else:
            fout.write("    - {{asymerror: {{plus: {0}, minus: -{1}}}, label: stat}}\n".format(stat_up,stat_down))
        fout.write("    - {{symerror: {0}, label: syst}}\n".format(syst))

    fout.write(
"""- header: {name: 'Multijet'}
  qualifiers:
  - {name: '$\\sqrt{s}$', value: 13 TeV}
  - {name: '$\\mathcal{L}_\\mathrm{int}$', value: '137 fb$^{-1}$'}
  values:
"""
)
    for d in data[:]:
        rate,(stat_up,stat_down,syst)  = d["qcd"]
        fout.write("  - value: {0}\n    errors:\n".format(rate))
        if stat_up == stat_down:
            fout.write("    - {{symerror: {0}, label: stat}}\n".format(stat_up))
        else:
            fout.write("    - {{asymerror: {{plus: {0}, minus: -{1}}}, label: stat}}\n".format(stat_up,stat_down))
        fout.write("    - {{symerror: {0}, label: syst}}\n".format(syst))

    fout.write(
"""- header: {name: 'Total background'}
  qualifiers:
  - {name: '$\\sqrt{s}$', value: 13 TeV}
  - {name: '$\\mathcal{L}_\\mathrm{int}$', value: '137 fb$^{-1}$'}
  values:
"""
)
    for d in data[:]:
        rate,(stat_up,stat_down,syst)  = d["total"]
        fout.write("  - value: {0}\n    errors:\n".format(rate))
        if stat_up == stat_down:
            fout.write("    - {{symerror: {0}, label: stat}}\n".format(stat_up))
        else:
            fout.write("    - {{asymerror: {{plus: {0}, minus: -{1}}}, label: stat}}\n".format(stat_up,stat_down))
        fout.write("    - {{symerror: {0}, label: syst}}\n".format(syst))

    fout.write(
"""- header: {name: 'Data'}
  qualifiers:
  - {name: '$\\sqrt{s}$', value: 13 TeV}
  - {name: '$\\mathcal{L}_\\mathrm{int}$', value: '137 fb$^{-1}$'}
  values:
"""
)
    for d in data[:]:
        obs = d["obs"]
        fout.write("  - value: {0}\n".format(obs))

    fout.close()



