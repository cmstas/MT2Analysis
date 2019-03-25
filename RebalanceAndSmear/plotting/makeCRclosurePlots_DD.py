import glob
import os
import ROOT as r
import numpy as np
from common import *  # useful things that are common to all plotting scripts

r.gROOT.SetBatch(1)

cr="crRSInvertDPhi"
# cr="crRSMT2SideBand"
# cr="crRSDPhiMT2"

tag = "V00-10-09_ptBinned_XXX_JetID_PUID_BTagSFs_core2sigma"
RSfromMC = False

year = "All"
lumi = 137

# year = 2018
# lumi = 60.0

# year = 2017
# lumi = 41.5

# year = 2016
# lumi = 35.9

RSFOF = 1.00

NBINS = sum([len(x[1]) for x in topo_reg_defs.items()])
print NBINS

username = os.environ["USER"]
outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/".format(username,tag)

dir_RS = "looper_output/{0}/{1}{2}".format(tag, "qcd" if RSfromMC else "data", year)
dir_data = "../SmearLooper/output/V00-10-08_94x_2017_noRS/"
dir_mc = "../SmearLooper/output/V00-10-08_94x_2017_noRS/"

f_rs = r.TFile(os.path.join(dir_RS,"merged_hists.root"))
f_data = r.TFile(os.path.join(dir_data,"data_Run{0}.root".format(year)))
f_dy = r.TFile(os.path.join(dir_mc,"dyjetsll_ht.root"))
f_zinv = r.TFile(os.path.join(dir_mc,"zinv_ht.root"))
f_top = r.TFile(os.path.join(dir_mc,"top.root"))
f_wjets = r.TFile(os.path.join(dir_mc,"wjets_ht.root"))

hrs = r.TH1D("hrs","",NBINS,0,NBINS)
hdata = r.TH1D("hdata","",NBINS,0,NBINS)
hzinv = r.TH1D("hzinv","",NBINS,0,NBINS)
hll = r.TH1D("hll","",NBINS,0,NBINS)

os.system("mkdir -p "+outdir)
os.system("cp ~/scripts/index.php "+outdir)

