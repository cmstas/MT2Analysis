# STCImplementation background estimates and plots

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

sys.path.append("../Nifty")

import NiftyUtils

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages
doRmtpt = False
fullPropagate = True


ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
simplecanvas = ROOT.TCanvas("SimpleNiftyPlotter")
simplecanvas.SetCanvasSize(600,600)
simplecanvas.SetTicks(1,2)
simplecanvas.SetLeftMargin(0.2)
simplecanvas.SetRightMargin(0.2)
simplecanvas.SetTopMargin(0.12)

if len(sys.argv) < 2: 
    print "Which tag?"
    exit(1)

tag = sys.argv[1]

d17 = "output_merged/data_2017_{}.root".format(tag)
d16 = "output_merged/data_2016_{}.root".format(tag)
m17 = "output_merged/mc_2017_{}.root".format(tag)
m16 = "output_merged/mc_2016_{}.root".format(tag)

filenames = [d17, m17, d16, m16]
#filenames = [d17, m17, d16]
#filenames = [m17]
#filenames = [d17]

for filename in filenames:

    outfile = ROOT.TFile.Open(filename,"RECREATE")

    if filename == d17:
        print "2017 Data"
        filepath = "output_unmerged/2017_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == d16:
        print "2016 Data"
        filepath = "output_unmerged/2016_{}/data/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == m17:
        print "2017 MC"
        filepath = "output_unmerged/2017_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
    elif filename == m16:
        print "2016 MC"
        filepath = "output_unmerged/2016_{}/".format(tag)
        inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]

    filelist = [ROOT.TFile.Open(infile) for infile in inputlist]

    h_fsMR = filelist[0].Get("h_FS")
    h_fsMR_Nj23 = filelist[0].Get("h_FS_23")
    h_fsMR_Nj4 = filelist[0].Get("h_FS_4")
#    h_fsMR_alt = filelist[0].Get("h_FS_alt_rel_err")
#    h_fsMR_Nj23_alt = filelist[0].Get("h_FS_23_alt_rel_err")
#    h_fsMR_Nj4_alt = filelist[0].Get("h_FS_4_alt_rel_err")
    if verbose:    
        h_fsMR.Print("all")
        h_fsMR_Nj23.Print("all")
        h_fsMR_Nj4.Print("all")

    fserr = { "h_LL_VR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_LH_VR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_HL_VR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_HH_VR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_LL_VR_23" : (h_fsMR_Nj23.GetBinError(1,1),h_fsMR_Nj23.GetBinError(2,1),h_fsMR_Nj23.GetBinError(3,1)),
              "h_LH_VR_23" : (h_fsMR_Nj23.GetBinError(1,1),h_fsMR_Nj23.GetBinError(2,1),h_fsMR_Nj23.GetBinError(3,1)),
              "h_HL_VR_4" : (h_fsMR_Nj4.GetBinError(1,1),h_fsMR_Nj4.GetBinError(2,1),h_fsMR_Nj4.GetBinError(3,1)),
              "h_HH_VR_4" : (h_fsMR_Nj4.GetBinError(1,1),h_fsMR_Nj4.GetBinError(2,1),h_fsMR_Nj4.GetBinError(3,1)),
              
              "h_LL_SR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_LH_SR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_HL_SR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_HH_SR" : (h_fsMR.GetBinError(1,1),h_fsMR.GetBinError(2,1),h_fsMR.GetBinError(3,1)),
              "h_LL_SR_23" : (h_fsMR_Nj23.GetBinError(1,1),h_fsMR_Nj23.GetBinError(2,1),h_fsMR_Nj23.GetBinError(3,1)),
              "h_LH_SR_23" : (h_fsMR_Nj23.GetBinError(1,1),h_fsMR_Nj23.GetBinError(2,1),h_fsMR_Nj23.GetBinError(3,1)),
              "h_HL_SR_4" : (h_fsMR_Nj4.GetBinError(1,1),h_fsMR_Nj4.GetBinError(2,1),h_fsMR_Nj4.GetBinError(3,1)),
              "h_HH_SR_4" : (h_fsMR_Nj4.GetBinError(1,1),h_fsMR_Nj4.GetBinError(2,1),h_fsMR_Nj4.GetBinError(3,1))                  
              }
        
    fs = { "h_LL_VR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_LH_VR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_HL_VR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_HH_VR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_LL_VR_23" : (h_fsMR_Nj23.GetBinContent(1,1),h_fsMR_Nj23.GetBinContent(2,1),h_fsMR_Nj23.GetBinContent(3,1)),
           "h_LH_VR_23" : (h_fsMR_Nj23.GetBinContent(1,1),h_fsMR_Nj23.GetBinContent(2,1),h_fsMR_Nj23.GetBinContent(3,1)),
           "h_HL_VR_4" : (h_fsMR_Nj4.GetBinContent(1,1),h_fsMR_Nj4.GetBinContent(2,1),h_fsMR_Nj4.GetBinContent(3,1)),
           "h_HH_VR_4" : (h_fsMR_Nj4.GetBinContent(1,1),h_fsMR_Nj4.GetBinContent(2,1),h_fsMR_Nj4.GetBinContent(3,1)),
           
           "h_LL_SR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_LH_SR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_HL_SR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_HH_SR" : (h_fsMR.GetBinContent(1,1),h_fsMR.GetBinContent(2,1),h_fsMR.GetBinContent(3,1)),
           "h_LL_SR_23" : (h_fsMR_Nj23.GetBinContent(1,1),h_fsMR_Nj23.GetBinContent(2,1),h_fsMR_Nj23.GetBinContent(3,1)),
           "h_LH_SR_23" : (h_fsMR_Nj23.GetBinContent(1,1),h_fsMR_Nj23.GetBinContent(2,1),h_fsMR_Nj23.GetBinContent(3,1)),
           "h_HL_SR_4" : (h_fsMR_Nj4.GetBinContent(1,1),h_fsMR_Nj4.GetBinContent(2,1),h_fsMR_Nj4.GetBinContent(3,1)),
           "h_HH_SR_4" : (h_fsMR_Nj4.GetBinContent(1,1),h_fsMR_Nj4.GetBinContent(2,1),h_fsMR_Nj4.GetBinContent(3,1))
           }

