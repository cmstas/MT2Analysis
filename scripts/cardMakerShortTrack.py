# Print cards for use with combine. See MT2Analysis/limits/SignalScan.
# Reimplementation of cardMaker.C.

import ROOT
import re
from math import sqrt
from sys import argv,exit
import os

# Suppresses warnings about TH1::Sumw2
ROOT.gErrorIgnoreLevel = ROOT.kError

verbose = False # Print more error messages
suppressZeroBins = False # Don't print cards for any bin with 0 signal, even if other bins in its region have nonzero signal
suppressZeroTRs = False # Don't print cards for any of the bins in a region with 0 signal in any bin
doDummySignalSyst = False
subtractSignalContam = True
doDummyBackgroundSyst = False
saveTemplates = True # Save templates separately from cards
setBGtoObs = False
setBGplusSigToObs = False

# We print only to a certain number of figures, so nonzero values at precisions lower than that need to be considered equivalent to 0
n_zero = 1e-3
alpha_zero = 1e-5

if len(argv) < 3:
    print "Usage: {0} [signal file name] [tag] [output directory] [doScan = False] [useMC = True] [useSR = True]".format(argv[0])
    exit(1)

signal = argv[1]
tag = argv[2]
outdir = argv[3]
if (not os.path.exists(outdir)): 
    os.mkdir(outdir)
    if saveTemplates:
        os.mkdir(outdir+"/templates")

eff_2016_lo = [2.32,0,0,1.17,0.78] # rescale 2017 fastsim counts based on 2016 count ratios
eff_2016_hi = [0.45,0,0,1.17,0.78] 

doScan = False
useMC = True
useSR = True
if len(argv) > 4:
    if argv[4] == "1" or argv[4].lower() == "true": doScan = True
    if len(argv) > 5:
        if argv[5] == "0" or argv[5].lower() == "false": useMC = False
        if len(argv) > 6:
            if argv[6] == "0" or argv[6].lower() == "false": useSR = False

bmark = ROOT.TBenchmark()
bmark.Start("benchmark")

# These are typically in MT2looper/output/some_directory
# See makeDataDrivenEstimates.sh
f_16 = ROOT.TFile("../ShortTrack/output_merged/data_2016_{0}.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2016_{0}.root".format(tag))
f_1718 = ROOT.TFile("../ShortTrack/output_merged/data_2017and2018_{0}.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2017and2018_{0}.root".format(tag))
if not doScan:
    f_16_contam = ROOT.TFile("../ShortTrack/output_merged/data_2016_{0}_contam.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2016_{0}_contam.root".format(tag))
    f_1718_contam = ROOT.TFile("../ShortTrack/output_merged/data_2017and2018_{0}_contam.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2017and2018_{0}_contam.root".format(tag))
#f_sig_16 = ROOT.TFile("../ShortTrack/output_unmerged/2016_{0}/signal/{1}.root".format(tag,signal))
#f_sig_16_GENMET = ROOT.TFile("../ShortTrack/output_unmerged/2016_{0}/signal/{1}_GENMET.root".format(tag,signal))
    f_sig_1718 = ROOT.TFile("../ShortTrack/output_unmerged/2017_{0}/signal/{1}.root".format(tag,signal))
    f_sig_16 = f_sig_1718 # for now, just use 2017 signal for 2016
    f_sig_1718_GENMET = ROOT.TFile("../ShortTrack/output_unmerged/2017_{0}/signal/{1}_GENMET.root".format(tag,signal))
    f_sig_16_GENMET = f_sig_1718_GENMET # for now, just use 2017 signal for 2016
else:
    f_sigscan = ROOT.TFile.Open("../ShortTrackScan/output_merged/{}_{}.root".format(signal,tag))
    f_16_contam = ROOT.TFile.Open("../ShortTrackScan/output_merged/data_2016_{}_{}_contam.root".format(signal,tag))
    f_1718_contam = ROOT.TFile.Open("../ShortTrackScan/output_merged/data_2017and2018_{}_{}_contam.root".format(signal,tag))

