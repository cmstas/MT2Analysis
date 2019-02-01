Used to find eta-phi holes for building the short track eta-phi veto (see make_etaphi_veto.py in FshortLooper).

In order to maximize statistics, we use an extremely loose skim, and so keep minimal information in these babies to keep things as lightweight as possible.

Edit writeAllConfig.sh so that it points towards all the Single Lepton primary datasets of interest, run it, and then submit to condor. Babies will show up in your hadoop area, and after being hadded, can be run on with ShortTrackTagAndProbeLooper.