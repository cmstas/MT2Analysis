#!/bin/bash

for i in condor/*; do
    condor_submit $i
done