if (f_16.IsZombie()):
    print "2016 not found"
    exit(1)
if (f_1718.IsZombie()):
    print "2017 and 2018 contam not found"
    exit(1)
if not doScan:
    if subtractSignalContam:
        if (f_16_contam.IsZombie()):
            print "2016 not found"
            exit(1)
        if (f_1718_contam.IsZombie()):
            print "2017 and 2018 contam not found"
            exit(1)
    if (f_sig_16.IsZombie()):
        print "2016 signal not found"
        exit(1)
    if (f_sig_1718.IsZombie()):
        print "2017 signal not found"
        exit(1)
    if (f_sig_16_GENMET.IsZombie()):
        print "2016 GENMET signal not found"
        exit(1)
    if (f_sig_1718_GENMET.IsZombie()):
        print "2017 GENMET signal not found"
        exit(1)

# All signal mass points share the same backgrounds. Assemble the background portion of the 
# cards once here, then fill in the mass-point-specific signal information later in makeCard.
def makeTemplate(year,region,length):
    category = ["P","P3","P4","M","L"][length-1]
    if verbose: print "Forming template for region {0}, track category: {1}\n".format(region,category)

    channel = region+"_"+category+"_"+year

    if year == "2016":   f = f_16
    elif year == "2017and2018": f = f_1718

    ptstring = ""
    if region.find("lo") > 0: ptstring = "_lo"
    elif region.find("hi") > 0: ptstring = "_hi"
    h_fs = f.Get("h_FS_"+("23" if region.find("23") > 0 else "4")+ptstring).Clone("h_fs_{}".format(channel))
    fs = h_fs.GetBinContent(length)
    if fs <= 0:
        print "fs = {:.3f} in {}, aborting".format(fs,channel)
        exit(1)
    h_fs_stat_up = f.Get("h_FS_"+("23" if region.find("23") > 0 else "4")+ptstring+"_up").Clone("h_fs_{}_up".format(channel))
    h_fs_stat_dn = f.Get("h_FS_"+("23" if region.find("23") > 0 else "4")+ptstring+"_dn").Clone("h_fs_{}_dn".format(channel))
    fserr_stat_up = 1+(h_fs_stat_up.GetBinError(length)/fs)
    fserr_stat_dn = 1-(h_fs_stat_dn.GetBinError(length)/fs)
    fshort_id_string = "{}_{}{}_{}".format(category,"23" if region.find("23") > 0 else "4",ptstring,year)

    if verbose: 
        print f.GetName()
        print "h_"+region
    h = f.Get("h_"+region).Clone("h_{}".format(channel))
    n_bkg = h.GetBinContent(length,2)
    n_obs = h.GetBinContent(length,1) if not setBGtoObs else n_bkg
    n_stc = round(h.GetBinContent(length,3))

    n_bkg = n_bkg if n_bkg >= n_zero else 0.001 # Printing exactly 0 background, when there's only one background, causes combine to crash
    n_stc = n_stc if n_stc >= n_zero else 0
    n_obs = n_obs if n_obs >= n_zero else 0

    # arbitrarily say that cr stats are 2 with alpha = 0 if cr is empty
