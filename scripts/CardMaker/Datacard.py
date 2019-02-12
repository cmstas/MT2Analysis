import ROOT
from math import log10, isinf, isnan, sqrt

class Datacard:
    def __init__(self, name, nbkg, bkg_names, nchan=1, chan_names=None, years=None, 
                 split_bkg_by_year=False, split_sig_by_year=False):
        self.name = name
        self.nbkg = nbkg
        self.bkg_names = bkg_names

        self.nchan = nchan
        if nchan != 1:
            raise Excpetion("ERROR: datacards with >1 channel not supported yet!")

        # make it a list
        if type(chan_names)==type(""):
            chan_names = [chan_names]

        
        # make some default names if none are provided
        if chan_names is None:
            if nchan==1:
                chan_names = [name]
            else:
                chan_names = ["bin{0}".format(i) for i in range(1, nchan+1)]

        self.chan_names = chan_names

        if years is not None and type(years) != type([]):
            raise Exception("ERROR: 'years' must be a list of year names")
        self.years = years

        self.split_bkg_by_year = split_bkg_by_year
        if split_bkg_by_year:
            self.n_split_bkg = self.nbkg * len(self.years)
            self.split_bkg_names = []
            for bkg in bkg_names:
                for year in years:
                    self.split_bkg_names.append(bkg+str(year))
        else:
            self.n_split_bkg = self.nbkg
            self.split_bkg_names = self.bkg_names

        self.split_sig_by_year = split_sig_by_year
        if split_sig_by_year:
            self.sig_rate = {}
            self.nsig = len(years)
            for y in years:
                self.sig_rate[y] = 0.0
        else:
            self.nsig = 1
            self.sig_rate = 0.0

        self.obs = 0
        self.bkg_rates = {}
        for bkg in self.split_bkg_names:
            self.bkg_rates[bkg] = 0.0

        self.nuisances = {}

        # put anything you want here, if it will be useful later. e.g. lostlep alpha and lostlep lastbin hybrid
        self.info = {}

    def SetName(self, name):
        self.name = name

    def GetName(self):
        return self.name

    def GetObservation(self):
        return self.obs

    def GetBackgroundRates(self, splitByYear=False):
        yields = {}
        if splitByYear:
            for bkg in self.split_bkg_names:
                yields[bkg] = self.bkg_rates[bkg]
        else:
            for bkg in self.bkg_names:
                yields[bkg] = 0.0
            for bkgsplit in self.split_bkg_names:
                for bkg in self.bkg_names:
                    if bkgsplit.startswith(bkg):
                        yields[bkg] += self.bkg_rates[bkgsplit]

        return yields

    def GetSignalRate(self, splitByYear=False):
        if splitByYear and not self.split_sig_by_year:
            raise Exception("ERROR: this datacard doesn't have signal split by year!")
        if splitByYear:
            return self.sig_rate
        else:
            if self.split_sig_by_year:
                return sum(self.sig_rate.values())
            else:
                return self.sig_rate

    def GetNuisanceNames(self):
        return self.nuisances.keys()

    def GetNuisanceDict(self):
        return self.nuisances

    def SetObservation(self, obs):
        self.obs = obs

    def SetSignalRate(self, rate, year=None):
        if year is None and self.split_sig_by_year:
            raise Exception("ERROR: you're splitting signal by year, must specify year!")
        if self.split_sig_by_year:
            self.sig_rate[year] = rate
        else:
            self.sig_rate = rate

    def SetBkgRate(self, rate, bkg_name, year=None):
        if year is None and self.years is not None:
            raise Exception("ERROR: must specify which year to set!")
        if year not in self.years:
            raise Exception("ERROR: specified year is not in the list of years!")

        fullname = bkg_name + ("" if year is None else str(year))
        if fullname not in self.split_bkg_names:
            raise Exception("ERROR: background {0} not in list of backgrounds!".format(fullname))
        
        self.bkg_rates[fullname] = rate

    def AddNuisance(self, name, nuisType, split_by_year=False):
        if split_by_year and not self.split_bkg_by_year:
            raise Exception("ERROR: cannot split nuisance by year if backgrounds aren't split by year!")

        if split_by_year:
            for year in self.years:
                fullname = name+"_"+str(year)
                nuis = Nuisance(fullname, nuisType, self.nsig, self.n_split_bkg)
                self.nuisances[fullname] = nuis
        else:
            nuis = Nuisance(name, nuisType, self.nsig, self.n_split_bkg)
            self.nuisances[name] = nuis

    def GetFullNuisName(self, nuisname, year=None):
        if nuisname in self.nuisances:
            return nuisname
        elif year is not None and nuisname+"_"+str(year) in self.nuisances:
            return nuisname+"_"+str(year)
        else:
            print self.nuisances.keys()
            raise Exception("ERROR: could not find nuisance {0}, either combined or for year {1}".format(nuisname, year))

    def SetGMNNuisanceN(self, nuisname, N, year=None):
        fullname = self.GetFullNuisName(nuisname, year)

        if self.nuisances[fullname].type != "gmN":
            raise Exception("ERROR: nuisance {0} is not of type gmN!".format(fullname))
        if type(N) != type(1):
            raise Exception("ERROR: must provide an integer! You gave {0}".format(N))

        self.nuisances[fullname].N = N

    def SetNuisanceSignalValue(self, nuisname, value, year=None):
        if self.split_sig_by_year and year is None:
            raise Exception("ERROR: you're splitting signal by year, must specify year!")
        if isnan(value) or isinf(value):
            raise Exception("ERROR: nuisance value is nan or inf for nuis {0}, signal, year {2}".format(nuisname, bkg_name, year))

        fullname = self.GetFullNuisName(nuisname, year)

        if self.split_sig_by_year:
            idx = self.years.index(year)
            self.nuisances[fullname].sig_value[idx] = value
        else:
            self.nuisances[fullname].sig_value[0] = value

    def SetNuisanceBkgValue(self, nuisname, value, bkg_name, year=None):
        if self.split_bkg_by_year and year is None:
            raise Exception("ERROR: backgrounds are split by year, you must specify which year you're doing!")        
        if year not in self.years:
            raise Exception("ERROR: year {0} not in list of years!".format(year))
        if bkg_name not in self.bkg_names:
            raise Exception("ERROR: bkg {0} not in list of backgrounds!".format(bkg_name))

        fullname = self.GetFullNuisName(nuisname, year)
        fullbkg = bkg_name + (str(year) if year is not None else "")
        fullidx = self.split_bkg_names.index(fullbkg)

        if type(value)==tuple:
            if self.nuisances[fullname].type not in  ["lnN","lnU"]:
                raise Exception("ERROR: only lnN/lnU nuisances support 2-sided values!")
            if value[0]>2.0 or value[1]>2.0 or value[0]<0.3 or value[1]<0.3:
                print "WARNING: nuisance {0} has a large value {1} (year {2}). Card: {3}".format(fullname, value, year, self.name)
            if isnan(value[0]) or isinf(value[0]) or isnan(value[1]) or isinf(value[1]):
                raise Exception("ERROR: nuisance value is nan or inf for nuis {0}, background {1}, year {2}".format(nuisname, bkg_name, year))
        elif type(value)==float:
            if self.nuisances[fullname].type in ["lnN","lnU"] and (value > 2.0 or value < 0.3):
                print "WARNING: nuisance {0} has a large value {1} (year {2}). Card: {3}".format(fullname, value, year, self.name)
            if isnan(value) or isinf(value):
                raise Exception("ERROR: nuisance value is nan or inf for nuis {0}, background {1}, year {2}".format(nuisname, bkg_name, year))
        else:
            raise Exception("ERROR: value must be a float or tuple of 2 float (upper,lower) (lnN/lnU only)")

        self.nuisances[fullname].bkg_values[fullidx] = value


    def GetTotalUncertainty(self):
        alpha = 1-0.6827
        tot_err_up = 0.0
        tot_err_dn = 0.0
        for nuis in self.nuisances.values():
            err_up = 0.0
            err_dn = 0.0
            if nuis.type in ["lnN","lnU"]:
                for i,val in enumerate(nuis.bkg_values):
                    bkg = self.split_bkg_names[i]
                    if val is None:
                        continue
                    if type(val)==tuple:
                        err_up += (val[0]-1.0) * self.bkg_rates[bkg]
                        err_dn += (val[1]-1.0) * self.bkg_rates[bkg]
                    else:
                        err_up += (val-1.0) * self.bkg_rates[bkg]
                        err_dn += (val-1.0) * self.bkg_rates[bkg]
            if nuis.type == "gmN":                
                for i,val in enumerate(nuis.bkg_values):
                    bkg = self.split_bkg_names[i]
                    if val is None:
                        continue
                    err_up += val * (ROOT.Math.gamma_quantile_c(alpha/2, nuis.N+1, 1) - nuis.N)
                    err_dn += val * (0 if nuis.N==0 else nuis.N-ROOT.Math.gamma_quantile(alpha/2, nuis.N, 1))

            # print nuis.name, err_up, err_dn
            tot_err_up += err_up**2
            tot_err_dn += err_dn**2

        return (sqrt(tot_err_up), sqrt(tot_err_dn))

    def Write(self, outname, sortkey=str):
        fid = open(outname, 'w')

        maxNuisLength = 20
        for name in self.nuisances:
            maxNuisLength = max(maxNuisLength, len(name))

        colsizes = [maxNuisLength+2, 12] + [13]*(self.nsig + self.n_split_bkg)
        ml = lambda s,l: s+" "*max(1,(l-len(s)))

        fid.write("imax {0}  number of channels\n".format(self.nchan))
        fid.write("jmax *  number of channels\n".format(self.n_split_bkg + self.nsig))
        fid.write("kmax *\n")
        fid.write("------------\n")
        fid.write("bin         {0}\n".format(self.chan_names[0]))
        fid.write("observation {0}\n".format(self.obs))
        unc_up, unc_dn = self.GetTotalUncertainty()
        fid.write("# total prediction: {0:.3f} + {1:.3f} - {2:.3f}\n".format(sum(self.bkg_rates.values()), unc_up, unc_dn))
        fid.write("# total signal: {0:.3f}\n".format(self.sig_rate if not self.split_sig_by_year else sum(self.sig_rate.values())))
        fid.write("------------\n")
        fid.write(ml("bin", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.n_split_bkg + self.nsig):
            fid.write(ml(self.chan_names[0], colsizes[i+2]))
        fid.write('\n')
        fid.write(ml("process", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.nsig):
            signame = "sig"+str(self.years[i]) if self.split_sig_by_year else "sig"
            fid.write(ml(signame, colsizes[i+2]))
        for i in range(self.n_split_bkg):
            fid.write(ml(self.split_bkg_names[i], colsizes[i+2+self.nsig]))
        fid.write('\n')
        fid.write(ml("process", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.nsig + self.n_split_bkg):
            idx = 0 if i<self.nsig else i+1-self.nsig
            fid.write(ml(str(idx), colsizes[i+2]))
        fid.write('\n')
        fid.write(ml("rate", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.nsig):
            sig_rate = self.sig_rate[self.years[i]] if self.split_sig_by_year else self.sig_rate
            fid.write(ml("{0:.3f}".format(sig_rate), colsizes[i+2]))
        for i in range(self.n_split_bkg):
            rate = self.bkg_rates[self.split_bkg_names[i]]
            fid.write(ml("{0:.3f}".format(rate), colsizes[i+2+self.nsig]))
        fid.write('\n')
        fid.write("-"*sum(colsizes)+'\n')

        for nuis in sorted(self.nuisances.keys(), key=sortkey):
            fid.write(ml(nuis, colsizes[0]))
            tp = self.nuisances[nuis].type
            sigval = self.nuisances[nuis].sig_value
            bkgvals = self.nuisances[nuis].bkg_values

            if tp in ["lnN","lnU"]:
                fid.write(ml(tp, colsizes[1]))                
                for i in range(self.nsig):
                    if sigval[i] is None:
                        fid.write(ml("-", colsizes[i+2]))
                    else:
                        fid.write(ml("{0:.3f}".format(sigval[i]), colsizes[i+2]))
                for i in range(self.n_split_bkg):
                    if bkgvals[i] is None:
                        fid.write(ml("-", colsizes[i+2+self.nsig]))
                    elif type(bkgvals[i])==tuple:
                        fid.write(ml("{0:.3f}/{1:.3f}".format(*bkgvals[i]), colsizes[i+2+self.nsig]))
                    else:
                        fid.write(ml("{0:.3f}".format(bkgvals[i]), colsizes[i+2+self.nsig]))
            elif tp=="gmN":
                N = self.nuisances[nuis].N
                fid.write(ml(tp+" "+str(N), colsizes[1]))
                for i in range(self.nsig):
                    if sigval[i] is None:
                        fid.write(ml("-", colsizes[i+2]))
                    else:
                        nround = max(0, int(-log10(sigval))+4)
                        fid.write(ml("{{0:.{0}f}}".format(nround).format(sigval[i]), colsizes[i+2]))
                for i in range(self.n_split_bkg):
                    if bkgvals[i] is None:
                        fid.write(ml("-", colsizes[i+2+self.nsig]))
                    else:
                        nround = max(0, int(-log10(bkgvals[i]))+4)
                        fid.write(ml("{{0:.{0}f}}".format(nround).format(bkgvals[i]), colsizes[i+2+self.nsig]))
                        if self.bkg_rates[self.split_bkg_names[i]] > 0 and \
                                abs(bkgvals[i]*N / self.bkg_rates[self.split_bkg_names[i]] - 1) > 1e-5:
                            raise Exception("ERROR: gmN values and rate don't line up! Nuis: {0}, Bkg: {1}, alpha: {2}, N: {3}".format(nuis, self.split_bkg_names[i], bkgvals[i], N))
                        if self.bkg_rates[self.split_bkg_names[i]] == 0.0 and bkgvals[i]*N != 0.0:
                            raise Exception("ERROR: gmN values and rate don't line up! Nuis: {0}, Bkg: {1}".format(nuis, self.split_bkg_names[i]))

            fid.write("\n")

        fid.close()


class Nuisance:
    def __init__(self, name, nuisType, nsig, nbkg):
        self.name = name
        if nuisType not in ["gmN", "lnN", "lnU"]:
            raise Exception("ERROR: nuisance type {0} not supported. Only gmN, lnN, and lnU".format(nuisType))
        self.type = nuisType
        self.nbkg = nbkg
        self.nsig = nsig

        if self.type=="gmN":
            self.N = 0
            self.bkg_values = [None]*nbkg
            self.sig_value = [None]*nsig
        elif self.type in ["lnN", "lnU"]:
            self.N = None
            self.bkg_values = [None]*nbkg
            self.sig_value = [None]*nsig


