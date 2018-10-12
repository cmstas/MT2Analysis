import ROOT
import glob
import os

ROOT.gROOT.SetBatch(1)

# cr="crRSInvertDPhi"
# cr="crRSMT2SideBand"
cr="crRSDPhiMT2"

tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"
# tag = "V00-10-01_31Mar2018_usedByJason"
# tag = "moriond_MCJRT"
RSfromMC = False

# year = 2017
# lumi = 41.5

year = 2016
lumi = 35.9

username = os.environ["USER"]
outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/".format(username,tag)

dir_RS = "looper_output/{0}/{1}{2}".format(tag, "qcd" if RSfromMC else "data", year)
dir_data = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"
dir_ewk = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"
# dir_RS = "/home/users/bemarsh/analysis/mt2/2016_final/MT2Analysis/RebalanceAndSmear/plotting/looper_output/{0}/{1}".format(tag, "qcd" if RSfromMC else "data")
# dir_data = "/home/users/bemarsh/analysis/mt2/2016_final/MT2Analysis/RebalanceAndSmear/plotting/looper_output/moriond/data_noRS"
# dir_ewk = "/home/users/bemarsh/analysis/mt2/2016_final/MT2Analysis/RebalanceAndSmear/plotting/looper_output/moriond_MCJRT/ewk_noRS"

f_rs = ROOT.TFile(os.path.join(dir_RS,"merged_hists.root"))
f_data = ROOT.TFile(os.path.join(dir_data,"data_Run{0}.root".format(year)))
f_ewk = ROOT.TFile(os.path.join(dir_ewk,"ewk.root"))
# f_rs = ROOT.TFile(os.path.join(dir_RS,"merged_hists.root"))
# f_data = ROOT.TFile(os.path.join(dir_data,"merged_hists.root"))
# f_ewk = ROOT.TFile(os.path.join(dir_ewk,"ewk.root"))

hrs = ROOT.TH1D("hrs","",51,0,51)
hdata = ROOT.TH1D("hdata","",51,0,51)
hewk = ROOT.TH1D("hewk","",51,0,51)

h_evts_rs = ROOT.TH1D("h_evts_rs","",1,0,2)
h_evts_data = ROOT.TH1D("h_evts_data","",1,0,2)
h_evts_ewk = ROOT.TH1D("h_evts_ewk","",1,0,2)

os.system("mkdir -p "+outdir)
os.system("cp ~/scripts/index.php "+outdir)

top_regs_vl=[1,2,3,12,13,14,15]
ibin = 0
for ht_reg in ["VL","L","M","H","UH"]:
    top_regs = list(range(1,12))
    if ht_reg=="VL":
        top_regs = top_regs_vl
    for top_reg in top_regs:
        ibin += 1

        h_evts_rs = f_rs.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_data = f_data.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_ewk = f_ewk.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        
        if type(h_evts_rs)==type(ROOT.TObject()):
            h_evts_rs = ROOT.TH1D("fsafaea","",1,0,2)
        if type(h_evts_ewk)==type(ROOT.TObject()):
            h_evts_ewk = ROOT.TH1D("fsawea","",1,0,2)

        h_evts_ewk.Scale(lumi)
        # if ht_reg == "L":    h_evts_rs.Scale(0.5)
        # if ht_reg == "M":    h_evts_rs.Scale(0.5)
        # if ht_reg == "H":    h_evts_rs.Scale(0.5)
        # if ht_reg == "UH":   h_evts_rs.Scale(0.5)

        # h_evts_rs.Add(h_evts_ewk)

        try:
            hrs.SetBinContent(ibin, h_evts_rs.GetBinContent(1))
            hrs.SetBinError(ibin, h_evts_rs.GetBinError(1))
        except:
            pass

        try:
            hdata.SetBinContent(ibin, h_evts_data.GetBinContent(1))
            hdata.SetBinError(ibin, h_evts_data.GetBinError(1))
        except:
            pass
        try:
            hewk.SetBinContent(ibin, h_evts_ewk.GetBinContent(1))
            hewk.SetBinError(ibin, h_evts_ewk.GetBinError(1))
        except:
            pass


