#!/bin/bash

SQLS_DIR=sqls

# target directory 
target=$1

run_tag=$2

if [ ! -n "$run_tag" ] ; then
    echo "you should input run_tag to describe current run"
fi

$result_dir=tmp/$run_tag
mkdir -p $result_dir

pg_ports=15432
pg_ip=192.168.12.21

sqls=`ls $target/*.sql`
for sql in $sqls
do
    number=`echo $sql | awk -F "." '{print $1}' | awk -F "/" '{print $3}'`
    output_file=$run_tag/$number.out

    psql -U postgres -d test -h $pg_ip -p $pg_ports -f $sql > $output_file
    querytime=`cat $output_file | grep "Time:" | awk '{print $2}'`
    echo $querytime
    sleep 3s

    ## run three times
    # total=0
    # for n in 1 2 3
    # do
    #     sleep 3s
    #     psql -U postgres -d test -h $pg_ip -p $pg_ports -f $sql > $output_file
    #     timeStr=`cat $output_file | grep "Time:" | awk '{print $2}'`
    #     total=`echo "$timeStr + $total"| bc -l`
    # done

    # result=`echo "$total/3" | bc -l`
    # echo $result
    # sleep 3s
done