# HEDB

![Status](https://img.shields.io/badge/Version-Experimental-green.svg)
[![License: MIT](https://img.shields.io/badge/License-Mulan-brightgreenn.svg)](http://license.coscl.org.cn/MulanPubL-2.0)

HEDB is a dual-mode encrypted database that removes the tension between security and maintenance. Its dual-mode design accomplishes two goals: 1) achieving interface security by preventig illegal invocations to operators in the execution mode, and 2) allowing DBA common maintenance tasks by replaying legal invocations in the maintenance mode.

Currently, HEDB is based on PostgreSQL and supports TPC-H workloads.

- [HEDB](#hedb)
  - [Paper](#paper)
  - [Artifact summary](#artifact-summary)
    - [Artifact check-list](#artifact-check-list)
    - [Supported platform](#supported-platform)
    - [Repo structure](#repo-structure)
  - [Environment setup on host (Optional)](#environment-setup-on-host-optional)
    - [Prerequisites](#prerequisites)
    - [Create the extension](#create-the-extension)
    - [Build \& install the extension](#build--install-the-extension)
  - [Experiments](#experiments)
    - [Claims](#claims)
    - [Experiment 1: End-to-end performance (1.5 hours)](#experiment-1-end-to-end-performance-15-hours)
    - [Experiment 2: Record overhead (40 mins)](#experiment-2-record-overhead-40-mins)
    - [Experiment 3: Replay overhead (50 mins)](#experiment-3-replay-overhead-50-mins)
    - [Experiment 4: Anonymized log generation time (4 hours)](#experiment-4-anonymized-log-generation-time-4-hours)
  - [Limitations](#limitations)
  - [Contacts](#contacts)

## Paper

* [Encrypted Databases Made Secure Yet Maintainable](https://www.usenix.org/conference/osdi23/presentation/li)<br>
Mingyu Li, Xuyang Zhao, Le Chen, Cheng Tan, Huorong Li, Sheng Wang, Zeyu Mi, Yubin Xia, Feifei Li, Haibo Chen<br>
The 17th USENIX Symposium on Operating Systems Design and Implementation (OSDI ‘23)

## Artifact summary

This artifact contains the implementation of HEDB and scripts for reproducing the main performance results of this work. The whole evaluation takes about 3 hours on an ARM server and about 4 hours on a x86 machine.

### Artifact check-list

- OS Version: Ubuntu 20.04
- Linux kernel version: >= 5.4
- Python version: >= 3.6
- Expected time: see the documents or runtime logs for each experiment.

### Supported platform

HEDB requires an ARM server that supports S-EL2, a hardware virtualization technology in ARM TrustZone. To reproduce the performance evaluation results, you can run HEDB using two VMs on an ARM server.

### Repo structure

```
Github Repo Root
├── Makefile
├── benchmark               # Data loader and data generator.
│   ├── bin                 # Executable jar files for loading data in database.
│   ├── config              # Configuration files for running the executable jar files.
│   ├── db_schemas          # Sql files for loading tpch schema.
│   └── tools               # Data generator.
├── klee_scripts            # Scripts for anonymized replay using KLEE.
├── scripts                 # Scripts for authenticated replay and others.
│   ├── config              # Json files for running different experiments.
│   ├── eval_AE_time.sh     # The script for running the experiments and getting the running time.
│   ├── fig                 # The scripts for plotting the figure.
│   ├── figures             # Output figures of the experiments.
│   ├── run_experiment.py   # The script for selecting and run an experiment.
│   ├── run_test.sh         # The script for running a series of Sqls.
│   ├── sqls                # Sql files for different experiments.
│   ├── tmp                 # Immediate files of the experiments.
│   └── util_py3            # Utils used by the scripts.
└── src                     # HEDB engine code base.
    ├── cmake               # Automated makefiles.
    ├── enclave             # Confidential operators protected by trusted domains.
    ├── extension           # Postgresql user-define functions for encrypted database.
    ├── include             # HEBD engine headers such as encrypted datatypes.
    └── utils               # Utils such as debugging required by HEDB engine.
```

## Environment setup on host (Optional)

### Prerequisites

Install Postgresql
```sh
sudo apt-get install postgresql postgresql-server-dev-all
```

### Create the extension
```sh
psql -U postgres -p 5432 -h localhost
```

```sql
CREATE USER test WITH PASSWORD 'password';
CREATE database test;
\c test;
CREATE extension encdb;
```

### Build & install the extension
```sh
make configure_sim
make
sudo make install
```

## Experiments

Unless otherwise specified, all HEDB's experiments run on top of a Kunpeng 96-core ARMv8.2 CPU machine with two VMs. Each experiment runs 3 times to warm up by default.

### Claims
- **(C1)** Figure 4: Type-II EDB's runtime overhead varies amongst TPC-H 22 queries. HEDB's optimizations speeds Type-II EDB.
- **(C2)** Figure 5 (a): HEBD's record overhead is low and acceptable.
- **(C3)** Figure 5 (b): HEBD's replay overhead is much faster than operator-based replay.
- **(C4)** Figure 5 (c): HEDB's optimizations boost the anonymized log generation time.

### Experiment 1: End-to-end performance (1.5 hours)

This experiment runs HEDB and three optimization strategies (i.e., O1-Parallel Decryption, O2-
Order-revealing Encryption, and O3-Expression Evaluation) over TPC-H. The output reports each
system's latency normalized to the SOTA: the ARM-version StealthDB as the baseline. 

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

### Experiment 2: Record overhead (40 mins)

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

### Experiment 3: Replay overhead (50 mins)

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

### Experiment 4: Anonymized log generation time (4 hours)

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

## Limitations

This repo is a research prototype rather than a production-ready system. Its current implementation has several main limitations.

1. HEDB relies on record-and-replay to reproduce the DBMS bugs, hence falling short in providing read-write transactional workloads such as TPC-C.
2. HEDB depends on KLEE to reproduce the operators bugs. The official version of KLEE cannot support floating-point numbers. HEDB inherits this limitation.
3. HEDB assumes an ARMv8.4 server with S-EL2 (TrustZone Virtualization) to support its dual-mode design. This design can be ported to other architectures (e.g., Intel SGX, AMD SEV-SNP, Intel TDX and ARMv9 CCA). Please refer to `docs/build-from-scratch.md` to replicate HEDB's performance results on Intel SGX (using SGX SDK) and ARM TrustZone (using OP-TEE). Note that the cross-domain VM fork (i.e., mode switch) on these architectures is not implemented and remains an open question.

## Contacts

- Mingyu Li: maxulle [at] sjtu [dot] edu [dot] cn
- Xuyang Zhao: brokenbones [at] sjtu [doc] edu [doc] cn
- Le Chen: cen-le [at] sjtu [dot] edu [dot] cn