#    if n_stc == 0:
#        n_stc = 2
#        alpha = n_bkg/n_stc # alpha can't be literally 0 since n_bkg > 0 to keep combine from crashing
#    else:
    alpha = n_bkg / n_stc if n_stc > 0 else fs
    
    # Assemble background estimate for this signal region. Write in variable names directly for signal quantities, and sub them in later point-by-point
    # within makeCard.
    template_list = []
    template_list.append("imax 1  number of channels\n")
    template_list.append("jmax 1  number of backgrounds\n")
    template_list.append("kmax *\n")
    template_list.append("------------\n")
    template_list.append("bin         {0}\n".format(channel))
    if not setBGplusSigToObs:
        template_list.append("observation {0:.3f}\n".format(n_obs))
    else:
        template_list.append("observation n_obs\n")
    template_list.append("------------\n")
    template_list.append("bin             {0} {0}\n".format(channel))
    template_list.append("process          sig                  bg\n")
    template_list.append("process           0                   1\n")
    template_list.append("rate             n_sig          {0:.3f}\n".format(n_bkg))

    # Write in signal uncertainties, including placeholders
    if year == "2016":
        lumi_syst_16 = 1.025
        template_list.append("lumi_syst_2016    lnN   {:.3f}     -\n".format(lumi_syst_16))
    else:
        lumi_syst_17 = 0.023
        lumi_syst_18 = 0.050
        lumi_syst_1718 = 1+sqrt(lumi_syst_17**2 + lumi_syst_18**2)
        template_list.append("lumi_syst_2017and2018    lnN   {:.3f}     -\n".format(lumi_syst_1718))


    template_list.append("name_sig_mcstat lnN    mcstat_sig - \n") # fully uncorrelated in all bins
    if subtractSignalContam: template_list.append("name_sig_contam        lnN   sig_contam_val -\n") # fully uncorrelated in all bins
    if (doDummySignalSyst):
        template_list.append("name_syst_sig      lnN   sig_syst    -\n")
    else:
        template_list.append("sig_pu                    lnN    1.046   -\n") # fully correlated across all bins, and years
        template_list.append("sig_jec_{}                    lnN    {:.3f}   -\n".format(year,1.05 if year == "2016" else 1+sqrt(2)*0.05)) # correlated across bins, but not years
        template_list.append("sig_renorm_{}                    lnN    {:.3f}   -\n".format(year,1.05 if year == "2016" else 1+sqrt(2)*0.05)) # correlated across bins, but not years
        template_list.append("sig_gen_{}                  lnN    genmet_sig   -\n".format(year)) # correlated across bins, but not years
        template_list.append("sig_isr_{}                  lnN    isr_sig   -\n".format(year)) # sig_isr is fully correlated across all bins, but not years
        template_list.append("sig_fastsim                 lnN    1.06  -\n") # fully correlated error assessed to cover fastsim vs fullsim signal efficiency
        template_list.append("sig_acceptance                 lnN    1.10  -\n") # fully correlated error assessed to cover signal acceptance uncertainty

    # background errors
    
    # statistics
    template_list.append("alphaerr_{0}        lnN    -    {1:.3f}/{2:.3f}\n".format(fshort_id_string,fserr_stat_dn,fserr_stat_up))
    template_list.append("crstat_{0}        gmN {1:.0f}    -    {2:.5f}\n".format(channel,n_stc,alpha))

    if (doDummyBackgroundSyst):     # ad hoc 100% error on L tracks, 50% on P and M
        template_list.append("adhoc_{0}         lnN     -   {1:.3f}\n".format(channel,2 if length == 5 else 1.5))
    else:
        # fshort systematic
        h_fs_syst = f.Get("h_FS_"+("23" if region.find("23") > 0 else "4")+ptstring+"_syst").Clone("h_fs_{}".format(channel))
        fserr_syst = 1+(h_fs_syst.GetBinError(length)/h_fs_syst.GetBinContent(length))
        template_list.append("fshort_syst_{0}        lnN    -    {1:.3f}\n".format(fshort_id_string,fserr_syst))        
        # nonclosure systematic
        h_nonclosure = f.Get("h_"+region.replace("SR","VR")+"_nonclosure_systematic").Clone("h_{}_nc".format(channel)) # We want the nonclosure syst from the validation region
        nc_syst = h_nonclosure.GetBinContent(length) + 1 # saved as a relative error, + 1 for combine formatting
        template_list.append("nonclosure_syst_{0}        lnN    -    {1:.3f}\n".format(channel,nc_syst))        

    template = "".join(template_list)

    # The template contains background information used for all signal mass points.
    # channel is needed to name the output file produced in makeCard.
    # lostlep_alpha and lastbin_hybrid are needed for signal contamination subtraction.
    return template,n_bkg

