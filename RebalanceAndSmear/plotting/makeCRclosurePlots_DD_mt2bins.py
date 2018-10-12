import glob
import os
import ROOT as r
import numpy as np

r.gROOT.SetBatch(1)

cr="crRSInvertDPhi"

HT_REG = "all"

tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma"
RSfromMC = False

year = 2017
lumi = 41.5

# year = 2016
# lumi = 35.9

RSFOF = 1.00

username = os.environ["USER"]
outdir = "/home/users/{0}/public_html/mt2/RebalanceAndSmear/{1}/".format(username,tag)

dir_RS = "looper_output/{0}/{1}{2}".format(tag, "qcd" if RSfromMC else "data", year)
dir_data = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"
dir_mc = "../SmearLooper/output/V00-10-04_94x_2017_noRS/"

f_rs = r.TFile(os.path.join(dir_RS,"merged_hists.root"))
f_data = r.TFile(os.path.join(dir_data,"data_Run{0}.root".format(year)))
f_dy = r.TFile(os.path.join(dir_mc,"dyjetsll_ht.root"))
f_zinv = r.TFile(os.path.join(dir_mc,"zinv_ht.root"))
f_top = r.TFile(os.path.join(dir_mc,"top.root"))
f_wjets = r.TFile(os.path.join(dir_mc,"wjets_ht.root"))

os.system("mkdir -p "+outdir)
os.system("cp ~/scripts/index.php "+outdir)

vec_ll_est = []
vec_ll_err = []
vec_zinv_est = []
vec_zinv_err = []
vec_rs_est = []
vec_rs_err = []
vec_data_obs = []
vec_data_err = []
vec_n_mt2bins = []
topo_reg_boundaries = []
ht_reg_boundaries = []
mt2bin_thresholds = []

HT_REGS = [HT_REG]
if HT_REG == "all":
    HT_REGS = ["VL", "L", "M", "H", "UH"]

