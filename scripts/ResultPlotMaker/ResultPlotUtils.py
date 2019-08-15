import os,sys
sys.path.append("../CardMaker")
from Datacard import *
import ROOT
import math
import cPickle as pickle
import numpy as np

lumi = 35.9
datacards = {}
datacards2 = {}
postfit_file = None

def LoadPickledDatacards(pickle_name, second_set=False):
    global datacards, datacards2
    if not second_set:
        datacards = pickle.load(open(pickle_name, 'rb'))
    else:
        datacards2 = pickle.load(open(pickle_name, 'rb'))

def LoadPostfitFile(fname):
    global postfit_file
    postfit_file = ROOT.TFile(fname)


def GetPull(pred, syst, obs, N=100000):
    relsyst = 1.0 + syst/pred
    s, t = 0, 0
    for i in range(N):
        t += 1
        toyobs = np.random.poisson(pred*(relsyst**np.random.randn()))
        if toyobs < obs:
            s += 1
        if obs < pred and toyobs == obs:
            s += 1
    return ROOT.Math.normal_quantile(float(s)/t, 1)

def GetPullPlot(g_data, g_pred, g_ratio):
    for ibin in range(g_data.GetN()):
        x, ydata, ypred = ROOT.Double(), ROOT.Double(), ROOT.Double()
        g_data.GetPoint(ibin, x, ydata)
        g_pred.GetPoint(ibin, x, ypred)
        if ydata==0.0 and ypred==0.0:
            continue
        if ypred < 200 and ydata > 0.0:
            if ydata > ypred:
                pull = GetPull(ypred, g_pred.GetErrorYhigh(ibin), ydata)
            else:
                pull = GetPull(ypred, g_pred.GetErrorYlow(ibin), ydata)
        else:
            if ydata > ypred:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYhigh(ibin)
            else:
                err_data = sqrt(ypred)
                err_pred = g_pred.GetErrorYlow(ibin)

            pull = (ydata - ypred) / math.sqrt(err_data**2 + err_pred**2)


        if abs(pull) > 2.5:
            print "BIN WITH BIG PULL: ", pull
        thisPoint = g_ratio.GetN()
        g_ratio.SetPoint(thisPoint, x, pull)
        g_ratio.SetPointError(thisPoint, 0.0, 0.0, 0.0, 0.0)


## DEPRECATED, ONLY USE IF USING OLD TEXT CARDS
## returns a list of yields, where the first is the data observation
## and the rest are background yields from the processes specified
## in bkg_names
def GetYieldsFromDatacard(datacard_fname, bkg_names, doSignal=False):
    lines = open(datacard_fname).readlines()
    
    names = []
    rates = []
    obs = 0
    for line in lines:
        if line.startswith("observation"):
            obs = int(float(line.split()[1]))
        if line.startswith("process") and len(names)==0:
            names = line.split()[2:]
        if line.startswith("rate"):
            for x in line.split()[2:]:
                if x=="nan":
                    rates.append(0.0)
                else:
                    rates.append(float(x))

    bkg_rates = []
    for name in bkg_names:
        try:
            ind = names.index(name)
        except:
            raise RuntimeError("provided list of backgrounds does not match those in datacard!")
        bkg_rates.append(rates[ind])

    sigyield = []
    if doSignal:
        try:
            ind = names.index("sig")
        except:
            raise RuntimeError("can't find signal yield in datacard!")
        sigyield = [rates[ind]]

    return [obs]+bkg_rates+sigyield

