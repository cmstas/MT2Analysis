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
suppressZeroBins = True # Don't print cards for any bin with 0 signal, even if other bins in its region have nonzero signal
suppressZeroTRs = False # Don't print cards for any of the bins in a region with 0 signal in any bin
doDummySignalSyst = True 
subtractSignalContam = True
doDummyBackgroundSyst = False
saveTemplates = True # Save templates separately from cards
setBGtoObs = True

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
f_16 = ROOT.TFile("../ShortTrack/output_merged/data_2016_{0}_syststat.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2016_{0}_syststat.root".format(tag))
f_1718 = ROOT.TFile("../ShortTrack/output_merged/data_2017and2018_{0}_syststat.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2017_{0}_syststat.root".format(tag))
f_16_contam = ROOT.TFile("../ShortTrack/output_merged/data_2016_{0}_contam.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2016_{0}_contam.root".format(tag))
f_1718_contam = ROOT.TFile("../ShortTrack/output_merged/data_2017and2018_{0}_contam.root".format(tag)) if not useMC else ROOT.TFile("../ShortTrack/output_merged/mc_2017_{0}_contam.root".format(tag))
#f_sig_16 = ROOT.TFile("../ShortTrack/output_unmerged/2016_{0}/signal/{1}.root".format(tag,signal))
f_sig_1718 = ROOT.TFile("../ShortTrack/output_unmerged/2017_{0}/signal/{1}.root".format(tag,signal))
f_sig_16 = f_sig_1718 # for now, just use 2017 signal for 2016

if (f_16.IsZombie()):
    print "2016 not found"
    exit(1)
if (f_1718.IsZombie()):
    print "2017 and 2018 contam not found"
    exit(1)
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
    h_fs_stat = f.Get("h_FS_"+("23" if region.find("23") > 0 else "4")+ptstring).Clone("h_fs_{}".format(channel))
    fserr_stat = 1+(h_fs_stat.GetBinError(length)/h_fs_stat.GetBinContent(length))
    fshort_id_string = "{}_{}{}".format(category,"23" if region.find("23") > 0 else "4",ptstring)

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
    if n_stc == 0:
        n_stc = 2
        alpha = 0
    else:
        alpha = n_bkg / n_stc
    
    # Assemble background estimate for this signal region. Write in variable names directly for signal quantities, and sub them in later point-by-point
    # within makeCard.
    template_list = []
    template_list.append("imax 1  number of channels\n")
    template_list.append("jmax 1  number of backgrounds\n")
    template_list.append("kmax *\n")
    template_list.append("------------\n")
    template_list.append("bin         {0}\n".format(channel))
    template_list.append("observation {0:.3f}\n".format(n_obs))
    template_list.append("------------\n")
    template_list.append("bin             {0} {0}\n".format(channel))
    template_list.append("process          sig                  bg\n")
    template_list.append("process           0                   1\n")
    template_list.append("rate             n_sig          {0:.3f}\n".format(n_bkg))

    # Write in signal placeholders
    if subtractSignalContam: template_list.append("name_sig_contam        lnN   sig_contam_val -\n")
    if (doDummySignalSyst):
        template_list.append("name_sig_syst      lnN   sig_syst    -\n")
    else:
        template_list.append("name_sig_lumi                    lnN    sig_lumi   -\n")
        template_list.append("name_sig_pu                    lnN    sig_pu   -\n")
        template_list.append("name_sig_mcstat     lnN    sig_mcstat   -\n")
        template_list.append("name_sig_genmet                  lnU    sig_genmet   -\n")
        template_list.append("name_sig_isr                  lnN    sig_isr   -\n")
        template_list.append("name_sig_btagsf_heavy            lnN    sig_btagsf_heavy   -\n")
        template_list.append("name_sig_btagsf_light            lnN    sig_btagsf_light   -\n")

    # background errors
    
    # statistics
    template_list.append("alphaerr_{0}        lnN    -    {1:.3f}\n".format(fshort_id_string,fserr_stat))
    template_list.append("crstat_{0}        gmN {1:.0f}    -    {2:.5f}\n".format(channel,n_stc,alpha))

    # systematics
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
    return template

