import os
import ROOT as r
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

MODEL = 'q'  # t, b, or q
outdir = "/home/users/bemarsh/public_html/mt2/limits/V00-10-17_FullRunII_withDYContam/LQ"
os.system("mkdir -p "+outdir)

fin = r.TFile("../PlotsSMS/config/mt2/T2{0}{0}_results.root".format(MODEL))

lnames = ["obs", "exp", "ep1s", "ep2s", "em1s", "em2s"]
limits = {}
for lim in lnames:
    h = fin.Get("{0}_xs0".format(lim))
    limits[lim] = []
    for ibin in range(1, h.GetNbinsX()+1):
        val = h.GetBinContent(ibin, 2)
        if val > 0:
            limits[lim].append((h.GetXaxis().GetBinCenter(ibin), val))

print limits["obs"]

if MODEL=='t':
    fin2 = r.TFile("../PlotsSMS/config/mt2/extraT2tt_results.root")
    for lim in lnames:
        h = fin2.Get("{0}_xs0".format(lim))
        for ibin in range(1, h.GetNbinsX()+1):
            val = h.GetBinContent(ibin, 2)
            if val > 0 and h.GetXaxis().GetBinCenter(ibin) > 2000:
                limits[lim].append((h.GetXaxis().GetBinCenter(ibin), val))

# if MODEL=='q':
#     limits["obs"].insert(0, (500.0, 0.0477))
#     limits["exp"].insert(0, (500.0, 0.0687))
#     limits["ep1s"].insert(0, (500.0, 0.0928))
#     limits["ep2s"].insert(0, (500.0, 0.1237))
#     limits["em1s"].insert(0, (500.0, 0.0530))
#     limits["em2s"].insert(0, (500.0, 0.0408))

if MODEL=='b':
    limits["obs"].insert(2, (500.0, 0.0112))
    limits["exp"].insert(2, (500.0, 0.0140))
    limits["ep1s"].insert(2, (500.0, 0.0195))
    limits["ep2s"].insert(2, (500.0, 0.0262))
    limits["em1s"].insert(2, (500.0, 0.0102))
    limits["em2s"].insert(2, (500.0, 0.00764))

obs = r.TGraph()
exp = r.TGraph()
exp1s = r.TGraphAsymmErrors()
exp2s = r.TGraphAsymmErrors()
for i,(x,y) in enumerate(limits["obs"]):
    obs.SetPoint(i, x, y)
for i,(x,y) in enumerate(limits["exp"]):
    ep1s = limits["ep1s"][i][1]
    ep2s = limits["ep2s"][i][1]
    em1s = limits["em1s"][i][1]
    em2s = limits["em2s"][i][1]

    exp.SetPoint(i, x, y)
    exp1s.SetPoint(i, x, y)
    exp2s.SetPoint(i, x, y)
    exp1s.SetPointError(i, 0, 0, y-em1s, ep1s-y)
    exp2s.SetPointError(i, 0, 0, y-em2s, ep2s-y)

xs_file = r.TFile("lq-xsec-MG-NNPDF-graph_0p25_k.root")
xs_scalar = xs_file.Get("slqxsec")
xs_scalarDN = xs_file.Get("slqxsecDown")
xs_scalarUP = xs_file.Get("slqxsecUp")
xs_vector = xs_file.Get("vlqxsec")
xs_vectorDN = xs_file.Get("vlqxsecDown")
xs_vectorUP = xs_file.Get("vlqxsecUp")
xs_vectork0 = xs_file.Get("vlqxsec_k0")
xs_vectork0DN = xs_file.Get("vlqxsecDown_k0")
xs_vectork0UP = xs_file.Get("vlqxsecUp_k0")
xs_vector_br0p5 = xs_file.Get("vlqxsec0p25")
xs_vector_br0p5DN = xs_file.Get("vlqxsec0p25Down")
xs_vector_br0p5UP = xs_file.Get("vlqxsec0p25Up")
xs_vectork0_br0p5 = xs_file.Get("vlqxsec0p25_k0")
xs_vectork0_br0p5DN = xs_file.Get("vlqxsec0p25Down_k0")
xs_vectork0_br0p5UP = xs_file.Get("vlqxsec0p25Up_k0")

for g in [xs_scalar,xs_scalarDN,xs_scalarUP,xs_vector,xs_vectorDN,xs_vectorUP,xs_vectork0,xs_vectork0DN,xs_vectork0UP,xs_vector_br0p5,xs_vector_br0p5DN,xs_vector_br0p5UP,xs_vectork0_br0p5,xs_vectork0_br0p5DN,xs_vectork0_br0p5UP]:
    N = g.GetN()
    x,y = r.Double(), r.Double()
    g.GetPoint(N-1, x, y)
    if x != 2300:
        raise Exception("x should be 2300 and it is {0}".format(x))
    x0, y0 = r.Double(), r.Double()
    g.GetPoint(N-2, x0, y0)
    xnew = 2400
    ynew = y * (y/y0)**((xnew-x)/(x-x0))
    g.SetPoint(N, xnew, ynew)