top_regs_vl=[1,2,3,12,13,14,15]
ibin = 0
for ht_reg in HT_REGS:
    ht_reg_boundaries.append(len(vec_data_obs))

    top_regs = list(range(1,12))
    if ht_reg=="VL":
        top_regs = top_regs_vl
    for top_reg in top_regs:
        topo_reg_boundaries.append(len(vec_data_obs))
        ibin += 1        

        h_evts_rs = f_rs.Get("{0}{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_data_dy = f_data.Get("{0}DY{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_data_of = f_data.Get("{0}DY{1}{2}/h_mt2binsOF".format(cr,top_reg,ht_reg))
        h_evts_data_sl = f_data.Get("{0}SL{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_data_sr = f_data.Get("{0}{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_zinv_sr = f_zinv.Get("{0}{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_dy_dy = f_dy.Get("{0}DY{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_top_sl = f_top.Get("{0}SL{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_top_sr = f_top.Get("{0}{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_wjets_sl = f_wjets.Get("{0}SL{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))
        h_evts_wjets_sr = f_wjets.Get("{0}{1}{2}/h_mt2bins".format(cr,top_reg,ht_reg))

        vec_n_mt2bins.append(h_evts_rs.GetNbinsX())

        imerge_ll = -1
        imerge_zinv = -1
        for imt2 in range(1, vec_n_mt2bins[-1]+1):
            if imt2 < vec_n_mt2bins[-1]:
                mt2bin_thresholds.append((int(h_evts_rs.GetXaxis().GetBinLowEdge(imt2)), int(h_evts_rs.GetXaxis().GetBinUpEdge(imt2))))
            else:
                mt2bin_thresholds.append((int(h_evts_rs.GetXaxis().GetBinLowEdge(imt2)), "#infty"))

            domerge_ll = False
            if imerge_ll > -1:
                domerge_ll = True
            else:
                n_top = h_evts_top_sr.Integral(imt2+1, -1) if type(h_evts_top_sr)!=type(r.TObject()) else 0.0
                n_wjets = h_evts_wjets_sr.Integral(imt2+1, -1) if type(h_evts_wjets_sr)!=type(r.TObject()) else 0.0
                if (n_top + n_wjets)*lumi < 50.0:
                    domerge_ll = True
                    imerge_ll = imt2

            domerge_zinv = False
            if imerge_zinv > -1:
                domerge_zinv = True
            else:
                n_zinv = h_evts_zinv_sr.Integral(imt2+1, -1) if type(h_evts_zinv_sr)!=type(r.TObject()) else 0.0
                if n_zinv*lumi < 50.0:
                    domerge_zinv = True
                    imerge_zinv = imt2

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
                n_rs = h_evts_rs.IntegralAndError(imt2, imt2, err_rs)
            if type(h_evts_data_dy) != type(r.TObject()):
                if domerge_zinv:
                    n_data_dy = h_evts_data_dy.IntegralAndError(imerge_zinv, -1, err_data_dy)
                else:
                    n_data_dy = h_evts_data_dy.IntegralAndError(imt2, imt2, err_data_dy)
            if type(h_evts_data_of) != type(r.TObject()):
                if domerge_zinv:
                    n_data_of = h_evts_data_of.IntegralAndError(imerge_zinv, -1, err_data_of)
                else:
                    n_data_of = h_evts_data_of.IntegralAndError(imt2, imt2, err_data_of)
            if type(h_evts_data_sl) != type(r.TObject()):
                if domerge_ll:
                    n_data_sl = h_evts_data_sl.IntegralAndError(imerge_ll, -1, err_data_sl)
                else:
                    n_data_sl = h_evts_data_sl.IntegralAndError(imt2, imt2, err_data_sl)
            if type(h_evts_data_sr) != type(r.TObject()):
                n_data_sr = h_evts_data_sr.IntegralAndError(imt2, imt2, err_data_sr)
            if type(h_evts_zinv_sr) != type(r.TObject()):
                n_zinv_sr = h_evts_zinv_sr.IntegralAndError(imt2, imt2, err_zinv_sr)
            if type(h_evts_dy_dy) != type(r.TObject()):
                if domerge_zinv:
                    n_dy_dy = h_evts_dy_dy.IntegralAndError(imerge_zinv, -1, err_dy_dy)
                else:
                    n_dy_dy = h_evts_dy_dy.IntegralAndError(imt2, imt2, err_dy_dy)
            if type(h_evts_top_sl) != type(r.TObject()):
                if domerge_ll:
                    n_top_sl = h_evts_top_sl.IntegralAndError(imerge_ll, -1, err_top_sl)
                else:
                    n_top_sl = h_evts_top_sl.IntegralAndError(imt2, imt2, err_top_sl)
            if type(h_evts_top_sr) != type(r.TObject()):
                n_top_sr = h_evts_top_sr.IntegralAndError(imt2, imt2, err_top_sr)
            if type(h_evts_wjets_sl) != type(r.TObject()):
                if domerge_ll:
                    n_wjets_sl = h_evts_wjets_sl.IntegralAndError(imerge_ll, -1, err_wjets_sl)
                else:
                    n_wjets_sl = h_evts_wjets_sl.IntegralAndError(imt2, imt2, err_wjets_sl)
            if type(h_evts_wjets_sr) != type(r.TObject()):
                n_wjets_sr = h_evts_wjets_sr.IntegralAndError(imt2, imt2, err_wjets_sr)

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
                                   
            rzinvdy = 1.0
            rzinvdy_err = 0.0
            if n_dy_dy > 0:
                rzinvdy = n_zinv_sr / n_dy_dy
                if n_zinv_sr > 0:
                    rzinvdy_err = rzinvdy * np.sqrt((err_zinv_sr/n_zinv_sr)**2 + (err_dy_dy/n_dy_dy)**2)

            top_contam = n_data_of * RSFOF
            top_contam_err = err_data_of * RSFOF
            n_zinv_est = rzinvdy * (n_data_dy - top_contam)
            err_zinv_est = 0.0
            if (n_data_dy - top_contam > 0) and rzinvdy > 0:
                err_zinv_est = np.sqrt((err_data_dy)**2 + (top_contam_err)**2) / (n_data_dy-top_contam)               
                err_zinv_est = n_zinv_est * np.sqrt((rzinvdy_err/rzinvdy)**2 + (err_zinv_est)**2)

            ll_tf = (n_top_sr + n_wjets_sr) / (n_top_sl + n_wjets_sl)
            n_ll_est, err_ll_est = 0., 0.
            if n_data_sl > 0 and ll_tf > 0:
                ll_tf_err = ll_tf * np.sqrt((err_top_sr**2+err_wjets_sr**2)/(n_top_sr+n_wjets_sr)**2 + (err_top_sl**2+err_wjets_sl**2)/(n_top_sl+n_wjets_sl)**2) 
                n_ll_est = ll_tf * n_data_sl
                err_ll_est = n_ll_est * np.sqrt((ll_tf_err/ll_tf)**2 + (err_data_sl/n_data_sl)**2)

            vec_rs_est.append(n_rs)
            vec_rs_err.append(err_rs)
            vec_ll_est.append(n_ll_est)
            vec_ll_err.append(err_ll_est)
            vec_zinv_est.append(n_zinv_est)
            vec_zinv_err.append(err_zinv_est)
            vec_data_obs.append(n_data_sr)
            vec_data_err.append(err_data_sr)

NBINS = len(vec_data_obs)

hrs = r.TH1D("hrs","",NBINS,0,NBINS)
hdata = r.TH1D("hdata","",NBINS,0,NBINS)
hzinv = r.TH1D("hzinv","",NBINS,0,NBINS)
hll = r.TH1D("hll","",NBINS,0,NBINS)
for ibin in range(NBINS):
    hrs.SetBinContent(ibin+1, vec_rs_est[ibin])
    hrs.SetBinError(ibin+1, vec_rs_err[ibin])
    hdata.SetBinContent(ibin+1, vec_data_obs[ibin])
    hdata.SetBinError(ibin+1, vec_data_err[ibin])
    hzinv.SetBinContent(ibin+1, vec_zinv_est[ibin])
    hzinv.SetBinError(ibin+1, vec_zinv_err[ibin])
    hll.SetBinContent(ibin+1, vec_ll_est[ibin])
    hll.SetBinError(ibin+1, vec_ll_err[ibin])

ht_reg_boundaries.append(NBINS)
topo_reg_boundaries.append(NBINS)
print ht_reg_boundaries
print topo_reg_boundaries            


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
pads[0].SetTickx(0)
pads[1].SetTickx(0)
pads[0].SetTicky(1)
pads[1].SetTicky(1)

MARKERSIZE = 0.6 if HT_REG=="all" else 1.0
hdata.SetLineColor(r.kBlack)
hdata.SetMarkerColor(r.kBlack)
hdata.SetMarkerStyle(20)
hdata.SetMarkerSize(MARKERSIZE)

hzinv.SetLineColor(r.kBlack)
hzinv.SetFillColor(418)

hll.SetLineColor(r.kBlack)
hll.SetFillColor(855)

hrs.SetLineColor(r.kBlack)
hrs.SetFillColor(401)

hzinv.GetYaxis().SetRangeUser(1e-1,1e6)
hzinv.GetXaxis().SetLabelSize(0)
hzinv.GetXaxis().SetTickLength(0.00)
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

if HT_REG=="all":
    line = r.TLine()
    line.SetLineStyle(2)
    for ix in ht_reg_boundaries[1:-1]:
        x = pads[0].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x,1-pads[0].GetTopMargin(),x,pads[0].GetBottomMargin())
    line.SetLineStyle(1)
    for ix in topo_reg_boundaries[1:]:
        x = pads[0].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x, pads[0].GetBottomMargin()-0.01, x, pads[0].GetBottomMargin()+0.03)
else:
    line = r.TLine()
    line.SetLineStyle(2)
    for ix in topo_reg_boundaries[1:-1]:
        x = pads[0].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x,1-pads[0].GetTopMargin()-0.1,x,pads[0].GetBottomMargin())
    line.SetLineStyle(1)
    for ix in range(1, NBINS):
        x = pads[0].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x, pads[0].GetBottomMargin()-0.01, x, pads[0].GetBottomMargin()+0.02)


leg = r.TLegend(0.815,0.78,0.94,0.9)
leg.AddEntry(hdata, "Data", 'lp')
leg.AddEntry(hrs, "R&S from " + ("MC" if RSfromMC else "Data"), 'f')
leg.AddEntry(hzinv , "Z#rightarrow#nu#bar{#nu}", 'f')
leg.AddEntry(hll , "Lost Lepton", 'f')
leg.Draw()

text = r.TLatex()
text.SetNDC(1)

binLabels = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "4-6j, 0b", "4-6j, 1b", "4-6j, 2b", "#geq7j, 0b", "#geq7j, 1b", "#geq7j, 2b", "2-6j, #geq3b", "#geq7j, #geq3b"]
binLabelsVL = ["2-3j, 0b", "2-3j, 1b", "2-3j, 2b", "#geq4j, 0b", "#geq4j, 1b", "#geq4j, 2b", "#geq2j, #geq3j"]

if HT_REG=="all":
    text.SetTextSize(0.03)
    text.SetTextAlign(22)
    ht_names = ["Very Low", "Low", "Medium", "High", "Extreme"]
    mod_bin_divisions = ht_reg_boundaries
    for i in range(1,len(mod_bin_divisions)):
        x = pads[0].GetLeftMargin() + bin_width*0.5*(mod_bin_divisions[i-1]+mod_bin_divisions[i])
        y = 0.79 if i<len(mod_bin_divisions)-1 else 0.73
        text.DrawLatex(x, y, ht_names[i-1]+" H_{T}")    
        ydata, edata = 0.0, r.Double(0.0)
        yewk, eewk = 0.0, r.Double(0.0)
        yrs, ers = 0.0, r.Double(0.0)
        ydata = hdata.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], edata)
        yewk = hewk.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], eewk)
        yrs = hrs.IntegralAndError(mod_bin_divisions[i-1]+1, mod_bin_divisions[i], ers)
        ratio = (ydata - yewk)/yrs
        err = ratio * np.sqrt((ers/yrs)**2 + (edata**2 + eewk**2)/(ydata-yewk)**2)
        text.DrawLatex(x, y-0.04, "{0:.2f} #pm {1:.2f}".format(ratio, err))
