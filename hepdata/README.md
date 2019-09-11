### hepdata

YAML files for submission to [hepdata](https://www.hepdata.net)

1. Edit/add files in the relevant directory
2. `./make_submission.sh` to tar this into `submission.tar.gz`
3. Upload this to your hepdata sandbox to inspect/debug

Issues:
* hepdata hangs when trying to display a table where one of the rows
has an independent variable with infinite range (e.g. MT2 1200-inf).
For now using "-1" instead of inf, maybe should find a better solution later.
