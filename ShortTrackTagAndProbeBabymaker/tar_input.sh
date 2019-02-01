#!/bin/bash

tar cfz input.tar RunTagAndProbe.exe *.so LinkDef_out_rdict.pcm -C ../babymaker/jsons/ . -C ../lepsf/ .
