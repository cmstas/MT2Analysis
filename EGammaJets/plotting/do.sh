#!/bin/bash

cd ../output/

hadd -f data_2017.root data_Run*

cd ../plotting

python plotter.py