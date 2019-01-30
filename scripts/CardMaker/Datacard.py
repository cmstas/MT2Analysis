from math import log10

class Datacard:
    def __init__(self, name, nbkg, bkg_names, nchan=1, chan_names=None, years=None, split_bkg_by_year=False):
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

        self.obs = 0
        self.sig_rate = 0.0
        self.bkg_rates = {}
        for bkg in self.split_bkg_names:
            self.bkg_rates[bkg] = 0.0

        self.nuisances = {}
        

    def SetObservation(self, obs):
        self.obs = obs

    def SetSignalRate(self, rate):
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
                nuis = Nuisance(fullname, nuisType, self.n_split_bkg)
                self.nuisances[fullname] = nuis
        else:
            nuis = Nuisance(name, nuisType, self.n_split_bkg)
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
        fullname = self.GetFullNuisName(nuisname, year)
        self.nuisances[fullname].sig_value = value

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

        if self.nuisances[fullname].type=="lnN" and abs(1.0-value) > 0.5:
            print "WARNING: nuisance {0} has a large value {1} (year {2})".format(fullname, value, year)

        self.nuisances[fullname].bkg_values[fullidx] = value

    def Write(self, outname):
        fid = open(outname, 'w')

        maxNuisLength = 20
        for name in self.nuisances:
            maxNuisLength = max(maxNuisLength, len(name))

        colsizes = [maxNuisLength+2, 12] + [12]*(self.n_split_bkg + 1)
        ml = lambda s,l: s+" "*max(1,(l-len(s)))

        fid.write("imax {0}  number of channels\n".format(self.nchan))
        fid.write("jmax {0}  number of channels\n".format(self.n_split_bkg))
        fid.write("kmax *\n")
        fid.write("------------\n")
        fid.write("bin         {0}\n".format(self.chan_names[0]))
        fid.write("observation {0}\n".format(self.obs))
        fid.write("------------\n")
        fid.write(ml("bin", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.n_split_bkg + 1):
            fid.write(ml(self.chan_names[0], colsizes[i+2]))
        fid.write('\n')
        fid.write(ml("process", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        fid.write(ml("sig", colsizes[2]))
        for i in range(self.n_split_bkg):
            fid.write(ml(self.split_bkg_names[i], colsizes[i+3]))
        fid.write('\n')
        fid.write(ml("process", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        for i in range(self.n_split_bkg+1):
            fid.write(ml(str(i), colsizes[i+2]))
        fid.write('\n')
        fid.write(ml("rate", colsizes[0]))
        fid.write(ml("", colsizes[1]))
        fid.write(ml("{0:.3f}".format(self.sig_rate), colsizes[2]))
        for i in range(self.n_split_bkg):
            fid.write(ml("{0:.3f}".format(self.bkg_rates[self.split_bkg_names[i]]), colsizes[i+3]))
        fid.write('\n')
        fid.write("-"*sum(colsizes)+'\n')

        for nuis in sorted(self.nuisances.keys()):
            fid.write(ml(nuis, colsizes[0]))
            tp = self.nuisances[nuis].type
            sigval = self.nuisances[nuis].sig_value
            bkgvals = self.nuisances[nuis].bkg_values

            if tp=="lnN":
                fid.write(ml(tp, colsizes[1]))                
                if sigval is None:
                    fid.write(ml("-", colsizes[2]))
                else:
                    fid.write(ml("{0:.3f}".format(sigval), colsizes[2]))
                for i in range(self.n_split_bkg):
                    if bkgvals[i] is None:
                        fid.write(ml("-", colsizes[i+3]))
                    else:
                        fid.write(ml("{0:.3f}".format(bkgvals[i]), colsizes[i+3]))
            elif tp=="gmN":
                N = self.nuisances[nuis].N
                fid.write(ml(tp+" "+str(N), colsizes[1]))
                if sigval is None:
                    fid.write(ml("-", colsizes[2]))
                else:
                    nround = max(0, int(-log10(sigval))+4)
                    fid.write(ml("{{0:.{0}f}}".format(nround).format(sigval), colsizes[2]))
                for i in range(self.n_split_bkg):
                    if bkgvals[i] is None:
                        fid.write(ml("-", colsizes[i+3]))
                    else:
                        nround = max(0, int(-log10(bkgvals[i]))+4)
                        fid.write(ml("{{0:.{0}f}}".format(nround).format(bkgvals[i]), colsizes[i+3]))
                        if abs(bkgvals[i]*N / self.bkg_rates[self.split_bkg_names[i]] - 1) > 1e-5:
                            raise Exception("ERROR: gmN values and rate don't line up! Nuis: {0}, Bkg: {1}".format(nuis, self.split_bkg_names[i]))

            fid.write("\n")

        fid.close()


class Nuisance:
    def __init__(self, name, nuisType, nbkg, correlation=0):
        self.name = name
        if nuisType not in ["gmN", "lnN"]:
            raise Exception("ERROR: nuisance type {0} not supported. Only gmN and lnN".format(nuisType))
        self.type = nuisType
        self.nbkg = nbkg
        
        if self.type=="gmN":
            self.N = 0
            self.bkg_values = [None]*nbkg
            self.sig_value = None
        elif self.type=="lnN":
            self.N = None
            self.bkg_values = [None]*nbkg
            self.sig_value = None


