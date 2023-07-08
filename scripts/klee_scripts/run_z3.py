#!/usr/bin/python3

import sys
from z3 import *
import io
import time
from os import listdir
from os.path import isfile, join
import re 
import hashlib

# smt2_file = sys.argv[1]
filename = "desen.log"
# set_param("parallel.enable", True)

def solve(smt2_file, iters):
    s = Solver()
    constraints = parse_smt2_file(smt2_file)
    # print(constraints)

    s.add(constraints)
    
    # loop for $iter different solutions
    for i in range(iters):
        if s.check() != sat:
            print("not able to generate enough output 1" + smt2_file)
            exit(1)

        m = s.model()

        if not m:
            return "no constraints"
            

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
    return out
# with open(filename, 'a') as f:
#     f.write(out + "\n")

def calc_hash(file):
    hash_md5 = hashlib.md5()
    with open(file, "rb") as f:
        for byte_block in iter(lambda: f.read(4096),b""):
            hash_md5.update(byte_block)
    return hash_md5.hexdigest()

if __name__ == "__main__":
    n = len(sys.argv)
    assert(n >= 2)
    smt2_dir = sys.argv[1]
    outputfile_name = sys.argv[2]
    files = [f for f in listdir(smt2_dir) if f.endswith(".smt2")]
    print("files num,", len(files))
    def atoi(text):
        return int(text) if text.isdigit() else text
    def natural_keys(text):
        return [ atoi(c) for c in re.split(r'(\d+)', text)]
    files.sort(key=natural_keys)

    cache = {}
    hit = 0
    miss = 0
    print("files num", len(files))
    with open(outputfile_name, "w+") as f:
        for file in files:
            filepath = join(smt2_dir, file)
            # print(filepath)
            if 0: # this branch is for cached
                file_hash = calc_hash(filepath)
                if file_hash not in cache:
                    miss += 1
                    res = solve(filepath, 1)
                    cache[file_hash] = res 
                else :
                    hit += 1   
                f.write(cache[file_hash]+"\n")    
            else: # this branch is no optimize
                f.write(solve(filepath, 1)+"\n")    

            
    # print("done hit: ",hit, ", miss: ",miss)
        
        
