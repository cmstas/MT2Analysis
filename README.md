# MT2 Analysis
## Setup:
To simply setup on UAF, do
``` bash
git clone git@github.com:cmstas/MT2Analysis.git
```
and then 
``` bash
cd MT2Analysis
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

## Laptop setup to run on MT2babies
(this assumes you already have root setup)
``` bash
git clone git@github.com:cmstas/MT2Analysis.git
cd MT2Analysis/MT2Looper
make
```

## Executing main analysis
Starting from the (skimmed) babies:

### Run QCDLooper to produce rphi estimate files
Go to `QCDLooper` and modify `do.sh` to point towards the data and mc babies you wish to use. The standard skims will not work here, as the rphi estimate needs low MT2 events. Worst case, you can use completely unskimmed babies, but this takes a long time.
``` bash
. do.sh
```

TODO: Modify QCDLooper to run on any of 2016, 2017, or 2018 intelligently. It currently (9 Aug 2018) runs with 2017 settings with 2016 commented out.

Now, follow the instructions in QCDLooper and scripts/qcdEstimate, then edit `string rphi_file_name` in `MT2Looper/MT2Looper.cc` point to your qcdHistos.root output.


### Run MT2Looper to produce root histogram files
Go to `MT2Looper` and modify `INDIR`, `OUTDIR`, and `Samples` in `do.sh`, then do:
``` bash
. do.sh
```
### Make the data driven backgrounds
Go to `scripts` and modify `INDIR` in `makeDataDrivenEstimates.sh` to point to the output dir from `MT2Looper`.  Then do:
``` bash
. makeDataDrivenEstimates.sh
```

### Merge histograms for signal scans, if necessary
Go to `scripts` and modify `INDIR` in `processSignal.sh` to point to the output dir from `MT2Looper`.  Also comment out any signal scans that you did not process.  Then do:
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