def FindInt(g1, g2):
    for i in range(g1.GetN()-1):
        for j in range(g2.GetN()-1):
            x1, y1, u1, v1 = r.Double(), r.Double(), r.Double(), r.Double()
            g1.GetPoint(i, x1, y1)
            g1.GetPoint(i+1, u1, v1)
            x2, y2, u2, v2 = r.Double(), r.Double(), r.Double(), r.Double()
            g2.GetPoint(j, x2, y2)
            g2.GetPoint(j+1, u2, v2)

            m1 = (v1-y1)/(u1-x1)
            m2 = (v2-y2)/(u2-x2)
            
            x = (m1*x1 - m2*x2 - y1 + y2) / (m1-m2)
            y = m1*(x - x1) + y1

            if not min(x1,u1) < x < max(x1,u1):
                continue
            if not min(x2,u2) < x < max(x2,u2):
                continue

            return (x,y)
    return None

tofind_int = [xs_scalar, xs_vector, xs_vectork0]
names = ["Scalar", "Vector (k=1, B=0)", "Vector (k=0, B=0)"]
if MODEL=='t':
    tofind_int += [xs_vector_br0p5, xs_vectork0_br0p5]
    names += ["Vector (k=1, B=0.5)", "Vector (k=0, B=0.5)"]

# print FindInt(obs, xs_vector)
for g,n in zip(tofind_int,names):
    print "{0}: {1:.0f} ({2:.0f})".format(n,  FindInt(obs, g)[0], FindInt(exp, g)[0])

# xs_scalar = r.TGraph()
# xs_vector = r.TGraph()
# xs_vector_br0p5 = r.TGraph()
# with open("xsecs.txt") as fid:
#     i = 0
#     for line in fid:
#         sp = line.strip().split()
#         m = float(sp[0])
#         s = float(sp[1])
#         v = float(sp[2])
#         xs_scalar.SetPoint(i, m, s)
#         xs_vector.SetPoint(i, m, v)
#         xs_vector_br0p5.SetPoint(i, m, v*0.5*0.5)
#         i += 1

h_axis = r.TH1F("h_axis","",60,0,3000)

c = r.TCanvas("c","c",800,800)
c.SetLogy(1)
c.SetTickx(1)
c.SetTicky(1)

c.SetLeftMargin(0.13)
c.SetRightMargin(0.04)
c.SetTopMargin(0.07)

h_axis.GetXaxis().SetRangeUser(500, 2500)
h_axis.GetYaxis().SetRangeUser(5e-5, 3e3)
h_axis.GetXaxis().SetTitle("m_{#kern[0.1]{LQ}} [GeV]")
h_axis.GetXaxis().SetTitleOffset(1.05)
h_axis.GetXaxis().SetTitleSize(0.042)
h_axis.GetXaxis().SetNdivisions(505)
h_axis.GetYaxis().SetTitle("#sigma #bf{#it{#beta}}^{2} [pb]")
h_axis.GetYaxis().SetTitleSize(0.042)

h_axis.Draw()

exp2s.SetFillColor(r.kOrange-2)
exp1s.SetFillColor(r.kGreen+1)
exp.SetLineStyle(7)
exp.SetLineWidth(3)

exp2s.Draw("3 SAME")
exp1s.Draw("3 SAME")
exp.Draw("L SAME")

obs.SetLineColor(r.kBlack)
obs.SetLineWidth(3)
obs.Draw("L SAME")

