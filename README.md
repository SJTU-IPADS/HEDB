# HEDB

![Status](https://img.shields.io/badge/Version-Experimental-green.svg)[![License: MIT](https://img.shields.io/badge/License-Mulan-brightgreenn.svg)](http://license.coscl.org.cn/MulanPubL-2.0)

<p align="center">
	<img src="scripts/figures/hedb.jpg" width = "300" height = "200" align=center />
</p>

HEDB is an extension of PostgreSQL to compute SQL over ciphertexts, in addition to a suite of maintenance tools.

*HEDB is an open project and highly values your feedback! We would like to hear your thoughts on our project and how we can improve it to better meet your needs.*

Here is a quick overview for any newcomer to understand the purpose of HEDB. It would take 15 minutes for you.

## What For?

Today's database systems contain much sensitive data, and some are outsourced to third-parties to manage, optimize, and diagnose, called database-as-a-service (DBaaS). To protect sensitive data in use, secrets should be kept encrypted as necessary.

**Option-1**: To build an encrypted database (EDB), one can put an entire database into an isolated domain, or confidential computing unit (like SGX, SEV, TDX, Realm, PEF, Nitro, HyperEnclave, and whatever you name it). We name it Type-I. Sadly, Type-I would prevent database admins, or DBAs, from managing the database, right? Note that DBAs can simply log into the DBMS and inspect any user data.

**Option-2**: Cloud DBaaS vendors such as Azure, Alibaba provision operator-based EDBs. You can look into `src` to learn how we build one using PostgreSQL' user-defined types (UDTs) and user-defined functions (UDFs). We name it Type-II. This allows a DBA to log into the database, but keeps data always in the ciphertext form to avoid potential leakage. Good idea!

<p align="center">
	<img src="scripts/figures/types.jpg" width = "600" height = "160" align=center />
</p>

Sad again, we've discovered an attack, which we name "smuggle". You can find it in `scripts/smuggle.py`. Why smuggle exists is that Type-II exposes too many expression operators for admins to construct an "oracle".

## Smuggle Attacks

Here is a minimal working example.

1. **Constructing oracles**: With ÷, DBAs obtain the ciphertext of ‘1’ by dividing a number by itself. With ‘1’, DBAs construct all encrypted integers by iteratively + the cipher ‘1’ to a counter.
2. **Recovering secrets**: With =, DBAs recover encrypted values by comparing them with known ciphertexts.

## HEDB

The idea of HEDB is simple. It splits the running mode of EDB into two: record for users, and replay for DBAs. HEDB is named after Helium, implying its two modes.

Briefly, HEDB is a dual-mode encrypted database that removes the tension between security and maintenance.
1) Execution Mode achieves interface security by blocking illegal operator invocations,
2) Maintenance Mode allows DBA common maintenance tasks by replaying legal invocations.

### Paper