def makeCard(year,region,template,signal,outdir,length,n_bkg,im1=-1,im2=-1):
    category = ["P","P3","P4","M","L"][length-1]
    channel = region+"_"+category+"_"+year
    cardname = "{0}/datacard_{1}_{2}_{3}_{4}.txt".format(outdir,channel,signal,im1,im2)

    to_print = template

    name_syst_sig = "adhoc_sig_"+channel

    if year == "2016": 
        f = f_sig_16
        f_gen = f_sig_16_GENMET
        f_contam = f_16_contam
        rescale_2016 = eff_2016_hi[length-1] if region.find("hi") >= 0 else eff_2016_lo[length-1]
        rescale_sig = rescale_2016 * 35.9 / 41.97
    if year == "2017and2018": 
        f = f_sig_1718
        f_gen = f_sig_1718_GENMET
        f_contam = f_1718_contam
        rescale_sig = (1 + 58.85 / 41.97)

    h_sig_reco = f.Get("h_"+region).Clone("sig_{}".format(channel))
    n_sig_reco = h_sig_reco.GetBinContent(length,1) * rescale_sig
    h_sig_gen = f_gen.Get("h_"+region).Clone("sig_{}_GEN".format(channel))
    n_sig_gen = h_sig_gen.GetBinContent(length,1) * rescale_sig
    n_sig = (n_sig_gen + n_sig_reco)/2
    mcstat_sig = 1 + h_sig_reco.GetBinError(length,1)*rescale_sig / n_sig if n_sig > 0 else 1 # use pre-adjusted n_sig for mc error
# sig isr not implemented yet
#    h_isr_up = f.Get("h_isr_up").Clone("h_isr_up_{}".format(region+signal))
#    h_isr_dn = f.Get("h_isr_dn").Clone("h_isr_dn_{}".format(region+signal))
#   isr_sig_up = h_isr_up.GetBinContent(1) / n_sig - 1 if n_sig > 0 else 0
#   isr_sig_dn = h_isr_dn.GetBinContent(1) / n_sig - 1 if n_sig > 0 else 0
#   isr_sig = max(abs(isr_sig_up),abs(isr_sig_dn))
#    isr_sig = 1.0 + isr_sig if isr_sig_up > 0 else 1.0/(1.0+isr_sig)
    isr_sig = 1.0

    # error assessed due to difference between reco and gen sig results
    genmet_sig = 1.0 + (n_sig_reco - n_sig_gen)/2/n_sig if n_sig > 0 else 1.0

    if subtractSignalContam:
        h_contam = f_contam.Get(("h_"+region+"_contam_"+signal).replace("-","_"))
        contam_efficiency = h_contam.GetBinContent(length)
        contam_err = (h_contam.GetBinError(length)/contam_efficiency) + 1
        n_sig *= contam_efficiency
        to_print = to_print.replace("sig_contam_val","{:.3f}".format(contam_err))
        to_print = to_print.replace("name_sig_contam","sig_contam_"+channel)

    n_sig = n_sig if n_sig >= n_zero else 0

    # Replace signal placeholders in template with actual values.

    if n_sig == 0 and suppressZeroBins: return False

    if setBGplusSigToObs:
        to_print = to_print.replace("n_obs","{:.3f}".format(n_sig+n_bkg))
    to_print = to_print.replace("n_sig","{:.3f}".format(n_sig))
    if (doDummySignalSyst):
        to_print = to_print.replace("name_sig_syst",name_syst_sig)
        to_print = to_print.replace("sig_syst","1.3") # arbitrary 30% signal systematic
    else:
        mcstat_channel = region+"_"+ (category if category != "P3" else "P") # correlate errors in 2016 and 2017-18, as we use the same signal; correlate P and P3
        to_print = to_print.replace("name_sig_mcstat","sig_mcstat_{}".format(mcstat_channel)) 
        to_print = to_print.replace("mcstat_sig","{0:.3f}".format(mcstat_sig))
        to_print = to_print.replace("isr_sig","{0:.3f}".format(isr_sig))
        to_print = to_print.replace("genmet_sig","{0:.3f}".format(genmet_sig))
        if year == "2016": to_print += "rescale_2017to2016_sig_{} lnN  {:.3f}   -\n".format(category, 1+abs(0.5*(1-rescale_2016)))

    outfile = open(cardname,"w")
    outfile.write(to_print)
    outfile.close()
    return n_sig > 0

