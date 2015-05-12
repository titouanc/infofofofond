#!/bin/bash

inFile="datasets/example-inputs.txt"

if [[ $# -ge 1 ]]; then
    inFile=$1
    shift
fi

make -C solver && sed -E 's/\#.*//g' < $inFile | solver/solver $*
