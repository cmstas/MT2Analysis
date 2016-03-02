#Making Custom SMS
This repo contains all the scripts you need to make custom SMS samples using condor, starting with decayed LHE files and ending with MiniAOD.

##General Instructions:
The scripts in this folder break up the generation process into 3 steps:
- pLHE
- AOD
- MiniAOD

The steps run in series for each condor job, and copy the final MiniAOD file back to hadoop. The output will be written to:
```
/hadoop/cms/store/<user>/mcProduction/<tag>_<sample>/
```
All you have to do is **set your input LHE files** in `writeAllConfig.sh`, and **make sure that the correct qcut is set** in `fragmentTemplate.py`. Once you do this, the workflow is simple:
```
./writeAllConfig.sh
./submitAll.sh
<wait for your jobs to finish>
./checkAllConfig.sh <config directory>
condor_submit <config directory>/<resubmit file>.cmd
```
Rinse and repeat until all your jobs are complete.

**NOTE**: There are a couple assumptions built into these scripts!

1. The LHE files are assumed to be zipped in the .xz format. If this isn't the case, you will need to edit `writeConfig.sh`
2. The number of events in the LHE file should be at least N(Jobs)*N(Events per Job)

###writeAllConfig.sh
The `writeAllConfig.sh` script will generate condor cfgs for each (decayed) LHE file in the target directories. There is an option to set a tag at the beginning of the file if desired. In addition, you should set the number of jobs to split each mass point into, and the number of events per job. The syntax for each line is then
```
./writeConfig.sh ${EVENTS} ${JOBS} <directory of LHE files> <name of sample> <python fragment>
```
which will generate a config file for every LHE file in the corresponding directory. It will also generate a `submitAll.sh` script to submit condor jobs for each of your config files. Once all your jobs are complete, you can use
```
./checkAllConfig.sh <config directory>
```
to scan for any missing files. This will make a .cmd file to submit only those jobs for which files are missing. you can then submit it with `condor_submit <config directory>/<resubmit file>.cmd`

###fragmentTemplate.py
Before running the AOD step, you need to define the qcut for the sample(s) you are processing, which depends on the parent process and sparticle mass. Either look up the qcut for a corresponding sample in MCM, or check the SUSY SMS spreadsheets in the tables [found here](https://docs.google.com/spreadsheets/d/1fsHXGf6s7sIm_8PWaoVermlN1Q9mEtCM-1mTxqz4X7k/). Once you know what qcut to use, edit the line in `fragmentTemplate.py` which looks like
```
'JetMatching:qCut = 116', #this is the actual merging scale
```
to reflect the appropriate qcut. You may also want to edit the `nJetMax` parameter corresponding to the maximum number of additional partons at matrix level (the default for SUSY signals is usually 2). The `writeAllConfig.sh` script takes a final argument on each line corresponding to the fragment template for that signal:
```
./writeConfig.sh <directory of LHE files> <name of sample> fragmentFile.py
```
If you want to process multiple samples in parallel with different qcuts, copy `fragmentTemplate.py` and point each set of samples to the fragment with the appropriate qcut (*i.e.*, it is wise to split up a full scan into different mass ranges corresponding to different qcuts).

###Future Development:
- functionality to generate undecayed LHE files
- add scripts for banner injections
- intelligently lookup qcuts for different processes and mass points
- incorporate scripts to change decay tables (in pLHE step)
- "post-processing" to merge each mass point into single file
