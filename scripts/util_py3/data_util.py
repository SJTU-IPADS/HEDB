#!/usr/bin/python3

####### Important Functions include:

import pandas as pd
import numpy as np
import sys
import os
import time
import datetime

# data show be a dict: 
#   key : [values...]
def saveData(data, filename): 
    # for k,v in data.items():
    dataframe = pd.DataFrame.from_dict(data)
    print(data)
    dataframe.to_csv(filename, index=False, sep=',')

# returned data show be a dict: 
#   key : [values...]
def loadData(filename): 
    # for k,v in data.items():
    dataframe = pd.read_csv(filename)
    return dataframe.to_dict()