## DEPRECATED, ONLY USE IF USING OLD TEXT CARDS
## returns a list of 2-tuples, one for each background.
## numbers are the upper and lower relative uncertainties
## for each background process
def GetUncertaintiesFromDatacard(datacard_fname, bkg_names, verbose=False):
    lines = open(datacard_fname).readlines()

    tot_err_up = [0 for i in bkg_names]
    tot_err_down = [0 for i in bkg_names]
    pastRates = 0
    for line in lines:
        if line.startswith("rate"):
            pastRates=True
        if not pastRates:
            continue

        procInd = -1
        for i in range(len(bkg_names)):
            if line.startswith(bkg_names[i]):
                procInd = i
                break

        if procInd == -1:
            continue

        if verbose:
            print "Found {0} syst: {1}".format(bkg_names[procInd], line.split()[0])

        # given a list of strings, return the first that is a valid number (expecting only 1)
        def getNum(lst):
            x = None
            for s in lst:
                try:
                    xt = float(s.split("/")[0])
                    x = xt
                except:
                    continue
                if x != None:
                    return x
            raise RuntimeError("Could not find valid number in datacard: "+str(lst))

        sp = line.split()


        ## COMMENT THIS OUT LATER. Have to figure out why this one is broken
        if sp[0]=="llep_renorm":
            continue
        

        errType = sp[1]
        if verbose: print "    errType =",errType
        err_up, err_down = 0.0, 0.0
        if errType == "lnN":
            unc = getNum(sp[2:])
            err_up   = abs(1-unc)
            err_down = abs(1-unc)
        if errType == "gmN":
            N = int(sp[2])
            alpha = 1 - 0.6827
            xfact = getNum(sp[3:])
            err_up   = 0.0 if N==0 else (ROOT.Math.gamma_quantile_c(alpha/2, N+1, 1) / N - 1)
            err_down = 0.0 if N==0 else (1 - ROOT.Math.gamma_quantile(alpha/2, N, 1) / N)
        if not math.isnan(err_up):
            tot_err_up[procInd] += err_up**2
        if not math.isnan(err_down):
            tot_err_down[procInd] += err_down**2
        if verbose:
            print "    err_up   =", err_up
            print "    err_down =", err_down
            print "    tot_err_up   =", tot_err_up[procInd]
            print "    tot_err_down =", tot_err_down[procInd]

    if verbose: print tot_err_up, tot_err_down
    tot_err_up = [math.sqrt(x) for x in tot_err_up]
    tot_err_down = [math.sqrt(x) for x in tot_err_down]
    if verbose: print tot_err_up, tot_err_down
    return [[tot_err_up[i],tot_err_down[i]] for i in range(len(tot_err_up))]



def GetJBJregions(ht_reg):

    if ht_reg in ["HT250to450", "HT450to575"]:
        return ["j2to3_b0","j2to3_b1","j2to3_b2","j4to6_b0","j4to6_b1","j4to6_b2","j7toInf_b0","j7toInf_b1","j7toInf_b2","j2to6_b3toInf","j7toInf_b3toInf"]
    elif ht_reg != "monojet":
        return ["j2to3_b0","j2to3_b1","j2to3_b2","j4to6_b0","j4to6_b1","j4to6_b2","j2to6_b3toInf","j7to9_b0","j7to9_b1","j7to9_b2","j7to9_b3","j7to9_b4toInf","j10toInf_b0","j10toInf_b1","j10toInf_b2","j10toInf_b3","j10toInf_b4toInf"]
    else:
        return ["j1_b0","j1_b1toInf"]

