# Anonymized log generation guide

> Art is to make the invisible visible.

For the maintenance of HEDB Ops that computes over user secrets, concolic execution and constraint solving are combined to synthesize the anonymized logs that can reproduce the control flow, and masking rules are involved to guarantee user secrets are not leaked.

To demonstrate how the tool works, let us use a real bug on the [prefix extension](https://github.com/dimitri/prefix/issues/13) of PostgreSQL as an example.

## 1. A buggy use case
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

## 2. Recording encrypted logs

The first thing is to record the encrypted inputs and their operations. To do so, you need to execute `select enable_record_mode('$log_name')` before issuing each queries, then the executed Ops will be logged down in `$log_name` file.

## 3. Synthesizing anonymized logs

This consists of 3 steps, which are combined in scripts `scripts/klee_scripts/desenitize.sh`

**Step-1: Decrypt inputs and convert them to KLEE inputs**

We use KLEE for concolic execution. As it only recognizes the ktest files, we convert the original input to ktest files.

**Step-2: Use KLEE to generate path condition constraints**

KLEE is a symbolic executor that can retrieve path conditions in seed mode. Several arguments should be set in order to enable the seed mode.
```sh
--write-smt2s: output .smt2 constraint file
--seed-file=${filename}: indicate a concrete input (ktest file generated in the previous step)
--only-replay-seeds: do not explore other control flow
```

**Step-3: Use Z3 to solve constraints and synthesize anonymized logs**

Z3 is a constraint solver that computes solutions given constraints.

In this step, we have two sources of constraints. First, ***path condition constraints*** from the previous step describe the control flow. Second,  ***masking rule constraints*** are leveraged to ensure user's secrets are not leaked.

In this example, path condition constraints show that two inputs consist of characters from `0` to `9`, and the first range `properly include` the second range. Because the inputs are phone number, so corresponding masking rule constraints mask four characters in the middle. For example, in `1398570201[2-7]`, `8570` should be masked and these four characters should equal to `'*'`.

The path condition constraints and masking rule constraints are both feed to z3 to solve. Z3's solution is exactly the anonymized inputs that can translate the encrypted logs into anonymized logs.

## Masking rule examples

| DataType | TPC-H Field                              | Rule                                           |
|----------|------------------------------------------|------------------------------------------------|
| Int      | quantity, balance, prices                | `2020-06-01 01:02:03` -> `2020-06-01 00:00:00` |
| Date     | o_orderdate, l_commitdate, l_receiptdate | `12345` -> `12000`                             |
| Text     | c_phone                                  | `1234567890` -> `1***567890`                   |

The above rules are under refactoring and have not yet been included in the `main` branch.

To learn more rules, we recommend you to refer to [PostgreSQL Anonymizer](https://postgresql-anonymizer.readthedocs.io/), an extension to mask or replace personally identifiable information (PII) or sensitive data from a PostgreSQL database.