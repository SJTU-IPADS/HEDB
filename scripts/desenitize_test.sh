#!/bin/bash 

TMP_PATH=$PWD/scripts/tmp
LOG_PATH=$TMP_PATH/logs
OUTPUT_FILE=$TMP_PATH/output.log


# check if logs.tar.gz in scripts/tmp exists
# if [ ! -f $TMP_PATH/logs.tar.gz ]; then
#     # wget link and save to directory logs
#     cd $TMP_PATH
#     wget https://github.com/zhaoxuyang13/hedb/releases/download/logs/logs.tar.gz
#     # unzip logs.tar.gz
#     tar -zxvf logs.tar.gz 
# fi

rm $OUTPUT_FILE
touch $OUTPUT_FILE
# docker run --entrypoint /home/klee/entrypoint-min.sh -v $LOG_PATH:/home/klee/001-log -v $OUTPUT_FILE:/home/klee/output.log zhaoxuyang13/klee-desen:1.0
# docker run --entrypoint /home/klee/entrypoint-without-one.sh -v $LOG_PATH:/home/klee/001-log -v $OUTPUT_FILE:/home/klee/output.log zhaoxuyang13/klee-desen:1.0
# logs is built in docker image, for simplicity 
docker run --entrypoint /home/klee/entrypoint.sh -v $OUTPUT_FILE:/home/klee/output.log  zhaoxuyang13/klee-desen:1.0

# filter out time, and compute and convert (h:mm:ss or m:ss) to seconds
cat $OUTPUT_FILE | grep "Elapsed (wall clock) time (h:mm:ss or m:ss):" \
    | awk '{print $8}' > $TMP_PATH/time.log

# filter out "total xxxx ktests, run with BATCH SIZE"
cat $OUTPUT_FILE | grep "ktests, run with BATCH SIZE" | awk '{print $2}' >> $TMP_PATH/time.log