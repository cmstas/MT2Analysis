import Datacard
import cPickle as pickle
import matplotlib.pyplot as plt

NUIS = "sig_bTagHeavySyst"

# pkl = "cards_FullRunII_17MCfor18_ttbbWeights_forPlotting/T1tttt/T1tttt_1600_1100/signal_datacards.pkl"
pkl = "cards_FullRunII_17MCfor18_ttbbWeights_forPlotting/T2qq/T2qq_1300_850/signal_datacards.pkl"
dcs = pickle.load(open(pkl, 'rb'))

vals = []
for name,dc in dcs.items():
    nuis = dc.GetNuisanceDict()[NUIS]
    sig_rate = dc.GetSignalRate(True)
    tot_syst = sig_rate[16]*(nuis.sig_value[0]-1.0) + sig_rate[17]*(nuis.sig_value[1]-1.0) + sig_rate[18]*(nuis.sig_value[2]-1.0)
    if sum(sig_rate.values()) > 0.0:
        tot_syst /= sum(sig_rate.values())
        tot_syst *= 100
    vals.append(tot_syst)
    if tot_syst>40.0:
        print name

plt.hist(vals, range=(0,100), bins=50, histtype='stepfilled')
plt.gca().set_yscale('log')
plt.gca().set_ylim(ymin=1e-1)
plt.show()