else:
    ht_thresh = {"VL":(250,450), "L":(450,575), "M":(575,1200), "H":(1200,1500), "UH":(1500,"#infty")}
    text.SetTextSize(0.03)
    text.SetTextAlign(22)
    text.DrawLatex(0.5, 1.0-pads[0].GetTopMargin()-0.05, "{0} #leq H_{{T}} < {1}".format(*ht_thresh[HT_REG]))
    labels = binLabels if HT_REG != "VL" else binLabelsVL
    labels = [x.split(", ") for x in labels]
    for i in range(len(topo_reg_boundaries)-1):
        x = pads[0].GetLeftMargin() + 0.5*(topo_reg_boundaries[i]+topo_reg_boundaries[i+1])*bin_width
        y = 0.79 if x<0.75 else 0.73
        text.DrawLatex(x, y, labels[i][0])
        text.DrawLatex(x, y-0.04, labels[i][1])

text.SetTextAlign(11)
text.SetTextFont(42)
text.SetTextSize(0.04)
text.DrawLatex(0.8,0.935,"{0} fb^{{-1}} (13 TeV)".format(lumi))


if HT_REG == "all":
    binLabels_all = binLabelsVL + 4*(binLabels)
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(bin_width * 6.0,0.027))
    text.SetTextFont(42)
    for ibin in range(len(binLabels_all)):
        x = pads[0].GetLeftMargin() + 0.5*(topo_reg_boundaries[ibin] + topo_reg_boundaries[ibin+1])*bin_width
        y = pads[0].GetBottomMargin()-0.009
        text.DrawLatex(x,y,binLabels_all[ibin])
