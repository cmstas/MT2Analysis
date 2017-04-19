# plot_defs.py
# collect the plot definitions for the various control regions
# format is:
#     (var_name, isLog, (xmin,xmax), (ymin,ymax), Rebin(optional), doOverflow(opt, def true))
# use None to use default params

lumi = 804
lumiUnit = "pb"

rl_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    ("leppt",True,None,None,4),
    ("lepeta",False,None,None),
    ("nlepveto",False,None,None),
]

sl_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    ("leppt",True,(0,400),None,2),
]

dy_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    ("zllmass",True,(70,120),None),
    ("nlepveto",False,None,None),
    ("htemu",True,(250,1500),None,2),
    ("metemu",True,(200,1000),None,2),
    ("mt2emu",True,(200,1000),None,2),
    ("mt2binsemu",True,None,None),
    ("nJet30emu",True,(0,14),None),
    ("nBJet20emu",True,(0,6),None),
    ("zllmassemu",True,(70,120),None),
    ("nlepvetoemu",False,None,None),
    # add zllpt plots once added to MT2looper
]

gj_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    ("gammaPt",True,None,None,4),
    ("gammaEta",False,None,None),
    ("nlepveto",False,None,None),
    ("chisoLoose",True,(0,10),None),
    ("chisoEELoose",True,(0,10),None),
    ("chisoEBLoose",True,(0,10),None),
    ("chisoLooseSieieSB",True,(0,10),None),
    ("chisoEELooseSieieSB",True,(0,10),None),
    ("chisoEBLooseSieieSB",True,(0,10),None),
    ("drMinParton",False,None,None),
    ("drMinParton_ht250to450",False,None,None),
    ("drMinParton_ht450to1000",False,None,None),
    ("drMinParton_ht1000toInf",False,None,None),
    ("SigmaIetaIetaEBLooseAllSieie",False,(0.0075,0.015),None),
    ("SigmaIetaIetaEELooseAllSieie",False,(0.02,0.0345),None,2),
]

qcdJ_plots = [
    ("J1pt",False,(30,330),None,1,False)
]

mt2_only = [
    ("mt2bins",True,None,None),
    ("mt2",True,(200,1000),None,2)
]

ht_njet_plots = [
    ("ht",True,(250,1500),None,2),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
]

ht_njet_extraNB_plots = [
    ("ht",True,(250,1500),None,2),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,4),None),
    ("nBJet20L",True,(0,4),None),
    ("nBJet20M",True,(0,4),None),
    ("nBJet20T",True,(0,4),None),
]

gamma_vars_only = [
    ("gammaPt",True,None,None,4),
    ("gammaEta",False,None,None),
]


# -- mt2higgs --
basic_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("nJet30",True,(2,14),None),
    ("nBJet20",True,(2,7),None),
]

hcand_xtra = [
    ("minMTbmet",True,(0,600),None),
    ("Mbbhcand",True,(100,150),None),
    ("nhcand",False,None,None),
]

Hcand_xtra = [
    ("minMTbmet",True,(0,600),None),
    ("MbbMax",True,(300,1000),None,2),
]

Zcand_xtra = [
    ("MbbZcand",True,(0,1000),None,2),
]

rl_add = [
    ("leppt",True,None,None,4),
    ("lepeta",False,None,None),
    ("nlepveto",False,None,None),
]

sl_add = [
    ("leppt",True,(0,400),None,2),
]

dy_add = [
    ("zllmass",True,(70,120),None),
    # ("nlepveto",False,None,None),
    # ("htemu",True,(250,1500),None,2),
    # ("metemu",True,(200,1000),None,2),
    # ("mt2emu",True,(200,1000),None,2),
    # ("mt2binsemu",True,None,None),
    # ("nJet30emu",True,(0,14),None),
    # ("nBJet20emu",True,(0,6),None),
    # ("zllmassemu",True,(70,120),None),
    # ("nlepvetoemu",False,None,None),
    # add zllpt plots once added to MT2looper
]

gj_add = [
    ("gammaPt",True,None,None,4),
    ("gammaEta",False,None,None),
    # ("nlepveto",False,None,None),
    # ("chisoLoose",True,(0,10),None),
    # ("chisoEELoose",True,(0,10),None),
    # ("chisoEBLoose",True,(0,10),None),
    # ("chisoLooseSieieSB",True,(0,10),None),
    # ("chisoEELooseSieieSB",True,(0,10),None),
    # ("chisoEBLooseSieieSB",True,(0,10),None),
    # ("drMinParton",False,None,None),
    # ("drMinParton_ht250to450",False,None,None),
    # ("drMinParton_ht450to1000",False,None,None),
    # ("drMinParton_ht1000toInf",False,None,None),
    # ("SigmaIetaIetaEBLooseAllSieie",False,(0.0075,0.015),None),
    # ("SigmaIetaIetaEELooseAllSieie",False,(0.02,0.0345),None,2),
]

srh_plots = basic_plots + hcand_xtra
srH_plots = basic_plots + Hcand_xtra
srZ_plots = basic_plots + Zcand_xtra

crslh_plots = srh_plots + sl_add
crslH_plots = srH_plots + sl_add
crslZ_plots = srZ_plots + sl_add

crdyh_plots = srh_plots + dy_add
crdyH_plots = srH_plots + dy_add
crdyZ_plots = srZ_plots + dy_add

crgjh_plots = srh_plots + gj_add
crgjH_plots = srH_plots + gj_add
# crgjZ_plots = srZ_plots + gj_add
crgjZ_plots = basic_plots + gj_add # MbbZcand seems to yields 0 in this case

mbbcr_plots = [
    ("ht_MbbCRall",True,(250,1500),None,2),
    ("met_MbbCRall",True,(200,1000),None,2),
    ("mt2_MbbCRall",True,(200,1000),None,2),
    ("nJet30_MbbCRall",True,(0,14),None),
    ("nBJet20_MbbCRall",True,(0,6),None),
    ("MbbMax_MbbCRall",True,(0,6),None),
]

# mbbcr_plots = [].append(x for x in basic_plots: x[0].append("_MbbCRall"))
