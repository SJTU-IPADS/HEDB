# Anonymized log generation guide

For the maitainence of HEDB UDFs, which perform computation on user's plaintext private data, we use concolic execution and constraint solving to generate anonymized logs that reproduce the control flow, and use masking rules to guarantee user's private data is not leaked. 

To demonstrate the idea and how our tool works, let's use a real bug on postgresql [prefix extension](https://github.com/dimitri/prefix/issues/13) as an example.


## 1. Buggy pr_inter implementation.
```cpp
static inline
prefix_range *pr_inter(prefix_range *a, prefix_range *b) {
  ...
  else if( gplen == alen && alen == blen ) {
    res = build_pr(gp,
		   a->first > b->first ? a->first : b->first,
		   a->last > b->last ? a->last : b->last      (1)
           a->last > b->last ? b->last : a->last      (2)
        );}
  ...
  return pr_normalize(res);
}
```
The bug is a simple mistake on line (1), which should be changed to line (2)

Assume we have a user's input pair (1398570201[2-7], 1398570201[4-5]) which are two private phone number ranges, so can not be directly given to DBA for maintainance.

## 2. Generation encrypted log.

We should first log down encrypted inputs. In psql, execute `select enable_record_mode('$log_name')` before issue queries, then encrypted inputs of all UDF executed will be logged down in `$log_name` file in postgresql directory.

## 3. Generate anonymized log.

This consists of 3 steps, which are combined in scripts `scripts/klee_scripts/desenitize.sh`

1. decrypt inputs and convert to klee input.

We use klee to do concolic execution, and it only recognize ktest files, so which should first convert original input to ktest file.

2. use klee to generate path constraint.

KLEE is a symbolic executor which can be used to retrive path constraint in seed mode. Several argument should be set to enable it.
- --write-smt2s: output .smt2 constraint file
- --seed-file=${filename}: indicate a concrete input (ktest file generated in previous step )
- --only-replay-seeds: do not explore other control flow

3. use z3 to solve contraint and get anonymized log.

Z3 is a constraint solver that compute solutions given constraints. 
In this step, we have two sources of constraints. First, **path constraints** from previous step, that describe the control flow. Second,  **masking rule constraints** that guarantee user's private data is not leaked.

In this example, path constraints shows that two inputs are consists of characters from `0` to `9`, and first range `properly include` second range. 
Because the inputs are phone number, so corresponding masking rule constraints masks four characters in the middle. For example, in `1398570201[2-7]`, `8570` should be masked and these four character should equal to `'*'`.

Then path constraints and masking rule constraints are both feed to z3 to solve. Z3's solution is the anonymized log, because it can reproduce the control flow, while guarantee user's privacy demand.