else:
    text.SetTextAlign(32)
    text.SetTextAngle(90)
    text.SetTextSize(min(bin_width * 1.25,0.027))
    text.SetTextFont(62)
    for ibin in range(NBINS):
        x = pads[0].GetLeftMargin() + (ibin+0.5)*bin_width
        y = pads[0].GetBottomMargin()-0.009
        if type(mt2bin_thresholds[ibin][1]) == int:
            text.DrawLatex(x,y,"[{0}, {1}]".format(*mt2bin_thresholds[ibin]))
        else:
            text.DrawLatex(x,y,"#geq {0}".format(mt2bin_thresholds[ibin][0]))

## ratio
pads[1].cd()

h_ratio = hdata.Clone("h_ratio")
h_ratio_err = hdata.Clone("h_ratio_err")
for i in range(1, h_ratio.GetNbinsX()+1):
    if h_pred.GetBinContent(i) > 0:
        h_ratio.SetBinContent(i, hdata.GetBinContent(i)/h_pred.GetBinContent(i))
        h_ratio.SetBinError(i, hdata.GetBinError(i)/h_pred.GetBinContent(i))
        h_ratio_err.SetBinContent(i, 1.0)
        h_ratio_err.SetBinError(i, h_pred.GetBinError(i)/h_pred.GetBinContent(i))
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
h_ratio.GetYaxis().SetTickLength(0.00)
h_ratio.GetXaxis().SetLabelSize(0)
h_ratio.GetXaxis().SetTitle("")
h_ratio.GetXaxis().SetNdivisions(NBINS,0,0)
h_ratio.GetXaxis().SetTickSize(0.00)
h_ratio.SetMarkerStyle(20)
h_ratio.SetMarkerSize(MARKERSIZE)
h_ratio.SetMarkerColor(r.kBlack)
h_ratio.SetLineWidth(1)

