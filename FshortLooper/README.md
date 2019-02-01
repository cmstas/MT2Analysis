# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit FshortLooper.cc, and set recalculate to true. Make sure to run on a loose enough skim if you do this. WARNING: The code currently works properly only for recalculate = true.

Do 
``` bash
make b
```

Then edit do.sh to point to the 2016/17/18 data and MC babies you wish to run on.

Root files containing various Fshort histograms will be placed in output_unmerged/<year>_<TAG>.

Then do
``` bash
source haddall.sh <TAG>
```

This will produce output_merged/<TAG>.root

Finally, run
``` bash
source postprocess.sh <TAG>
```

This will make a file at output/Fshort_<data or mc>_<year>_<TAG>.root containing transfer factors for use in ShortTrackLooper, as well as some pngs.

Run
``` bash
python sigcontam.py <TAG>
```

to calculate the effect of signal contamination on fshort. You need to run this sigcontam.py before running the final sigcontam.py in ShortTrack.

To print plots of fshort display the systematic calculation with respect to HT/MET variations, do

``` bash
python print_variations.py <TAG>
```

To make an etaphi veto map, update and run

``` bash
python make_etaphi_veto.py
```