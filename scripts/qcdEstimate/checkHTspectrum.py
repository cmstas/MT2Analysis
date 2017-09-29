## plot full HT spectrum using prescaled pure HT triggers
## should look like a smooth curve if prescales are correct

import ROOT

justPlot = False

## method = 1 will used hard-coded effective prescale values (enter these below)
## method = 2 will use the prescale values stored in the babies on an event-by-event basis
method = 2

triggers = ["PFHT180_Prescale", "PFHT370_Prescale", "PFHT430_Prescale", "PFHT510_Prescale", "PFHT590_Prescale", "PFHT680_Prescale", "PFHT780_Prescale", "PFHT890_Prescale", "PFHT1050"]
ht_cuts = [240, 440, 520, 620, 700, 800, 900, 1000, 1200, 1000000]
eff_prescales = [1961, 770, 331, 152, 74.3, 47.3, 27.6, 15.2, 1]

if not justPlot:
    c = ROOT.TChain("mt2")
    c.Add("/nfs-6/userdata/mt2/RebalanceAndSmear_V00-09-02_json_294927-301997_PromptReco_13p88fb/*Run2017*JetHT*.root")
    
    hs = [ROOT.TH1D("h_"+s,"",100,0,2000) for s in triggers]
    
    base = "isGolden && nVert>0 && Flag_globalSuperTightHalo2016Filter && Flag_eeBadScFilter && Flag_HBHENoiseFilter && Flag_HBHENoiseIsoFilter && Flag_EcalDeadCellTriggerPrimitiveFilter && Flag_goodVertices"
    
    for i,tname in enumerate(triggers):
        print "Plotting HT for {0}...".format(tname)
        if method==1:
            print "   ","(HLT_{0}!=0 && ht>={1} && ht<{2} && {3})*{4}".format(tname, ht_cuts[i], ht_cuts[i+1], base, eff_prescales[i])
            c.Draw("ht>>h_"+tname, "(HLT_{0}!=0 && ht>={1} && ht<{2} && {3})*{4}".format(tname, ht_cuts[i], ht_cuts[i+1], base, eff_prescales[i]), "goff")
        if method==2:
            print "   ","(ht>={1} && ht<{2} && {3})*HLT_{0}".format(tname, ht_cuts[i], ht_cuts[i+1], base)
            c.Draw("ht>>h_"+tname, "(ht>={1} && ht<{2} && {3})*HLT_{0}".format(tname, ht_cuts[i], ht_cuts[i+1], base), "goff")
        
    fout = ROOT.TFile("htSpect_meth{0}.root".format(method),"RECREATE")
    fout.cd()
    [h.Write() for h in hs]

    fout.Close()

ROOT.gStyle.SetOptStat(0)

fin = ROOT.TFile("htSpect_meth{0}.root".format(method))

hs = [fin.Get("h_"+s) for s in triggers]

cols = [ROOT.kAzure+7, ROOT.kRed-7, ROOT.kSpring-5, ROOT.kOrange-2, ROOT.kMagenta-4, ROOT.kCyan-7, ROOT.kOrange+7, ROOT.kTeal+3, ROOT.kGray]

c = ROOT.TCanvas()
c.SetLogy()

if method==1:
    hs[0].SetTitle("effective prescales")
if method==2:
    hs[0].SetTitle("prescales vals from cms4")
hs[0].GetYaxis().SetRangeUser(1e4, 1e9)
hs[0].GetXaxis().SetNdivisions(510,0)
hs[0].GetXaxis().SetTitle("H_{T} [GeV]")
hs[0].GetYaxis().SetTitle("Events / 20 GeV")

leg = ROOT.TLegend(0.65,0.5,0.87,0.87)

for i in range(len(hs)):
    hs[i].SetFillColor(cols[i])
    hs[i].SetLineColor(ROOT.kBlack)
    hs[i].Draw("HIST SAME")
    leg.AddEntry(hs[i],triggers[i],'f')
hs[0].Draw("SAME AXIS")

leg.Draw()

c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/htSpect_meth{0}.pdf".format(method))
c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/htSpect_meth{0}.png".format(method))

raw_input()