def makeCardScan(year,region,template,signal,outdir,length,n_bkg,im1=-1,im2=-1):
    category = ["P","P3","P4","M","L"][length-1]
    channel = region+"_"+category+"_"+year
    cardname = "{0}/datacard_{1}_{2}_{3}_{4}.txt".format(outdir,channel,signal,im1,im2)
    
    njht = region[0:region.find("_")]
    if region.find("lo") >= 0:
        pt = "_lo"
    elif region.find("hi") >= 0:
        pt = "_hi"
    else:
        pt = ""
    if region.find("SR") >= 0:
        MT2 = "SR"
    else:
        MT2 = "VR"

    sigscan_name = "h_{}_{}{}_ST".format(njht,MT2,pt)
    sigscan_delta_name = "h_{}_{}{}_ST_RECO_MINUS_GENMET".format(njht,MT2,pt)
    sigscan_isr_name = "h_{}_{}{}_ST_ISR".format(njht,MT2,pt)
    h_sigscan = f_sigscan.Get(sigscan_name).Clone("{}_{}_{}".format(channel,im1,im2))
    h_deltascan = f_sigscan.Get(sigscan_delta_name).Clone("{}_{}_{}_RECO_MINUS_GENMET".format(channel,im1,im2))
    h_isrscan = f_sigscan.Get(sigscan_isr_name).Clone("{}_{}_{}_ISR".format(channel,im1,im2))
    biny = h_sigscan.GetYaxis().FindBin(im1)
    binz = h_sigscan.GetZaxis().FindBin(im2)

    to_print = template

    name_syst_sig = "adhoc_sig_"+channel

    if year == "2016": 
        f_contam = f_16_contam
        rescale_2016 = eff_2016_hi[length-1] if region.find("hi") >= 0 else eff_2016_lo[length-1]
        rescale_sig = rescale_2016 * 35.9 / 41.97
    if year == "2017and2018": 
        f_contam = f_1718_contam
        rescale_sig = 1 + 58.83 / 41.97

    n_sig = h_sigscan.GetBinContent(length,biny,binz) * rescale_sig
