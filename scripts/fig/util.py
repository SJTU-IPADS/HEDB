import json
import math
import random
from collections import defaultdict

# input: excel data file 
# output: eps file
# other args: as below
def parse_args(parser):
    parser.add_argument('figure', type=str, help='figure to plot')
    parser.add_argument('input', type=str, help='input data')
    parser.add_argument('output', type=str, help='output file')
    # parser.add_argument('-b', '--baseline', help='baseline data')
    parser.add_argument('-t', '--title', help='set graph title')
    parser.add_argument('-l', '--large', action='store_true',
                        help='output large graph (default: false)')
    args = parser.parse_args()
    return args
