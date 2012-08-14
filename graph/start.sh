#!/bin/bash

echo "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0">data
kst2 graph.kst&
./parse.py /dev/ttyUSB0>data

echo "Don't forget to remove the data file."
ls -lah data