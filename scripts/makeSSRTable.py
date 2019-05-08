import os, sys
sys.path.append("CardMaker")
from Datacard import Datacard
sys.path.append("ResultPlotMaker")
from ResultPlotUtils import GetPull
import cPickle as pickle
from math import log10

cards = pickle.load(open("CardMaker/cards_V00-10-16_FullRunII_newQCD/templates_ssr/template_datacards.pkl", 'rb'))

ssrs = {
    "2j loose" : [2, 0, 1200, 1200],
    "2j tight" : [2, 0, 1500, 1400],
    "4j loose" : [4, 0, 1200, 1000],
    "4j tight" : [4, 0, 1500, 1400],
    "7j loose" : [7, 0, 1200, 600],
    "7j tight" : [7, 0, 1500, 800],
    "10j loose" : [10, 0, 1200, 400],
    "10j tight" : [10, 0, 1500, 600],
    "2b loose" : [2, 2, 1200, 600],
    "2b tight" : [2, 2, 1500, 600],
    "3b loose" : [2, 3, 1200, 400],
    "3b tight" : [2, 3, 1500, 400],
    "4b loose" : [2, 4, 1200, 400],
    "4b tight" : [2, 4, 1500, 400],
    "7j3b loose" : [7, 3, 1200, 400],
    "7j3b tight" : [7, 3, 1500, 400],
    "7j4b loose" : [7, 4, 1200, 400],
    "7j4b tight" : [7, 4, 1500, 400],
    "10j4b loose" : [10, 4, 1200, 400],
    "10j4b tight" : [10, 4, 1500, 400],
    "MonoPhi" : ["HT250to350_j1_b0_m0toInf", "HT350to450_j1_b0_m0toInf", "HT250to450_j2to3_b0_m200to300"]
}

vals = {}

for ssr,cuts in ssrs.items():
    good_cards = []
    if ssr == "MonoPhi":
        good_cards = [cards[name] for name in cuts]
    else:
        for name in cards:
            if "HT{0}toInf".format(cuts[2]) not in name:
                continue
            if "j{0}toInf".format(cuts[0]) not in name:
                continue
            if "b{0}toInf".format(cuts[1]) not in name:
                continue

            mt2low = int(name.split("_m")[-1].split("to")[0])
            if mt2low < cuts[3]:
                continue

            good_cards.append(cards[name])

    pred = sum(sum(dc.GetBackgroundRates().values()) for dc in good_cards)
    unc_up, unc_dn = Datacard.GetTotalMergedUncertainty(good_cards)
    obs = int(sum(dc.GetObservation() for dc in good_cards))

    # print ssr, ":", "{0:.1f} + {1:.1f} - {2:.1f}   {3:d}".format(pred, unc_up, unc_dn, obs)
    vals[ssr] = (pred, unc_up, unc_dn, obs)


sout = ""
sout += r"""
\begin{table}[!htpb]                                                                                                                                                                                               
  \begin{center}
  \topcaption{\label{tab:ssr_def_yields} Definitions of super signal regions, along with predictions, observed data, and the observed 95\% CL limit 
on the number of signal events contributing to each region ($N_{95}^{obs}$).  No uncertainty on the signal acceptance is 
assumed in calculating these limits.  A dash in the selections means that no cut is applied.}
 \begin{tabular}{l||c|c|c|c||c|c|c}                                                                                                                                                                                                                                       
      \hline
      Region & $N_\text{jets}$ & $N_\text{b-tags}$ & $H_\text{T}$ [GeV] & $M_\text{T2}$ [GeV] & Prediction & Data & $N_{95}^{obs}$ \\
      \hline
"""
order = ["2j loose", "2j tight", "4j loose", "4j tight", "7j loose", "7j tight", "10j loose", "10j tight",
         "2b loose", "2b tight", "3b loose", "3b tight", "4b loose", "4b tight",
         "7j3b loose", "7j3b tight", "7j4b loose", "7j4b tight", "10j4b loose", "10j4b tight", "MonoPhi"]

for ssr in order:
    pred, unc_up, unc_dn, obs = vals[ssr]
    sig = GetPull(pred, unc_up, obs)
    if ssr == "MonoPhi":
        nj, nb, ht, mt2 = "1-3", 0, "250-450","200-300 ($\geq$1j)"
        exp = int(log10(obs))
        pred /= 10**exp
        unc_up /= 10**exp
        sout += r"{0} & {1} & {2} & {3} & ${4}$ & $({5:.1f}\pm{6:.1f})\times10^{{{7:d}}}$ & ${8:.1f}\times10^{{{7:d}}}$ & - \\".format(
            ssr, nj, nb, ht, mt2, pred, unc_up, exp, obs/10.0**exp)
    else:
        nj, nb, ht, mt2 = ssrs[ssr]
        sout += r"{0} & $\geq{1}$ & {2} & $>{3}$ & $>{4}$ & ${5:.1f}\pm{6:.1f}$ & {7} & - \\".format(
            ssr, nj, "$\geq{0}$".format(nb) if nb>0 else "-", ht, mt2, pred, unc_up, obs)

    if "tight" in ssr or "MonoPhi" in ssr:
        sout += r" \hline"
    sout += '\n'

sout += r"""
\end{tabular}
\end{center}
\end{table}
"""

print sout
