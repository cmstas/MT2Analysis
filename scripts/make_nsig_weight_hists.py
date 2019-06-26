import os, sys
import glob
import ROOT as r

babies_tag = sys.argv[1]
MODEL = sys.argv[2]

files = glob.glob("/hadoop/cms/store/user/bemarsh/mt2babies/{0}_{1}*/mt2_baby*.root".format(babies_tag, MODEL))

print "Adding {0} files...".format(len(files))
c = r.TChain("mt2")
for fname in files:
    c.Add(fname)

x_nbins = 121
x_min = -12.5
x_max = 3012.5
y_nbins = 85
y_min = -12.5
y_max = 2112.5

if "T2cc" in MODEL:
    y_nbins = 221
    y_min = -2.5
    y_max = 1102.5

if MODEL=="T2tt":
    x_nbins = 241
    x_min = -25.0/3/2
    x_max = 2000+25.0/3/2
    y_nbins = 93
    y_min = -6.25
    y_max = 1156.25
    

fout = r.TFile("nsig_weights_{0}_{1}.root".format(babies_tag, MODEL), "RECREATE")
h_nsig = r.TH2D("h_nsig", ";mass1 [GeV];mass2 [GeV]", x_nbins, x_min, x_max, y_nbins, y_min, y_max)
h_avg_weight_btagsf = h_nsig.Clone("h_avg_weight_btagsf")
h_avg_weight_btagsf_heavy_UP = h_nsig.Clone("h_avg_weight_btagsf_heavy_UP")
h_avg_weight_btagsf_light_UP = h_nsig.Clone("h_avg_weight_btagsf_light_UP")
h_avg_weight_btagsf_heavy_DN = h_nsig.Clone("h_avg_weight_btagsf_heavy_DN")
h_avg_weight_btagsf_light_DN = h_nsig.Clone("h_avg_weight_btagsf_light_DN")
h_avg_weight_isr = h_nsig.Clone("h_avg_weight_isr")
h_avg_weight_isr_UP = h_nsig.Clone("h_avg_weight_isr_UP")
h_avg_weight_isr_DN = h_nsig.Clone("h_avg_weight_isr_DN")



c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_nsig", "", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_btagsf","(met_pt > -1.)*weight_btagsf", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_btagsf_heavy_UP","(met_pt > -1.)*weight_btagsf_heavy_UP", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_btagsf_light_UP","(met_pt > -1.)*weight_btagsf_light_UP", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_btagsf_heavy_DN","(met_pt > -1.)*weight_btagsf_heavy_DN", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_btagsf_light_DN","(met_pt > -1.)*weight_btagsf_light_DN", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_isr","(met_pt > -1.)*weight_isr", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_isr_UP","(met_pt > -1.)*weight_isr_UP", "goff")
c.Draw("GenSusyMScan2:GenSusyMScan1+(GenSusyMScan1%25==8 ? 1 : 0)>>h_avg_weight_isr_DN","(met_pt > -1.)*weight_isr_DN", "goff")

h_avg_weight_btagsf.Divide(h_nsig)
h_avg_weight_btagsf_heavy_UP.Divide(h_nsig)
h_avg_weight_btagsf_light_UP.Divide(h_nsig)
h_avg_weight_btagsf_heavy_DN.Divide(h_nsig)
h_avg_weight_btagsf_light_DN.Divide(h_nsig)
h_avg_weight_isr.Divide(h_nsig)
h_avg_weight_isr_UP.Divide(h_nsig)
h_avg_weight_isr_DN.Divide(h_nsig)

fout.Write()
fout.Close()



