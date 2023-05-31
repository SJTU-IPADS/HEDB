#!/bin/bash 

/usr/bin/time -v python3 ./scripts/run_experiment.py -f record
python3 ./scripts/run_experiment.py -c 

/usr/bin/time -v python3 ./scripts/run_experiment.py -f replay
python3 ./scripts/run_experiment.py -c 

/usr/bin/time -v python3 ./scripts/run_experiment.py -f base 
python3 ./scripts/run_experiment.py -c 
