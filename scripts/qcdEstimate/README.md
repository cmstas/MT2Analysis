First run the QCDLooper to get necessary inputs.
See `MT2Analysis/QCDLooper` directory for instructions.

Edit `inputs.txt` with the following lines:

1. Output directory of QCDLooper
2. Output directory of MT2Looper

Run `makeQCDHistos.py` to get rphi, rb, fj histograms. After doing this,
you can run `makeQCDPlots.py` to make plots and `makeQCDEstimate.py` to 
get make the qcd estimate root file.

makeQCDEstimate.py requires you to edit `string rphi_file_name` in MT2Looper.cc to point to the qcdHistos.root file produced by makeQCDHistos.py, then run MT2Looper. After MT2Looper finishes, go to the output directory for the run and do:
``` bash
hadd top.root tt* singletop.root
hadd data_Run<year>.root data_Run<year>*
hadd wjets_ht.root wjets_ht*
```

Now, makeQCDEstimate.py should run nicely.