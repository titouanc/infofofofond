#!/bin/bash

INFILE=example-inputs.txt

if [[ $# == 2 ]]; then
    INFILE=$2
fi
make -C solver && solver/solver $1 < $INFILE
