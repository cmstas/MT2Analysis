import os
import ROOT as r
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

MODEL = 'q'  # t, b, or q
outdir = "/home/users/bemarsh/public_html/mt2/limits/V00-10-16_FullRunII_newQCD/LQ"
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

if MODEL=='t':
    fin2 = r.TFile("../PlotsSMS/config/mt2/extraT2tt_results.root")
    for lim in lnames:
        h = fin2.Get("{0}_xs0".format(lim))
        for ibin in range(1, h.GetNbinsX()+1):
            val = h.GetBinContent(ibin, 2)
            if val > 0 and h.GetXaxis().GetBinCenter(ibin) > 2000:
                limits[lim].append((h.GetXaxis().GetBinCenter(ibin), val))


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
c.SetRightMargin(0.07)

h_axis.GetXaxis().SetRangeUser(500, 2300)
h_axis.GetYaxis().SetRangeUser(5e-5, 2e3)
h_axis.GetXaxis().SetTitle("m_{LQ} [GeV]")
h_axis.GetXaxis().SetTitleOffset(1.05)
h_axis.GetXaxis().SetTitleSize(0.042)
h_axis.GetXaxis().SetNdivisions(505)
h_axis.GetYaxis().SetTitle("#sigma #times BR^{2} [pb]")
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
xs_vector_br0p5.SetLineColor(r.kMagenta)
xs_vector_br0p5.SetLineWidth(2)
xs_vector_br0p5DN.SetLineColor(r.kMagenta)
xs_vector_br0p5DN.SetLineWidth(1)
xs_vector_br0p5DN.SetLineStyle(2)
xs_vector_br0p5UP.SetLineColor(r.kMagenta)
xs_vector_br0p5UP.SetLineWidth(1)
xs_vector_br0p5UP.SetLineStyle(2)

# xs_vector.SetLineColor(r.kRed)
# xs_vector.SetLineWidth(2)
# xs_vector_br0p5.SetLineColor(r.kMagenta)
# xs_vector_br0p5.SetLineWidth(2)

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

h_axis.Draw("AXIS SAME")


text = r.TLatex()
text.SetNDC(1)
text.SetTextFont(62)
text.SetTextSize(0.045)
text.SetTextAlign(11)
text.DrawLatex(0.14, 0.91, "CMS")
text.SetTextFont(52)
# text.DrawLatex(0.24, 0.91, "Preliminary")
text.SetTextFont(42)
text.SetTextAlign(31)
text.DrawLatex(0.92,0.91, "137 fb^{-1} (13 TeV)")

text.SetTextFont(42)
text.SetTextSize(0.033)
text.SetTextAlign(11)
text.DrawLatex(0.18, 0.82, "pp #rightarrow LQ LQ")
if MODEL == 't':
    text.DrawLatex(0.18, 0.77, "BR(LQ #rightarrow t #nu) = 1 #minus #beta")
elif MODEL == 'b':
    text.DrawLatex(0.18, 0.77, "BR(LQ #rightarrow b #nu) = 100%")
elif MODEL == 'q':
    text.DrawLatex(0.18, 0.77, "BR(LQ #rightarrow q #nu) = 100%")
    text.DrawLatex(0.18, 0.72, "(q = u, d, s, or c)")
    
x1, y1, x2, y2 = 0.5, 0.38 if MODEL=='t' else 0.4425, 0.9, 0.88
leg = r.TLegend(x1, y1, x2, y2)
leg.SetFillStyle(0)
leg.SetLineWidth(0)
leg.AddEntry(obs, "Observed Limit (95% CL)", 'l')
leg.AddEntry(exp, "Median expected limit", 'l')
leg.AddEntry(exp1s, "68% expected", 'f')
leg.AddEntry(exp2s, "95% expected", 'f')
if MODEL == 't':
    leg.AddEntry(xs_vector, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1; #beta = 0)", 'l')
    leg.AddEntry(xs_vector_br0p5, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1; #beta = 0.5)", 'l')
    leg.AddEntry(xs_vectork0, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 0; #beta = 0)", 'l')
    leg.AddEntry(xs_scalar, "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, LO} (#beta = 0)", 'l')
else:
    leg.AddEntry(xs_vector,   "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 1)", 'l')
    leg.AddEntry(xs_vectork0, "#sigma^{pp #rightarrow LQ_{V} LQ_{V}}_{theory, LO} (#kappa = 0)", 'l')
    leg.AddEntry(xs_scalar,   "#sigma^{pp #rightarrow LQ_{S} LQ_{S}}_{theory, LO}", 'l')
leg.Draw()

line = r.TLine()
line.SetLineStyle(2)
delta = (y2-y1)/(8 if MODEL=='t' else 7)
line.SetLineColor(r.kRed)
line.DrawLineNDC(0.5152, y2-4.5*delta+0.01, 0.5853, y2-4.5*delta+0.01)
line.DrawLineNDC(0.5152, y2-4.5*delta-0.01, 0.5853, y2-4.5*delta-0.01)
if MODEL == 't':
    line.SetLineColor(r.kMagenta)
    line.DrawLineNDC(0.5152, y2-5.5*delta+0.01, 0.5853, y2-5.5*delta+0.01)
    line.DrawLineNDC(0.5152, y2-5.5*delta-0.01, 0.5853, y2-5.5*delta-0.01)
    line.SetLineColor(r.kCyan+2)
    line.DrawLineNDC(0.5152, y2-6.5*delta+0.01, 0.5853, y2-6.5*delta+0.01)
    line.DrawLineNDC(0.5152, y2-6.5*delta-0.01, 0.5853, y2-6.5*delta-0.01)
    line.SetLineColor(r.kBlue)
    line.DrawLineNDC(0.5152, y2-7.5*delta+0.01, 0.5853, y2-7.5*delta+0.01)
    line.DrawLineNDC(0.5152, y2-7.5*delta-0.01, 0.5853, y2-7.5*delta-0.01)
else:
    line.SetLineColor(r.kCyan+1)
    line.DrawLineNDC(0.5152, y2-5.5*delta+0.01, 0.5853, y2-5.5*delta+0.01)
    line.DrawLineNDC(0.5152, y2-5.5*delta-0.01, 0.5853, y2-5.5*delta-0.01)
    line.SetLineColor(r.kBlue) 
    line.DrawLineNDC(0.5152, y2-6.5*delta+0.01, 0.5853, y2-6.5*delta+0.01)
    line.DrawLineNDC(0.5152, y2-6.5*delta-0.01, 0.5853, y2-6.5*delta-0.01)

c.SaveAs(outdir+"/LQ_{0}.png".format(MODEL))
c.SaveAs(outdir+"/LQ_{0}.pdf".format(MODEL))




