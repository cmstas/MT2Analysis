#!/bin/bash

for dir in output_CRSL/*; do
    hadd -f ${dir}/ttsl.root ${dir}/ttsl_from*
done