def GetMT2bins(ht_reg, jbj_regs=None):
    mt2bins = []
    if jbj_regs is None:
        jbj_regs = GetJBJregions(ht_reg)
    if type(jbj_regs)==type(""):
        jbj_regs = [jbj_regs]

    full_jbj_regs = GetJBJregions(ht_reg)

    for jbj_reg in jbj_regs:
        i = full_jbj_regs.index(jbj_reg)
        bins = []
        if ht_reg == "HT250to450":
            if 0 <= i <= 5:            bins = [200,300,400]
            if i==6:                   bins = [200,300,500]
            if 7 <= i <= 8:            bins = [200, 300]
            if i==9:                   bins = [200,300,400]
            if i==10:                  bins = [200,300]            
        if ht_reg == "HT450to575":
            if 0 <= i <= 5:            bins = [200,300,400,500]
            if 6 <= i <= 8:            bins = [200,300,400]
            if i==9:                   bins = [200,300,400,500]
            if i==10:                  bins = [200,300,400]
        if ht_reg == "HT575to1200":
            if i==0:                   bins = [200,300,400,500,600,700,800,900,1000,1100]
            if i==1:                   bins = [200,300,400,600,800,1000]
            if i==2:                   bins = [200,300,400,600,800]
            if i==3:                   bins = [200,300,400,500,600,700,800,900,1000,1100]
            if i==4:                   bins = [200,300,400,600,800,1000]
            if 5 <= i <= 9:            bins = [200,300,400,600,800]
            if i==10:                  bins = [200,300,400,600]
            if i==11:                  bins = [200,300,400]
            if 12 <= i <= 14:          bins = [200,300,500]
            if i==15:                  bins = [200,300]
            if i==16:                  bins = [200]
        if ht_reg == "HT1200to1500":
            if 0 <= i <= 1:            bins = [200,400,600,800,1000,1200]
            if 2 <= i <= 2:            bins = [200,400,600,800,1000]
            if 3 <= i <= 4:            bins = [200,400,600,800,1000,1200]
            if 5 <= i <= 5:            bins = [200,400,600,800,1000]
            if 6 <= i <= 6:            bins = [200,400,600]
            if 7 <= i <= 8:            bins = [200,400,600,800,1000]
            if 9 <= i <= 9:            bins = [200,400,600,800]
            if 10 <= i <= 10:          bins = [200,400,600]
            if 11 <= i <= 11:          bins = [200,400]
            if 12 <= i <= 14:          bins = [200,400,600]
            if 15 <= i <= 15:          bins = [200,400]
            if 16 <= i <= 16:          bins = [200]
        if ht_reg == "HT1500toInf":
            if i==0:                   bins = [400,600,800,1000,1200,1400,1800]
            if i==1:                   bins = [400,600,800,1000,1200]
            if i==2:                   bins = [400]
            if i==3:                   bins = [400,600,800,1000,1200,1400,1600]
            if i==4:                   bins = [400,600,800,1000,1400]
            if i==5:                   bins = [400,600,800]
            if i==6:                   bins = [400,600]
            if i==7:                   bins = [400,600,800,1000,1400]
            if 8 <= i <= 9:            bins = [400,600,800]
            if i==10:                  bins = [400,800]
            if i==11:                  bins = [400]
            if 12 <= i <= 13:          bins = [400,800]
            if 14 <= i <= 16:          bins = [400]
        # for monojet, mt2bins are actually ht bins
        if ht_reg == "monojet":
            if i==0: bins = [250,350,450,575,700,1000,1200]
            if i==1: bins = [250,350,450,575,700]

        if bins==[]:
            raise RuntimeError("Couldn't get MT2 binning for region {0} {1}".format(ht_reg,jbj_reg))
        
        bins.append(-1)
        mt2bins.append(bins)

    return mt2bins

def GetMacroregions(macro_reg):

    if macro_reg == 1:
        return ["HT250to450_j2to3_b0",
                "HT450to575_j2to3_b0",
                "HT575to1000_j2to3_b0"]
    if macro_reg == 2:
        return ["HT250to450_j2to3_b1",
                "HT450to575_j2to3_b1",
                "HT575to1000_j2to3_b1",
                "HT250to450_j2to3_b2",
                "HT450to575_j2to3_b2",
                "HT575to1000_j2to3_b2"]
    if macro_reg == 3:
        return ["HT250to450_j4toInf_b0",
                "HT450to575_j4to6_b0",
                "HT575to1000_j4to6_b0",
                "HT450to575_j7toInf_b0",
                "HT575to1000_j7toInf_b0"]
    if macro_reg == 4:
        return ["HT250to450_j4toInf_b1",
                "HT450to575_j4to6_b1",
                "HT575to1000_j4to6_b1",
                "HT250to450_j4toInf_b2",
                "HT450to575_j4to6_b2",
                "HT575to1000_j4to6_b2",
                "HT450to575_j7toInf_b1",
                "HT575to1000_j7toInf_b1",
                "HT450to575_j7toInf_b2",
                "HT575to1000_j7toInf_b2"]

    if macro_reg == 5:
        return ["HT1000to1500_j2to3_b0",
                "HT1000to1500_j4to6_b0",
                "HT1000to1500_j7toInf_b0"]
    if macro_reg == 6:
        return ["HT1000to1500_j2to3_b1",
                "HT1000to1500_j4to6_b1",
                "HT1000to1500_j7toInf_b1",
                "HT1000to1500_j2to3_b2",
                "HT1000to1500_j4to6_b2",
                "HT1000to1500_j7toInf_b2"]
    if macro_reg == 7:
        return ["HT1500toInf_j2to3_b0",
                "HT1500toInf_j4to6_b0",
                "HT1500toInf_j7toInf_b0"]
    if macro_reg == 8:
        return ["HT1500toInf_j2to3_b1",
                "HT1500toInf_j4to6_b1",
                "HT1500toInf_j7toInf_b1",
                "HT1500toInf_j2to3_b2",
                "HT1500toInf_j4to6_b2",
                "HT1500toInf_j7toInf_b2"]



