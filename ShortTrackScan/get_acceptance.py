# Combine and process histograms produced by ShortTrackLooper

import ROOT
from math import sqrt
import re
import sys
import os
from os.path import isfile,join
import subprocess

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more status messages

ROOT.gROOT.SetBatch(True)

if len(sys.argv) < 2: 
    print "Usage: python get_acceptance.py tag"
    exit(1)

tag = sys.argv[1]

#models=["T1qqqq","T2qq","T2bt"]
models=["T1qqqq","T2qq"]
ctaus=[10,50,200]


xs_file = ROOT.TFile.Open("../babymaker/data/xsec_susy_13tev_run2.root")
xs_gluino = xs_file.Get("h_xsec_gluino")
xs_gluino.SetDirectory(0)
xs_squark = xs_file.Get("h_xsec_squark")
xs_squark.SetDirectory(0)
xs_file.Close()
xs = {}
xs["T1qqqq"]=xs_gluino
xs["T2qq"]=xs_squark

masspoints = {}
masspoints[("T1qqqq",10)] = [(1600,1575),(1800,1500),(2500,150)]
masspoints[("T1qqqq",50)] = [(1900,1875),(2400,1700),(2400,150)]
masspoints[("T1qqqq",200)] = [(2000,1975),(2400,1800),(2200,150)]
masspoints[("T2qq",10)] = [(950,925),(1250,875),(1700,100)]
masspoints[("T2qq",50)] = [(1200,1175),(1500,1175),(1600,200)]
masspoints[("T2qq",200)] = [(1250,1225),(1500,1175),(1400,200)]
#masspoints[("T2bt",10)] = [(,),(,),(,)]
#masspoints[("T2bt",50)] = [(,),(,),(,)]
#masspoints[("T2bt",200)] = [(,),(,),(,)]

regionsNoL1718SR = [cat + " " + kin + " " + reg + " " + pt for reg in ["SR"] for cat in ["P3","P4","M"] for pt in ["lo","hi"] for kin in ["LL","LM","LH","HL","HM","HH"] ]
regionsLSR = ["L " + kin + " " + reg for kin in ["LLM","LH","HLM","HH"] for reg in ["SR"]]
regions = regionsNoL1718SR + regionsLSR

rescale_to_1718 = 1+(59./42.)

def convertRegion (region):
    tokens = region.split()
    cat = tokens[0]
    kin = tokens[1]
    index_to_return = 1 # P
    if cat == "P3": index_to_return = 2
    elif cat == "P4": index_to_return = 3
    elif cat == "M": index_to_return = 4
    elif cat == "L": index_to_return = 5
    pt = ""
    if region.find("hi") > 0:
        pt = "_hi"
    elif region.find("lo") > 0:
        pt = "_lo"
    return "h_{}_SR{}_ST".format(kin,pt),index_to_return

print ""
print "In 2017-18 Regions, events in signal region / total = efficiency%, for points near the exclusion curve:"
for model in models:    
    xs_hist = xs[model]
    for ctau in ctaus:
        inname = "output_merged/{}_{}_{}.root".format(model,ctau,tag)
        infile = ROOT.TFile.Open(inname)
        infile.cd()
        compressed = 0
        corner = 0
        boosted = 0
        mps = masspoints[(model,ctau)]
        for region in regions:
            histname,index = convertRegion(region)
            hist = infile.Get(histname).Clone(histname+region.replace(" ","_"))
            compressed_bin_y = hist.GetYaxis().FindBin(mps[0][0])
            compressed_bin_z = hist.GetZaxis().FindBin(mps[0][1])
            corner_bin_y = hist.GetYaxis().FindBin(mps[1][0])
            corner_bin_z = hist.GetZaxis().FindBin(mps[1][1])
            boosted_bin_y = hist.GetYaxis().FindBin(mps[2][0])
            boosted_bin_z = hist.GetZaxis().FindBin(mps[2][1])
            compressed += hist.GetBinContent(index,compressed_bin_y,compressed_bin_z)
            corner += hist.GetBinContent(index,corner_bin_y,corner_bin_z)
            boosted += hist.GetBinContent(index,boosted_bin_y,boosted_bin_z)
        xs_compressed = xs_hist.GetBinContent(xs_hist.GetXaxis().FindBin(mps[0][0])) 
        xs_corner = xs_hist.GetBinContent(xs_hist.GetXaxis().FindBin(mps[1][0])) 
        xs_boosted = xs_hist.GetBinContent(xs_hist.GetXaxis().FindBin(mps[2][0])) 
        N_compressed = 1000 * xs_compressed * 41.97
        N_corner = 1000 * xs_corner * 41.97
        N_boosted = 1000 * xs_boosted * 41.97
        print ""
        print model,ctau,"cm"
        print "Compressed ({},{})".format(mps[0][0],mps[0][1]),"%#.2g" % (compressed * rescale_to_1718),"/","%#.2g" % (N_compressed * rescale_to_1718),"=",(("%#.2g" % (100*compressed / N_compressed)) + "%").replace(".%","%")
        print "Corner ({},{})".format(mps[1][0],mps[1][1]),"%#.2g" % (corner * rescale_to_1718),"/","%#.2g" % (N_corner * rescale_to_1718),"=",(("%#.2g" % (100*corner / N_corner)) + "%").replace(".%","%")
        print "Boosted ({},{})".format(mps[2][0],mps[2][1]),"%#.2g" % (boosted * rescale_to_1718),"/","%#.2g" % (N_boosted * rescale_to_1718),"=",(("%#.2g" % (100*boosted / N_boosted)) + "%").replace(".%","%")
        infile.Close()

print "Done"
