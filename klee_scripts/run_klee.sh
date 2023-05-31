#!/bin/bash

# udf is already compiled with clang 
BITCODE_FILE_NAME=$1

# the function to be tested is called ${FUNC_NAME}
FUNC_NAME=$2

# we have a ktest file with users data named ${SECRET_INPUT_FILE}
SECRET_INPUT_FILE=$3


# we use klee to run the input, and generate path constraints (as smt2 file);
klee \
    --write-smt2s \
    --entry-point=${FUNC_NAME}_wrapper \
    --posix-runtime --libc=uclibc \
    --seed-file=${SECRET_INPUT_FILE} --named-seed-matching --only-replay-seeds \
    ${BITCODE_FILE_NAME}