ibin = 0
ht_regs = ["VL","L","M","H","UH"]
for ht_reg in ht_regs:
    top_regs = topo_reg_defs[ht_reg]
    for top_reg in top_regs:
        ibin += 1

        h_evts_rs = f_rs.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_data_dy = f_data.Get("{0}DY{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_data_of = f_data.Get("{0}DY{1}{2}/h_Events_wOF".format(cr,top_reg,ht_reg))
        h_evts_data_sl = f_data.Get("{0}SL{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_data_sr = f_data.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_zinv_sr = f_zinv.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_dy_dy = f_dy.Get("{0}DY{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_top_sl = f_top.Get("{0}SL{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_top_sr = f_top.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_wjets_sl = f_wjets.Get("{0}SL{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))
        h_evts_wjets_sr = f_wjets.Get("{0}{1}{2}/h_Events_w".format(cr,top_reg,ht_reg))

        n_rs, err_rs = 0., r.Double(0.0)
        n_data_dy, err_data_dy = 0., r.Double(0.0)
        n_data_of, err_data_of = 0., r.Double(0.0)
        n_data_sl, err_data_sl = 0., r.Double(0.0)
        n_data_sr, err_data_sr = 0., r.Double(0.0)
        n_dy_dy, err_dy_dy = 0., r.Double(0.0)
        n_zinv_sr, err_zinv_sr = 0., r.Double(0.0)
        n_top_sl, err_top_sl = 0., r.Double(0.0)
        n_top_sr, err_top_sr = 0., r.Double(0.0)
        n_wjets_sr, err_wjets_sr = 0., r.Double(0.0)
        n_wjets_sl, err_wjets_sl = 0., r.Double(0.0)
        
        if type(h_evts_rs) != type(r.TObject()):
            n_rs = h_evts_rs.IntegralAndError(1, 1, err_rs)
        if type(h_evts_data_dy) != type(r.TObject()):
            n_data_dy = h_evts_data_dy.IntegralAndError(1, 1, err_data_dy)
        if type(h_evts_data_of) != type(r.TObject()):
            n_data_of = h_evts_data_of.IntegralAndError(1, 1, err_data_of)
        if type(h_evts_data_sl) != type(r.TObject()):
            n_data_sl = h_evts_data_sl.IntegralAndError(1, 1, err_data_sl)
        if type(h_evts_data_sr) != type(r.TObject()):
            n_data_sr = h_evts_data_sr.IntegralAndError(1, 1, err_data_sr)
        if type(h_evts_zinv_sr) != type(r.TObject()):
            n_zinv_sr = h_evts_zinv_sr.IntegralAndError(1, 1, err_zinv_sr)
        if type(h_evts_dy_dy) != type(r.TObject()):
            n_dy_dy = h_evts_dy_dy.IntegralAndError(1, 1, err_dy_dy)
        if type(h_evts_top_sl) != type(r.TObject()):
            n_top_sl = h_evts_top_sl.IntegralAndError(1, 1, err_top_sl)
        if type(h_evts_top_sr) != type(r.TObject()):
            n_top_sr = h_evts_top_sr.IntegralAndError(1, 1, err_top_sr)
        if type(h_evts_wjets_sl) != type(r.TObject()):
            n_wjets_sl = h_evts_wjets_sl.IntegralAndError(1, 1, err_wjets_sl)
        if type(h_evts_wjets_sr) != type(r.TObject()):
            n_wjets_sr = h_evts_wjets_sr.IntegralAndError(1, 1, err_wjets_sr)

        n_zinv_sr *= lumi
        n_dy_dy *= lumi
        n_top_sl *= lumi
        n_top_sr *= lumi
        n_wjets_sl *= lumi
        n_wjets_sr *= lumi
        err_zinv_sr *= lumi
        err_dy_dy *= lumi
        err_top_sl *= lumi
        err_top_sr *= lumi
        err_wjets_sl *= lumi
        err_wjets_sr *= lumi

        rzinvdy = 0.0
        rzinvdy_err = 2.5
        if n_dy_dy > 0:
            rzinvdy = n_zinv_sr / n_dy_dy
            if n_zinv_sr > 0:
                rzinvdy_err = rzinvdy * np.sqrt((err_zinv_sr/n_zinv_sr)**2 + (err_dy_dy/n_dy_dy)**2)

        top_contam = n_data_of * RSFOF
        top_contam_err = err_data_of * RSFOF
        n_zinv_est = rzinvdy * (n_data_dy - top_contam)
        err_zinv_est = 0.0
        if (n_data_dy - top_contam > 0 and rzinvdy > 0):
            err_zinv_est = np.sqrt((err_data_dy)**2 + (top_contam_err)**2) / (n_data_dy-top_contam)
            err_zinv_est = n_zinv_est * np.sqrt((rzinvdy_err/rzinvdy)**2 + (err_zinv_est)**2)
        else:
            n_zinv_est = n_zinv_sr
            err_zinv_est = err_zinv_sr * 2

        ll_tf = 1.0
        ll_tf_err = 1.0
        if n_top_sl + n_wjets_sl > 0.0:
            ll_tf = (n_top_sr + n_wjets_sr) / (n_top_sl + n_wjets_sl)
            ll_tf_err = ll_tf * np.sqrt((err_top_sr**2+err_wjets_sr**2)/(n_top_sr+n_wjets_sr)**2 + (err_top_sl**2+err_wjets_sl**2)/(n_top_sl+n_wjets_sl)**2)
        n_ll_est, err_ll_est = 0., 0.
        if n_data_sl > 0:
            n_ll_est = ll_tf * n_data_sl
            err_ll_est = n_ll_est * np.sqrt((ll_tf_err/ll_tf)**2 + (err_data_sl/n_data_sl)**2)
        else:
            n_ll_est = n_top_sr + n_wjets_sr
            err_ll_est = np.sqrt(err_top_sr**2 + err_wjets_sr**2)*2

        hrs.SetBinContent(ibin, n_rs)
        hrs.SetBinError(ibin, err_rs)
        hdata.SetBinContent(ibin, n_data_sr)
        hdata.SetBinError(ibin, err_data_sr)
        hzinv.SetBinContent(ibin, n_zinv_est)
        hzinv.SetBinError(ibin, err_zinv_est)
        hll.SetBinContent(ibin, n_ll_est)
        hll.SetBinError(ibin, err_ll_est)
        


r.gStyle.SetOptStat(0)

c = r.TCanvas("c","c",900,600)

pads = []
pads.append(r.TPad("1","1",0.0,0.18,1.0,1.0))
pads.append(r.TPad("2","2",0.0,0.0,1.0,0.19))

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

hdata.SetLineColor(r.kBlack)
hdata.SetMarkerColor(r.kBlack)
hdata.SetMarkerStyle(20)

hzinv.SetLineColor(r.kBlack)
hzinv.SetFillColor(418)

hll.SetLineColor(r.kBlack)
hll.SetFillColor(855)

hrs.SetLineColor(r.kBlack)
hrs.SetFillColor(401)

hzinv.GetYaxis().SetRangeUser(1e-1,1e6 if cr=="crRSInvertDPhi" else 1e8)
hzinv.GetXaxis().SetLabelSize(0)
hzinv.GetXaxis().SetTickLength(0.02)
hzinv.GetXaxis().SetNdivisions(hzinv.GetNbinsX(), 0, 0)

hstack = r.THStack()
hstack.Add(hll)
hstack.Add(hzinv)
hstack.Add(hrs)

minim = 99999
maxim = 0
for i in range(hdata.GetNbinsX()):
    minim = min(minim, hdata.GetBinContent(i) if hdata.GetBinContent(i)>0 else minim)
    maxim = max(maxim, hdata.GetBinContent(i))
hstack.SetMinimum(0.1)
hstack.SetMaximum(maxim**1.5)

hzinv.Draw("HIST")
hstack.Draw("HIST SAME")
hdata.Draw("PE SAME")
hzinv.Draw("AXIS SAME")

h_pred = hzinv.Clone("h_pred")
h_pred.Add(hll)
h_pred.Add(hrs)
hewk = hzinv.Clone("h_ewk")
hewk.Add(hll)

bin_width = (1-pads[0].GetLeftMargin()-pads[0].GetRightMargin()) / NBINS
bin_divisions = [0]
for i in range(len(ht_regs)):
    bin_divisions.append(bin_divisions[i]+len(topo_reg_defs[ht_regs[i]]))
bin_divisions = bin_divisions[1:-1]
line = r.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[0].GetLeftMargin() + ix * bin_width
    line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())

leg = r.TLegend(0.76,0.73,0.945,0.91)
leg.AddEntry(hdata, "Data", 'lp')
leg.AddEntry(hrs, "R&S from " + ("MC" if RSfromMC else "Data"), 'f')
leg.AddEntry(hzinv , "Z#rightarrow#nu#bar{#nu}", 'f')
leg.AddEntry(hll , "Lost Lepton", 'f')
leg.Draw()

ht_names = ["Very Low", "Low", "Medium", "High", "Extreme"]
mod_bin_divisions = [0] + bin_divisions + [NBINS]
print bin_divisions
print mod_bin_divisions
text = r.TLatex()
text.SetNDC(1)
text.SetTextSize(0.036)
text.SetTextAlign(22)
for i in range(1,len(mod_bin_divisions)):
    x = pads[0].GetLeftMargin() + bin_width*0.5*(mod_bin_divisions[i-1]+mod_bin_divisions[i])
    if i==1:
        x += 0.007
    y = 0.78 if i<len(mod_bin_divisions)-1 else 0.68
    text.DrawLatex(x, y, ht_names[i-1]+" H_{T}")    
    ydata, edata = 0.0, r.Double(0.0)
    yewk, eewk = 0.0, r.Double(0.0)
    yrs, ers = 0.0, r.Double(0.0)
    ydata = hdata.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], edata)
    yewk = hewk.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], eewk)
    yrs = hrs.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], ers)
    if yrs > 0.0:
        ratio = (ydata - yewk)/yrs
        err = ratio * np.sqrt((ers/yrs)**2 + (edata**2 + eewk**2)/(ydata-yewk)**2)
    else:
        ratio = 999.
        err = 999.
    # text.DrawLatex(x, y-0.04, "{0:.2f} #pm {1:.2f}".format(ratio, err))

