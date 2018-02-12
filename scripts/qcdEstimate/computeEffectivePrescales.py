import ROOT

justplot = False

if not justplot:

  ROOT.gROOT.SetBatch(1)

  c = ROOT.TChain("mt2")
  c.Add("/nfs-6/userdata/mt2/V00-09-04_json_294927-306462_PromptReco_41p96fb/*Run2017*JetHT*.root")
  c.SetBranchStatus("*",0)
  c.SetBranchStatus("HLT_PFHT*",1)
  c.SetBranchStatus("isGolden",1)
  c.SetBranchStatus("ht",1)

  h_PFHT1050 = ROOT.TH1D("h_PFHT1050","PFHT1050",80,0,2000)    
  h_PFHT890  = ROOT.TH1D("h_PFHT890", "PFHT890",  80,0,2000)
  h_PFHT780  = ROOT.TH1D("h_PFHT780", "PFHT780",  80,0,2000)    
  h_PFHT680  = ROOT.TH1D("h_PFHT680", "PFHT680",  80,0,2000)
  h_PFHT590  = ROOT.TH1D("h_PFHT590", "PFHT590",  80,0,2000)
  h_PFHT510  = ROOT.TH1D("h_PFHT510", "PFHT510",  80,0,2000)    
  h_PFHT430  = ROOT.TH1D("h_PFHT430", "PFHT430",  80,0,2000)    
  h_PFHT370  = ROOT.TH1D("h_PFHT370", "PFHT370",  80,0,2000)
  h_PFHT250  = ROOT.TH1D("h_PFHT250", "PFHT250",  80,0,2000)
  h_PFHT180  = ROOT.TH1D("h_PFHT180", "PFHT180",  80,0,2000)
  
  print "Getting PFHT1050 histogram..."
  c.Draw("ht>>h_PFHT1050","HLT_PFHT1050!=0 && isGolden","goff")
  print "Getting PFHT890 histogram..."
  c.Draw("ht>>h_PFHT890","HLT_PFHT890_Prescale!=0 && isGolden","goff")
  print "Getting PFHT780 histogram..."
  c.Draw("ht>>h_PFHT780","HLT_PFHT780_Prescale!=0 && isGolden","goff")
  print "Getting PFHT680 histogram..."
  c.Draw("ht>>h_PFHT680","HLT_PFHT680_Prescale!=0 && isGolden","goff")
  print "Getting PFHT590 histogram..."
  c.Draw("ht>>h_PFHT590","HLT_PFHT590_Prescale!=0 && isGolden","goff")
  print "Getting PFHT510 histogram..."
  c.Draw("ht>>h_PFHT510","HLT_PFHT510_Prescale!=0 && isGolden","goff")
  print "Getting PFHT430 histogram..."
  c.Draw("ht>>h_PFHT430","HLT_PFHT430_Prescale!=0 && isGolden","goff")
  print "Getting PFHT370 histogram..."
  c.Draw("ht>>h_PFHT370","HLT_PFHT370_Prescale!=0 && isGolden","goff")
  print "Getting PFHT250 histogram..."
  c.Draw("ht>>h_PFHT250","HLT_PFHT250_Prescale!=0 && isGolden","goff")
  print "Getting PFHT180 histogram..."
  c.Draw("ht>>h_PFHT180","HLT_PFHT180_Prescale!=0 && isGolden","goff")

  
  h_PFHT1050over890 = h_PFHT1050.Clone("h_PFHT1050over890")
  h_PFHT1050over890.Sumw2()
  h_PFHT1050over890.Divide(h_PFHT890)
  h_PFHT1050over780 = h_PFHT1050.Clone("h_PFHT1050over780")
  h_PFHT1050over780.Sumw2()
  h_PFHT1050over780.Divide(h_PFHT780)
  h_PFHT1050over680 = h_PFHT1050.Clone("h_PFHT1050over680")
  h_PFHT1050over680.Sumw2()
  h_PFHT1050over680.Divide(h_PFHT680)
  h_PFHT1050over590 = h_PFHT1050.Clone("h_PFHT1050over590")
  h_PFHT1050over590.Sumw2()
  h_PFHT1050over590.Divide(h_PFHT590)
  h_PFHT1050over510 = h_PFHT1050.Clone("h_PFHT1050over510")
  h_PFHT1050over510.Sumw2()
  h_PFHT1050over510.Divide(h_PFHT510)
  h_PFHT1050over430 = h_PFHT1050.Clone("h_PFHT1050over430")
  h_PFHT1050over430.Sumw2()
  h_PFHT1050over430.Divide(h_PFHT430)
  h_PFHT1050over370 = h_PFHT1050.Clone("h_PFHT1050over370")
  h_PFHT1050over370.Sumw2()
  h_PFHT1050over370.Divide(h_PFHT370)
  h_PFHT1050over250 = h_PFHT1050.Clone("h_PFHT1050over250")
  h_PFHT1050over250.Sumw2()
  h_PFHT1050over250.Divide(h_PFHT250)
  h_PFHT1050over180 = h_PFHT1050.Clone("h_PFHT1050over180")
  h_PFHT1050over180.Sumw2()
  h_PFHT1050over180.Divide(h_PFHT180)
  h_PFHT510over430 = h_PFHT510.Clone("h_PFHT510over430")
  h_PFHT510over430.Sumw2()
  h_PFHT510over430.Divide(h_PFHT430)
  h_PFHT510over370 = h_PFHT510.Clone("h_PFHT510over370")
  h_PFHT510over370.Sumw2()
  h_PFHT510over370.Divide(h_PFHT370)
  h_PFHT510over250 = h_PFHT510.Clone("h_PFHT510over250")
  h_PFHT510over250.Sumw2()
  h_PFHT510over250.Divide(h_PFHT250)
  h_PFHT510over180 = h_PFHT510.Clone("h_PFHT510over180")
  h_PFHT510over180.Sumw2()
  h_PFHT510over180.Divide(h_PFHT180)

  fout = ROOT.TFile("prescales.root","RECREATE")
  h_PFHT1050.Write()
  h_PFHT890.Write()
  h_PFHT780.Write()
  h_PFHT680.Write()
  h_PFHT590.Write()
  h_PFHT510.Write()
  h_PFHT430.Write()    
  h_PFHT370.Write()
  h_PFHT250.Write()
  h_PFHT180.Write()
  h_PFHT1050over890.Write()
  h_PFHT1050over780.Write()    
  h_PFHT1050over680.Write()
  h_PFHT1050over590.Write()
  h_PFHT1050over510.Write()
  h_PFHT1050over430.Write()
  h_PFHT1050over370.Write()
  h_PFHT1050over250.Write()
  h_PFHT1050over180.Write()
  h_PFHT510over430.Write()
  h_PFHT510over370.Write()
  h_PFHT510over250.Write()    
  h_PFHT510over180.Write()
  fout.Close()

