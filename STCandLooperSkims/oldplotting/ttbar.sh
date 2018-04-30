#!/bin/bash

python plotter_ttbar.py ../output_CRSL_GV_EPV pngs_CRSL_GV_EPV yes
tar cfz pngs_CRSL_GV_EPV.tar pngs_CRSL_GV_EPV

python plotter_ttbar.py ../output_SR_GV_EPV pngs_SR_GV_EPV yes
tar cfz pngs_SR_GV_EPV.tar pngs_SR_GV_EPV

python plotter_ttbar.py ../output_HT_GV_EPV pngs_HT_GV_EPV yes
tar cfz pngs_HT_GV_EPV.tar pngs_HT_GV_EPV

#python plotter_ttbar.py ../output_GV_EPV pngs_GV_EPV yes
#tar cfz pngs_GV_EPV.tar pngs_GV_EPV

# No GV

#python plotter_ttbar.py ../output_CRSL_EPV pngs_CRSL_EPV yes
#tar cfz pngs_CRSL_EPV.tar pngs_CRSL_EPV

#python plotter_ttbar.py ../output_SR_EPV pngs_SR_EPV yes
#tar cfz pngs_SR_EPV.tar pngs_SR_EPV

#python plotter_ttbar.py ../output_HT_EPV pngs_HT_EPV yes
#tar cfz pngs_HT_EPV.tar pngs_HT_EPV

#python plotter_ttbar.py ../output_EPV pngs_EPV yes
#tar cfz pngs_EPV.tar pngs_EPV

# GV but not EPV

#python plotter_ttbar.py ../output_CRSL_GV pngs_CRSL_GV yes
#tar cfz pngs_CRSL_GV.tar pngs_CRSL_GV

#python plotter_ttbar.py ../output_SR_GV pngs_SR_GV yes
#tar cfz pngs_SR_GV.tar pngs_SR_GV
