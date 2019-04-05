import sys, os
import cPickle as pickle
sys.path.append("CardMaker")
import Datacard
import numpy as np
import ROOT as r
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

datacard_dir = "CardMaker/cards_V00-10-15_FullRunII/templates/"
datacards = pickle.load(open(os.path.join(datacard_dir, "template_datacards.pkl"), 'rb'))
# use MC to estimate fraction of lostlep background from top/wjets
topfile = r.TFile("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-14_combined/top.root")
wjetsfile = r.TFile("/home/users/bemarsh/analysis/mt2/current/MT2Analysis/MT2Looper/output/V00-10-14_combined/wjets_ht.root")

ht_reg = "HT1200to1500"

# hardcode these because they're different
monojet_regs = [
    "HT1200toInf_j1_b0",
    "HT700toInf_j1_b1toInf"
]

MAX_NJ = 10
MAX_NB = 4

trs = {}
for name,dc in datacards.items():
    tr = name.split("_m")[0]
    if "j1_" in name and tr in monojet_regs:
        jbj_reg = "j" + tr.split("_j")[-1]
        if jbj_reg not in trs:
            trs[jbj_reg] = {"cards":[]}
        trs[jbj_reg]["cards"].append(name)
        trs[jbj_reg]["nj"] = (1,1)
        if "b0" in tr:
            trs[jbj_reg]["nb"] = (0,0)
        else:
            trs[jbj_reg]["nb"] = (1,MAX_NB)
        
    elif "j1_" not in name and ht_reg in tr:
        if tr not in trs:
            trs[tr] = {"cards":[]}
            nj = tr.split("j")[-1].split("_")[0].replace("Inf", str(MAX_NJ))
            if "to" in nj:
                trs[tr]["nj"] = tuple(int(x) for x in nj.split("to"))
            else:
                trs[tr]["nj"] = tuple(int(nj), int(nj))
            nb = tr.split("b")[-1].replace("Inf", str(MAX_NB))
            if "to" in nb:
                trs[tr]["nb"] = tuple(int(x) for x in nb.split("to"))
            else:
                trs[tr]["nb"] = (int(nb), int(nb))

        trs[tr]["cards"].append(name)


for tr in trs:
    cards = trs[tr]["cards"]
    llep, zinv, qcd, top, wjets = 0.0, 0.0, 0.0, 0.0, 0.0
    for card in cards:
        dc = datacards[card]
        rates = datacards[card].GetBackgroundRates()
        llep += rates["llep"]
        zinv += rates["zinv"]
        qcd += rates["qcd"]

        dirname = dc.info["dirname"]
        topmc = topfile.Get(dirname).Get("h_mt2bins").Integral()
        wjetsmc = wjetsfile.Get(dirname).Get("h_mt2bins").Integral()
        top += rates["llep"] * topmc / (topmc + wjetsmc)
        wjets += rates["llep"] * wjetsmc / (topmc + wjetsmc)

    total = llep + zinv + qcd
    trs[tr]["llep"] = llep / total
    trs[tr]["top"] = top / total
    trs[tr]["wjets"] = wjets / total
    trs[tr]["zinv"] = zinv / total
    trs[tr]["qcd"] = qcd / total

    # print tr, trs[tr]["llep"], trs[tr]["top"], trs[tr]["wjets"], trs[tr]["zinv"], trs[tr]["qcd"]


c = r.TCanvas("c","c",900,900)
xmin, xmax = 0.44, MAX_NJ+1.1
ymin, ymax = -0.56, MAX_NB+2.0
c.SetBottomMargin(0.2)
c.SetTopMargin(0.2)
c.SetLeftMargin(0.14)
c.SetRightMargin(0.02)
left = c.GetLeftMargin()
right = c.GetRightMargin()
top = c.GetTopMargin()
bottom = c.GetBottomMargin()

# hdummy = r.TH2D("hdummy","", 1, xmin, xmax, 1, ymin, ymax)
# hdummy.Draw("AXIS")

hzinv = r.TH1D("hzinv", "", 1, 0, 1)
htop = r.TH1D("htop", "", 1, 0, 1)
hwjets = r.TH1D("hwjets", "", 1, 0, 1)
hqcd = r.TH1D("hqcd", "", 1, 0, 1)

hzinv.SetFillColor(418)
htop.SetFillColor(855)
hwjets.SetFillColor(417)
hqcd.SetFillColor(401)

def ConvertX(x):
    return left + (1.0-left-right) * (x - xmin) / (xmax - xmin)
def ConvertY(y):
    return bottom + (1.0-top-bottom) * (y - ymin) / (ymax - ymin)