fin = ROOT.TFile("prescales.root")

fit_1050_890 = ROOT.TF1("fit_1050_890","[0]",1250,2000)
h_PFHT1050over890 = fin.Get("h_PFHT1050over890")
h_PFHT1050over890.Fit("fit_1050_890","QN","goff",1250,2000)

fit_1050_780 = ROOT.TF1("fit_1050_780","[0]",1250,2000)
h_PFHT1050over780 = fin.Get("h_PFHT1050over780")
h_PFHT1050over780.Fit("fit_1050_780","QN","goff",1250,2000)

fit_1050_680 = ROOT.TF1("fit_1050_680","[0]",1250,2000)
h_PFHT1050over680 = fin.Get("h_PFHT1050over680")
h_PFHT1050over680.Fit("fit_1050_680","QN","goff",1250,2000)

fit_1050_590 = ROOT.TF1("fit_1050_590","[0]",1250,2000)
h_PFHT1050over590 = fin.Get("h_PFHT1050over590")
h_PFHT1050over590.Fit("fit_1050_590","QN","goff",1250,2000)

fit_1050_510 = ROOT.TF1("fit_1050_510","[0]",1250,2000)
h_PFHT1050over510 = fin.Get("h_PFHT1050over510")
h_PFHT1050over510.Fit("fit_1050_510","QN","goff",1250,2000)

fit_1050_430 = ROOT.TF1("fit_1050_430","[0]",1250,2000)
h_PFHT1050over430 = fin.Get("h_PFHT1050over430")
h_PFHT1050over430.Fit("fit_1050_430","QN","goff",1250,2000)

fit_1050_370 = ROOT.TF1("fit_1050_370","[0]",1250,2000)
h_PFHT1050over370 = fin.Get("h_PFHT1050over370")
h_PFHT1050over370.Fit("fit_1050_370","QN","goff",1250,2000)