#    fsalterr = { "h_LL_VR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_LH_VR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_HL_VR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_HH_VR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_LL_VR_23" : (h_fsMR_Nj23_alt.GetBinContent(1),h_fsMR_Nj23_alt.GetBinContent(2),h_fsMR_Nj23_alt.GetBinContent(3)),
#              "h_LH_VR_23" : (h_fsMR_Nj23_alt.GetBinContent(1),h_fsMR_Nj23_alt.GetBinContent(2),h_fsMR_Nj23_alt.GetBinContent(3)),
#              "h_HL_VR_4" : (h_fsMR_Nj4_alt.GetBinContent(1),h_fsMR_Nj4_alt.GetBinContent(2),h_fsMR_Nj4_alt.GetBinContent(3)),
#              "h_HH_VR_4" : (h_fsMR_Nj4_alt.GetBinContent(1),h_fsMR_Nj4_alt.GetBinContent(2),h_fsMR_Nj4_alt.GetBinContent(3)),
              
#              "h_LL_SR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_LH_SR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_HL_SR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_HH_SR" : (h_fsMR_alt.GetBinContent(1),h_fsMR_alt.GetBinContent(2),h_fsMR_alt.GetBinContent(3)),
#              "h_LL_SR_23" : (h_fsMR_Nj23_alt.GetBinContent(1),h_fsMR_Nj23_alt.GetBinContent(2),h_fsMR_Nj23_alt.GetBinContent(3)),
#              "h_LH_SR_23" : (h_fsMR_Nj23_alt.GetBinContent(1),h_fsMR_Nj23_alt.GetBinContent(2),h_fsMR_Nj23_alt.GetBinContent(3)),
#              "h_HL_SR_4" : (h_fsMR_Nj4_alt.GetBinContent(1),h_fsMR_Nj4_alt.GetBinContent(2),h_fsMR_Nj4_alt.GetBinContent(3)),
#              "h_HH_SR_4" : (h_fsMR_Nj4_alt.GetBinContent(1),h_fsMR_Nj4_alt.GetBinContent(2),h_fsMR_Nj4_alt.GetBinContent(3))                  
#              }

    for histname in ["h_LL_VR","h_LH_VR","h_HL_VR","h_HH_VR","h_LL_SR","h_LH_SR","h_HL_SR","h_HH_SR","h_LL_VR_23","h_LH_VR_23","h_HL_VR_4","h_HH_VR_4","h_LL_SR_23","h_LH_SR_23","h_HL_SR_4","h_HH_SR_4"]:
