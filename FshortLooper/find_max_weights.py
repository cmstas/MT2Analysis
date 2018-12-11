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

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetLegendTextSize(0.1)
ROOT.gStyle.SetPaintTextFormat("#3.3g")
ROOT.gStyle.SetLegendBorderSize(0)

if len(sys.argv) < 2: 
    print "Which file?"
    exit()

filename = sys.argv[1]
shortname = filename[filename.rfind("/")+1:filename.find(".")]

isData = shortname.split("_")[0] == "data"
year = shortname.split("_")[1]
tag = shortname[shortname.find(year)+5:] # e.g. 2016_

filepath = "output_unmerged/{0}_{1}/".format(year,tag)
if isData: filepath += "data/"
inputlist = [filepath+f for f in os.listdir(filepath) if isfile(join(filepath, f))]
filelist = [ROOT.TFile.Open(infile) for infile in inputlist]

os.system("mkdir -p output")
os.system("mkdir -p pngs/{0}".format(shortname))

tfile = ROOT.TFile.Open(filename,"READ")
names = tfile.GetKeyNames()

outfile = ROOT.TFile.Open("output/Fshort_{}.root".format(shortname),"RECREATE")
outfile.cd()

# loop over every key and process histograms 
for rawname in names:
    name = rawname[1:] # Get rid of leading /
    # Fshort histograms
    if name.find("fs") > 0 and name.find("max_weight") < 0 and name.find("1000") > 0 and name.find("SR") > 0 and name.find("4") > 0:        
        h_fs = tfile.Get(name)
        for length in range(2,5):
            printable_length = ["Incl","P","M","L"][length-1]
            if not isData: # Additional error term for MC, accounting for large weights appearing in STC denominator, but not ST numerator
                max_stc_weight = 0
                max_st_weight = 0
                max_stc_f = None
                max_st_f = None
                for f in filelist:
                    h_max_weight = f.Get(name+"_max_weight")
                    h_max_weight_ST = f.Get(name+"_max_weight_ST")
                    this_max_weight = h_max_weight.GetBinContent(length-1)
                    this_max_weight_ST = h_max_weight_ST.GetBinContent(length-1)
                    if this_max_weight > max_stc_weight:
                        max_stc_weight = this_max_weight
                        max_stc_f = f
                    if this_max_weight_ST > max_st_weight:
                        max_st_weight = this_max_weight_ST
                        max_st_f = f
                # there's an additional statistical error in the MC numerator coming from high weight events in the denominator not entering the numerator                        
#                nerr_alt = sqrt( nerr**2 + max_weight**2 )
                if max_stc_f != None: print "max_stc_weight for",name,printable_length,max_stc_weight,"from",max_stc_f.GetName()
                else: print "No STC entries for",name,printable_length
                if max_st_f != None: print "max_st_weight  for",name,printable_length,max_st_weight,"from",max_st_f.GetName()
                else: print "No ST entries for",name,printable_length
