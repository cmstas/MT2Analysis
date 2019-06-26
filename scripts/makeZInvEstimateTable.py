import ROOT as r

fin = r.TFile("../MT2Looper/output/V00-10-16_combined_newEGamma/zinvFromDY.root")

topo_nums_j = [1,2,3,4,5,6,7,11,12,13,14,15]
topo_nums_vll = [1,2,3,4,5,6,7,8,9,10,11]
topo_nums_mhuh = [1,2,3,4,5,6,10,20,21,22,23,24,25,26,27,28,29]

srs = []
srs += ["sr{0}J".format(i) for i in topo_nums_j]
srs += ["sr{0}VL".format(i) for i in topo_nums_vll]
srs += ["sr{0}L".format(i) for i in topo_nums_vll]
srs += ["sr{0}M".format(i) for i in topo_nums_mhuh]
srs += ["sr{0}H".format(i) for i in topo_nums_mhuh]
srs += ["sr{0}UH".format(i) for i in topo_nums_mhuh]

def GetHTNJNB(srname):
    srname = srname[2:]
    if "VL" in srname or "UH" in srname:
        topo_reg = int(srname[:-2])
        ht_reg = srname[-2:]
    else:
        topo_reg = int(srname[:-1])
        ht_reg = srname[-1]

    HT, NJ, NB = "", "", ""
    if ht_reg == "J":
        NJ = "1"
        NB = "0"
        if topo_reg >= 10:
            NB = "$\geq1$"
        if topo_reg==1 : HT="$[250,350]$"
        if topo_reg==2 : HT="$[350,450]$"
        if topo_reg==3 : HT="$[450,575]$"
        if topo_reg==4 : HT="$[575,700]$"
        if topo_reg==5 : HT="$[700,1000]$"
        if topo_reg==6 : HT="$[1000,1200]$"
        if topo_reg==7 : HT="$\geq1200$"
        if topo_reg==11: HT="$[250,350]$"
        if topo_reg==12: HT="$[350,450]$"
        if topo_reg==13: HT="$[450,575]$"
        if topo_reg==14: HT="$[575,700]$"
        if topo_reg==15: HT="$\geq700$"
    else:
        if ht_reg == "VL": HT = "$[250,450]$"
        if ht_reg == "L":  HT = "$[450,575]$"
        if ht_reg == "M":  HT = "$[575,1200]$"
        if ht_reg == "H":  HT = "$[1200,1500]$"
        if ht_reg == "UH": HT = "$\geq1500$"
        
        if 1 <= topo_reg <= 3: NJ = "2-3"
        if 4 <= topo_reg <= 6: NJ = "4-6"
        if 7 <= topo_reg <= 9 or topo_reg==11: NJ = "$\geq7$"
        if topo_reg==10: NJ = "2-6"
        if 20 <= topo_reg <= 24: NJ = "7-9"
        if 25 <= topo_reg <= 29: NJ = "$\geq10$"

        if topo_reg in [1,4,7,20,25]: NB = "0"
        if topo_reg in [2,5,8,21,26]: NB = "1"
        if topo_reg in [3,6,9,22,27]: NB = "2"
        if topo_reg in [10,11]: NB = "$\geq3$"
        if topo_reg in [23,28]: NB = "3"
        if topo_reg in [24,29]: NB = "$\geq4$"

    return HT,NJ,NB



fout = open("/home/users/bemarsh/public_html/mt2/samples_tex/zinv_table.txt", 'w')
fout.write(r"""
\begin{table}[!ht]
\footnotesize
\caption{\label{tab:estimateZinv_lowHt} The control region estimated
 Drell-Yan (DY) yield, same-flavor (SF) yield, opposite-flavor (OF)
 yield, purity, and $R^{\zinv/\zll}_{\mathrm{MC}}$ for the invisible
 Z estimate as a function of \HT, \Nj, and \Nb, in the monojet, Very-Low \HT, and Low \HT regions. Note that the the
 regions marked with a * or $^\dagger$  share the same control region (with the same yield and purity),
 and the distribution of events in the  \Nb and/or \Nj dimensions are folded into the Ratio value.}
\centering
\begin{tabular}{c|c|c|c|c|c|c|c}
\hline\hline
\multicolumn{8}{c}{Invisible Z} \\ \hline
\multicolumn{3}{c}{Region} & DY Yield & SF Yield & OF Yield & Purity & Ratio \\
$H_T$ [GeV] & N$_j$ & N$_b$ & & & & & \\ \hline
""")

