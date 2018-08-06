# Calculating Fshort

The babymaker counts STs and STCs for a standard STC isolation and quality factor (as of Aug 6, 2018, isolation is loosened by 6x and quality by 3x). If you wish to use a different STC definition, edit FshortLooper.cc, and set recalculate to true.

Do 
``` bash
make b
```

Then
``` bash
./FshortLooper.exe <tag_for_this_run> <input_file>
```

Alternatively
``` bash
./writeConfig.sh <directory_containing_many_input_files> <tag_for_this_run>
condor_submit condor/condor_<TAG>.cmd
```

As is, the script submits UAF jobs, so keep the number of input files below ~200 at most.

In either case, output root files containing various Fshort histograms will be placed in output/<TAG>.

Edit postprocess.py to set the sample names you wish to include in your Fshort calculation. Then
``` bash
python postprocess.py <TAG>
```

This will produce a merged output/Fshort_<TAG>.root file for use with ShortTrackLooper, as well as some pngs.