text.SetTextAlign(31)
text.SetTextFont(42)
text.SetTextSize(0.04)
text.DrawLatex(0.94,0.935,"{0} fb^{{-1}} (13 TeV)".format(lumi))
text.SetTextFont(62)
text.SetTextAlign(11)
text.DrawLatex(0.08, 0.935, "CMS Preliminary")

binLabels_all = []
for ht_reg in ht_regs:
    binLabels_all += [binLabels[tr] for tr in topo_reg_defs[ht_reg]]
text = r.TLatex()
text.SetNDC(1)
text.SetTextAlign(32)
text.SetTextAngle(90)
text.SetTextSize(min(bin_width * 1.76,0.027))
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
h_ratio_err = hdata.Clone("h_ratio_err")
for i in range(1, h_ratio.GetNbinsX()+1):
    if h_pred.GetBinContent(i) > 0:
        h_ratio.SetBinContent(i, hdata.GetBinContent(i)/h_pred.GetBinContent(i))
        h_ratio.SetBinError(i, hdata.GetBinError(i)/h_pred.GetBinContent(i))
        relerr = h_pred.GetBinError(i)/h_pred.GetBinContent(i)
        ht_reg = ibinToHTReg(i)
        topo_reg = ibinToTopoReg(i)
        systs = []
        systs.extend(extraSysts_HT[year][ht_reg])
        systs.extend(getJBJSysts(ht_reg, topo_reg))
        for syst in systs:
            relsyst = syst * hrs.GetBinContent(i) / h_pred.GetBinContent(i)
            relerr = np.sqrt(relerr**2 + relsyst**2)
        h_ratio_err.SetBinContent(i, 1.0)
        h_ratio_err.SetBinError(i, relerr)
        
    else:
        h_ratio.SetBinContent(i, 0.0)
        h_ratio.SetBinError(i, 0.0)
        h_ratio_err.SetBinContent(i, 1.0)
        h_ratio_err.SetBinError(i, 0.0)

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
h_ratio.SetMarkerColor(r.kBlack)
h_ratio.SetLineWidth(1)

h_ratio_err.SetMarkerStyle(1)
h_ratio_err.SetMarkerColor(r.kGray)
h_ratio_err.SetFillStyle(1001)
h_ratio_err.SetFillColor(r.kGray)

h_ratio.Draw("PE")
h_ratio_err.Draw("E2 SAME")
h_ratio.Draw("PE SAME")
h_ratio.Draw("AXIS SAME")

line = r.TLine()
line.DrawLine(0,1,h_ratio.GetNbinsX(),1)

line = r.TLine()
line.SetLineStyle(2)
for ix in bin_divisions:
    x = pads[1].GetLeftMargin() + ix*bin_width
    line.DrawLineNDC(x,1-pads[1].GetTopMargin(),x,pads[1].GetBottomMargin())

c.SaveAs(outdir+"/{0}_closure_{1}_DD.pdf".format(cr, year))
c.SaveAs(outdir+"/{0}_closure_{1}_DD.png".format(cr, year))

