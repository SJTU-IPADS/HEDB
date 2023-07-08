#!/usr/bin/python3

import sys
from z3 import *
import io
import time
n = len(sys.argv)
assert(n >= 3)

smt2_file = sys.argv[1]
iters = int(sys.argv[2])
filename = "desen.log"
# set_param("parallel.enable", True)

s = Solver()
constraints = parse_smt2_file(smt2_file)
# print(constraints)

s.add(constraints)

# loop for $iter different solutions
for i in range(iters):
    if s.check() != sat:
        print("not able to generate enough output")
        exit(1)

    m = s.model()

    if not m:
        print("not able to generate enough output")
        exit(1)

    c = []
    for v in m:
        name = v.name()
        if name != "model_version" and name != "op" and name != "type" and name != "array-ext":
            c.append((v() != m[v]))  
    # print(c)
    s.add(Not(And(c)))


def print_to_string(*args, **kwargs):
    output = io.StringIO()
    print(*args, file=output, **kwargs)
    contents = output.getvalue()
    output.close()
    return contents

out = ""

# print result
for v in m:
    name = v.name()
    if name != "model_version" and name != "op" and name != "type" and name != "array-ext":
        out += print_to_string(name, m[v], sep=",") + ";"

out = out.replace("\n", "").replace(" ","")
print(out)
# with open(filename, 'a') as f:
#     f.write(out + "\n")