* [Encrypted Databases Made Secure Yet Maintainable](https://www.usenix.org/conference/osdi23/presentation/li-mingyu)<br>
Mingyu Li, Xuyang Zhao, Le Chen, Cheng Tan, Huorong Li, Sheng Wang, Zeyu Mi, Yubin Xia, Feifei Li, Haibo Chen<br>
The 17th USENIX Symposium on Operating Systems Design and Implementation (OSDI ‘23)

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

### How to Build?

- For non-VM setup, refer to [install.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/install.md). Note that this is for development only.
- For 2-VM setup, please refer to [vm-setup-aarch64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-aarch64.md) or [vm-setup-x86_64.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-x86_64.md).

We recommend you to use 2-VM setup, which is exactly how HEDB works.

### FAQs

#### Q1: Is HEDB limited to ARM?

***Absolutely not!*** You can deploy it to any TEE or CC platform you like. For exmaple, confidential VM (CVM) is widely available on today's trusted hardware, such as AMD SEV(-ES,-SNP), Intel TDX, IBM PEF, ARMv9 Realm. You can deploy HEDB's integrity zone (DBMS+extension) using one CVM, and HEDB's privacy zone (operators) in another CVM. That's it!

To reproduce the performance evaluation results, you can run HEDB using two CVMs on a CC machine.

#### Q2: How to realize two modes?

For SEV, TDX, PEF, and Realm, your task is to enable support for VM fork or VM migration between trusted domains and untrusted domains. Currently, this task remains unfinished. If VM fork/migration is also needed in other scenarios, it could serve as a potential research area.

In our paper, our prototype relies on an ARM server that supports S-EL2, a hardware virtualization technology present in ARMv8.4. [Twinvisor](https://github.com/TwinVisor/twinvisor-prototype) is an S-EL2 hypervisor developed by IPADS@SJTU. We plan to commit the Twinvisor patch, but no guarantee (for intellectual property reasons).

#### Q3: Supporting TPC-C?

The current version of HEDB is based on PostgreSQL and supports the TPC-H benchmark only.

We encourage further research to overcome the challenges posed by non-determinism when supporting TPC-C atop HEDB. We believe your excellent work will absolutely be published and known to the industry.

### Notes

This repository is intended as a research prototype, and is not ready for production use. Its purpose is to serve as an experimental platform for conducting research and exploring new ideas.

In addition, HEDB serves as an educational project focused on enhancing students' understanding of the internal of an EDB.

#### Limitations

HEDB's current implementation has several limitations.

1. HEDB relies on deterministic record-and-replay of operator interfaces to reproduce the DBMS bugs, hence falling short in providing read-write transactional workloads such as TPC-C. To maintain TPC-C, only a subset of HEDB operators (e.g., comparison) need to be exposed to both users and DBAs. See Figure-5 in [Azure AEv2](https://dl.acm.org/doi/abs/10.1145/3318464.3386141).
2. HEDB depends on KLEE to reproduce the operators bugs. The official version of KLEE cannot support floating-point numbers. HEDB inherits this limitation. You may use [KLEE-Float](https://github.com/srg-imperial/klee-float).
3. HEDB leverages an ARMv8.4 server with S-EL2 (TrustZone Virtualization) to support its dual-mode design. This design can be ported to other architectures (e.g., Intel SGX, AMD SEV-SNP, Intel TDX and ARMv9 CCA). Please refer to `docs/porting.md` to replicate HEDB's performance results on Intel SGX (using SGX SDK) and ARM TrustZone (using OP-TEE). Note that the mode switch, namely, cross-domain VM fork, on these architectures is not implemented and remains an open question.

#### Caveats

The proof-of-concept of HEDB has some insecure implementations:

1. The prototype contains encryption and decryption operators (e.g., `enc_int4_encrypt`) for the purpose of debugging. They should be all eliminated. A DBA should only see ciphertexts in the `base64` form, by turning on `SELECT enable_server_mode()`.
2. The operators uses a hard-coded key and iv for data en/decryption; see [src/privacy_zone/crypto.cpp](https://github.com/SJTU-IPADS/HEDB/blob/main/src/privacy_zone/crypto.cpp). A good practice would harness cliend-side encryption. See [Acra](https://github.com/cossacklabs/acra).
3. The anonymized replay does not support troubleshooting cryptographic algorithms (e.g., mbedtls), because concolic executors such as KLEE cannot scale over them. You may be interested in this [research direction](https://ieeexplore.ieee.org/document/8023121).
4. The authenticated replay does not support query rewriting. Replay should use search instead of match. Improve them in [src/integrity_zone/rr](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/rr).

#### Anecdotes

Why name HEDB (Helium Database)?

Helium or He is the lightest neutral gas, known for its lack of reactivity and low density. In the context of HEDB, the analogy to helium highlights its ability to achieve isolation from the rest of the system while maintaining simplicity in usage. Additionally, the reference to helium being the 2nd element alludes to the dual modes supported by HEDB.

HEDB is pronounced [haɪdiːbiː] or 嗨嘀哔.

## Maintainers

- Xuyang Zhao: https://github.com/zhaoxuyang13
- Le Chen: https://github.com/Casieee
- Mingyu Li: https://github.com/Maxul

## Acknowledgement

- Database and Storage Lab@Alibaba DAMO Academy, who provides insights of real-world DBA tasks. Special thanks to Huorong Li and Sheng Wang!
- [StealthDB](https://github.com/cryptograph/stealthdb), who provides the initial version of EDB extensions for PostgreSQL.