def GetMacroregionTitle(macro_reg):
    if macro_reg == 1: return "250 < H_{T} < 1000 GeV, 2-3j, 0b"
    if macro_reg == 2: return "250 < H_{T} < 1000 GeV, 2-3j, 1-2b"
    if macro_reg == 3: return "250 < H_{T} < 1000 GeV, #geq4j, 0b"
    if macro_reg == 4: return "250 < H_{T} < 1000 GeV, #geq4j, 1-2b"
    if macro_reg == 5: return "1000 < H_{T} < 1500 GeV, #geq2j, 0b"
    if macro_reg == 6: return "1000 < H_{T} < 1500 GeV, #geq2j, 1-2b"
    if macro_reg == 7: return "H_{T} > 1500 GeV, #geq2j, 0b"
    if macro_reg == 8: return "H_{T} > 1500 GeV, #geq2j, 1-2b"



def GetMT2binsFull(topo_reg):

    #return ["j2to3_b0","j2to3_b1","j2to3_b2","j4to6_b0","j4to6_b1","j4to6_b2","j7toInf_b0","j7toInf_b1","j7toInf_b2","j2to6_b3toInf","j7toInf_b3toInf"]

    bins = []
    if "HT250to450" in topo_reg :         bins = [200,300,400]
    if "HT450to575" in topo_reg :         
        if "j7toInf" in topo_reg:         bins = [200,300,400]
        else:                             bins = [200,300,400,500]
    if "HT575to1000" in topo_reg :        
        if "j2to3" in topo_reg:           bins = [200,300,400,600,800]
        elif "j4to6" in topo_reg:         bins = [200,300,400,600,800]
        elif "j7toInf_b0" in topo_reg:    bins = [200,300,400,600,800]
        else:                             bins = [200,300,400,600]
    if "HT1000to1500" in topo_reg :       
        if "b3" in topo_reg:              bins = [200,400,600]
        elif "j7toInf_b0" in topo_reg:    bins = [200,400,600,800,1000]
        elif "j7toInf_b1" in topo_reg:    bins = [200,400,600,800]
        elif "j7toInf_b2" in topo_reg:    bins = [200,400,600,800]
        elif "b2" in topo_reg:            bins = [200,400,600,800,1000]
        else:                             bins = [200,400,600,800,1000,1200]
    if "HT1500toInf" in topo_reg :       
        if "j2to3_b0" in topo_reg:        bins = [400,600,800,1000,1400]
        elif "j2to3_b1" in topo_reg:      bins = [400,600,800,1000]
        elif "j2to3_b2" in topo_reg:      bins = [400]
        elif "j4to6_b0" in topo_reg:      bins = [400,600,800,1000,1400]
        elif "j4to6_b1" in topo_reg:      bins = [400,600,800,1000,1400]
        elif "j4to6_b2" in topo_reg:      bins = [400,600,800]
        elif "j7toInf_b0" in topo_reg:    bins = [400,600,800,1000]
        elif "j7toInf_b1" in topo_reg:    bins = [400,600,800]
        elif "j7toInf_b2" in topo_reg:    bins = [400,600,800]
        elif "j7toInf_b3" in topo_reg:    bins = [400,600,800]
        else:                             bins = [400]

    if bins==[]:
        raise RuntimeError("Couldn't get MT2 binning for region {0}".format(topo_reg))
        
    bins.append(1800)
    return bins


