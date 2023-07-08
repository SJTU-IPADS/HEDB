### Natacha Crooks - 2014
### Contains utility functions related to
### dealing with property files
############################################

import json
import os
import subprocess
from pprint import pprint


def writePropFile(propertyFile, properties):
    with open(propertyFile, 'w') as fp:
        json.dump(properties, fp, indent=2, sort_keys=True)


## Returns property file as
## a python dictionary.
## Throws an exception if
## file does note xist and returns
## empty dict
def loadPropertyFile(pFileName):
    data = {}
    try:
        json_data = open(pFileName)
    except:
        print("File does not exist " + pFileName)
    try:
        data = json.load(json_data)
    except:
       print("Incorrect JSON Format  " + pFileName)
       raise
    return data

## Prints Content of Json
## property file

def printPropertyFile(pFileName):
    try:
        json_data = open(pFileName)
        data = json.loads(json_data)
        pprint(data)
        json_data.close()
    except:
        print("File does not exist")

## Generates a property file
## with corresponding file name
## from directionary
def generatePropertyFile(dic, filename):
    with open(filename,'wb') as f:
        json.dump(dic,f)

def toBool(st):
    if st == "true":
        return True
    else:
        return False

## Get GIT Revision Hash
def getGitHash(projectpath):
  currentdir = os.getcwd();
  os.chdir(projectpath)
  hash = subprocess.check_output(['git', 'rev-parse', 'HEAD'])
  os.chdir(currentdir)
  return hash
