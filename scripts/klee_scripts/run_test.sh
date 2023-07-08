#!/bin/bash

outfile=$1 # this is log output 
fullsqls=$(seq 1 22)
minsqls=$(seq 2 6)
onesqls=$(seq 2 22)
if [ $2 = "min" ]; then
    sqls=$minsqls
elif [ $2 = "without-one" ]; then
    sqls=$onesqls
else 
    sqls=$fullsqls
fi
for i in $sqls; do 
    LOG_FILE=~/001-log/Q${i}.log
    OUT_FILE=desen/Q${i}-desen.log
    echo ${i} ${LOG_FILE} ${OUT_FILE}
    ../klee_scripts/desenitize.sh ${LOG_FILE} ${OUT_FILE}  >>${outfile} 2>&1
    #usr/bin/time -v ../klee_scripts/desenitize.sh ${LOG_FILE} ${OUT_FILE}  >>${outfile} 2>&1
    sleep 10 # wait for system to be stable
done