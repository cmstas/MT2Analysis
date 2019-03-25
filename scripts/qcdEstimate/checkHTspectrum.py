## plot full HT spectrum using prescaled pure HT triggers
## should look like a smooth curve if prescales are correct

import glob
import ROOT

justPlot = False

## method = 1 will used hard-coded effective prescale values (enter these below)
## method = 2 will use the prescale values stored in the babies on an event-by-event basis
method = 2

# year = 2016
# year = 2017
year = 2018

if year==2016:
    triggers = ["PFHT125_Prescale", "PFHT200_Prescale", "PFHT300_Prescale", "PFHT350_Prescale", "PFHT475_Prescale", "PFHT600_Prescale", "PFHT900"]
    ht_cuts = [240, 300, 400, 460, 580, 700, 1000, 1000000]
    eff_prescales = [77.6*112.4, 46.8*112.4, 7.99*112.4, 3.99*112.4, 112.4, 28.1, 1]
else:
    triggers = ["PFHT180_Prescale", "PFHT250_Prescale", "PFHT370_Prescale", "PFHT430_Prescale", "PFHT510_Prescale", "PFHT590_Prescale", "PFHT680_Prescale", "PFHT780_Prescale", "PFHT890_Prescale", "PFHT1050"]
    ht_cuts = [240, 340, 480, 540, 640, 720, 820, 940, 1040, 1200, 1000000]
    if year == 2017:
        eff_prescales = [3981,  2760,  863.3, 309.6, 190.0,  94.8, 53.1, 28.5, 14.8, 1]    #2017
    else:
        eff_prescales = [11433, 4120, 1604.3, 502.5, 255.3, 128.7, 64.8, 32.5, 16.3, 1] #2018

if not justPlot:
    c = ROOT.TChain("mt2")

    if year==2016:
        files = glob.glob("/hadoop/cms/store/user/bemarsh/mt2babies/V00-10-08_2016fullYear_17Jul2018_data_Run2016*JetHT*/mt2_baby*.root")
    elif year==2017:
        files = glob.glob("/nfs-6/userdata/mt2/V00-10-07_2017fullYear_31Mar2018/data_Run2017*JetHT*.root")
    else:
        files = glob.glob("/nfs-6/userdata/mt2/V00-10-07_2018fullYear_17Sep2018/data_Run2018*JetHT*.root")

    print "Adding {0} files...".format(len(files))
    for f in files:
        c.Add(f)

    hs = [ROOT.TH1D("h_"+s,"",100,0,2000) for s in triggers]
    
    base = "isGolden && nVert>0 && Flag_globalSuperTightHalo2016Filter && Flag_eeBadScFilter && Flag_HBHENoiseFilter && Flag_HBHENoiseIsoFilter && Flag_EcalDeadCellTriggerPrimitiveFilter && Flag_goodVertices && Flag_badMuonFilter && Flag_badChargedCandidateFilter && Flag_ecalBadCalibFilter"
    
    for i,tname in enumerate(triggers):
        print "Plotting HT for {0}...".format(tname)
        if method==1:
            print "   ","(HLT_{0}!=0 && ht>={1} && ht<{2} && {3})*{4}".format(tname, ht_cuts[i], ht_cuts[i+1], base, eff_prescales[i])
            c.Draw("ht>>h_"+tname, "(HLT_{0}!=0 && ht>={1} && ht<{2} && {3})*{4}".format(tname, ht_cuts[i], ht_cuts[i+1], base, eff_prescales[i]), "goff")
        if method==2:
            print "   ","(ht>={1} && ht<{2} && {3})*HLT_{0}".format(tname, ht_cuts[i], ht_cuts[i+1], base)
            c.Draw("ht>>h_"+tname, "(ht>={1} && ht<{2} && {3})*HLT_{0}".format(tname, ht_cuts[i], ht_cuts[i+1], base), "goff")
        
    fout = ROOT.TFile("htSpect{1}_meth{0}.root".format(method, year),"RECREATE")
    fout.cd()
    [h.Write() for h in hs]

    fout.Close()

ROOT.gStyle.SetOptStat(0)

fin = ROOT.TFile("htSpect{1}_meth{0}.root".format(method, year))

hs = [fin.Get("h_"+s) for s in triggers]

cols = [ROOT.kAzure+7, ROOT.kRed-7, ROOT.kSpring-5, ROOT.kOrange-2, ROOT.kMagenta-4, ROOT.kCyan-7, ROOT.kOrange+7, ROOT.kTeal+3, ROOT.kMagenta+3, ROOT.kGray, ]

c = ROOT.TCanvas()
c.SetLogy()

if method==1:
    hs[0].SetTitle("effective prescales")
if method==2:
    hs[0].SetTitle("prescales vals from cms4")
hs[0].GetYaxis().SetRangeUser(1e4, 6e9)
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

c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/htSpect{1}_meth{0}.pdf".format(method, year))
c.SaveAs("~/public_html/mt2/RebalanceAndSmear/prescales/htSpect{1}_meth{0}.png".format(method, year))

raw_input()