def makeCard(year,region,template,signal,outdir,length,im1=-1,im2=-1):
    category = ["P","P3","P4","M","L"][length-1]
    channel = region+"_"+category+"_"+year
    cardname = "{0}/datacard_{1}_{2}_{3}_{4}.txt".format(outdir,channel,signal,im1,im2)

    to_print = template

    name_sig_syst = "adhoc_sig_"+channel

    if year == "2016": 
        f = f_sig_16
        f_contam = f_16_contam
    if year == "2017and2018": 
        f = f_sig_1718
        f_contam = f_1718_contam

    h_sig = f.Get("h_"+region).Clone("sig_{}".format(channel))
    n_sig = h_sig.GetBinContent(length,1)

    # For now, 16 and 18 signals are rescaled 2017
    if year == "2016": n_sig *= 35.9 / 41.97
    if year == "2017and2018": n_sig *= (1 + 58.83 / 41.97)

    if subtractSignalContam:
        h_contam = f_contam.Get(("h_"+region+"_contam_"+signal).replace("-","_"))
        contam_efficiency = h_contam.GetBinContent(length)
        contam_err = (h_contam.GetBinError(length)/contam_efficiency) + 1
        n_sig *= contam_efficiency
        to_print = to_print.replace("sig_contam_val","{:.3f}".format(contam_err))
        to_print = to_print.replace("name_sig_contam","sig_contam_"+channel)

    n_sig = n_sig if n_sig >= n_zero else 0

    # Replace signal placeholders in template with actual values.

    if n_sig == 0: return False

    to_print = to_print.replace("n_sig","{:.3f}".format(n_sig))
    if (doDummySignalSyst):
        to_print = to_print.replace("name_sig_syst",name_sig_syst)
        to_print = to_print.replace("sig_syst","1.3") # arbitrary 30% signal systematic
    else:
        to_print = to_print.replace("name_sig_lumi",name_sig_lumi)
        to_print = to_print.replace("sig_lumi","{0:.3f}".format(sig_lumi))
        to_print = to_print.replace("name_sig_pu",name_sig_pu)
        to_print = to_print.replace("sig_pu","{0:.3f}".format(sig_pu))
        to_print = to_print.replace("name_sig_mcstat",name_sig_mcstat)
        to_print = to_print.replace("sig_mcstat","{0:.3f}".format(sig_mcstat))
        to_print = to_print.replace("name_sig_genmet",name_sig_genmet)
        to_print = to_print.replace("sig_genmet","{0:.3f}".format(sig_genmet))
        to_print = to_print.replace("name_sig_isr",name_sig_isr)
        to_print = to_print.replace("sig_isr","{0:.3f}".format(sig_isr))
        to_print = to_print.replace("name_sig_btagsf_heavy",name_sig_btagsf_heavy)
        to_print = to_print.replace("sig_btagsf_heavy","{0:.3f}".format(sig_btagsf_heavy))
        to_print = to_print.replace("name_sig_btagsf_light",name_sig_btagsf_light)
        to_print = to_print.replace("sig_btagsf_light","{0:.3f}".format(sig_btagsf_light))
    outfile = open(cardname,"w")
    outfile.write(to_print)
    outfile.close()
    return True

signal_points = set() # This set of successfully processed mass points is printed to a file and used by limits/SignalScan scripts to process the cards
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
                template = makeTemplate(year,fullregion,length) 
                if template == None: 
                    if not (suppressZeroBins or suppressZeroTRs):
                        print "Template could not be formed due to missing histograms in region {0}, but we're not suppressing zero bins. Aborting.".format(fullregion)
                        exit(1)
                    else:
                        print "Template could not be formed due to missing histograms in region {0}, but we're suppressing zero bins anyway. Continuing.".format(fullregion)
                        continue
                if saveTemplates:
                    templatefile = open(outdir+"/templates/"+fullregion+"_"+year+"_template.txt","w")
                    templatefile.write(template)
                    templatefile.close()            
                # If we're doing a full scan, loop over mass points and replace placeholders in the template with appropriate signal values for each point.
                if (doScan):
                    y_binwidth = 25
                    y_max = 1600
                    if signal.find("T2cc") != -1:
                        y_binwidth = 5
                        y_max = 800
                    for im1 in range(0,2301,25):
                        for im2 in range(0,y_max+1,y_binwidth):
                            # Replace signal placeholders and print the card
                            success = makeCard(year,fullregion,template,signal,outdir,length,im1,im2)
                            # If no histograms are found for that mass point, don't add it to the list of processed points
                            if success:
                                signal_points.add( (im1,im2) )
                else:
                    makeCard(year,fullregion,template,signal,outdir,length,im1,im2)

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
        for mass_point in signal_points:
            points_file.write("{0}_{1}_{2}\n".format(signal,mass_point[0],mass_point[1]))
        points_file.close()

f_16.Close()
f_1718.Close()
f_sig_1718.Close()

bmark.Stop("benchmark")    
print "\n"
print "------------------------------"
print "CPU Time:   {0:.1f} s\n".format(bmark.GetCpuTime("benchmark"))
print "Real Time:  {0:.1f} s\n".format(bmark.GetRealTime("benchmark"))
print "\n"