maxy2 = 0.0
pies = []
paves = []
for tr in trs:
    vals = [trs[tr]["zinv"], trs[tr]["wjets"], trs[tr]["top"], trs[tr]["qcd"]]
    colors = [418, 417, 855, 401]
    pie = r.TPie("pie"+tr, "", 4)
    for i in range(len(vals)):
        pie.SetEntryVal(i, vals[i])
        pie.SetEntryLabel(i, "")
        pie.SetEntryFillColor(i, colors[i])
    nj_cent = sum(trs[tr]["nj"]) / 2.0
    nb_cent = sum(trs[tr]["nb"]) / 2.0
    x = ConvertX(nj_cent)
    y = ConvertY(nb_cent)
    rad = 0.4 * min(1.0 / (xmax-xmin) * (1.0-left-right), 1.0 / (ymax-ymin) * (1.0-top-bottom))
    
    pie.SetCircle(x, y, rad)
    # pie.SetX(nj_cent)
    # pie.SetY(nb_cent)
    pie.Draw("SAME nol" if len(pies)>0 else "nol")

    x1 = ConvertX(trs[tr]["nj"][0]-0.45)
    x2 = ConvertX(trs[tr]["nj"][1]+0.45)
    y1 = ConvertY(trs[tr]["nb"][0]-0.45)
    y2 = ConvertY(trs[tr]["nb"][1]+0.45)
    maxy2 = max(maxy2, y2)
    pave = r.TPave(x1, y1, x2, y2, 4, "NDC BR ARC")
    pave.SetFillStyle(0)
    pave.SetCornerRadius(50)
    # pave.SetLineColor(855)
    pave.SetLineColor(r.kRed-9)
    pave.SetLineWidth(3)
    pave.Draw("SAME")

    pies.append(pie)
    paves.append(pave)

arrow = r.TArrow(0, 0, 0, 0, 0.01, "|>")
arrow.SetLineWidth(3)
arrow.DrawArrow(left, bottom, 1.0-right, bottom)
arrow.DrawArrow(left, bottom, left, 1.0-top)

lx1 = left + 0.02
lx2 = left + 0.56*(1.0-right-left)
ly1 = maxy2+0.02
ly2 = 1.0-top-0.01
leg = r.TLegend(lx1, ly1, lx2, ly2)
leg.SetBorderSize(0)
leg.SetTextFont(62)
leg.SetNColumns(2)
leg.AddEntry(hzinv, "Z #rightarrow #nu#bar{#nu}", 'f')
leg.AddEntry(hwjets, "W+jets", 'f')
leg.AddEntry(htop, "Top quark", 'f')
leg.AddEntry(hqcd, "Multijet", 'f')
leg.Draw()

htlow = int(ht_reg.split("to")[0].split("HT")[-1])
hthi = int(ht_reg.split("to")[1])

text = r.TLatex()
text.SetNDC(1)
text.SetTextFont(42)
text.SetTextSize(0.036)
text.SetTextAlign(11)
text.DrawLatex(lx2+0.02, ly1+0.015, "H_{{T}} [{0}, {1}] GeV".format(htlow, hthi))

text.SetTextFont(62)
text.SetTextSize(0.036)
text.DrawLatex(left+0.01, 1.0-top+0.02, "CMS")
text.SetTextFont(52)
text.SetTextSize(0.036)
text.DrawLatex(left+0.089, 1.0-top+0.02, "Preliminary")
text.SetTextFont(42)
text.SetTextAlign(31)
text.DrawLatex(1.0-right-0.06, 1.0-top+0.02, "(13 TeV)")

text.SetTextFont(42)
text.SetTextAlign(22)
text.SetTextSize(0.035)
for i in range(1, MAX_NJ+1):
    s = str(i)
    if i==MAX_NJ:
        s = "#geq "+s
    text.DrawLatex(ConvertX(i), ConvertY(ymin-0.3), s)
for i in range(0, MAX_NB+1):
    s = str(i)
    if i==MAX_NB:
        s = "#geq "+s
    text.DrawLatex(ConvertX(xmin-0.4), ConvertY(i), s)

text.SetTextSize(0.046)
text.SetTextFont(42)
text.DrawLatex(ConvertX((MAX_NJ+1)/2.0), ConvertY(ymin-1.0), "N_{J} (p_{T} > 30 GeV)")
text.SetTextAngle(90)
text.DrawLatex(ConvertX(xmin-1.2), ConvertY(MAX_NB/2.0), "N_{b} (p_{T} > 20 GeV)")

c.SaveAs("~/public_html/test.pdf")


