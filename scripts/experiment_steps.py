#!/usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import io
import os
import os.path
import sys
import datetime
import time
import math
import re
from util_py3.ssh_util import *
from util_py3.prop_util import *
from util_py3.graph_util import *
from util_py3.data_util import *

from tqdm import tqdm, trange
from pathlib import Path, PurePath

START_VM_CONFIG="scripts/config/vms.json"
DEFAULT_TPCH_CONFIG="scripts/config/baseTPCH.json"
GEN_ORDER_CONFIG="scripts/config/order.json"

def generateOrder(propFile = GEN_ORDER_CONFIG):
    properties = loadPropertyFile(propFile)
    
    orderScriptPath = properties['order_script_path']
    pgPort = properties['pg_port']
    pgIp = properties['pg_ip']
    orderSqlName = properties['sql_name']
    
    executeCommand("cd %s && psql -h %s -p %s -U postgres -d test -f %s" % (orderScriptPath, pgIp, pgPort, orderSqlName))

    # vaccum 
    executeCommand("psql -h %s -p %s -U postgres -d test -f scripts/sqls/util_sqls/vacuum.sql" %(pgIp, pgPort))


def compileCodes(parallel = False):
    # make before run vm, so that ops-server can be run in OPS VM.
    if parallel:
        executeCommand("make clean && make configure_sim_parallel && make")
    else:
        executeCommand("make clean && make configure_sim && make")
    
    executeCommand("mkdir -p scripts/tmp")

    print("compilation done, trying to start VMs")


# after VM started, ops-server and database will be automatically started.
# restart ops-server requires restart of VM.
# reload database extension does not need to restart VM
def startVMs(propFile = START_VM_CONFIG):
    properties = loadPropertyFile(propFile)
    
    vmScriptPath = properties['vm_scripts_path']
    # logPath = properties['logPath']
    DBVMScriptName = properties['dbms_script']
    OpsVMScriptName = properties['ops_script']
    pgPort = properties['pg_port']
    pgIp = properties['pg_ip']
    
    executeNonBlockingCommandNoOutput("cd %s && bash %s" %(vmScriptPath, DBVMScriptName))
    executeNonBlockingCommandNoOutput("cd %s && bash %s" %(vmScriptPath, OpsVMScriptName))

    time.sleep(10)
    
    print("vm started, waiting for postgres-server to be ready for connection")
    
    while True:
        output = executeCommandWithOutputReturn("pg_isready  -h %s -p %s" % (pgIp, pgPort))
        # print(output)
        if b'accepting connections' in output:
            break
        time.sleep(1)
    # time.sleep(5)
    print("VM started and postgres-server is ready for connection")
 
# build the HEDB project, load corresponding schema and data.    
def prepBenchmark(propFile = DEFAULT_TPCH_CONFIG):
    properties = loadPropertyFile(propFile)
    
    homePath = properties['home_path']
    pgPort = properties['pg_port']
    pgIp = properties['pg_ip']
    experimentName = properties['experiment_name']
    
    # start vms here, ops vm should be run after make is complete.
    # startVMs()
    
    dataSize = properties['data_size']
    executeCommand("cd benchmark/tools && rm -rf *.tbl && ./dbgen -s %s" % dataSize)
    
    # edit benchmark file 
    executeCommand("cd benchmark/config && cp tpch_config.xml tmp.xml && \
            sed -i 's#<DBUrl>.*</DBUrl>#<DBUrl>jdbc:postgresql://%s:%s/test</DBUrl>#' tmp.xml" % (pgIp, pgPort))
    
    schemaName = properties['schema_name']
    indexName = properties['index_name']

    # load schema
    executeCommand("cd benchmark && psql -h %s -p %s -U postgres -d test -f db_schemas/%s" % (pgIp, pgPort, schemaName))
    executeCommand("cd benchmark && psql -h %s -p %s -U postgres -d test -f db_schemas/%s" % (pgIp, pgPort, indexName))

    # load tpch data
    executeCommand("cd benchmark && java -Dlog4j.configuration=log4j.properties -jar bin/tpch.jar -b tpch -o output -s 100 --config config/tpch_config.xml --load true --execute false")
    
    # vaccum 
    executeCommand("psql -h %s -p %s -U postgres -d test -f scripts/sqls/util_sqls/vacuum.sql" %(pgIp, pgPort))
    
    executeCommand("clear")
    
    print("benchmark preparation finish")
    
