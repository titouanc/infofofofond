#!/bin/bash

make -C solver && solver/solver $1 < example-inputs.txt
