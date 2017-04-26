# MT2 Analysis
## Setup:
To simply setup on UAF, do
``` bash
git clone https://github.com/cmstas/MT2Analysis.git
```
and then 
``` bash
cd MT2Analysis
git checkout mt2higgs
source setup.sh
```
## Babymaker
### For a test run
``` bash
cd babymaker
source doTest.sh
```
### Babymaking in batch mode
See further instructions in the `README.md` in folder
```
babymaker/batchsubmit
```

## Executing main analysis for mt2higgs
Starting from the (skimmed) babies:
Note: The mt2higgs studies can directly run on the babies genenrated by the standard MT2 search.

### Run MT2looper to produce root histogram files
Before runing, check the signal regions definition at `MT2CORE/sigSelectionsMT2Higgs.cc`.
Go to `MT2looper` and modify `INDIR`, `OUTDIR`, and `Samples` in `do.sh`, then do:
``` bash
. do.sh
```
### Make the data driven backgrounds
Go to `scripts` and modify `INDIR` in `makeDataDrivenEstimates.sh` to point to the output dir from `MT2looper`.  Then do:
``` bash
. makeDataDrivenEstimates.sh
```

### Make kinematic plots
Though kinematic plots can also be made by the plotMaker in previous step, a nicer version can be made by the `pyPlotMaker`. 
In the folder, edit the `do.py` with correct information, then do:
``` bash
python do.py
```

### Merge histograms for signal scans, if necessary
Go to `scripts` and modify `INDIR` in `processSignal.sh` to point to the output dir from `MT2looper`.  Also comment out any signal scans that you did not process.  Then do:
``` bash
. processSignal.sh
```
Note that this also creates histogram files translated to ETH's format, for sharing with them.

### Create datacards
Go to `scripts` and modify `INDIR`, `OUTDIR`, `DOFULLSCAN`, and `samples` in `makeAllCards.sh`.  Then do:
``` bash
root -b -q cardMaker.C+  # to compile
. makeAllCards.sh
```
### Do the SRyieldsHist and SR yields Tables
When making the datacards, turn on the flag `printTable` and the tablecards summarizing the backgrounds and their uncertainties for each bin will be print in the same folder as the datacards.
Then go to `ResultPlotMaker` and edit the `do.py` with the datacards/tablecards dir. Then do:
``` bash
python do.py
```
the output of plots and tables will be in the folder `plots/` and `tables/` subfolders.

### Run limits
See instructions in `README` inside the `limits/batchsubmit` subdirectory.
