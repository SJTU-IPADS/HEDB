# Artifact summary

Note: this is a archive of artifact evaluation.

This artifact contains the implementation of HEDB and scripts for reproducing the performance results. The whole evaluation takes about 3 hours on an ARM server and about 4 hours on an x86 machine.

## Artifact check-list

- OS Version: Ubuntu 20.04
- Linux kernel version: >= 5.4
- Python version: >= 3.6
- Expected time: see the documents or runtime logs for each experiment.

# Experiments

Unless otherwise specified, all HEDB's experiments run on top of a Kunpeng 96-core ARMv8.2 CPU machine with two VMs. Each experiment runs 3 times by default.

## Claims
- **(C1)** Figure 4: Type-II EDB's runtime overhead varies amongst TPC-H 22 queries. HEDB's optimizations speeds up Type-II EDB.
- **(C2)** Figure 5 (a): HEBD's record overhead is low and acceptable.
- **(C3)** Figure 5 (b): HEBD's replay overhead is much faster than operator-based replay.
- **(C4)** Figure 5 (c): HEDB's optimizations boost the anonymized log generation time.

## Experiment 1: End-to-end performance (1.5 hours)

This experiment runs HEDB and three optimization strategies (i.e., O1: Parallel Decryption, O2: 
Order-revealing Encryption, and O3: Expression Evaluation) over TPC-H. The output reports each
system's latency normalized to an ARM version of [StealthDB](https://github.com/cryptograph/stealthdb) as the baseline. 

**Command to run:**

```shell
python3 ./scripts/run_experiments.py -f fig4
```

**Output:**

- `scripts/figures/optimization.pdf`

**Expected results:**

- O1 can improve all queries and reduce around 15% end-to-end query execution time on average (matching **C1**).
- O2 makes the Q1’s overhead decreased by around 50% (matching **C1**).
- O3 optimizes Q1’s to decrease its overhead by around 10% (matching **C1**).

**Important notes of Experiment 1:**

- The reproduced results may not exactly match the results presented in the paper due to the noise nature of system such as scheduling, disk I/O, etc.

## Experiment 2: Record overhead (40 mins)

This experiment runs HEDB with log recording enabled, for replaying and debugging later on.

**Command to run:**

```shel
python3 ./scripts/run_experiments.py -f fig5a
```

**Output:**

- `scripts/figures/record.pdf`.

**Expected results:**

- HEDB's record incurs overhead no more than 10% (matching **C3**).

**Important notes:**

- Owing to the randomness, the results may not exactly match the origial.

## Experiment 3: Replay overhead (50 mins)

This experiment runs HEDB and re-executes TPC-H queries by replaying logs. The less time cost the better, which saves the DBA’s time and effort.

**Command to run:**

```shell
python3 ./scripts/run_experiments.py -f fig5b
```

**Output:**
- `scripts/figures/replay.pdf`.

**Expected results:**

- HEDB’s log-based replay is faster than operator-based replay (by honestly calling operators), effectively saving the DBA’s time and effort (matching **C3**).
- HEDB’s replay still incurs 5x slowdown compared with the vanilla without encryption (matching **C3**).

## Experiment 4: Anonymized log generation time (4 hours)

This experiment translates the log recorded in Experiment 2 into an anonymized form. It firsr uses KLEE to collect the path constraint, and then uses masking rule constraint to generate new inputs.

This experiment can run on your local x86 machine in docker: `docker pull zhaoxuyang13/klee-desen:1.0`.
You should have **docker** environment available.

**Command to run:**

```shell
git clone (this repo)

# install dependencies for drawing graph
sudo apt install python3 python3-pip texlive-font-utils texlive-extra-utils 
pip3 install tqdm seaborn numpy pandas openpyxl matplotlib

# if you want this experiment to be faster (e.g. from 4h to 1h), 
# you could change file 'scripts/desenitize_test.sh' line-22 
# change "--entrypoint /home/klee/entrypoint.sh" to "--entrypoint /home/klee/entrypoint-without-one.sh", then run the experiment
# this should skip Q1, which takes over 3 hours.
python3 ./scripts/run_experiments.py -f fig5c

```

**Output:**

- `scripts/figures/desenitize.pdf`.

**Expected results:**

- Baseline of KLEE/Z3 estimation is very long (matching **C4**).
- HEDB's optimization achieves 10x to 200x (matching **C9**). 

**Important notes:**

- We do not run KLEE/Z3 using the log, as it takes too long. Instead, we measure the cost for one invocation and multiply it by the invocation frequency.
- KLEE does not support floating-point numbers, hence Q18 is not supported.
