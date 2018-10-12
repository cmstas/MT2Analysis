import os
import ROOT
import glob

ROOT.gROOT.SetBatch(1)

tag = "V00-10-04_ptBinned_94x_JetID_PUID_BTagSFs_noJERsmear"

indir = "looper_output/{0}".format(tag)

outdir = "/home/users/bemarsh/public_html/mt2/RebalanceAndSmear/{0}/ewk_contamination/".format(tag)
os.system("mkdir -p "+outdir)
os.system("cp ~/scripts/index.php "+outdir)

h_ewk = ROOT.TH1D("h_ewk","",50,0,500)
h_qcd = ROOT.TH1D("h_qcd","",50,0,500)

ewk_samps = ["tt", "singletop", "wjets", "zinv"]

for ht_reg in ["VL", "L", "M", "H", "UH"]:
    h_ewk.Reset()
    h_qcd.Reset()
    
    for samp in ewk_samps + ["qcd"]:
        for f in glob.glob("{0}/{1}/merged_hists.root".format(indir, samp)):
            if f.find("qcd") > -1:
                h = h_qcd
            else:
                h = h_ewk

            fid = ROOT.TFile(f)

            top_regs = range(1,12)
            if ht_reg == "VL":
                top_regs = [1,2,3,12,13,14,15]
            for top_reg in top_regs:
                try:
                    h.Add(fid.Get("sr{0}{1}/h_reb_met".format(top_reg,ht_reg)))
                except:
                    pass

            fid.Close()

    c = ROOT.TCanvas()
    c.SetLogy(1)

    h_qcd.SetLineColor(ROOT.kBlack)
    h_qcd.SetFillColor(ROOT.kBlue)
    h_ewk.SetLineColor(ROOT.kBlack)
    h_ewk.SetFillColor(ROOT.kRed)

    stack = ROOT.THStack()

    stack.Add(h_ewk)
    stack.Add(h_qcd)

    stack.SetMinimum(1e-3)
    stack.SetMaximum(1e2)

    stack.Draw("HIST")

    stack.GetXaxis().SetTitle("Rebalanced MET (GeV)")

    leg = ROOT.TLegend(0.68,0.80,0.88,0.88)
    leg.AddEntry(h_qcd, "QCD", "f")
    leg.AddEntry(h_ewk, "EWK", "f")
    leg.Draw()
    
    qcd_yield = h_qcd.Integral(0,-1)
    under100 = h_qcd.Integral(0,10) + h_ewk.Integral(0,10)
    factor = qcd_yield/under100

    text = ROOT.TLatex()
    text.SetNDC(1)
    text.SetTextFont(42)
    text.SetTextSize(0.03)
    text.DrawLatex(0.15,0.87, "QCD total yield = {0:.2f}".format(qcd_yield))
    text.DrawLatex(0.15,0.835, "(QCD + EWK) w/ reb_met<100 GeV = {0:.2f}".format(under100))
    text.DrawLatex(0.15,0.80, "Correction Factor = {0:.2f}".format(factor))

    c.SaveAs("{0}/ewk_{1}.pdf".format(outdir,ht_reg))
    c.SaveAs("{0}/ewk_{1}.png".format(outdir,ht_reg))

    # raw_input()