xs_scalar.SetLineColor(r.kBlue)
xs_scalar.SetLineWidth(2)
xs_scalarDN.SetLineColor(r.kBlue)
xs_scalarDN.SetLineWidth(1)
xs_scalarDN.SetLineStyle(2)
xs_scalarUP.SetLineColor(r.kBlue)
xs_scalarUP.SetLineWidth(1)
xs_scalarUP.SetLineStyle(2)
xs_vector.SetLineColor(r.kRed)
xs_vector.SetLineWidth(2)
xs_vectorDN.SetLineColor(r.kRed)
xs_vectorDN.SetLineWidth(1)
xs_vectorDN.SetLineStyle(2)
xs_vectorUP.SetLineColor(r.kRed)
xs_vectorUP.SetLineWidth(1)
xs_vectorUP.SetLineStyle(2)
xs_vectork0.SetLineColor(r.kCyan+1)
xs_vectork0.SetLineWidth(2)
xs_vectork0DN.SetLineColor(r.kCyan+1)
xs_vectork0DN.SetLineWidth(1)
xs_vectork0DN.SetLineStyle(2)
xs_vectork0UP.SetLineColor(r.kCyan+1)
xs_vectork0UP.SetLineWidth(1)
xs_vectork0UP.SetLineStyle(2)
xs_vector_br0p5.SetLineColor(r.kMagenta-7)
xs_vector_br0p5.SetLineWidth(2)
xs_vector_br0p5DN.SetLineColor(r.kMagenta-7)
xs_vector_br0p5DN.SetLineWidth(1)
xs_vector_br0p5DN.SetLineStyle(2)
xs_vector_br0p5UP.SetLineColor(r.kMagenta-7)
xs_vector_br0p5UP.SetLineWidth(1)
xs_vector_br0p5UP.SetLineStyle(2)
xs_vectork0_br0p5.SetLineColor(r.kBlack)
xs_vectork0_br0p5.SetLineWidth(2)
xs_vectork0_br0p5DN.SetLineColor(r.kBlack)
xs_vectork0_br0p5DN.SetLineWidth(1)
xs_vectork0_br0p5DN.SetLineStyle(2)
xs_vectork0_br0p5UP.SetLineColor(r.kBlack)
xs_vectork0_br0p5UP.SetLineWidth(1)
xs_vectork0_br0p5UP.SetLineStyle(2)

xs_scalar.Draw("L SAME")
xs_scalarDN.Draw("L SAME")
xs_scalarUP.Draw("L SAME")
xs_vector.Draw("L SAME")
xs_vectorDN.Draw("L SAME")
xs_vectorUP.Draw("L SAME")
xs_vectork0.Draw("L SAME")
xs_vectork0DN.Draw("L SAME")
xs_vectork0UP.Draw("L SAME")
if MODEL == 't':
    xs_vector_br0p5.Draw("L SAME")
    xs_vector_br0p5DN.Draw("L SAME")
    xs_vector_br0p5UP.Draw("L SAME")

    # xs_vectork0_br0p5.Draw("L SAME")
    # xs_vectork0_br0p5DN.Draw("L SAME")
    # xs_vectork0_br0p5UP.Draw("L SAME")

h_axis.Draw("AXIS SAME")


text = r.TLatex()
text.SetNDC(1)
text.SetTextFont(62)
text.SetTextSize(0.045)
text.SetTextAlign(11)
text.DrawLatex(0.14, 0.94, "CMS")
text.SetTextFont(52)
# text.DrawLatex(0.24, 0.91, "Preliminary")
text.SetTextFont(42)
text.SetTextAlign(31)
text.DrawLatex(0.95,0.94, "137 fb^{-1} (13 TeV)")

text.SetTextFont(42)
text.SetTextSize(0.033)
text.SetTextAlign(11)
x = 0.17
y = 0.85
text.DrawLatex(x, y, "pp #rightarrow LQ LQ")
if MODEL == 't':
    text.DrawLatex(x, y-0.05, "#bf{#it{#Beta}}(LQ #rightarrow t #nu) = 1 #minus #beta")
elif MODEL == 'b':
    text.DrawLatex(x, y-0.05, "#bf{#it{#Beta}}(LQ #rightarrow b #nu) = 100%")
elif MODEL == 'q':
    text.DrawLatex(x, y-0.05, "#bf{#it{#Beta}}(LQ #rightarrow q #nu) = 100%")
    text.DrawLatex(x, y-0.10, "(q = u, d, s, or c)")
    
# x1, y1, x2, y2 = 0.47, 0.41 if MODEL=='t' else 0.4725, 0.925, 0.91
# leg = r.TLegend(x1, y1, x2, y2)
# leg.SetFillStyle(0)
# leg.SetLineWidth(0)
# leg.AddEntry(obs, "Observed Limit (95% CL)", 'l')
# leg.AddEntry(exp, "Median expected limit", 'l')
# leg.AddEntry(exp1s, "#scale[1.2]{68% expected}", 'f')
# leg.AddEntry(exp2s, "95% expected", 'f')
# if MODEL == 't':
#     leg.AddEntry(xs_vector, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1; #beta = 0)", 'l')
#     leg.AddEntry(xs_vector_br0p5, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1; #beta = 0.5)", 'l')
#     leg.AddEntry(xs_vectork0, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 0; #beta = 0)", 'l')
#     leg.AddEntry(xs_scalar, "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, NLO} (#beta = 0)", 'l')
# else:
#     leg.AddEntry(xs_vector,   "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1)", 'l')
#     leg.AddEntry(xs_vectork0, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 0)", 'l')
#     leg.AddEntry(xs_scalar,   "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, NLO}", 'l')
# leg.Draw()

