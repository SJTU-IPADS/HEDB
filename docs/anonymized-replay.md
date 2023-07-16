# Anonymized log generation guide

> Art is to make the invisible visible.

For the maintenance of HEDB Ops that computes over user secrets, we use concolic execution and constraint solving to generate anonymized logs that reproduce the control flow, and leverage masking rules to guarantee user secrets are not leaked.

To demonstrate how our tool works, let's use a real bug on the [prefix extension](https://github.com/dimitri/prefix/issues/13) of PostgreSQL as an example.


## 1. Buggy pr_inter implementation
```cpp
static inline
prefix_range *pr_inter(prefix_range *a, prefix_range *b) {
  ...
  else if( gplen == alen && alen == blen ) {
    res = build_pr(gp,
       a->first > b->first ? a->first : b->first,
(1)    a->last > b->last ? a->last : b->last
(2)    a->last > b->last ? b->last : a->last
        );}
  ...
  return pr_normalize(res);
}
```
The bug is caused by line (1), which should be changed to line (2).

Assume we have a user's inputs (`1398570201`[2-7], `1398570201`[4-5]) which are two private phone number ranges, which can not be exposed to DBAs.

## 2. Creating encrypted logs

We should first record encrypted inputs. In HEDB, execute `select enable_record_mode('$log_name')` before issue queries, then encrypted inputs of all Ops executed will be logged down in `$log_name` file in postgresql directory.

## 3. Synthesizing anonymized logs

This consists of 3 steps, which are combined in scripts `scripts/klee_scripts/desenitize.sh`

**Step-1: Decrypt inputs and convert them to KLEE inputs.**

We use KLEE for concolic execution. As it only recognizes the ktest files, we convert the original input to ktest files.

**Step-2: Use KLEE to generate path condition constraints.**

KLEE is a symbolic executor that can retrieve path conditions in seed mode. Several arguments should be set to enable it.
```sh
--write-smt2s: output .smt2 constraint file
--seed-file=${filename}: indicate a concrete input (ktest file generated in the previous step)
--only-replay-seeds: do not explore other control flow
```

**Step-3: Use Z3 to solve constraints and synthesize anonymized logs.**

Z3 is a constraint solver that computes solutions given constraints. In this step, we have two sources of constraints. First, **path condition constraints** from the previous step describe the control flow. Second,  **masking rule constraints** that guarantee user's secrets are not leaked.

In this example, path condition constraints show that two inputs consist of characters from `0` to `9`, and the first range `properly include` the second range. Because the inputs are phone number, so corresponding masking rule constraints mask four characters in the middle. For example, in `1398570201[2-7]`, `8570` should be masked and these four characters should equal to `'*'`.

The path condition constraints and masking rule constraints are both feed to z3 to solve. Z3's solution is exactly the anonymized inputs that can translate the encrypted logs into anonymized logs.
