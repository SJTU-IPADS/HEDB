<p align="center">
	<img src="scripts/figures/hedb.jpg" width = "300" height = "200" align=center />
</p>

![Status](https://img.shields.io/badge/Version-Experimental-green.svg) [![License](https://img.shields.io/badge/License-Mulan-brightgreenn.svg)](http://license.coscl.org.cn/MulanPubL-2.0)

HEDB is a PostgreSQL extension to compute queries over ciphertexts, with a suite of maintenance tools.

## Prerequisite

- CMake version: >= 3.10
- Postgres version: >= 17.6
- OS version: Ubuntu >= 24.04
- Linux kernel version: >= 6.17
- Python version: >= 3.13

## Quick Start

Install the dependencies:
```bash
$ sudo apt update
$ sudo apt install -y build-essential cmake libmbedtls-dev
$ sudo apt install -y postgresql postgresql-contrib postgresql-server-dev-all
$ sudo service postgresql restart
```

Pull the HEDB repo, build and install:
```bash
$ git clone -b main --depth 1 https://github.com/SJTU-IPADS/HEDB
$ cd HEDB
$ make
$ sudo make install
$ make run

$ sudo -u postgres psql
```

Run your 1st SQL:
```sql
CREATE EXTENSION hedb;

DROP TABLE IF EXISTS test;
CREATE TABLE test (a int, b enc_int4);

SELECT enable_client_mode();            --- use client mode to insert user value
INSERT INTO test VALUES (1, '1'::enc_int4); --- note that encrypted data is inserted as string
INSERT INTO test VALUES (2, '2'::enc_int4); --- note that encrypted data is inserted as string
SELECT * FROM test;

SELECT enable_server_mode();            --- use server mode for database admins (DBAs) to maintain the database
SELECT * FROM test;
```

There are 4 encrypted datatypes to protect the data you select in PostgreSQL. To learn more about their usage, see [tests/unit-test](https://github.com/SJTU-IPADS/HEDB/blob/main/tests/unit-test/unit-test.sql).

| data type | encrypted data type |
|-----------|---------------------|
| int       | enc_int4            |
| float     | enc_float4          |
| text      | enc_text            |
| timestamp | enc_timestamp       |

In fact, the above setting is ***NOT secure*** at all.

Here is a quick overview for newcomers to understand the purpose of HEDB (*10-minute reading*).

## Encrypted Databases

Databases may contain sensitive data, and can be outsourced to third parties to manage, optimize, and diagnose, called database-as-a-service (DBaaS). To protect sensitive data in use, secrets should be kept encrypted as necessary, resulting in an encrypted databases (EDB). EDB systems help enterprises obey data protection laws such as EU GDPR, US HIPAA, US PCI/DSS, PRC DSL, PRC PIPL, etc.

Two types of EDBs are established so far.

<p align="center">
  <img src="scripts/figures/types.jpg" width = "760" height = "180" align=center />
</p>

+ **Type-I EDB**: To build an EDB, one can [place an entire database inside an isolated domain](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/type-1.md), or confidential computing unit (as in Intel SGX, AMD SEV, Intel TDX, ARM Realm, IBM PEF, AWS Nitro, Ant HyperEnclave, or whatever you name it). However, Type-I EDB should prevent database admins (or DBAs in short) from managing the database. If DBAs were able to log into the DBMS, they would inspect user data.
+ **Type-II EDB**: Cloud DBaaS vendors such as Azure, Alibaba, Huawei and others provision operator-based EDBs. You can dive into the source code to navigate how to build such an EDB using PostgreSQL's [user-defined types (UDTs)](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/hedb--1.0.sql) and [user-defined functions (UDFs)](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/udf). Type-II EDB allows DBAs to log into the database, but keeps data always in ciphertext (at rest on disk, in transit over network, and in use in memory) to avoid potential leakage.

However, for Type-II, we have discovered a type of attack named "Smuggle". You can learn how it works in [tools/smuggle.py](https://github.com/SJTU-IPADS/HEDB/blob/main/tools/smuggle.py), which recovers an integer column in TPC-H. The reason why Smuggle exists is that the Type-II EDB exposes sufficient expression operators for admins to construct oracles.

## Smuggle Attacks

Here is a minimal working example.

1. **Constructing oracles**:
   1. With ÷, DBAs obtain the ciphertext of '1' by dividing a number by itself.
   2. With '1', DBAs construct all encrypted integers by iteratively + the cipher '1' to a counter.

2. **Recovering secrets**: With =, DBAs recover encrypted values by comparing them with known ciphertexts.

## HEDB as a Solution

<p align="center">
  <img src="scripts/figures/arch.jpg" width = "500" height = "260" align=center />
</p>

HEDB splits the running mode of an EDB into two: *Execution Mode* for users, and *Maintenance Mode* for admins. HEDB is named after Helium, implying its two modes. HEDB removes the tension between security and maintenance.

1) *Execution Mode* prevents Smuggle attacks by blocking non-user operator invocations,
2) *Maintenance Mode* allows admins to perform maintenance tasks by replaying invocations.

### Defense

To launch HEDB, you need to use two confidential units (e.g., secure enclaves, confidential processes or virtual machines) as the setting, one as [Integrity Zone](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone) and the other as [Privacy Zone](https://github.com/SJTU-IPADS/HEDB/tree/main/src/privacy_zone). In fact, HEDB executes these two zones using two OS processes, and leverages [inter-zone shared memory](https://github.com/SJTU-IPADS/HEDB/tree/main/tools/drivers/ivshmem-driver) for fast communication.

We recommend you to use two confidential VMs (CVMs), as evaluated in the paper. For those who do not have CVMs computers (e.g., ARM CCA, AMD SEV, Intel TDX), you can use 2 QEMU-KVM VMs to simulate CVMs. Depending on your computer architecture, either choose [vm-setup-aarch64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-aarch64.md) or [vm-setup-x86_64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-x86_64.md). These tutorials will guide you on how to create 2 VMs that host DBMS and operators, separately, and how to perform a mode switch using QEMU-based VM snapshotting.

The mode switch forks a CVM, creates a snapshot and places it in the Management Zone that admins are able to access. A security guarantee is that DBAs cannot log into the CVMs, including Integrity Zone and Privacy Zone. HEDB records operator invocations in the Integrity Zone (*Execution Mode*) and replays it in the Management Zone (*Maintenance Mode*).

### Record/Replay

HEDB [record/replay](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/record_replay) is meant for reproducing bugs. It logs all ops invocations, including parameters and results, for later replays. We use TPC-H as the demonstrative benchmark.

For privacy reasons, SQL constants should be encrypted in advance. Since the current implementations lack client-side encryption, we should transform the constants into encrypted values using operators in the client mode. A future work is to seek and implement a simple client-side encryption or proxy-side encryption.

```bash
## make dependencies installed
$ pip3 install psycopg2 tqdm

# go to TPC-H test dir
$ cd tests/tpch

# update the password of the postgres user
$ sudo -u postgres psql
postgres=# ALTER USER postgres WITH PASSWORD 'postgres';

## generate and load data
$ python3 run.py --load

## transform TPC-H queries into secure SQLs, which generates `cipher-query`
$ python3 run.py --transform

## record TPC-H
$ python3 run.py --skip-generate --record-replay record

## replay TPC-H
$ python3 run.py --skip-generate --record-replay replay
```

If you shut down the operators VM, you can still replay the queries. This is exactly how HEDB prevents Smuggle but is still able to reproduce bugs for DBAs.

### Hotfix

HEDB translates common DBA actions into hotfix templates.
HEDB runs a hotfix server inside the DBMS CVM, and allows a skillful DBA to instruct the server as a maintenance agent. Please read details in [tools/hotfix](https://github.com/SJTU-IPADS/HEDB/tree/main/tools/hotfix).

### Code Structure

```
├── docs                     # Most info you may want to know
├── src
│    ├── integrity_zone      # VM-#1
│    │    ├── interface      # 2-VM shared memory based communication
│    │    ├── ops_client     # Ops producer
│    │    ├── record_replay  # Ops invocation record-replay
│    │    └── udf            # PostgreSQL user-defined functions
│    ├── privacy_zone        # VM-#2
│    │    ├── enc_ops        # Ops consumer
│    │    ├── klee_wrapper   # For KLEE trace generator using real data as seed
│    │    └── plain_ops      # Ops that compute secret data, decoupled for KLEE
│    └── utils               # utility libraries
├── tests
│    ├── tpch                # Macro: TPCH tests
│    └── unit-test           # Micro: operator tests
└── tools
     ├── drivers
     │    ├── arm-pmu-driver # For AARCH64 CPU timestamp
     │    └── ivshmem-driver # For 2-VM shared memory
     └── hotfix
```

### Paper

* [Encrypted Databases Made Secure Yet Maintainable](https://www.usenix.org/conference/osdi23/presentation/li-mingyu), USENIX OSDI 2023 <br>

```bibtex
@inproceedings{li2023hedb,
  author     = {Mingyu Li and Xuyang Zhao and Le Chen and Cheng Tan and Huorong Li and Sheng Wang and Zeyu Mi and Yubin Xia and Feifei Li and Haibo Chen},
  title      = {Encrypted Databases Made Secure Yet Maintainable},
  booktitle  = {17th USENIX Symposium on Operating Systems Design and Implementation (OSDI 23)},
  pages      = {117--133},
  url        = {https://www.usenix.org/conference/osdi23/presentation/li-mingyu},
  isbn       = {978-1-939133-34-2},
  publisher  = {{USENIX} Association},
  address    = {Boston, MA},
  year       = {2023},
  month      = jul,
}
```

### FAQs

#### Q1: Is HEDB limited to ARM?

***Absolutely not!*** You can deploy it to whatever trusted execution environment (TEE) or confidential computing (CC) platform you like. For instance, confidential VM (CVM) is widely supported on modern trusted hardware, such as AMD SEV(-ES,-SNP), Intel TDX, IBM PEF, ARMv9 Realm. You can deploy HEDB's integrity zone (DBMS + extensions) using one CVM, and HEDB's privacy zone (operators) in another CVM. If you trust the hypervisor, like AWS Nitro Enclave, you can run them in two Nitro VMs. That's it!

To reproduce the performance evaluation results, you can run HEDB using two CVMs on a CC machine.

#### Q2: How to realize two modes?

Our prototype assumes an ARM server that supports S-EL2, a hardware virtualization technology present in ARMv8.4. [Twinvisor](https://github.com/TwinVisor/twinvisor-prototype) is an S-EL2 hypervisor developed by IPADS@SJTU. We plan to commit the Twinvisor patch, but no guarantee (for intellectual property reasons). You may also run two VMs atop Linux/KVM to implement two modes, as described in the [vm-setup.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-aarch64.md).

For other platforms such as AMD SEV, Intel TDX, IBM PEF, and ARM Realm, the task is to enable CVM fork or migration between trusted domains and untrusted domains. This task remains undone. If CVM fork/migration is needed in other scenarios, it could serve as a potential research area.

#### Q3: Supporting TPC-C?

The released code of HEDB is built on PostgreSQL. Its current record/replay supports the TPC-H benchmark only. We encourage future research to overcome this challenge posed by non-determinism when running TPC-C atop HEDB. We believe your excellent work will also be published and known to the industry.

### Anecdote

Why name HEDB (Helium Database)?

HE, short for [Helium](https://en.wikipedia.org/wiki/Helium), is the lightest neutral gas, known for its lacking reactivity and low density. The analogy to Helium highlights HEDB's ability to achieve isolation from the rest while maintaining simplicity in usage. The reference to Helium being the 2nd element also alludes to HEDB's dual modes.

HEDB is pronounced [haɪdiːbiː] or 嗨嘀哔.

### Note

This repository is a research prototype, not for production use. It is intended for experimentation, research, and education to enhance understanding of EDB internals.

## Maintainers

- Xuyang Zhao: https://github.com/zhaoxuyang13
- Le Chen: https://github.com/Casieee
- Mingyu Li: https://github.com/Maxul

## Acknowledgements

- Database and Storage Lab@Alibaba DAMO Academy, who provides insights from real-world DBA tasks.
- [StealthDB](https://github.com/cryptograph/stealthdb), who provides the initial version of EDB extensions for PostgreSQL.
