#!/bin/bash

# Second -C path must be given relative to the first -C path
tar cfz input.tar ShortTrackLooper.exe *.so LinkDef_out_rdict.pcm -C ../babymaker/jsons/ . -C ../../FshortLooper/output/ .
