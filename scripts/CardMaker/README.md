Card-making workflow for the full Run II combination. `Datacard.py` contains a generic Datacard class that
automatically handles splitting nuisances and backgrounds by year (and writes out a nicely-formatted datacard!).
See `smallExample.py` for a minimal example of use.

The general workflow is as follows:
1. make your estimates with the various estimate-making procedures. You should have a single directory
(MT2Looper output directory?) with zinv, lostlep, and qcd estimates for each year. There should also
be a combined data file.
2. Edit the input paths and `TAG` in `makeTemplateDatacards.py`. Then run. It will make "template" (i.e. signal
info left blank) datacards in `cards_<TAG>/templates`. It will also save a pickled `.pkl` file containing a dictionary
of all Datacard objects, which can be read into python scripts later (like `makeSignalPoint.py`).
3. Trying to run over all points by reading from TH3s will take 10s of GB of memory and probably cripple the uaf.
So it's a good idea to "flatten" your signal output (i.e. turn TH3s into TH1s) using something like:
``` 
python flattenLooperOutput.py ../path/to/looper/outputdir T1tttt
```

4. This might take a while but it speeds things up significantly in the following step. Now edit makeSignalPoint.py so that it will point
to your new directory of flattened histograms, and then you can run `do_scan.py` after editting your `TAG` and `MODEL`. 
This will submit jobs locally, 1 per point, and manage them so that only ~25 are running at once (again so we don't 
run 1000 processes and cripple the uaf). It only takes a few seconds per point, and the whole thing should finish in ~10 min.
5. This will output into `cards_<TAG>/MODEL/MODEL_M1_M2/datacard_*.txt`. This is your final set of cards that can be 
fed into the limit-making machinery.
