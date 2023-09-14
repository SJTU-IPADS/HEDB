# HEDB

![Status](https://img.shields.io/badge/Version-Experimental-green.svg)[![License: MIT](https://img.shields.io/badge/License-Mulan-brightgreenn.svg)](http://license.coscl.org.cn/MulanPubL-2.0)

<p align="center">
	<img src="scripts/figures/hedb.jpg" width = "300" height = "200" align=center />
</p>

HEDB is an extension of PostgreSQL to compute SQL over ciphertexts, in addition to a suite of maintenance tools.

*HEDB is an open project and highly values your feedback! We would like to hear your thoughts on our project and how we can improve it.*

## Quick Start

Install the dependencies:
```sh
$ sudo apt update
$ sudo apt install -y build-essential cmake libmbedtls-dev
$ sudo apt install -y postgresql postgresql-contrib postgresql-server-dev-all
$ sudo service postgresql restart
```

Pull the HEDB repo, build and install:
```sh
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

SELECT enable_client_mode();      --- use client mode to insert user value
INSERT INTO test VALUES (1, '1'); --- note that encrypted data is inserted as string
INSERT INTO test VALUES (2, '2'); --- note that encrypted data is inserted as string
SELECT * FROM test;

SELECT enable_server_mode();      --- use server mode for database admins (DBAs) to maintain the database
SELECT * FROM test;
```

There are currently four encrypted datatypes for you to selectively protect your data stored in PostgreSQL.
| data type | encrypted data type |
|-----------|---------------------|
| int       | enc_int4            |
| float     | enc_float4          |
| text      | enc_text            |
| timestamp | enc_timestamp       |

So far so good! **But** it is NOT secure at all!

Here is a quick overview for any newcomers to understand the purpose of HEDB. It would take 15 minutes for you.

## Encrypted Databases

Database systems may contain sensitive data, and some are outsourced to third-parties to manage, optimize, and diagnose, called database-as-a-service (DBaaS). To protect sensitive data in use, secrets should be kept encrypted as necessary.

**Option-1**: To build an encrypted database (EDB), one can place an entire database into an isolated domain, or confidential computing unit (like Intel SGX, AMD SEV, Intel TDX, ARM Realm, IBM PEF, AWS Nitro, Ant HyperEnclave, and whatever you name it). We call it Type-I EDB. Sadly, Type-I would prevent database admins, or DBAs, from managing the database, right? If DBAs were able to log into the DBMS, they would inspect any user data.

**Option-2**: Cloud DBaaS vendors such as Azure, Alibaba, Huawei and others provision operator-based EDBs. You can dive into [here](https://github.com/SJTU-IPADS/HEDB/blob/main/src/) to navigate how to build such an EDB using PostgreSQL' user-defined types (UDTs) and user-defined functions (UDFs). We call it Type-II EDB. Type-II EDB allows a DBA to log into the database, but keeps data always in ciphertext to avoid potential leakage. Cool!

<p align="center">
	<img src="scripts/figures/types.jpg" width = "600" height = "160" align=center />
</p>

Sad again, we've discovered an attack, which we name "smuggle". You can find it in [scripts/smuggle.py](https://github.com/SJTU-IPADS/HEDB/blob/main/scripts/smuggle.py). The reason why smuggle exists is that the Type-II EDB exposes sufficient expression operators for admins to construct an "oracle".

## Smuggle Attacks

Here is a minimal working example.

1. **Constructing oracles**:
   i. With ÷, DBAs obtain the ciphertext of '1' by dividing a number by itself.
   ii. With '1', DBAs construct all encrypted integers by iteratively + the cipher '1' to a counter.

2. **Recovering secrets**: With =, DBAs recover encrypted values by comparing them with known ciphertexts.

## HEDB as a Solution

The idea of HEDB is simple. It splits the running mode of an EDB into two: *record* for users, and *replay* for DBAs.

HEDB is named after Helium, implying its two modes. Briefly, HEDB is a dual-mode encrypted database that removes the tension between security and maintenance.

1) Execution Mode achieves interface security by blocking illegal operator invocations,
2) Maintenance Mode allows DBA common maintenance tasks by replaying invocations.

### Paper