lastreg = ""
for srname in srs:
    reg = "VL" if "VL" in srname else "UH" if "UH" in srname else srname[-1]
    
    if lastreg=="L" and reg=="M":
        fout.write(r"""
\hline
\end{tabular}
\end{table}

\begin{table}[!ht]
\footnotesize
\caption{\label{tab:estimateZinv_highHt} Same as Table \ref{tab:estimateZinv_lowHt}, but for the Medium, High, and Extreme \HT regions.}
\centering
\begin{tabular}{c|c|c|c|c|c|c|c}
\hline\hline
\multicolumn{8}{c}{Invisible Z} \\ \hline
\multicolumn{3}{c}{Region} & DY Yield & SF Yield & OF Yield & Purity & Ratio \\
$H_T$ [GeV] & N$_j$ & N$_b$ & & & & & \\ \hline
""")
    if lastreg != reg:
        fout.write("\\hline \n")

    lastreg = reg

    h_purity = fin.Get(srname+"/purityCard")
    h_ratio = fin.Get(srname+"/ratioCard")
    h_cryield = fin.Get(srname+"/CRyieldCard")
    h_mccr = fin.Get(srname+"/h_mt2binsMCCR")
    h_ofyield = fin.Get(srname+"/CRyieldEMCard")

    purity = h_purity.GetBinContent(1)
    ratioerr = r.Double()
    ratio = h_ratio.IntegralAndError(1,-1,ratioerr)
    sfyield = int(h_cryield.GetBinContent(1))
    ofyield = int(h_ofyield.GetBinContent(1)) if h_ofyield else 0
    dyyield = h_mccr.Integral()

    rsfof = (1-purity)*sfyield/ofyield if ofyield>0 else 1.07

    alpha = 1-0.6827
    of_up = r.Math.gamma_quantile_c(alpha/2, ofyield+1, 1)
    of_dn = r.Math.gamma_quantile(alpha/2, ofyield, 1)
    
    purity_up = 1 - rsfof * of_dn/sfyield if sfyield>0 else purity
    purity_dn = 1 - rsfof * of_up/sfyield if sfyield>0 else purity

    perr_up = purity_up - purity
    perr_dn = purity - purity_dn

    if ratio < 0.001:
        ratio = round(ratio, 5)
        ratioerr = round(ratioerr, 5)
    elif ratio < 0.01:
        ratio = round(ratio, 4)
        ratioerr = round(ratioerr, 4)
    elif ratio < 0.1:
        ratio = round(ratio, 3)
        ratioerr = round(ratioerr, 3)
    else:
        ratio = round(ratio, 2)
        ratioerr = round(ratioerr, 2)

    # print srname, purity, ratio, sfyield, ofyield, dyyield

    HT, NJ, NB = GetHTNJNB(srname)

    asterisk = ""
    sr = srname[2:]
    if sr in ["8VL","9VL","11VL","8L","9L","11L"]: asterisk = " $^{(*)}$"
    if sr in ["20M","25M","20H","25H","20UH","25UH"]: asterisk = " $^{(*)}$"
    if sr in ["21M","22M","23M","24M","21H","22H","23H","24H","21UH","22UH","23UH","24UH"]: asterisk = " $^{(\dagger)}$"
    if sr in ["26M","27M","28M","29M","26H","27H","28H","29H","26UH","27UH","28UH","29UH"]: asterisk = " $^{(\dagger)}$"

    fout.write("{0} & {1} & {2} & {3:.1f} & {4:d} & {5:d} & ${6:.3f}^{{+{7:.3f}}}_{{-{8:.3f}}}$ & ${9} \\pm {10}${11} \\\\ \n".format(HT, NJ, NB, dyyield, sfyield, ofyield, purity, perr_up, perr_dn, ratio, ratioerr, asterisk))


fout.write(r"""
\hline
\end{tabular}
\end{table}
""")

fout.close()
