import ROOT as r

TAG = "V00-10-15_FullRunII_decorrHalfLepEff"
# TAG = "V00-10-15_FullRunII_decorrHalfLepEff_T2qq_700_350"
# TAG = "V00-10-15_FullRunII_decorrHalfLepEff_T2qq_950_0"

f = r.TFile("{0}/fitDiagnostics_all.root".format(TAG))

fit_b = f.Get("fit_b")
fit_s = f.Get("fit_s")
prefit = f.Get("nuisances_prefit")

nuisances = {}

fpf_b = fit_b.floatParsFinal()
fpf_s = fit_s.floatParsFinal()

for i in range(fpf_b.getSize()):
    nuis_b = fpf_b.at(i)
    name = nuis_b.GetName()
    nuis_s = fpf_s.find(name)
    nuis_p = prefit.find(name)

    sigma_p = nuis_p.getError()
    sigma_b = nuis_b.getError()
    sigma_s = nuis_s.getError()
    val_p = nuis_p.getVal()
    val_b = nuis_b.getVal()
    val_s = nuis_s.getVal()

    diff = (val_b - val_p) / sigma_p
    diffunc = sigma_b / sigma_p

    diffs = (val_s - val_p) / sigma_p
    diffuncs = sigma_s / sigma_p

    nuisances[name] = [diff, diffunc, diffs, diffuncs, val_p, sigma_p]

fout = open("/home/users/bemarsh/public_html/mt2/t2qq_study/nuis_pulls/{0}.txt".format(TAG), 'w')

fout.write("{0:60s} {1:13s} {2:13s}\n".format("", "    b-only", "       s+b"))
fout.write("----------------------------------------------------------------------------------------------------------------------------\n")
for name in sorted(nuisances.keys(), key=lambda x:abs(nuisances[x][0]), reverse=True):
# for name in sorted(nuisances.keys(), key=lambda x:nuisances[x][1]):
    fout.write("{0:60s} {1:+6.2f} {2:6.2f} {3:+6.2f} {4:6.2f} {5:12.2f} {6:12.2f}\n".format(name, *nuisances[name]))