* [Encrypted Databases Made Secure Yet Maintainable](https://www.usenix.org/conference/osdi23/presentation/li-mingyu)<br>
Mingyu Li, Xuyang Zhao, Le Chen, Cheng Tan, Huorong Li, Sheng Wang, Zeyu Mi, Yubin Xia, Feifei Li, Haibo Chen<br>
The 17th USENIX Symposium on Operating Systems Design and Implementation (OSDI 2023)

```bibtex
@inproceedings {li2023hedb,
	author = {Mingyu Li and Xuyang Zhao and Le Chen and Cheng Tan and Huorong Li and Sheng Wang and Zeyu Mi and Yubin Xia and Feifei Li and Haibo Chen},
	title = {Encrypted Databases Made Secure Yet Maintainable},
	booktitle = {17th USENIX Symposium on Operating Systems Design and Implementation (OSDI 23)},
	year = {2023},
	isbn = {978-1-939133-34-2},
	address = {Boston, MA},
	pages = {117--133},
	url = {https://www.usenix.org/conference/osdi23/presentation/li-mingyu},
	publisher = {USENIX Association},
	month = jul,
}
```

### Build

- For non-VM setup, refer to [native-setup.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/native-setup.md). Note that this is for development only.
- For 2-VM setup, please refer to [vm-setup-aarch64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-aarch64.md) or [vm-setup-x86_64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-x86_64.md).

We recommend you to use 2-VM setup, which is exactly how HEDB works.

### FAQs

#### Q1: Is HEDB limited to ARM?

***Absolutely not!*** You can deploy it to any TEE or CC platform you like. For exmaple, confidential VM (CVM) is widely supported on modern trusted hardware, such as AMD SEV(-ES,-SNP), Intel TDX, IBM PEF, ARMv9 Realm. You can deploy HEDB's integrity zone (DBMS+extension) using one CVM, and HEDB's privacy zone (operators) in another CVM. That's it!

To reproduce the performance evaluation results, you can run HEDB using two CVMs on a CC machine.

#### Q2: How to realize two modes?

For AMD SEV, Intel TDX, IBM PEF, and ARM Realm, your task is to enable CVM fork or migration between trusted domains and untrusted domains. This task remains undone. If CVM fork/migration is needed in other scenarios, it could serve as a potential research area.

Our current prototype uses an ARM server that supports S-EL2, a hardware virtualization technology present in ARMv8.4. [Twinvisor](https://github.com/TwinVisor/twinvisor-prototype) is an S-EL2 hypervisor developed by IPADS@SJTU. We plan to commit the Twinvisor patch, but no guarantee (for intellectual property reasons).

#### Q3: Supporting TPC-C?

The current version of HEDB is based on PostgreSQL and supports the TPC-H benchmark only.

We encourage further research to overcome the challenges posed by non-determinism when running TPC-C atop HEDB. We believe your excellent work will also be published and known to the industry.

### Notes

This repository is intended as a research prototype, and is not ready for any production use. Its purpose is to serve as an experimental platform for conducting research and exploring new ideas. Additionally, HEDB serves as an educational project to enhance students' understanding of the EDB's internal.

#### Limitations

HEDB's current implementation has limitations.

1. HEDB relies on deterministic record-and-replay of operator interfaces to reproduce the DBMS bugs, hence falling short in providing read-write transactional workloads such as TPC-C. To maintain TPC-C, only a subset of HEDB operators (e.g., comparison) need to be exposed to both users and DBAs. See Figure-5 in [Azure AEv2](https://dl.acm.org/doi/abs/10.1145/3318464.3386141).
2. HEDB depends on KLEE to reproduce the operators bugs. The official version of KLEE cannot support floating-point numbers. HEDB inherits this limitation. You may use [KLEE-Float](https://github.com/srg-imperial/klee-float).
3. HEDB leverages an ARMv8.4 server with S-EL2 (TrustZone Virtualization) to support its dual-mode design. This design can be ported to other platforms. Refer to [porting.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/porting.md) to replicate HEDB's performance results on Intel SGX (using SGX SDK) and ARM TrustZone (using OP-TEE). Note that the mode switch on these platforms is not implemented.

#### Caveats

The proof-of-concept of HEDB has some insecure implementations:

1. The prototype contains encryption and decryption operators (e.g., `enc_int4_encrypt`) for the purpose of debugging. They should be all eliminated. A DBA should only see ciphertexts in the `base64` form, by turning on `SELECT enable_server_mode()`.
2. The operators uses a hard-coded key and iv for data en/decryption; see [src/privacy_zone/crypto.cpp](https://github.com/SJTU-IPADS/HEDB/blob/main/src/privacy_zone/crypto.cpp). A good practice would harness cliend-side encryption. See [Acra](https://github.com/cossacklabs/acra).
3. The anonymized replay does not support troubleshooting cryptographic algorithms (e.g., mbedtls), because concolic executors such as KLEE cannot scale over them. You may be interested in this [research direction](https://ieeexplore.ieee.org/document/8023121).
4. The authenticated replay does not support query rewriting. Replay should use search instead of match. Improve them in [src/integrity_zone/rr](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/rr).

#### Anecdotes

Why name HEDB (Helium Database)?

HE, short for Helium, is the lightest neutral gas, known for its lack of reactivity and low density. The analogy to helium highlights HEDB's ability to achieve isolation from the rest while maintaining simplicity in usage. More, the reference to helium being the 2nd element alludes to HEDB's dual modes.

HEDB is pronounced [haɪdiːbiː] or 嗨嘀哔.

## Maintainers

- Xuyang Zhao: https://github.com/zhaoxuyang13
- Le Chen: https://github.com/Casieee
- Mingyu Li: https://github.com/Maxul

## Acknowledgement

- Database and Storage Lab@Alibaba DAMO Academy, who provides insights from real-world DBA tasks.
- [StealthDB](https://github.com/cryptograph/stealthdb), who provides the initial version of EDB extensions for PostgreSQL.