def GetInclusiveRegions(doMonojet=True, ht_regs=None):
    regions = []

    # handle monojet
    if doMonojet:
        jbj_mj = GetJBJregions("monojet")
        mt2bins_mj = GetMT2bins("monojet")
        for jbj, mt2bins in zip(jbj_mj, mt2bins_mj):
            for i in range(len(mt2bins)-1):
                binname = "HT{0}to{1}_{2}".format(mt2bins[i], mt2bins[i+1], jbj)
                binname = binname.replace("-1","Inf")
                regions.append(binname)

    if ht_regs is None:
        ht_regs = ["HT250to450", "HT450to575", "HT575to1200", "HT1200to1500", "HT1500toInf"]

    for ht_reg in ht_regs:
        jbj_regs = GetJBJregions(ht_reg)
        for jbj_reg in jbj_regs:
            regname = "{0}_{1}".format(ht_reg, jbj_reg)
            regions.append(regname)

    return regions

def GetInclusiveDatacards(datacard_dir, datacard_name, regions, fullPath=True):
    datacards = []
    for reg in regions:
        ht_reg = reg.split("_")[0]
        jbj_reg = reg[reg.find("_")+1:]
        if "j1_" in reg:
            if fullPath:
                dname = os.path.join(datacard_dir, datacard_name).format(ht_reg, jbj_reg, "m0toInf")
            else:
                dname = "{0}_{1}_m0toInf".format(ht_reg, jbj_reg)
            datacards.append([dname])
        else:
            mt2bins = GetMT2bins(ht_reg, jbj_reg)[0]
            dnames = []
            for i in range(len(mt2bins)-1):
                mt2bin = "m{0}to{1}".format(mt2bins[i], mt2bins[i+1]).replace("-1","Inf")
                if fullPath:
                    dname = os.path.join(datacard_dir, datacard_name).format(ht_reg, jbj_reg, mt2bin)
                else:
                    dname = "{0}_{1}_{2}".format(ht_reg, jbj_reg, mt2bin)
                dnames.append(dname)
            datacards.append(dnames)

    return datacards


def GetHTtitle(ht_reg):
    if ht_reg == "HT250to450": return "250 < H_{T} < 450 GeV"
    if ht_reg == "HT450to575": return "450 < H_{T} < 575 GeV"
    if ht_reg == "HT575to1200": return "575 < H_{T} < 1200 GeV"
    if ht_reg == "HT1200to1500": return "1200 < H_{T} < 1500 GeV"
    if ht_reg == "HT1500toInf": return "H_{T} > 1500 GeV"
    if ht_reg == "monojet": return "Monojet Region"

    return ht_reg

def GetJBJtitle(jbj_reg):
    lines = ["",""]
    jreg = jbj_reg.split("_")[0]
    if jreg=="j2to3":   lines[0] = "2-3j"
    elif jreg=="j4to6": lines[0] = "4-6j"
    elif jreg=="j2to6": lines[0] = "2-6j"
    elif jreg=="j7toInf": lines[0] = "#geq7j"
    elif jreg=="j7to9": lines[0] = "7-9j"
    elif jreg=="j10toInf": lines[0] = "#geq10j"
    elif jreg=="j1": lines[0] = "1j"
    else: lines[0] = jreg

    bjreg = jbj_reg.split("_")[1]
    if bjreg=="b0": lines[1] = "0b"
    elif bjreg=="b1": lines[1] = "1b"
    elif bjreg=="b2": lines[1] = "2b"
    elif bjreg=="b3": lines[1] = "3b"
    elif bjreg=="b3toInf": lines[1] = "#geq3b"
    elif bjreg=="b4toInf": lines[1] = "#geq4b"
    elif bjreg=="b1toInf": lines[1] = "#geq1b"
    else: lines[1] = bjreg

    return lines

