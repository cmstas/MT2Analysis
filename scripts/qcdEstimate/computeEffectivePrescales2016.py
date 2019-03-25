import ROOT
import glob

justplot = True

if not justplot:

  ROOT.gROOT.SetBatch(1)

  c = ROOT.TChain("mt2")
  files = glob.glob("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-08_2016fullYear_17Jul2018_data_Run2016*JetHT*/mt2_baby*.root")
  print "Adding {0} files...".format(len(files))
  for f in files:
    c.Add(f)

  h_PFHT900 = ROOT.TH1D("h_PFHT900","PFHT900",80,0,2000)    
  h_PFHT800 = ROOT.TH1D("h_PFHT800","PFHT800",80,0,2000)
  h_PFHT600 = ROOT.TH1D("h_PFHT600","PFHT600",80,0,2000)    
  h_PFHT475 = ROOT.TH1D("h_PFHT475","PFHT475",80,0,2000)
  h_PFHT350 = ROOT.TH1D("h_PFHT350","PFHT350",80,0,2000)
  h_PFHT300 = ROOT.TH1D("h_PFHT300","PFHT300",80,0,2000)    
  h_PFHT200 = ROOT.TH1D("h_PFHT200","PFHT200",80,0,2000)    
  h_PFHT125 = ROOT.TH1D("h_PFHT125","PFHT125",80,0,2000)
  
  print "Getting PFHT900 histogram..."
  c.Draw("ht>>h_PFHT900","HLT_PFHT900!=0","goff")
  print "Getting PFHT800 histogram..."
  c.Draw("ht>>h_PFHT800","HLT_PFHT800!=0","goff")
  print "Getting PFHT600 histogram..."
  c.Draw("ht>>h_PFHT600","HLT_PFHT600_Prescale!=0","goff")
  print "Getting PFHT475 histogram..."
  c.Draw("ht>>h_PFHT475","HLT_PFHT475_Prescale!=0","goff")
  print "Getting PFHT350 histogram..."
  c.Draw("ht>>h_PFHT350","HLT_PFHT350_Prescale!=0","goff")
  print "Getting PFHT300 histogram..."
  c.Draw("ht>>h_PFHT300","HLT_PFHT300_Prescale!=0","goff")
  print "Getting PFHT200 histogram..."
  c.Draw("ht>>h_PFHT200","HLT_PFHT200_Prescale!=0","goff")
  print "Getting PFHT125 histogram..."
  c.Draw("ht>>h_PFHT125","HLT_PFHT125_Prescale!=0","goff")

  
  h_PFHT900over800 = h_PFHT900.Clone("h_PFHT900over800")
  h_PFHT900over800.Sumw2()
  h_PFHT900over800.Divide(h_PFHT800)
  h_PFHT900over600 = h_PFHT900.Clone("h_PFHT900over600")
  h_PFHT900over600.Sumw2()
  h_PFHT900over600.Divide(h_PFHT600)
  h_PFHT900over475 = h_PFHT900.Clone("h_PFHT900over475")
  h_PFHT900over475.Sumw2()
  h_PFHT900over475.Divide(h_PFHT475)
  h_PFHT900over350 = h_PFHT900.Clone("h_PFHT900over350")
  h_PFHT900over350.Sumw2()
  h_PFHT900over350.Divide(h_PFHT350)
  h_PFHT900over125 = h_PFHT900.Clone("h_PFHT900over125")
  h_PFHT900over125.Sumw2()
  h_PFHT900over125.Divide(h_PFHT125)
  h_PFHT475over350 = h_PFHT475.Clone("h_PFHT475over350")
  h_PFHT475over350.Sumw2()
  h_PFHT475over350.Divide(h_PFHT350)
  h_PFHT475over300 = h_PFHT475.Clone("h_PFHT475over300")
  h_PFHT475over300.Sumw2()
  h_PFHT475over300.Divide(h_PFHT300)
  h_PFHT475over200 = h_PFHT475.Clone("h_PFHT475over200")
  h_PFHT475over200.Sumw2()
  h_PFHT475over200.Divide(h_PFHT200)
  h_PFHT475over125 = h_PFHT475.Clone("h_PFHT475over125")
  h_PFHT475over125.Sumw2()
  h_PFHT475over125.Divide(h_PFHT125)

  fout = ROOT.TFile("prescales_2016.root","RECREATE")
  h_PFHT900.Write()
  h_PFHT800.Write()
  h_PFHT600.Write()
  h_PFHT475.Write()
  h_PFHT350.Write()
  h_PFHT300.Write()
  h_PFHT200.Write()    
  h_PFHT125.Write()
  h_PFHT900over800.Write()
  h_PFHT900over600.Write()    
  h_PFHT900over475.Write()
  h_PFHT900over350.Write()
  h_PFHT900over125.Write()
  h_PFHT475over350.Write()
  h_PFHT475over300.Write()
  h_PFHT475over200.Write()    
  h_PFHT475over125.Write()
  fout.Close()

