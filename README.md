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

## The mt2higgs Branch Workflow
The mt2higgs studies can directly run on the babies genenrated by the standard MT2 search.
### 1. Run on the MT2looper
Before runing, check the signal regions definition at `MT2CORE/sigSelectionsMT2Higgs.cc`
``` bash
cd MT2looper
. do.sh
```
### 2. Do the data-driven Estimates
This step will also combined the output files to a more compact format
``` bash
cd scripts
. makeDataDrivenEstimates.sh
```
### 3. Do the SRyieldsHist and SR yields Tables
Choose the desired SRs and the proper funcion in the file and then run
``` bash
edit plotMakerHcand.C
root -b -q plotMakerHcand.C
```
the output of plots will be in the folder `plots/` and the tables will be in the file `tables/table.tex`

### 4. Make kinematic plots
Though kinematic plots can also be made by the plotMaker in previous step, a nicer version can be made by the `pyPlotMaker`
``` bash
cd CRplotMaker
edit do.py
python do.py
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

## Laptop setup to run on MT2babies
(this assumes you already have root setup)
``` bash
git clone git@github.com:cmstas/MT2Analysis.git
cd MT2Analysis/MT2looper
make
```

## Executing main analysis
Starting from the (skimmed) babies:

### Run MT2looper to produce root histogram files
Go to `MT2looper` and modify `INDIR`, `OUTDIR`, and `Samples` in `do.sh`, then do:
``` bash
. do.sh
```
### Make the data driven backgrounds
Go to `scripts` and modify `INDIR` in `makeDataDrivenEstimates.sh` to point to the output dir from `MT2looper`.  Then do:
``` bash
. makeDataDrivenEstimates.sh
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

### Run limits
See instructions in `README` inside the `limits/batchsubmit` subdirectory.
