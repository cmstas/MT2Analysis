#Making Custom SMS

This repo contains all the scripts you need to make custom SMS samples using condor, starting with decayed LHE files and ending with MiniAOD.

##Instructions:
There are 3 sets of scripts used for each step:
- pLHE
- AOD
- MiniAOD

###pLHE
The `writeAllConfig_pLHE.sh` script will generate condor cfgs for each decayed LHE files in the target directories. There is an option to set a tag at the beginning of the file if desired. The syntax is
```
./writeConfig_pLHE.sh <directory of LHE files> <name of sample>
```
It will also generate a `submitAll_pLHE.sh` script to submit condor jobs for each of your config files. Once all your jobs are complete, you can use
```
./checkAllConfig.sh <config directory>
```
to scan for any missing files. This will automatically make a .cmd file which you can use to just resubmit jobs for the missing files.

###Future Development:
- functionality to generate undecayed LHE files
- add scripts for banner injections