#    print channel,n_sig
    delta_sig = h_deltascan.GetBinContent(length,biny,binz) * rescale_sig
    mcstat_sig = 1 + h_sigscan.GetBinError(length,biny,binz) * rescale_sig / n_sig if n_sig > 0 else 1 # use pre-adjusted n_sig for mc error

    isr_sig = 1.0
    if n_sig > 0:
        h_isr_up = f.Get("h_isr_up").Clone("h_isr_up_{}".format(region+signal))
        isr_sig = h_isr_up.GetBinContent(length,biny,binz)


    # error assessed due to difference between reco and gen sig results
    genmet_sig = 1.0 + delta_sig/2/n_sig if n_sig > 0 else 1.0

    if subtractSignalContam:
        contam_efficiency = 1.0
        contam_err = 1.0
        if n_sig > 0:
            h_contamscan = f_contam.Get(("h_"+region+"_contam_"+signal).replace("-","_"))
            contam_efficiency = h_contamscan.GetBinContent(length,biny,binz)
            contam_err = 1 + (1-contam_efficiency)/2
        if contam_efficiency == 0:
            print "Signal completely lost to signal contamination in {} for mass point ({},{}); raw count was {}".format(channel,im1,im2,n_sig)
        n_sig *= contam_efficiency
        to_print = to_print.replace("sig_contam_val","{:.3f}".format(contam_err))
        to_print = to_print.replace("name_sig_contam","sig_contam_"+channel)

    n_sig = n_sig if n_sig >= n_zero else 0

    # Replace signal placeholders in template with actual values.

    if n_sig == 0 and suppressZeroBins: return False

    if setBGplusSigToObs:
        to_print = to_print.replace("n_obs","{:.3f}".format(n_sig+n_bkg))
    to_print = to_print.replace("n_sig","{:.3f}".format(n_sig))
    if (doDummySignalSyst):
        to_print = to_print.replace("name_sig_syst",name_syst_sig)
        to_print = to_print.replace("sig_syst","1.3") # arbitrary 30% signal systematic
    else:
        mcstat_channel = region+"_"+ (category if category != "P3" else "P") # correlate errors in 2016 and 2017-18, as we use the same signal; correlate P and P3
        to_print = to_print.replace("name_sig_mcstat","sig_mcstat_{}".format(mcstat_channel))
        to_print = to_print.replace("mcstat_sig","{0:.3f}".format(mcstat_sig))
        to_print = to_print.replace("isr_sig","{0:.3f}".format(isr_sig))
        to_print = to_print.replace("genmet_sig","{0:.3f}".format(genmet_sig))
        if year == "2016": to_print += "rescale_2017to2016_sig_{} lnN  {:.3f}   -\n".format(category, 1+abs(0.5*(1-rescale_2016)))

    outfile = open(cardname,"w")
    outfile.write(to_print)
    outfile.close()
    return n_sig > 0

all_points = set()
nonzero_points = set() # This set of successfully processed mass points is printed to a file and used by limits/SignalScan scripts to process the cards
if useSR: print "Using SRs"
else: print "Using VRs"
regions = ["LL_SR_23","LM_SR_23","LLM_SR_23","LH_SR_23","HL_SR_4","HM_SR_4","HLM_SR_4","HH_SR_4"] if useSR else ["LL_VR_23","LM_VR_23","LLM_VR_23","LH_VR_23","HL_VR_4","HM_VR_4","HLM_VR_4","HH_VR_4"]
# find ctau and mass point if only one in file
if not doScan:
    tokens = signal.replace("-","_").split("_")
    ctau = tokens[len(tokens)-3]
    im1 = int(tokens[len(tokens)-2])
    im2 = int(tokens[len(tokens)-1])
    print "Cards are for mass point",im1,im2
# Loop through every directory in the signal file
for region in regions:
    if verbose: print region
    # Loop over track length bins in this signal region
    for length in range(1,6): # length 1 is inclusive P track, then P3, P4, M, L
        track_category = ["P","P3","P4","M","L"][length-1]
        if region[0:3] == "LLM" or region[0:3] == "HLM":
            if length < 5: continue # LLM only used for L tracks
        elif region[1] == "L" or region[1] == "M":
            if length == 5: continue # L and M not used for L tracks
        if length == 1: years = ["2016"]
        elif length == 2 or length == 3: years = ["2017and2018"]
        else: years = ["2016","2017and2018"]
        ptstrings = ["_lo","_hi"] if length != 5 else [""]
        for year in years:
            for ptstr in ptstrings:
                fullregion = region+ptstr
                # Make a template for this bin containing the common background components of the card, with placeholders for signal.
                template,background = makeTemplate(year,fullregion,length) 
                if template == None: 
                    if not (suppressZeroBins or suppressZeroTRs):
                        print "Template could not be formed due to missing histograms in region {0}, but we're not suppressing zero bins. Aborting.".format(fullregion)
                        exit(1)
                    else:
                        print "Template could not be formed due to missing histograms in region {0}, but we're suppressing zero bins anyway. Continuing.".format(fullregion)
                        continue
                if saveTemplates:
                    templatefile = open(outdir+"/templates/"+track_category+"_"+fullregion+"_"+year+"_template.txt","w")
                    templatefile.write(template)
                    templatefile.close()            
                # If we're doing a full scan, loop over mass points and replace placeholders in the template with appropriate signal values for each point.
                if (doScan):
                    x_binwidth = 25
                    x_max = 3000
                    y_binwidth = 25
                    y_max = 3000
                    if signal.find("T2bt") != -1:
                        x_binwidth = 16.6667
                        x_max = 1600
                        y_binwidth = 16.6667
                        y_max = 1600
                    im1 = 0
                    while im1 < x_max:
#                    im1 = 1150
#                    if True:
                        m1 = int(round(im1))
                        im2 = 0
                        while im2 < im1:
#                        im2 = 800
#                        if True:
                            m2 = int(round(im2))
                            all_points.add( (m1,m2) )
                            # Replace signal placeholders and print the card
                            masspoint_outdir = outdir+"/{}_{}".format(m1,m2)
                            os.system("mkdir -p "+masspoint_outdir)
                            nonzero = makeCardScan(year,fullregion,template,signal,masspoint_outdir,length,background,m1,m2)
                            # If no histograms are found for that mass point, don't add it to the list of processed points
                            if nonzero:
                                nonzero_points.add( (m1,m2) )
                            im2 += y_binwidth
                        im1 += x_binwidth
                else:
                    makeCard(year,fullregion,template,signal,outdir,length,background,im1,im2)

        # The 'del's throughout this script remove the python interpreter's reference to the object, which would normally cause an object
        # to be garbage collected. Unfortunately, only TObjects created by constructors are owned by python; all others are owned by ROOT. 
        # Note that most of the TObjects in this script are created by TFile::Get, not a constructor...
        # ROOT diligently tracks every one of these guys until the script ends, then goes through and deletes them all one by one, causing
        # the script to hang for ages when it finishes, instead of letting python release the memory back to the operating system all at once. 
        # While unnoticeable in a shorter script, this process takes a very long time for this one due to the sheer number of TH1s we access.
        # There are two solutions. I recommend the first unless memory usage is a problem.

        # 1) Fast, but leaks a lot of memory (O(10GB) by script's end) since it doesn't actually delete the TObjects. Stops ROOT from trying
        # to delete everything at the end, so the script exits promptly and just releases everything back to the OS when python closes.
        ROOT.gDirectory.Clear() 
        
        # 2) Very, very slow, but without the massive memory leaks since the TObjects are actually deleted. This does the same thing as
        # no solution at all *in terms of total running time*, but spreads the slowness out instead of hanging at the end.
        # The only purpose of choosing this "solution" vs. no solution at all is preventing the memory leak.
        # ROOT.gDirectory.DeleteAll() 
        
        # Note: exactly the same problem applies to cardMaker.C, except replace "python" with "cling", the C++ interpeter used by ROOT macros.

    # Print signal_points to a file
    if (doScan):
        points_file = open("{0}/points_{1}.txt".format(outdir,signal),"w")
        points_file.write("--------------------------------------------\n")
        points_file.write("- saw nonzero signal entries for the following points: \n")
        for mass_point in nonzero_points:
            points_file.write("{0}_{1}_{2}\n".format(signal,mass_point[0],mass_point[1]))
        for mass_point in (all_points - nonzero_points):
            os.system("rm -rf {}/{}_{}".format(outdir,mass_point[0],mass_point[1]))
        points_file.close()

f_16.Close()
f_1718.Close()
if not doScan:
    f_sig_1718.Close()
else:
    f_sigscan.Close()
f_16_contam.Close()
f_1718_contam.Close()

bmark.Stop("benchmark")    
print "\n"
print "------------------------------"
print "CPU Time:   {0:.1f} s\n".format(bmark.GetCpuTime("benchmark"))
print "Real Time:  {0:.1f} s\n".format(bmark.GetRealTime("benchmark"))
print "\n"

