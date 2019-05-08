import ROOT as r
r.gROOT.SetBatch(1)
r.gStyle.SetOptStat(0)

generateHists = False

if generateHists:

    cqcd = r.TChain("mt2")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht300to500/mt2_baby_*.root")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht500to700/mt2_baby_*.root")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht700to1000/mt2_baby_*.root")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht1000to1500/mt2_baby_*.root")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht1500to2000/mt2_baby_*.root")
    cqcd.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_qcd_ht2000toInf/mt2_baby_*.root")
    
    ctop = r.TChain("mt2")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_ttsl_top_mg/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_ttsl_tbar_mg/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_ttdl_mg/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_ttw_mg/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_ttz_mg/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_singletop_schan/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_singletop_tW_tbar/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_singletop_tW_top/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_singletop_tchan_tbar/mt2_baby_*.root")
    ctop.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_singletop_tchan_top/mt2_baby_*.root")
    
    cwjets = r.TChain("mt2")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht100to200/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht200to400/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht400to600/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht600to800/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht800to1200/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht1200to2500/mt2_baby_*.root")
    cwjets.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_wjets_ht2500toInf/mt2_baby_*.root")
    
    czinv = r.TChain("mt2")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht100to200/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht200to400/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht400to600/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht600to800/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht800to1200/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht1200to2500/mt2_baby_*.root")
    czinv.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-14_2018fullYear_zinv_ht2500toInf/mt2_baby_*.root")
    
    cT1tttt = r.TChain("mt2")
    cT1tttt.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1tttt/mt2_baby_*.root")
    cT1tttt.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1tttt_ext1/mt2_baby_*.root")

    cT1bbbb = r.TChain("mt2")
    cT1bbbb.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1bbbb/mt2_baby_*.root")
    cT1bbbb.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1bbbb_ext1/mt2_baby_*.root")
    
    cT1qqqq = r.TChain("mt2")
    cT1qqqq.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1qqqq/mt2_baby_*.root")
    cT1qqqq.Add("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-15_2017fullYear_T1qqqq_ext1/mt2_baby_*.root")
    
    sel = "nJet30>=2 && mt2>0 && met_pt>30 && ht>250 && deltaPhiMin>0.3 && diffMetMht/met_pt<0.5 && nElectrons10+nMuons10+nPFLep5LowMT+nPFHad10LowMT==0"
    sel += "&& (evt_id!=153 || ht<600) && (evt_id!=154 || ht<850) && (evt_id!=155 || ht<1200) && (evt_id!=156 || ht<1800)"

    hqcd = r.TH1D("hqcd", ";M_{T2} [GeV]", 60, 0, 1500)
    htop = r.TH1D("htop", ";M_{T2} [GeV]", 60, 0, 1500)
    hwjets = r.TH1D("hwjets", ";M_{T2} [GeV]", 60, 0, 1500)
    hzinv = r.TH1D("hzinv", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1ttttO = r.TH1D("hT1ttttO", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1bbbbO = r.TH1D("hT1bbbbO", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1qqqqO = r.TH1D("hT1qqqqO", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1ttttC = r.TH1D("hT1ttttC", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1bbbbC = r.TH1D("hT1bbbbC", ";M_{T2} [GeV]", 60, 0, 1500)
    hT1qqqqC = r.TH1D("hT1qqqqC", ";M_{T2} [GeV]", 60, 0, 1500)
    
    
    print "qcd"
    cqcd.Draw("mt2>>hqcd", "evt_scale1fb*({0})".format(sel), "goff")
    print "top"
    ctop.Draw("mt2>>htop", "evt_scale1fb*({0})".format(sel), "goff")
    print "wjets"
    cwjets.Draw("mt2>>hwjets", "evt_scale1fb*({0})".format(sel), "goff")
    print "zinv"
    czinv.Draw("mt2>>hzinv", "evt_scale1fb*({0})".format(sel), "goff")
    print "T1ttttO"
    cT1tttt.Draw("mt2>>hT1ttttO", "evt_scale1fb*({0} && GenSusyMScan1==2100 && GenSusyMScan2==100)".format(sel), "goff")
    print "T1ttttC"
    cT1tttt.Draw("mt2>>hT1ttttC", "evt_scale1fb*({0} && GenSusyMScan1==1600 && GenSusyMScan2==1100)".format(sel), "goff")
    print "T1bbbbO"
    cT1bbbb.Draw("mt2>>hT1bbbbO", "evt_scale1fb*({0} && GenSusyMScan1==2200 && GenSusyMScan2==100)".format(sel), "goff")
    print "T1bbbbC"
    cT1bbbb.Draw("mt2>>hT1bbbbC", "evt_scale1fb*({0} && GenSusyMScan1==1600 && GenSusyMScan2==1300)".format(sel), "goff")
    print "T1qqqqO"
    cT1qqqq.Draw("mt2>>hT1qqqqO", "evt_scale1fb*({0} && GenSusyMScan1==1500 && GenSusyMScan2==100)".format(sel), "goff")
    print "T1qqqqC"
    cT1qqqq.Draw("mt2>>hT1qqqqC", "evt_scale1fb*({0} && GenSusyMScan1==1400 && GenSusyMScan2==1100)".format(sel), "goff")
    
    
    fout = r.TFile("../scratch/blah_fullbaseline.root", "RECREATE")
    hqcd.Write()
    hwjets.Write()
    htop.Write()
    hzinv.Write()
    hT1ttttO.Write()
    hT1ttttC.Write()
    hT1bbbbO.Write()
    hT1bbbbC.Write()
    hT1qqqqO.Write()
    hT1qqqqC.Write()
    
    fout.Close()

    exit(0)

fin = r.TFile("../scratch/blah_fullbaseline.root")
hqcd = fin.Get("hqcd")
htop = fin.Get("htop")
hwjets = fin.Get("hwjets")
hzinv = fin.Get("hzinv")
hT1ttttO = fin.Get("hT1ttttO")
hT1ttttC = fin.Get("hT1ttttC")
hT1bbbbO = fin.Get("hT1bbbbO")
hT1bbbbC = fin.Get("hT1bbbbC")
hT1qqqqO = fin.Get("hT1qqqqO")
hT1qqqqC = fin.Get("hT1qqqqC")

hqcd.Scale(137.4)
htop.Scale(137.4)
hwjets.Scale(137.4)
hzinv.Scale(137.4)
hT1ttttO.Scale(10 * 137.4 * 0.000598 * 1000 / 48057)
hT1ttttC.Scale(10 * 137.4 * 0.008870 * 1000 / 53387)
hT1bbbbO.Scale(10 * 137.4 * 0.000356 * 1000 / 22356)
hT1bbbbC.Scale(10 * 137.4 * 0.008870 * 1000 / 23988)
hT1qqqqO.Scale(1 * 137.4 * 0.015700 * 1000 / 25665)
hT1qqqqC.Scale(10 * 137.4 * 0.028400 * 1000 / 48013)

c = r.TCanvas("c", "c", 750, 700)
c.SetLeftMargin(0.13)
c.SetTopMargin(0.07)
c.SetLogy()
c.SetTicky()

hdummy = hqcd.Clone("hdummy")
hdummy.Reset()
hdummy.GetYaxis().SetRangeUser(1e0, 1e12)
hdummy.GetXaxis().SetTitleOffset(1.2)
hdummy.GetYaxis().SetTitleSize(0.04)
hdummy.GetYaxis().SetTitleOffset(1.5)
hdummy.GetYaxis().SetTitle("Events / {0} GeV".format(int(hqcd.GetBinWidth(1))))

hdummy.Draw()

hqcd.SetFillColor(401)
htop.SetFillColor(855)
hwjets.SetFillColor(417)
hzinv.SetFillColor(419)
hqcd.SetLineColor(r.kBlack)
htop.SetLineColor(r.kBlack)
hwjets.SetLineColor(r.kBlack)
hzinv.SetLineColor(r.kBlack)

hT1ttttO.SetLineColor(r.kRed)
hT1ttttO.SetLineWidth(2)
hT1ttttO.SetLineStyle(1)
hT1ttttC.SetLineColor(r.kRed)
hT1ttttC.SetLineWidth(2)
hT1ttttC.SetLineStyle(2)
hT1bbbbO.SetLineColor(r.kMagenta)
hT1bbbbO.SetLineWidth(2)
hT1bbbbO.SetLineStyle(1)
hT1bbbbC.SetLineColor(r.kMagenta)
hT1bbbbC.SetLineWidth(2)
hT1bbbbC.SetLineStyle(2)
hT1qqqqO.SetLineColor(r.kRed)
hT1qqqqO.SetLineWidth(2)
hT1qqqqO.SetLineStyle(1)
hT1qqqqC.SetLineColor(r.kBlack)
hT1qqqqC.SetLineWidth(2)
hT1qqqqC.SetLineStyle(2)

stack = r.THStack()
stack.Add(hqcd)
stack.Add(htop)
stack.Add(hwjets)
stack.Add(hzinv)

stack.Draw("HIST SAME")

# hT1ttttO.Draw("HIST SAME")
# hT1ttttC.Draw("HIST SAME")
# hT1bbbbO.Draw("HIST SAME")
# hT1bbbbC.Draw("HIST SAME")
hT1qqqqO.Draw("HIST SAME")
# hT1qqqqC.Draw("HIST SAME")

hdummy.Draw("AXIS SAME")

hdummy.SetLineColor(r.kWhite)

leg = r.TLegend(0.47, 0.5, 0.85, 0.88)
leg.SetBorderSize(0)
leg.AddEntry(hqcd, "QCD Multijet", 'f')
leg.AddEntry(htop, "Top", 'f')
leg.AddEntry(hwjets, "W+Jets", 'f')
leg.AddEntry(hzinv, "Z+Jets", 'f')
leg.AddEntry(hT1qqqqO, "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow qq#tilde{#chi}_{1}^{0}", 'l')
leg.AddEntry(hdummy, "m_{ #tilde{g}} = 1500 GeV", 'l')
leg.AddEntry(hdummy, "m_{ #tilde{#chi}_{1}^{0}} = 100 GeV", 'l')
# leg.AddEntry(hT1qqqqC, "T1qqqq (1400, 1100) #times10", 'l')
# leg.AddEntry(hT1bbbbO, "T1bbbb (2200, 100) #times10", 'l')
# leg.AddEntry(hT1bbbbC, "T1bbbb (1600, 1400) #times10", 'l')
# leg.AddEntry(hT1ttttO, "T1tttt (2100, 100) #times10", 'l')
# leg.AddEntry(hT1ttttC, "T1tttt (1600, 1100) #times10", 'l')
leg.Draw()

text = r.TLatex()
text.SetNDC(1)
text.SetTextAlign(11)
text.SetTextFont(62)
text.SetTextSize(0.040)
text.DrawLatex(c.GetLeftMargin()+0.01, 1.0-c.GetTopMargin()+0.01, "CMS")
text.SetTextFont(52)
text.DrawLatex(c.GetLeftMargin()+0.093, 1.0-c.GetTopMargin()+0.01, "Simulation Preliminary")
text.SetTextFont(42)
text.SetTextAlign(31)
text.DrawLatex(1.0-c.GetRightMargin()-0.01, 1.0-c.GetTopMargin()+0.01, "137 fb^{-1} (13 TeV)")

text.SetTextFont(42)
text.SetTextSize(0.033)
text.SetTextAlign(11)
text.DrawLatex(c.GetLeftMargin()+0.08, 1.0-c.GetTopMargin()-0.07, "H_{T} > 250 GeV")
text.DrawLatex(c.GetLeftMargin()+0.08, 1.0-c.GetTopMargin()-0.12, "E_{T}^{miss} > 30 GeV")
text.DrawLatex(c.GetLeftMargin()+0.08, 1.0-c.GetTopMargin()-0.17, "#geq2j, #geq0b")

c.SaveAs("~/public_html/mt2/supplementary_figures/inclusive_mt2.pdf")




