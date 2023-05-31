# not used file
# not used file 
# not used file 
#!/bin/python3
#
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import sys
import argparse

from util_py3.data_util import *
from util import parse_args


if len(sys.argv) <= 1:
    print('input argument')
    exit


parser = argparse.ArgumentParser(description='basic graph plot')
args = parse_args(parser)
in_name, out_name = args.input, args.output

data = loadData("scripts/tmp/%s" % in_name)

print(data)

## plotting logic 


# generate graph to out_name