h_ratio_err.SetMarkerStyle(1)
h_ratio_err.SetMarkerColor(r.kGray)
h_ratio_err.SetFillStyle(1001)
h_ratio_err.SetFillColor(r.kGray)

h_ratio.Draw("PE")
h_ratio_err.Draw("E2 SAME")
h_ratio.Draw("SAME AXIS")
h_ratio.Draw("PE SAME")

line = r.TLine()
line.DrawLine(0,1,h_ratio.GetNbinsX(),1)


if HT_REG=="all":
    line.SetLineStyle(2)
    for ix in ht_reg_boundaries[1:-1]:
        x = pads[1].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x,1-pads[1].GetTopMargin(),x,pads[1].GetBottomMargin())
    line.SetLineStyle(1)
    for ix in topo_reg_boundaries[1:]:
        x = pads[1].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x, pads[1].GetBottomMargin(), x, pads[1].GetBottomMargin()+0.03)
        line.DrawLineNDC(x, 1.0-pads[1].GetTopMargin(), x, 1.0-pads[1].GetTopMargin()-0.03)
else:
    line.SetLineStyle(2)
    for ix in topo_reg_boundaries[1:-1]:
        x = pads[1].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x,1-pads[1].GetTopMargin(),x,pads[1].GetBottomMargin())
    line.SetLineStyle(1)
    for ix in range(1, NBINS):
        x = pads[1].GetLeftMargin() + ix * bin_width
        line.DrawLineNDC(x, pads[1].GetBottomMargin(), x, pads[1].GetBottomMargin()+0.03)
        line.DrawLineNDC(x, 1.0-pads[1].GetTopMargin(), x, 1.0-pads[1].GetTopMargin()-0.03)

c.SaveAs(outdir+"/{0}_closure_{1}_DD_mt2bins_{2}.pdf".format(cr, year, HT_REG))
c.SaveAs(outdir+"/{0}_closure_{1}_DD_mt2bins_{2}.png".format(cr, year, HT_REG))

