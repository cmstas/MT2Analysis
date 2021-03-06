# plot_defs.py
# collect the plot definitions for the various control regions
# format is:
#     (var_name, isLog, (xmin,xmax), (ymin,ymax), Rebin(optional), doOverflow(opt, def true))
# use None to use default params

lumi = 804
lumiUnit = "pb"
cmsPreliminaryText = "CMS"

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
    ("J0pt",True,None,None,3),
    ("J1pt",True,None,None,3),
    # ("jet1_eta_highpt",False,None,None,1),
    # ("jet2_eta_highpt",False,None,None,1),
]


dy_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(100,1000),None,2),
    ("mt2",True,(100,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    ("zllmass",True,(70,120),None),
    ("zllpt",True,(0,1000),None,1),
    ("nlepveto",False,None,None),
    ("jetpt1",True,(0,1200),None,2),
    ("jetpt2",True,(0,1200),None,2),    
    ("leppt1",True,(0,500),None),
    ("leppt2",True,(0,500),None),    
    ("htEle",True,(250,1500),None),
    ("htMu",True,(250,1500),None),
    # ("dRleplep",False,(0,1.5),None,2),
    # ("dRleplepEle",False,(0,1.5),None,2),
    # ("dRleplepMu",False,(0,1.5),None,2),
]

dy_OF_plots = [
    ("htemu",True,(250,1500),None,2),
    ("metemu",True,(200,1000),None,2),
    ("mt2emu",True,(200,1000),None,2),
    ("mt2binsemu",True,None,None),
    ("nJet30emu",True,(0,14),None),
    ("nBJet20emu",True,(0,6),None),
    ("zllmassemu",True,(70,120),None),
    ("nlepvetoemu",False,None,None),
]

gj_plots = [
    ("ht",True,(250,1500),None,2),
    # ("simpleht",True,(250,2000),None,2),
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
    # ("drMinParton_ht250to450",False,None,None),
    # ("drMinParton_ht450to1000",False,None,None),
    # ("drMinParton_ht1000toInf",False,None,None),
    ("SigmaIetaIetaEBLooseAllSieie",False,(0.0075,0.015),None),
    ("SigmaIetaIetaEELooseAllSieie",False,(0.02,0.0345),None,2),
    # ("jet1_eta_highpt",False,None,None,1),
    # ("jet2_eta_highpt",False,None,None,1),
]

qcdJ_plots = [
    ("J0pt",False,(250,1000),None,1,False),
    ("J0eta",False,None,None,2),
    ("J0phi",False,None,None,2),
    ("J1pt",False,(30,330),None,1,False),
    ("J1eta",False,None,None,2),
    ("J1phi",False,None,None,2),
]

qcd_plots = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(0,1000),None,2),
    # ("mt2",True,(100,350),(4e3,1e6),2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
    # ("deltaPhiMin",False,None,None,2),
    ("deltaPhiMin",False,None,None,1),
    ("J0pt",False,None,None,2),
    ("J0eta",False,None,None,2),
    ("J0phi",False,None,None,2),
    ("J1pt",False,None,None,2),
    ("J1eta",False,None,None,2),
    ("J1phi",False,None,None,2),
]

mt2_only = [
    ("mt2bins",True,None,None),
    ("mt2",True,(200,1000),None,2)
]

basic_kinematic = [
    ("ht",True,(250,1500),None,2),
    ("met",True,(200,1000),None,2),
    ("mt2",True,(200,1000),None,2),
    ("mt2bins",True,None,None),
    ("nJet30",True,(0,14),None),
    ("nBJet20",True,(0,6),None),
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
    ("gammaPhi",False,None,None),
    # ("simpleht",True,(250,2000),None,2),
]
