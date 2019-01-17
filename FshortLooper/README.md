# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (as of Jan 15, 2018, isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit FshortLooper.cc, and set recalculate to true. Make sure to run on a loose skim if you do this.

Do 
``` bash
make b
```

Then edit do.sh to point to the 2016/17/18 data and MC babies you wish to run on.

Root files containing various Fshort histograms will be placed in output_unmerged/<TAG>.

Then do
``` bash
haddall.sh <TAG>
```

This will produce output_merged/<TAG>.root

Finally, run
``` bash
postprocess.sh
```

This will produce a merged output/Fshort_<TAG>.root containing transfer factors for use in ShortTrackLooper, as well as some pngs.

Run
``` bash
python sigcontam.py <TAG>
```

to calculate the effect of signal contamination on fshort.