fin = ROOT.TFile("prescales_2016.root")

fit_900_600 = ROOT.TF1("fit_900_600","[0]",1000,2000)
h_PFHT900over600 = fin.Get("h_PFHT900over600")
h_PFHT900over600.Fit("fit_900_600","QN","goff",1000,2000)

fit_900_475 = ROOT.TF1("fit_900_475","[0]",1000,2000)
h_PFHT900over475 = fin.Get("h_PFHT900over475")
h_PFHT900over475.Fit("fit_900_475","QN","goff",1000,2000)

fit_900_350 = ROOT.TF1("fit_900_350","[0]",1000,2000)
h_PFHT900over350 = fin.Get("h_PFHT900over350")
h_PFHT900over350.Fit("fit_900_350","QN","goff",1000,2000)

fit_900_125 = ROOT.TF1("fit_900_125","[0]",1000,2000)
h_PFHT900over125 = fin.Get("h_PFHT900over125")
h_PFHT900over125.Fit("fit_900_125","QN","goff",1000,2000)

fit_475_350 = ROOT.TF1("fit_475_350","[0]",575,2000)
h_PFHT475over350 = fin.Get("h_PFHT475over350")
h_PFHT475over350.Fit("fit_475_350","QN","goff",575,2000)

fit_475_300 = ROOT.TF1("fit_475_300","[0]",575,2000)
h_PFHT475over300 = fin.Get("h_PFHT475over300")
h_PFHT475over300.Fit("fit_475_300","QN","goff",575,2000)

fit_475_200 = ROOT.TF1("fit_475_200","[0]",575,2000)
h_PFHT475over200 = fin.Get("h_PFHT475over200")
h_PFHT475over200.Fit("fit_475_200","QN","goff",575,2000)

fit_475_125 = ROOT.TF1("fit_475_125","[0]",575,2000)
h_PFHT475over125 = fin.Get("h_PFHT475over125")
h_PFHT475over125.Fit("fit_475_125","QN","goff",575,2000)

ROOT.gStyle.SetOptStat(0)
c = ROOT.TCanvas("c1","c1",650,650)
c.SetLogy(1)
h_PFHT900over600.SetLineColor(ROOT.kMagenta)
h_PFHT900over475.SetLineColor(ROOT.kBlack)
h_PFHT900over350.SetLineColor(ROOT.kRed)
h_PFHT900over125.SetLineColor(ROOT.kBlue)
h_PFHT475over350.SetLineColor(ROOT.kGreen)
h_PFHT475over300.SetLineColor(ROOT.kAzure)
h_PFHT475over200.SetLineColor(ROOT.kOrange+3)
h_PFHT475over125.SetLineColor(ROOT.kViolet-1)
h_PFHT900over600.SetLineWidth(2)
h_PFHT900over475.SetLineWidth(2)
h_PFHT900over350.SetLineWidth(2)
h_PFHT900over125.SetLineWidth(2)
h_PFHT475over350.SetLineWidth(2)
h_PFHT475over300.SetLineWidth(2)
h_PFHT475over200.SetLineWidth(2)
h_PFHT475over125.SetLineWidth(2)

