#!/bin/bash

LOG_FILE=$1 # Qx.log
DESEN_FILE=$2 # Qx-desen.log

ROOT_DIR=${PWD}/..
SCRIPT_DIR=${ROOT_DIR}/klee_scripts
BUILD_DIR=${ROOT_DIR}/build

# decrypt and transform logfile to ktest files into directory ${BUILD}/ktests
rm -rf ${BUILD_DIR}/ktests
mkdir ${BUILD_DIR}/ktests
${BUILD_DIR}/desenitizer ${LOG_FILE}
ls ${BUILD_DIR}/ktests/ | wc -l

# use klee to generate constraints(SMT2 files) into directory ${BUILD}/klee-output-tmp
rm -rf ${BUILD_DIR}/klee-output-tmp
/usr/bin/time -v ${SCRIPT_DIR}/gen_constraint.sh ${BUILD_DIR}/ktests

# use z3 to solve constraints and output to file
/usr/bin/time -v ${SCRIPT_DIR}/run_z3.py ${BUILD_DIR}/smt2-files ${DESEN_FILE}



