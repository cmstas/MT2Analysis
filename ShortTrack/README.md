# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit ShortTrackLooper.cc, and set recalculate to true. Make sure to produce and provide a corresponding Fshort rootfile. WARNING: Currently, the code only works properly for recalculate = true.

Do 
``` bash
make b
```

Then, edit do.sh and
``` bash
source do.sh
```

In either case, output root files containing various STC->ST histograms will be placed in output/<TAG>.

After these show up, run shorttrack_merge.py
``` bash
python shorttrack_merge.py <TAG>
```

This will produce files of the form output_merged/<data or mc>_<year>_<TAG>.root containing the final estimate and observation.

Then run sigcontam.py
``` bash
python sigcontam.py <TAG>
```

This script calculates the inefficiency caused by signal contamination of the fshort region (run the sigcontam.py in FshortLooper first) and the STC control regions. The output is saved to output_merged/<data or mc>_<year>_<TAG>_contam.root

To print nice plots and tables, edit and use
``` bash
python print_counts.py <TAG>
```