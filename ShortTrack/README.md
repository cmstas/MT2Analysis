# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (as of Aug 6, 2018, isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit ShortTrackLooper.cc, and set recalculate to true. Make sure to produce and provide a corresponding Fshort rootfile.

Do 
``` bash
make b
```

Then edit do.sh to point to the babies you wish to run on, and execute it.

Output root files containing various STC->ST histograms will be placed in output_unmerged/<year>_<TAG>.

Then do

``` bash
python shorttrack_merge.py <TAG>
```

This will produce a merged output_merged/<data or mc>_<year>_<TAG>.root file containing the final estimate, and some pngs.

You can make tables and plots of fshort and estimates with

``` bash
python print_counts.py
```