def GetMT2name(left, right):
    s = "m{0}to{1}".format(left,right)
    s = s.replace("-1","Inf")
    return s

def GetMT2label(left, right):
    if right != -1:
        return "[{0}, {1}]".format(left,right)
    else:
        return "> {0}".format(left)

def GetInclusiveBinLabel(reg):
    if "j1_" in reg:
        ht_reg = reg.split("_")[0]
        pos = ht_reg.find("t")
        low = ht_reg[2:pos]
        high = ht_reg[pos+2:]
        if high=="Inf":
            return "> {0}".format(low)
        else:
            return "[{0},{1}]".format(low,high)
    else:
        jbj_reg = reg[reg.find("_")+1:]
        jbj_title = GetJBJtitle(jbj_reg)
        return "{0}, {1}".format(jbj_title[0], jbj_title[1])

def GetLegendName(proc):
    if proc=="zinv": return "Z#rightarrow#nu#bar{#nu}"
    if proc=="llep": return "Lost lepton"
    if proc=="qcd": return "Multijet"
    
    return proc

#__________________________________________________
# fills nuisance dictionary for lnN uncertainties
def fillNuisanceDictLNN( dict_nuis_up, dict_nuis_dn, nuis_name, nuis_val_string, val_cent ):
    abserr_up = 0.
    abserr_dn = 0.
    
    # read nuisance values
    # first case: asymmetric, separated by /
    if nuis_val_string.count('/'):
        nuis_asym = nuis_val_string.split('/')
        nuis_val_up = float(nuis_asym[0])
        abserr_up = (nuis_val_up - 1.) * val_cent
        nuis_val_dn = float(nuis_asym[1])
        abserr_dn = (1. - nuis_val_dn) * val_cent
    # default case: single number
    else:
        nuis_val = float(nuis_val_string)
        abserr_up = (nuis_val - 1.) * val_cent
        abserr_dn = abserr_up

    if math.isnan(abserr_up) or math.isnan(abserr_dn): return
        
    # fill nuisance dict
    if nuis_name in dict_nuis_up:
        dict_nuis_up[nuis_name] += abserr_up
        dict_nuis_dn[nuis_name] += abserr_dn
    else:
        dict_nuis_up[nuis_name] = abserr_up
        dict_nuis_dn[nuis_name] = abserr_dn
    return

#__________________________________________________
# fills nuisance dictionary for gmN uncertainties
def fillNuisanceDictGMN( dict_nuis_up, dict_nuis_dn, nuis_name, cr_yield, alpha ):
    # get poisson uncertainties on yield
    cr_yield_up = ROOT.Double()
    cr_yield_dn = ROOT.Double()
    ROOT.RooHistError.instance().getPoissonInterval(cr_yield,cr_yield_dn,cr_yield_up,1.)
    abserr_up = (cr_yield_up - cr_yield)*alpha
    abserr_dn = (cr_yield - cr_yield_dn)*alpha
    
    if math.isnan(abserr_up) or math.isnan(abserr_dn): return
        
    # fill nuisance dict
    if nuis_name in dict_nuis_up:
        dict_nuis_up[nuis_name] += abserr_up
        dict_nuis_dn[nuis_name] += abserr_dn
    else:
        dict_nuis_up[nuis_name] = abserr_up
        dict_nuis_dn[nuis_name] = abserr_dn
    return

