# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit ShortTrackLooper.cc, and set recalculate to true. Make sure to produce and provide a corresponding Fshort rootfile.

Do 
``` bash
make b
```

Then
``` bash
./ShortTrackLooper.exe <tag_for_this_run> <input_file>
```

In either case, output root files containing various STC->ST histograms will be placed in output/<TAG>.

After these show up, run shorttrack_merge.py
``` bash
python shorttrack_merge.py <TAG> syststat
```

(syststat will produce output files with errors set to the quadrature sum of systematic and statistical error)

This will produce files of the form output_merged/<data or mc>_<year>_<TAG>_syststat.root containing the final estimate.

Then run sigcontam.py
``` bash
python sigcontam.py <TAG>
```

This script calculates the inefficiency caused by signal contamination of the fshort region (run the sigcontam.py in FshortLooper first) and the STC control regions. The output is saved to output_merged/<data or mc>_<year>_<TAG>_contam.root