fit_1050_250 = ROOT.TF1("fit_1050_250","[0]",1250,2000)
h_PFHT1050over250 = fin.Get("h_PFHT1050over250")
h_PFHT1050over250.Fit("fit_1050_250","QN","goff",1250,2000)

fit_1050_180 = ROOT.TF1("fit_1050_180","[0]",1250,2000)
h_PFHT1050over180 = fin.Get("h_PFHT1050over180")
h_PFHT1050over180.Fit("fit_1050_180","QN","goff",1250,2000)

fit_510_430 = ROOT.TF1("fit_510_430","[0]",650,2000)
h_PFHT510over430 = fin.Get("h_PFHT510over430")
h_PFHT510over430.Fit("fit_510_430","QN","goff",650,2000)

fit_510_370 = ROOT.TF1("fit_510_370","[0]",650,2000)
h_PFHT510over370 = fin.Get("h_PFHT510over370")
h_PFHT510over370.Fit("fit_510_370","QN","goff",650,2000)

fit_510_250 = ROOT.TF1("fit_510_250","[0]",650,2000)
h_PFHT510over250 = fin.Get("h_PFHT510over250")
h_PFHT510over250.Fit("fit_510_250","QN","goff",650,2000)

fit_510_180 = ROOT.TF1("fit_510_180","[0]",650,2000)
h_PFHT510over180 = fin.Get("h_PFHT510over180")
h_PFHT510over180.Fit("fit_510_180","QN","goff",650,2000)

ROOT.gStyle.SetOptStat(0)
c = ROOT.TCanvas("c1","c1",650,650)
c.SetLogy(1)
h_PFHT1050over890.SetLineColor(ROOT.kOrange+9)
h_PFHT1050over780.SetLineColor(ROOT.kMagenta)
h_PFHT1050over680.SetLineColor(ROOT.kBlack)
h_PFHT1050over590.SetLineColor(ROOT.kRed)
h_PFHT1050over510.SetLineColor(ROOT.kBlue)
h_PFHT510over430.SetLineColor(ROOT.kGreen)
h_PFHT510over370.SetLineColor(ROOT.kAzure)
h_PFHT510over250.SetLineColor(ROOT.kOrange+3)
h_PFHT510over180.SetLineColor(ROOT.kViolet-1)
h_PFHT1050over890.SetLineWidth(2)
h_PFHT1050over780.SetLineWidth(2)
h_PFHT1050over680.SetLineWidth(2)
h_PFHT1050over590.SetLineWidth(2)
h_PFHT1050over510.SetLineWidth(2)
h_PFHT510over430.SetLineWidth(2)
h_PFHT510over370.SetLineWidth(2)
h_PFHT510over250.SetLineWidth(2)
h_PFHT510over180.SetLineWidth(2)

fit_1050_890.SetLineColor(h_PFHT1050over890.GetLineColor())
fit_1050_780.SetLineColor(h_PFHT1050over780.GetLineColor())
fit_1050_680.SetLineColor(h_PFHT1050over680.GetLineColor())
fit_1050_590.SetLineColor(h_PFHT1050over590.GetLineColor())
fit_1050_510.SetLineColor(h_PFHT1050over510.GetLineColor())
fit_510_430.SetLineColor(h_PFHT510over430.GetLineColor())
fit_510_370.SetLineColor(h_PFHT510over370.GetLineColor())
fit_510_250.SetLineColor(h_PFHT510over250.GetLineColor())
fit_510_180.SetLineColor(h_PFHT510over180.GetLineColor())
fit_1050_890.SetLineWidth(2)
fit_1050_780.SetLineWidth(2)
fit_1050_680.SetLineWidth(2)
fit_1050_590.SetLineWidth(2)
fit_1050_510.SetLineWidth(2)
fit_510_430.SetLineWidth(2)
fit_510_370.SetLineWidth(2)
fit_510_250.SetLineWidth(2)
fit_510_180.SetLineWidth(2)

h_PFHT1050over890.GetXaxis().SetRangeUser(300, 2000)
h_PFHT1050over890.GetYaxis().SetRangeUser(1e-2,4e2)
h_PFHT1050over890.Draw("E0")
h_PFHT1050over780.Draw("E0 SAME")
h_PFHT1050over680.Draw("E0 SAME")
h_PFHT1050over590.Draw("E0 SAME")
h_PFHT1050over510.Draw("E0 SAME")
h_PFHT510over430.Draw("E0 SAME")
h_PFHT510over370.Draw("E0 SAME")
h_PFHT510over250.Draw("E0 SAME")
h_PFHT510over180.Draw("E0 SAME")

