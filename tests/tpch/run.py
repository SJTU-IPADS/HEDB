#!/usr/bin/env python
# -*- coding: utf-8 -*-

# sudo apt-get install python3-pip
# python3 -m pip install psycopg2 tqdm
import psycopg2
import os
import argparse
import time

from util_py3.ssh_util import *
from util_py3.prop_util import *

DEFAULT_TPCH_CONFIG="tpch-config.json"

tables = ["nation", "part", "region", "partsupp", "customer", "supplier", "lineitem", "orders"]

def PrepBenchmark(propFile = DEFAULT_TPCH_CONFIG):
    properties = loadPropertyFile(propFile)
    
    pgIp = properties['pg_ip']
    pgPort = properties['pg_port']
    pgUser = properties['pg_user']
    pgPW = properties['pg_password']
    dataSize = properties['data_size']
    secureQuery = properties['secure']
    
    # prepare data
    executeCommand("cd dbgen && make && ./dbgen -v -f -s %s && mv *.tbl .. && cd .." % dataSize)
    executeCommand("chmod a+rw *.tbl")
    
    # load data
    if secureQuery == "y": 
        schema = "tpch-schema-enc.sql"
        pgDB = "secure_test"
    else:
        schema = "tpch-schema.sql"
        pgDB = "insecure_test"
        
    cmd = f"PGPASSWORD={pgPW} psql -h {pgIp} -p {pgPort} -U {pgUser} -f {schema}"
    executeCommand(cmd)
    for table in tables:
        cmd = f"cat {table}.tbl | PGPASSWORD={pgPW} psql -h {pgIp} -p {pgPort} -U {pgUser} -d {pgDB} -c \"copy {table} from stdin with DELIMITER as '|';\""
        executeCommand(cmd)
        
    executeCommand("find . -name \"*.tbl\" | xargs rm")
    
    
def RunTest(propFile = DEFAULT_TPCH_CONFIG, query = 0, recordReplay='none', mode='seq'):
    properties = loadPropertyFile(propFile)
    
    pgIp = properties['pg_ip']
    pgPort = properties['pg_port']
    pgUser = properties['pg_user']
    pgPW = properties['pg_password']
    pgLogDir = properties['pg_log_dir']
    secureQuery = properties['secure']
    secureQueryDir = properties['secure_query_dir']
    insecureQueryDir = properties['insecure_query_dir']
    outputDir = properties['output_dir']
    
    record = False
    replay = False
    
    if secureQuery == 'y':
        pgDB = 'secure_test'
        queryDirectory = secureQueryDir
        
        if recordReplay == 'record':
            record = True
        elif recordReplay == 'replay':
            replay = True
    else:
        pgDB = 'insecure_test'
        queryDirectory = insecureQueryDir
    
    folder = os.path.exists(outputDir)
    if not folder:
        os.makedirs(outputDir)
        
    if query:
        queryRange = range(query, query + 1)
    else:
        queryRange = range(1, 23)
    
    for i in queryRange:
        conn = psycopg2.connect(database = pgDB, user = pgUser, password = pgPW, host = pgIp, port = pgPort)
        cur = conn.cursor()
        
        startTime = time.time()

        queryStr = f"Q{i}"
        queryFile = queryDirectory + f"/Q{i}.sql" 
        outputFile = open(outputDir + f"/Q{i}.out", "w+");

        if record:
            cur.execute('SELECT enable_record_mode(%s, %s);', (queryStr, pgLogDir))
                            
        if replay:
            cur.execute('SELECT enable_replay_mode(%s, %s, %s);', (queryStr, pgLogDir, mode))

        cur.execute(open(queryFile, "r").read())
        result = cur.fetchall()

        outputFile.write(str(cur.description) + '\n')
        for row in result:
            outputFile.write(str(row) + '\n')

        outputFile.close()
        
        conn.commit()

        endTime = time.time()
        print(f"query Q{i}: {int((endTime - startTime) * 1000)}ms")

        cur.close()
        conn.close()

def main():
    # parse arguments 
    parser = argparse.ArgumentParser(description='Run test.')
    parser.add_argument('-rr', '--record-replay', type=str, default='none', help='enable \'record\' mode or \'replay\' mode')
    parser.add_argument('-sg', '--skip-generate', action='store_true',
                        help='skip data generation and table loading (default: false)')
    parser.add_argument('-l', '--load', action='store_true',
                        help='only generate data and load table(default: false)')
    parser.add_argument('-Q', '--query', type=int, default=0, help='run the given query')
    parser.add_argument('-m', '--mode', type=str, default='seq', help='\'seq\' mode for essential replay, \'perf\' mode for performance replay')
    args = parser.parse_args()
    
    if not args.skip_generate:
        PrepBenchmark()
    
    if not args.load:
        RunTest(query=args.query, recordReplay=args.record_replay, mode=args.mode)
        
if __name__ == '__main__':
    main()