#    for histname in ["h_LH_VR_23"]:
        print histname
        h = filelist[0].Get(histname)
        # First, sum all region histograms
        for f in filelist[1:]:
            h.Add(f.Get(histname))
        # Now, propagate fshort errors to the predicted counts of these histograms (y bin 2)
        # ROOT takes care of observed STC and ST counts for us, and tracks the error in bin 2 coming from STC statistics, but not that coming from fshort
        # if CR count is 0, then take upper STC count as 1.83, and upper end of predicted error as 1.83 * (fshort+fshorterr)
        fs_P = fs[histname][0]
        fs_P_err = fserr[histname][0]
        fs_M = fs[histname][1]
        fs_M_err = fserr[histname][1]
        fs_L = fs[histname][2]
        fs_L_err = fserr[histname][2]

        # Errors generated by considering the largest weight in the denominator of fshort, and varying fshort by adding that weight to the numerator
 #       alt_P_err = fsalterr[histname][0] if not fullPropagate else 0
 #       alt_M_err = fsalterr[histname][1] if not fullPropagate else 0
 #       alt_L_err = fsalterr[histname][2] if not fullPropagate else 0

        Perr = h.GetBinError(1,2) # pred ST err for P, currently based only on STC stats
        Pcount = h.GetBinContent(1,2) # pred ST counts for P
 #       new_Perr = 1.83 * (fs_P + sqrt(fs_P_err**2 + (alt_P_err*fs_P)**2)) if Pcount == 0 else sqrt( (Perr/Pcount)**2 + (fs_P_err / fs_P)**2 ) * Pcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        new_Perr = 1.83 * (fs_P + fs_P_err) if Pcount == 0 else sqrt( (Perr/Pcount)**2 + (fs_P_err / fs_P)**2 ) * Pcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
#        h.SetBinError(1,2,sqrt( new_Perr**2 + (alt_P_err * Pcount )**2) if Pcount > 0 else new_Perr) 
        h.SetBinError(1,2,new_Perr if Pcount > 0 else new_Perr) 
        Merr = h.GetBinError(2,2) # pred ST err for M, currently based only on STC stats
        Mcount = h.GetBinContent(2,2) # pred ST counts for M
#        new_Merr = 1.83 * (fs_M + sqrt(fs_M_err + (alt_M_err*fs_M)**2)) if Mcount == 0 else sqrt( (Merr/Mcount)**2 + (fs_M_err / fs_M)**2 ) * Mcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        new_Merr = 1.83 * (fs_M + fs_M_err) if Mcount == 0 else sqrt( (Merr/Mcount)**2 + (fs_M_err / fs_M)**2 ) * Mcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
#        h.SetBinError(2,2,sqrt( new_Merr**2 + (alt_M_err * Mcount)**2) if Mcount > 0 else new_Merr)
        h.SetBinError(2,2,new_Merr if Mcount > 0 else new_Merr)
        Lerr = h.GetBinError(3,2) # pred ST err for L, currently based only on STC stats
        Lcount = h.GetBinContent(3,2) # pred ST counts for L
#        new_Lerr = 1.83 * (fs_L + sqrt(fs_L_err + (alt_L_err*fs_L)**2)) if Lcount == 0 else sqrt( (Lerr/Lcount)**2 + (fs_L_err / fs_L)**2 ) * Lcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
        new_Lerr = 1.83 * (fs_L + fs_L_err) if Lcount == 0 else sqrt( (Lerr/Lcount)**2 + (fs_L_err / fs_L)**2 ) * Lcount # total relative error is rel error from STC counts and rel error from fshort, in quadrature
#        h.SetBinError(3,2,sqrt( new_Lerr**2 + (alt_L_err * Lcount)**2) if Lcount > 0 else new_Lerr )
        h.SetBinError(3,2,new_Lerr if Lcount > 0 else new_Lerr )
        outfile.cd()
#        print histname
#        print "P",new_Perr,alt_P_err * Pcount
#        print "M",new_Merr,alt_M_err * Mcount
#        print "L",new_Lerr,alt_L_err * Lcount
        h.Write()
    outfile.cd()
    h_fsMR.Write()
    h_fsMR_Nj23.Write()
    h_fsMR_Nj4.Write()
    for f in filelist:
        f.Close()
    outfile.Close()

print "Done"