fit_1050_890.Draw("SAME")
fit_1050_780.Draw("SAME")
fit_1050_680.Draw("SAME")
fit_1050_590.Draw("SAME")
fit_1050_510.Draw("SAME")
fit_510_430.Draw("SAME")
fit_510_370.Draw("SAME")
fit_510_250.Draw("SAME")
fit_510_180.Draw("SAME")

ratio1050over430 = fit_1050_510.GetParameter(0) * fit_510_430.GetParameter(0)
ratio1050over430err = ROOT.TMath.Sqrt((fit_1050_510.GetParameter(0)*fit_510_430.GetParError(0))**2 + (fit_510_430.GetParameter(0)*fit_1050_510.GetParError(0))**2)
ratio1050over370 = fit_1050_510.GetParameter(0) * fit_510_370.GetParameter(0)
ratio1050over370err = ROOT.TMath.Sqrt((fit_1050_510.GetParameter(0)*fit_510_370.GetParError(0))**2 + (fit_510_370.GetParameter(0)*fit_1050_510.GetParError(0))**2)
ratio1050over250 = fit_1050_510.GetParameter(0) * fit_510_250.GetParameter(0)
ratio1050over250err = ROOT.TMath.Sqrt((fit_1050_510.GetParameter(0)*fit_510_250.GetParError(0))**2 + (fit_510_250.GetParameter(0)*fit_1050_510.GetParError(0))**2)
ratio1050over180 = fit_1050_510.GetParameter(0) * fit_510_180.GetParameter(0)
ratio1050over180err = ROOT.TMath.Sqrt((fit_1050_510.GetParameter(0)*fit_510_180.GetParError(0))**2 + (fit_510_180.GetParameter(0)*fit_1050_510.GetParError(0))**2)

leg = ROOT.TLegend(0.45,0.12,0.9,0.43)
leg.SetTextFont(62)

leg.AddEntry(fit_1050_890, "PFHT1050/890: {0:.1f} #pm {1:.1f}".format(fit_1050_890.GetParameter(0), fit_1050_890.GetParError(0)),'l')
leg.AddEntry(fit_1050_780, "PFHT1050/780: {0:.1f} #pm {1:.1f}".format(fit_1050_780.GetParameter(0), fit_1050_780.GetParError(0)),'l')
leg.AddEntry(fit_1050_680, "PFHT1050/680: {0:.1f} #pm {1:.1f}".format(fit_1050_680.GetParameter(0), fit_1050_680.GetParError(0)),'l')
leg.AddEntry(fit_1050_590, "PFHT1050/590: {0:.1f} #pm {1:.1f}".format(fit_1050_590.GetParameter(0), fit_1050_590.GetParError(0)),'l')
leg.AddEntry(fit_1050_510, "PFHT1050/510: {0:.1f} #pm {1:.1f}".format(fit_1050_510.GetParameter(0), fit_1050_510.GetParError(0)),'l')
leg.AddEntry(fit_510_430, "PFHT510/430: {0:.2f} #pm {1:.1f} #rightarrow {2:.1f} #pm {3:.1f}".format(fit_510_430.GetParameter(0), fit_510_430.GetParError(0),ratio1050over430,ratio1050over430err),'l')
leg.AddEntry(fit_510_370, "PFHT510/370: {0:.2f} #pm {1:.1f} #rightarrow {2:.1f} #pm {3:.1f}".format(fit_510_370.GetParameter(0), fit_510_370.GetParError(0),ratio1050over370,ratio1050over370err),'l')
leg.AddEntry(fit_510_250, "PFHT510/250: {0:.2f} #pm {1:.2f} #rightarrow {2:.0f} #pm {3:.0f}".format(fit_510_250.GetParameter(0), fit_510_250.GetParError(0),ratio1050over250,ratio1050over250err),'l')
leg.AddEntry(fit_510_180, "PFHT510/180: {0:.2f} #pm {1:.2f} #rightarrow {2:.0f} #pm {3:.0f}".format(fit_510_180.GetParameter(0), fit_510_180.GetParError(0),ratio1050over180,ratio1050over180err),'l')
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()

c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/prescales_2017.pdf")
c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/prescales_2017.png")

raw_input()

fin.Close()