ROOT.gStyle.SetOptStat(0)

c = ROOT.TCanvas("c","c",900,600)

pads = []
pads.append(ROOT.TPad("1","1",0.0,0.18,1.0,1.0))
pads.append(ROOT.TPad("2","2",0.0,0.0,1.0,0.19))

pads[0].SetTopMargin(0.08)
pads[0].SetBottomMargin(0.13)
pads[0].SetRightMargin(0.05)
pads[0].SetLeftMargin(0.07)

pads[1].SetRightMargin(0.05)
pads[1].SetLeftMargin(0.07)

pads[0].Draw()
pads[1].Draw()
pads[0].cd()

pads[0].SetLogy(1)
pads[0].SetTickx(1)
pads[1].SetTickx(1)
pads[0].SetTicky(1)
pads[1].SetTicky(1)

hdata.SetLineColor(ROOT.kBlack)
hdata.SetMarkerColor(ROOT.kBlack)
hdata.SetMarkerStyle(20)

hewk.SetLineColor(ROOT.kBlack)
hewk.SetFillColor(855)

hrs.SetLineColor(ROOT.kBlack)
hrs.SetFillColor(401)
hrs.SetMarkerColor(401)
hrs.SetMarkerStyle(20)

hewk.GetYaxis().SetRangeUser(1e0,1e6)
hewk.GetXaxis().SetLabelSize(0)
hewk.GetXaxis().SetTickLength(0.02)
hewk.GetXaxis().SetNdivisions(hewk.GetNbinsX(), 0, 0)

hstack = ROOT.THStack()
hstack.Add(hewk)
hstack.Add(hrs)

minim = 99999
maxim = 0
for i in range(hdata.GetNbinsX()):
    minim = min(minim, hdata.GetBinContent(i) if hdata.GetBinContent(i)>0 else minim)
    maxim = max(maxim, hdata.GetBinContent(i))
hstack.SetMinimum(minim)
hstack.SetMaximum(maxim**1.5)

hewk.Draw("HIST")
hstack.Draw("HIST SAME")
hdata.Draw("PE SAME")
# hrs.Draw("PE SAME")
hewk.Draw("AXIS SAME")

h_pred = hewk.Clone("h_pred")
h_pred.Add(hrs)

NBINS = hrs.GetNbinsX()
bin_width = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS
bin_divisions = [7,18,29,40]
line = ROOT.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[0].GetLeftMargin() + ix * bin_width
    line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())

leg = ROOT.TLegend(0.815,0.78,0.94,0.9)
leg.AddEntry(hdata, "Data")
leg.AddEntry(hrs, "R&S from " + ("MC" if RSfromMC else "Data"))
leg.AddEntry(hewk , "Non-QCD MC")
leg.Draw()

ht_names = ["Very Low", "Low", "Medium", "High", "Extreme"]
mod_bin_divisions = [0] + bin_divisions + [hrs.GetNbinsX()]
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextSize(0.03)
text.SetTextAlign(22)
for i in range(1,len(mod_bin_divisions)):
    x = pads[0].GetLeftMargin() + bin_width*0.5*(mod_bin_divisions[i-1]+mod_bin_divisions[i])
    y = 0.79 if i<len(mod_bin_divisions)-1 else 0.73
    text.DrawLatex(x, y, ht_names[i-1]+" H_{T}")    
    text.DrawLatex(x, y-0.04, "{0:.2f}".format((hdata.Integral(mod_bin_divisions[i-1]+1, mod_bin_divisions[i]) - hewk.Integral(mod_bin_divisions[i-1]+1, mod_bin_divisions[i]))/hrs.Integral(mod_bin_divisions[i-1]+1, mod_bin_divisions[i])))

