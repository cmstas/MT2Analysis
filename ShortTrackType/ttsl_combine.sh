#!/bin/bash

for dir in output_*/*; do
    hadd -f ${dir}/ttsl.root ${dir}/ttsl_from*
done