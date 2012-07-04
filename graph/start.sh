#!/bin/bash

echo "0 0 0">data
kst2 graph.kst&
miniterm2.py /dev/ttyUSB0 --baud 115200|tee data

echo "Don't forget to remove the data file."
ls -lah data