fit_900_600.SetLineColor(h_PFHT900over600.GetLineColor())
fit_900_475.SetLineColor(h_PFHT900over475.GetLineColor())
fit_900_350.SetLineColor(h_PFHT900over350.GetLineColor())
fit_900_125.SetLineColor(h_PFHT900over125.GetLineColor())
fit_475_350.SetLineColor(h_PFHT475over350.GetLineColor())
fit_475_300.SetLineColor(h_PFHT475over300.GetLineColor())
fit_475_200.SetLineColor(h_PFHT475over200.GetLineColor())
fit_475_125.SetLineColor(h_PFHT475over125.GetLineColor())
fit_900_600.SetLineWidth(2)
fit_900_475.SetLineWidth(2)
fit_900_350.SetLineWidth(2)
fit_900_125.SetLineWidth(2)
fit_475_350.SetLineWidth(2)
fit_475_300.SetLineWidth(2)
fit_475_200.SetLineWidth(2)
fit_475_125.SetLineWidth(2)

h_PFHT900over600.GetYaxis().SetRangeUser(0.001,1000)
h_PFHT900over600.Draw("E0 SAME")
h_PFHT900over475.Draw("E0 SAME")
h_PFHT900over350.Draw("E0 SAME")
# h_PFHT900over125.Draw("E0 SAME")
h_PFHT475over350.Draw("E0 SAME")
h_PFHT475over300.Draw("E0 SAME")
h_PFHT475over200.Draw("E0 SAME")
h_PFHT475over125.Draw("E0 SAME")

fit_900_600.Draw("SAME")
fit_900_475.Draw("SAME")
fit_900_350.Draw("SAME")
# fit_900_125.Draw("SAME")
fit_475_350.Draw("SAME")
fit_475_300.Draw("SAME")
fit_475_200.Draw("SAME")
fit_475_125.Draw("SAME")

leg = ROOT.TLegend(0.3,0.12,0.8,0.35)
leg.SetTextFont(62)

leg.AddEntry(fit_900_600, "PFHT900/600: {0:.1f} #pm {1:.1f}".format(fit_900_600.GetParameter(0), fit_900_600.GetParError(0)),'l')
leg.AddEntry(fit_900_475, "PFHT900/475: {0:.1f} #pm {1:.1f}".format(fit_900_475.GetParameter(0), fit_900_475.GetParError(0)),'l')
leg.AddEntry(fit_900_350, "PFHT900/350: {0:.1f} #pm {1:.1f}".format(fit_900_350.GetParameter(0), fit_900_350.GetParError(0)),'l')
# leg.AddEntry(fit_900_125, "PFHT900/125: {0:.0f} #pm {1:.0f}".format(fit_900_125.GetParameter(0), fit_900_125.GetParError(0)),'l')
leg.AddEntry(fit_475_350, "PFHT475/350: {0:.2f} #pm {1:.2f}".format(fit_475_350.GetParameter(0), fit_475_350.GetParError(0)),'l')
leg.AddEntry(fit_475_300, "PFHT475/300: {0:.2f} #pm {1:.2f}".format(fit_475_300.GetParameter(0), fit_475_300.GetParError(0)),'l')
leg.AddEntry(fit_475_200, "PFHT475/200: {0:.1f} #pm {1:.1f}".format(fit_475_200.GetParameter(0), fit_475_200.GetParError(0)),'l')
leg.AddEntry(fit_475_125, "PFHT475/125: {0:.1f} #pm {1:.1f}".format(fit_475_125.GetParameter(0), fit_475_125.GetParError(0)),'l')
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()

c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/prescales_2016_94x.png")
c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/prescales_2016_94x.pdf")

raw_input()

fin.Close()