def runBenchmark(propFile = DEFAULT_TPCH_CONFIG):
        
    properties = loadPropertyFile(propFile)
    # Username for ssh-ing.
    # username = properties['username']
    # Name of the experiment that will be run
    experimentName = properties['experiment_name']
    
    pgPort = properties['pg_port']
    pgIp = properties['pg_ip']
    
    # mkdir tmp for the experiment.
    # tmpResultPath =  Path("scripts/tmp/") / experimentName
    # executeCommand("mkdir -p %s" % tmpResultPath)
     
    #
    sqlsPath = properties['sqls_path']
    results = []
    for i in range(1,23):
        queryName = "Q%d.sql" % i
        queryTimes = []
    
        for j in range(4):
            output = executeCommandWithOutputReturn("psql -h %s -p %s -U postgres -d test -f %s/%s" %(pgIp, pgPort, sqlsPath, queryName))

            # print(queryNazme)
            # print(output)
            time_match = re.match(r'.*Time: ([0-9\.]*) ms.*', str(output))
            assert(time_match is not None)
            queryTime = time_match.group(1)
            queryTimes.append(queryTime)
            time.sleep(1)
            
        # print(queryTime)
        results.append({
            "query": i, 
            "times": queryTimes ## need to compute average
        })
        
    print(results)
    return results
    dataFile = tmpResultPath
    saveData(results, "scripts/tmp", experimentName)

def graphData(propFile = DEFAULT_TPCH_CONFIG):
   
    properties = loadPropertyFile(propFile)
    figure = properties["figure"]
    # homePath = Path(properties["home_path"]) 
    homePath = Path('.') 
    scriptPath = homePath / 'scripts'
    paperFigDir = scriptPath / 'figures' / 'paper'
    artifactFigDir = scriptPath / 'figures' / 'artifact'
    cmpFigDir = scriptPath / 'figures' 
    #
    # directory structure
    # scripts
    #   - figures           result figures
    #       - paper         figures in paper
    #       - artifact      figures in AE 
    #   - tmp               AE experiment data
    # add elif statement here add new pictures. 
    
    if figure == "record":
        figName = 'record.eps'
        pdfName = 'record.pdf'
        title = "Figure record"
        script = scriptPath / 'fig' / 'hedb-plot.py'
        paperDataFile =  scriptPath / 'fig' / 'paper-data.xlsx'
        artifactDataFile = scriptPath / 'tmp' / 'record.xlsx'
    elif figure == "replay":
        figName = 'replay.eps'
        pdfName = 'replay.pdf'
        title = "Figure replay"
        script = scriptPath / 'fig' / 'hedb-plot.py'
        paperDataFile =  scriptPath / 'fig' / 'paper-data.xlsx'
        artifactDataFile = scriptPath / 'tmp' / 'replay.xlsx'
    elif figure == 'optimization':
        figName = 'optimization.eps'
        pdfName = 'optimization.pdf'
        title = "Figure optimization"
        script = scriptPath / 'fig' / 'hedb-plot.py'
        paperDataFile =  scriptPath / 'fig' / 'paper-data.xlsx'
        artifactDataFile = scriptPath / 'tmp' / 'optimization.xlsx'
    elif figure == 'desenitize':
        figName = 'desenitize.eps'
        pdfName = 'desenitize.pdf'
        title = "Figure desenitize"
        script = scriptPath / 'fig' / 'hedb-plot.py'
        paperDataFile =  scriptPath / 'fig' / 'paper-data.xlsx'
        artifactDataFile = scriptPath / 'tmp' / 'desenitize.xlsx'
    else:
        print("Unsupported figure:", figure)
        return    
   
    executeCommand(f'mkdir -p scripts/figures/paper')
    executeCommand(f'mkdir -p scripts/figures/artifact')
 
    # graph in paper
    cmd = f'python3 {script} -l -t "Paper {title}" {figure} {paperDataFile} {paperFigDir / figName}'
    executeCommand(cmd)
    cmd = f'epstopdf {paperFigDir / figName}'
    executeCommand(cmd)
    
    # graph in AE
    cmd = f'python3 {script} -l -t "Artifact Evaluation" {figure}  {artifactDataFile} {artifactFigDir / figName}'
    executeCommand(cmd)
    cmd = f'epstopdf {artifactFigDir / figName}'
    executeCommand(cmd)
    
    # jam two figures together
    cmd = f"pdfjam {artifactFigDir / pdfName} {paperFigDir / pdfName} --landscape --nup 2x1 --outfile {cmpFigDir / pdfName}"
    executeCommand(cmd)

def shutdownVMs():
    cmd = r"ps aux | grep ./qemu-system-aarch64 | awk '{print $2}' | head -n -1 | xargs kill -9"
    executeCommand(cmd)
    print("qemu vm shut down")
    
def cleanLogs():
    print("clean logs")

def cleanupExperiment():
    shutdownVMs()
    cleanLogs()