compression = 0.81 if MODEL=='t' else 0.85
x1 = 0.48
x2 = 0.54
y1 = 0.40
y2 = 0.90
delta = (y2-y1) / 8
text.SetTextFont(42)
text.SetTextSize(0.035)
text.SetTextAlign(12)
line = r.TLine()
line.SetLineStyle(1)
line.SetLineWidth(3)
line.SetLineColor(r.kBlack)
y = y2-0.5*delta*compression
line.DrawLineNDC(x1, y, x2, y)
text.DrawLatex(x2+0.02, y, "Observed limit (95% CL)")
line.SetLineStyle(2)
y = y2-1.5*delta*compression
line.DrawLineNDC(x1, y, x2, y)
text.DrawLatex(x2+0.02, y, "Median expected limit")
y = y2-2.5*delta*compression
box1 = r.TLegend(x1, y-0.32*delta, x2, y+0.32*delta)
box1.SetFillColor(r.kGreen+1)
box1.SetLineWidth(0)
box1.Draw()
text.DrawLatex(x2+0.02, y, "68% expected")
y = y2-3.5*delta*compression
box2 = r.TLegend(x1, y-0.32*delta, x2, y+0.32*delta)
box2.SetFillColor(r.kOrange-2)
box2.SetLineWidth(0)
box2.Draw()
text.DrawLatex(x2+0.02, y, "95% expected")

def DrawLines(line, y, color):
    line.SetLineWidth(1)
    line.SetLineStyle(2)
    line.SetLineColor(color)
    line.DrawLineNDC(x1, y+0.01, x2, y+0.01)
    line.DrawLineNDC(x1, y-0.01, x2, y-0.01)
    line.SetLineWidth(2)
    line.SetLineStyle(1)
    line.DrawLineNDC(x1, y, x2, y)
newy2 = y2-4*delta*compression
newdelta = (newy2-y1) / 4
newdelta *= 1.2 if MODEL!='t' else 1.0
y = newy2-0.5*newdelta
DrawLines(line, y, r.kRed)
text.SetTextSize(0.040)
if MODEL == 't':
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} #scale[0.80]{(#kappa = 1; #beta = 0)}")
else:
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} #scale[0.80]{(#kappa = 1)}")
y = newy2-1.5*newdelta
DrawLines(line, y, r.kMagenta if MODEL=='t' else r.kCyan+1)
if MODEL == 't':
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} #scale[0.80]{(#kappa = 1; #beta = 0.5)}")
else:
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} #scale[0.80]{(#kappa = 0)}")
y = newy2-2.5*newdelta
DrawLines(line, y, r.kCyan+1 if MODEL=='t' else r.kBlue)
if MODEL == 't':
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} #scale[0.80]{(#kappa = 0; #beta = 0)}")
else:
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, NLO}")
if MODEL == 't':
    y = newy2-3.5*newdelta
    DrawLines(line, y, r.kBlue)
    text.DrawLatex(x2+0.02, y, "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, NLO} #scale[0.80]{(#beta = 0)}")

# line.SetLineColor(r.kMagenta)
# line.DrawLineNDC(x1, y2-5.5*delta+0.01, x2, y2-5.5*delta+0.01)
# line.DrawLineNDC(x1, y2-5.5*delta-0.01, x2, y2-5.5*delta-0.01)
# line.SetLineColor(r.kCyan+2)
# line.DrawLineNDC(x1, y2-6.5*delta+0.01, x2, y2-6.5*delta+0.01)
# line.DrawLineNDC(x1, y2-6.5*delta-0.01, x2, y2-6.5*delta-0.01)
# line.SetLineColor(r.kBlue)
# line.DrawLineNDC(x1, y2-7.5*delta+0.01, x2, y2-7.5*delta+0.01)
# line.DrawLineNDC(x1, y2-7.5*delta-0.01, x2, y2-7.5*delta-0.01)
# else:
#     line.SetLineColor(r.kCyan+1)
#     line.DrawLineNDC(x1, y2-5.5*delta+0.01, x2, y2-5.5*delta+0.01)
#     line.DrawLineNDC(x1, y2-5.5*delta-0.01, x2, y2-5.5*delta-0.01)
#     line.SetLineColor(r.kBlue) 
#     line.DrawLineNDC(x1, y2-6.5*delta+0.01, x2, y2-6.5*delta+0.01)
#     line.DrawLineNDC(x1, y2-6.5*delta-0.01, x2, y2-6.5*delta-0.01)

c.SaveAs(outdir+"/LQ_{0}.png".format(MODEL))
c.SaveAs(outdir+"/LQ_{0}.pdf".format(MODEL))