# text.DrawLatex(0.18,0.79,"Low H_{T}")
# text.DrawLatex(0.18,0.75,"{0:.2f}".format(hdata.Integral(8,18)/hrs.Integral(8,18)))
# text.DrawLatex(0.40,0.79,"Medium H_{T}")
# text.DrawLatex(0.40,0.75,"{0:.2f}".format(hdata.Integral(12,22)/hrs.Integral(12,22)))
# text.DrawLatex(0.62,0.79,"High H_{T}")
# text.DrawLatex(0.62,0.75,"{0:.2f}".format(hdata.Integral(23,33)/hrs.Integral(23,33)))
# text.DrawLatex(0.84,0.73,"Extreme H_{T}")
# text.DrawLatex(0.84,0.69,"{0:.2f}".format(hdata.Integral(34,44)/hrs.Integral(34,44)))

text.SetTextAlign(11)
text.SetTextFont(42)
text.SetTextSize(0.04)
text.DrawLatex(0.8,0.935,"{0} fb^{{-1}} (13 TeV)".format(lumi))


binLabels = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "4-6j, 0b", "4-6j, 1b", "4-6j, 2b", "#geq7j, 0b", "#geq7j, 1b", "#geq7j, 2b", "2-6j, #geq3b", "#geq7j, #geq3b"]
binLabelsVL = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "#geq4j, 0b", "#geq4j, 1b", "#geq4j, 2b", "#geq2j, #geq3j"]
binLabels_all = binLabelsVL + 4*(binLabels)
text = ROOT.TLatex()
text.SetNDC(1)
text.SetTextAlign(32)
text.SetTextAngle(90)
text.SetTextSize(min(bin_width * 1.5,0.027))
text.SetTextFont(42)
for ibin in range(len(binLabels_all)):
    x = pads[0].GetLeftMargin() + (ibin+0.5)*bin_width
    y = pads[0].GetBottomMargin()-0.009
    text.DrawLatex(x,y,binLabels_all[ibin])
    text.DrawLatex(x,y,binLabels_all[ibin])
    text.DrawLatex(x,y,binLabels_all[ibin])
    text.DrawLatex(x,y,binLabels_all[ibin])



## ratio
pads[1].cd()

h_ratio = hdata.Clone("h_ratio")
h_ratio.Divide(h_pred)

h_ratio.GetYaxis().SetRangeUser(0,2)
h_ratio.GetYaxis().SetNdivisions(505)
h_ratio.GetYaxis().SetTitle("Data/Pred")
h_ratio.GetYaxis().SetTitleSize(0.16)
h_ratio.GetYaxis().SetTitleOffset(0.18)
h_ratio.GetYaxis().SetLabelSize(0.13)
h_ratio.GetYaxis().CenterTitle()
h_ratio.GetYaxis().SetTickLength(0.02)
h_ratio.GetXaxis().SetLabelSize(0)
h_ratio.GetXaxis().SetTitle("")
h_ratio.GetXaxis().SetNdivisions(NBINS,0,0)
h_ratio.GetXaxis().SetTickSize(0.06)
h_ratio.SetMarkerStyle(20)
h_ratio.SetMarkerSize(1.0)
h_ratio.SetMarkerColor(ROOT.kBlack)
h_ratio.SetLineWidth(1)

h_ratio.Draw("PE")

line = ROOT.TLine()
line.DrawLine(0,1,h_ratio.GetNbinsX(),1)

line = ROOT.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[1].GetLeftMargin() + ix*bin_width
    line.DrawLineNDC(x,1-pads[1].GetTopMargin(),x,pads[1].GetBottomMargin())

c.SaveAs(outdir+"/{0}_closure_{1}.pdf".format(cr, year))
c.SaveAs(outdir+"/{0}_closure_{1}.png".format(cr, year))

