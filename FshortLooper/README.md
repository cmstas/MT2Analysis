# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (as of Aug 6, 2018, isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit FshortLooper.cc, and set recalculate to true.

Do 
``` bash
make b
```

Then, edit do.sh to point to the babies you wish to run on, and execute the script.

Output root files will be placed in output_unmerged/<year>_<TAG>.

Execute haddall.sh (possibly edited, if you only ran on a subset of datasets). This will combine files into output_merged/<mc or data>_<year>_<tag>.root

Execute postprocess.sh to produce output Fshort histograms in the output/ directory, plus plots in pngs/, etc.
``` bash
. postprocess.sh <TAG>
```

This will produce a merged output/Fshort_<mc or data>_<year>_<TAG>.root file for use with ShortTrackLooper.

You can produce Fshort kinematic invariance plots and tables with

``` bash
python print_variations.py
```
