import ROOT as r
r.gStyle.SetOptStat(0)
r.gStyle.SetPalette(r.kTemperatureMap)
r.gStyle.SetNumberContours(32)

# tag1 = "V00-10-17_FullRunII_withDYContam"
tag1 = "V00-10-17_FullRunII"
tag2 = "V00-10-18_FullRunII"

MODEL = "T2qq"
hist = "obs_mu0"

f1 = r.TFile("limits_{0}_{1}.root".format(MODEL, tag1))
f2 = r.TFile("limits_{0}_{1}.root".format(MODEL, tag2))

h1 = f1.Get(hist)
h2 = f2.Get(hist)

h2.Divide(h1)

gr = r.TGraph2D(h2)
gr.SetNpx(int((gr.GetXmax() - gr.GetXmin()) / 10))
gr.SetNpy(int((gr.GetYmax() - gr.GetYmin()) / 10))
h_interp = gr.GetHistogram()

level = 0.1
h_interp.GetZaxis().SetRangeUser(1-level,1+level)
h2.GetZaxis().SetRangeUser(1-level,1+level)

print h_interp.GetBinContent(h_interp.GetMaximumBin())
print h_interp.GetBinContent(h_interp.GetMinimumBin())

h_interp.Draw("colz")
# h2.Draw("colz")

raw_input()