#__________________________________________________
# reads in a list of input datacard files and prints integrated yields with uncertainties
def getMacroRegionUncertainties( region, datacard_list, doSignal=False ):
    
    # for each background:
    # - sum up central values
    # - make up/dn dicts of {nuisance : absunc}.  For correlated uncertainties, just add on to the existing total
    #   - for gmN uncertainties: evaluate poisson error based on CR count
    # - after running on all cards, sum up all absunc values from dict.  That gives single uncertainty number

    n_obs = 0
    n_zinv = 0.
    n_llep = 0.
    n_qcd = 0.
    n_sig = 0.

    dict_zinv_nuisances_up = {}
    dict_zinv_nuisances_dn = {}
    dict_llep_nuisances_up = {}
    dict_llep_nuisances_dn = {}
    dict_qcd_nuisances_up = {}
    dict_qcd_nuisances_dn = {}
    dict_sig_nuisances_up = {}
    dict_sig_nuisances_dn = {}

    for datacard_name in datacard_list:
        #print '  checking card:',datacard_name
        lines = []
        try:
            f = open(datacard_name,'r')
            lines = f.readlines()
            f.close()
        except IOError:
            print 'Could not open file, skipping:',datacard_name
            continue

        val_zinv = 0.
        val_llep = 0.
        val_qcd = 0.
        val_sig = 0.

        passed_rate = False
        for i, line in enumerate(lines):
            line = line.strip()
            # observation line
            # 'observation 14'
            if line.startswith('observation'):
                n_obs += int(float(line.split()[1]))

            # prediction values line
            # 'rate            0.034    7.232      2.789      0.037'
            elif line.startswith('rate'):
                pred_vals_tokens = line.split()
                val_zinv = float(pred_vals_tokens[2])
                val_llep = float(pred_vals_tokens[3])
                val_qcd = float(pred_vals_tokens[4])
                if (doSignal):
                    val_sig = float(pred_vals_tokens[1])

                n_zinv += val_zinv
                n_llep += val_llep
                n_qcd += val_qcd
                n_sig += val_sig
                #print '    bkgs: %.3f  %.3f  %.3f'%(val_zinv, val_llep, val_qcd)

                passed_rate = True

            # lines for nuisances
            # 'zinv_ZGratio_nj_j2to3      lnN    -   1.028    -    -'
            elif passed_rate and len(line.split()) >= 6:
                nuis_tokens = line.split()
                if len(nuis_tokens) < 3:
                    continue
                nuis_name = nuis_tokens[0]
                nuis_type = nuis_tokens[1]

                ## COMMENT THIS OUT LATER. Have to figure out why this one is broken
                if nuis_name=="llep_renorm":
                    continue
                
                if nuis_type == 'lnN':
                    nuis_sig = nuis_tokens[2].strip()
                    nuis_zinv = nuis_tokens[3].strip()
                    nuis_llep = nuis_tokens[4].strip()
                    nuis_qcd = nuis_tokens[5].strip()
                    if nuis_zinv != "-":
                        fillNuisanceDictLNN( dict_zinv_nuisances_up, dict_zinv_nuisances_dn, nuis_name, nuis_zinv, val_zinv )
                    elif nuis_llep != "-":                        
                        fillNuisanceDictLNN( dict_llep_nuisances_up, dict_llep_nuisances_dn, nuis_name, nuis_llep, val_llep )
                    elif nuis_qcd != "-":
                        fillNuisanceDictLNN( dict_qcd_nuisances_up, dict_qcd_nuisances_dn, nuis_name, nuis_qcd, val_qcd )
                    elif nuis_sig != "-":
                        fillNuisanceDictLNN( dict_sig_nuisances_up, dict_sig_nuisances_dn, nuis_name, nuis_sig, val_sig )
                # gamma function: get poisson uncertainty based on CR count
                # 'zinv_CRstat_HT1000to1500_j2to3_b0     gmN 105    -  0.06888   -   -'
                elif nuis_type == 'gmN':
                    cr_yield = int(nuis_tokens[2])
                    alpha_zinv = nuis_tokens[4].strip()
                    alpha_llep = nuis_tokens[5].strip()
                    alpha_qcd  = nuis_tokens[6].strip()
                    if alpha_zinv != "-":
                        alpha_zinv = float(alpha_zinv)
                        fillNuisanceDictGMN( dict_zinv_nuisances_up, dict_zinv_nuisances_dn, nuis_name, cr_yield, alpha_zinv )
                    if alpha_llep != "-":
                        alpha_llep = float(alpha_llep)
                        fillNuisanceDictGMN( dict_llep_nuisances_up, dict_llep_nuisances_dn, nuis_name, cr_yield, alpha_llep )
                    if alpha_qcd != "-":
                        alpha_qcd = float(alpha_qcd)
                        fillNuisanceDictGMN( dict_qcd_nuisances_up, dict_qcd_nuisances_dn, nuis_name, cr_yield, alpha_qcd )


    # done with loop on datacards: sum up nuisances    
    abserr_sum_zinv_up = 0.
    abserr_sum_zinv_dn = 0.
    abserr_sum_llep_up = 0.
    abserr_sum_llep_dn = 0.
    abserr_sum_qcd_up = 0.
    abserr_sum_qcd_dn = 0.
    abserr_sum_sig_up = 0.
    abserr_sum_sig_dn = 0.

    for nuis_abserr in dict_zinv_nuisances_up.itervalues():
        abserr_sum_zinv_up += nuis_abserr*nuis_abserr
    abserr_sum_zinv_up = math.sqrt(abserr_sum_zinv_up)

    for nuis_abserr in dict_zinv_nuisances_dn.itervalues():
        abserr_sum_zinv_dn += nuis_abserr*nuis_abserr
    abserr_sum_zinv_dn = math.sqrt(abserr_sum_zinv_dn)

    for nuis_abserr in dict_llep_nuisances_up.itervalues():
        abserr_sum_llep_up += nuis_abserr*nuis_abserr
    abserr_sum_llep_up = math.sqrt(abserr_sum_llep_up)

    for nuis_abserr in dict_llep_nuisances_dn.itervalues():
        abserr_sum_llep_dn += nuis_abserr*nuis_abserr
    abserr_sum_llep_dn = math.sqrt(abserr_sum_llep_dn)

    for nuis_abserr in dict_qcd_nuisances_up.itervalues():
        abserr_sum_qcd_up += nuis_abserr*nuis_abserr
    abserr_sum_qcd_up = math.sqrt(abserr_sum_qcd_up)

    for nuis_abserr in dict_qcd_nuisances_dn.itervalues():
        abserr_sum_qcd_dn += nuis_abserr*nuis_abserr
    abserr_sum_qcd_dn = math.sqrt(abserr_sum_qcd_dn)

    for nuis_abserr in dict_sig_nuisances_up.itervalues():
        abserr_sum_sig_up += nuis_abserr*nuis_abserr
    abserr_sum_sig_up = math.sqrt(abserr_sum_sig_up)

    for nuis_abserr in dict_sig_nuisances_dn.itervalues():
        abserr_sum_sig_dn += nuis_abserr*nuis_abserr
    abserr_sum_sig_dn = math.sqrt(abserr_sum_sig_dn)

    # if region=="HT1500toInf_j10toInf_b0":
    #     # for nuis in sorted(dict_zinv_nuisances_up.keys(), key=lambda x:dict_zinv_nuisances_up[x], reverse=True):
    #     #     print nuis, dict_zinv_nuisances_up[nuis]
    #     print "Systematics for", region
    #     print abserr_sum_zinv_up
    #     print abserr_sum_llep_up
    #     print abserr_sum_qcd_up
    #     print abserr_sum_zinv_dn
    #     print abserr_sum_llep_dn
    #     print abserr_sum_qcd_dn

    n_bkg = n_zinv + n_llep + n_qcd
    abserr_sum_bkg_up = math.sqrt(abserr_sum_zinv_up*abserr_sum_zinv_up + abserr_sum_llep_up*abserr_sum_llep_up + abserr_sum_qcd_up*abserr_sum_qcd_up)
    abserr_sum_bkg_dn = math.sqrt(abserr_sum_zinv_dn*abserr_sum_zinv_dn + abserr_sum_llep_dn*abserr_sum_llep_dn + abserr_sum_qcd_dn*abserr_sum_qcd_dn)

    if not doSignal:
        return max(abserr_sum_bkg_up, abserr_sum_bkg_dn)
    else:
        return max(abserr_sum_bkg_up, abserr_sum_bkg_dn), max(abserr_sum_sig_up, abserr_sum_sig_dn)

