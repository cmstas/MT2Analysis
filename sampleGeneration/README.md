#Making Custom SMS
This repo contains all the scripts you need to make custom SMS samples using condor, starting with decayed LHE files and ending with MiniAOD.

##General Instructions:
There are 3 sets of scripts used for each step:
- pLHE
- AOD
- MiniAOD

Each step has the same workflow, assuming the previous step is complete: make all the condor configs, submit all your jobs, check for missing files (due to job failures), and resubmit until all your jobs have finished. The output will be written to hadoop:
```
/hadoop/cms/store/<user>/mcProduction/<tag>_<sample>/<step>/
```

###pLHE
The `writeAllConfig_pLHE.sh` script will generate condor cfgs for each decayed LHE files in the target directories. **NOTE**: this assumes your LHE files are zipped in the .xz format! There is an option to set a tag at the beginning of the file if desired. The syntax for each line is
```
./writeConfig_pLHE.sh <directory of LHE files> <name of sample>
```
which will generate a .cmd file for every LHE file in the corresponding directory. It will also generate a `submitAll_pLHE.sh` script to submit condor jobs for each of your config files. Once all your jobs are complete, you can use
```
./checkAllConfig.sh <config directory>
```
to scan for any missing files. This will make a .cmd file to submit only those jobs for which files are missing. you can then submit it with `condor_submit <config directory>/<resubmit file>.cmd`

###AOD
Before running the AOD step, you will need to define the qcut for the sample(s) you are processing, which depends on the parent process and sparticle mass (e.g. gluino production with mGluino==1000). Either look up the qcut for a corresponding sample in MCM, or check the SUSY SMS spreadsheets in the tables [found here](https://docs.google.com/spreadsheets/d/1fsHXGf6s7sIm_8PWaoVermlN1Q9mEtCM-1mTxqz4X7k/). Once you know what qcut to use, edit the line `fragmentTemplate.py` which looks like
```
'JetMatching:qCut = 116', #this is the actual merging scale
```
to reflect the appropriate qcut. The `writeAllConfig_AOD.sh` script takes an additional argument on each line corresponding to the fragment template for the signal:
```
./writeConfig_AOD.sh <directory of LHE files> <name of sample> fragmentFile.py
```
If you want to process multiple samples in parallel with different qcuts, copy `fragmentTemplate.py` and point each set of samples to the fragment with the appropriate qcut.

Once your fragment is setup, the workflow proceeds just like the pLHE step:
```
./writeAllConfig_AOD.sh
./submitAllConfig_AOD.sh
<wait for your jobs to finish>
./checkAllConfig.sh <config directory>
condor_submit <config directory>/<resubmit file>.cmd
```
Rinse and repeat until all your jobs are complete.

###MiniAOD
The MiniAOD (v2) step proceeds just like the pLHE & AOD steps:
```
./writeAllConfig_MiniAOD.sh
./submitAllConfig_MiniAOD.sh
<wait for your jobs to finish>
./checkAllConfig.sh <config directory>
condor_submit <config directory>/<resubmit file>.cmd
```
Rinse and repeat until all your jobs are complete.

###Future Development:
- functionality to generate undecayed LHE files
- add scripts for banner injections
- intelligently lookup qcuts for different processes and mass points
- add the MiniAOD step
- incorporate scripts to change decay tables (in pLHE step)
