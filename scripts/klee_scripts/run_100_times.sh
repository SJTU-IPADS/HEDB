#!/bin/bash

for i in `seq 1 100`; do
    z3 --smt2 klee-out-prefix/test000001.smt2 > /dev/null
done