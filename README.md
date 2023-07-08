# HEDB

![Status](https://img.shields.io/badge/Version-Experimental-green.svg)
[![License: MIT](https://img.shields.io/badge/License-Mulan-brightgreenn.svg)](http://license.coscl.org.cn/MulanPubL-2.0)

<img src="scripts/hedb.jpg" width = "300" height = "200" align=center />

Here is a quick overview for innocent audience to understand the purpose of this project. It would take 10 minutes for you.

## What For?

Today's database contain much sensitive data, and these databases are outsourced to third-parties like clouds to manage, optimize, and diagnose, so-called database-as-a-service (DBaaS). To protect data in use, data should be kept encrypted as necessary.

**Option-1**: To make up an encrypted database (EDB), one can put an the entire database into an isolated domain, or confidential computing unit (for example, SGX, SEV, TDX, Realm, PEF, Nitro, etc.). We name it Type-I. Sadly, Type-I would prevent database admins, or DBAs, from managing the database, right? Note that the DBA can simply log into the DBMS and inspect user data.

**Option-2**: Cloud DBaaS vendors such as Azure, Alibaba provision operator-based EDBs. You can look into `src` to learn how to build one using Postgres' user-defined types (UDTs) and user-defined functions (UDFs). We name it Type-II. This allows a DBA to log into the database, but keeps data always in the ciphertext form to avoid potential leakage. Good idea!

Sad again, we find an attack, which we name "smuggle". You can find it in `scripts/smuggle.py`. Why smuggle exists is that Type-II exposes too many expression operators for an external manipulator to construct an "oracle".

## Smuggle Attacks

Let me show you a minimal working example.

1. **Constructing oracles**: With ÷, DBAs obtain the ciphertext of ‘1’ by dividing a number by itself. With ‘1’, DBAs construct all encrypted integers by iteratively + the cipher ‘1’ to a counter.
2. **Recovering secrets**: With =, DBAs recover encrypted values by comparing them with known ciphertexts.

## HEDB System

The idea of HEDB is quite simple. It splits the running mode of EDB into two: record for users, and replay for DBAs. HEDB is named after Helium, implying its two modes.

Briefly, HEDB is a dual-mode encrypted database that removes the tension between security and maintenance.
1) Execution Mode achieves interface security by blocking illegal operator invocations,
2) Maintenance Mode allows DBA common maintenance tasks by replaying legal invocations.

## How to Build?

- For native setp, refer to `docs/install.md`.
- For 2-VM setup, refer to `docs/vm-setup.md`.

## Paper

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

## FAQs

### Is the project limited to ARM?

Absolutely not! You can deploy it to any TEE or CC platform you like. For exmaple, confidential VM (CVM) is widely available on today's trusted hardware, such as AMD SEV(-ES,-SNP), Intel TDX, IBM PEF, ARMv9 Realm. You can deploy HEDB's integrity zone (DBMS+extension) using one CVM, and HEDB's privacy zone (operators) in another CVM. That's it!

To reproduce the performance evaluation results, you can run HEDB using two CVMs on a CC machine.

### How do you realize two modes?

For SEV, TDX, PEF, Realm, your task is to support VM fork or VM migration between trusted domain and untrusted domain. Note that this is an undone work.

In our paper, our prototype relies on an ARM server that supports S-EL2, a hardware virtualization technology in ARMv8.4 TrustZone. [Twinvisor](https://github.com/TwinVisor/twinvisor-prototype) is an S-EL2 hypervisor developed by IPADS@SJTU. We plan to commit the Twinvisor patch, but no guarantee (for intellectual property reasons).

### Can we run TPC-C?

The current form of HEDB is based on PostgreSQL and supports the TPC-H benchmark only.

We welcome any follow-up work, which defeats the non-determinism challenges when supporting TPC-C atop HEDB. We believe your excellent work will absolutely be published and known to the industry.

## Limitations

This repo is a research prototype rather than a production-ready system. Its current implementation has several main limitations.

1. HEDB relies on deterministic record-and-replay of operator interfaces to reproduce the DBMS bugs, hence falling short in providing read-write transactional workloads such as TPC-C.
2. HEDB depends on KLEE to reproduce the operators bugs. The official version of KLEE cannot support floating-point numbers. HEDB inherits this limitation. You may use [KLEE-Float](https://github.com/srg-imperial/klee-float).
3. HEDB leverages an ARMv8.4 server with S-EL2 (TrustZone Virtualization) to support its dual-mode design. This design can be ported to other architectures (e.g., Intel SGX, AMD SEV-SNP, Intel TDX and ARMv9 CCA). Please refer to `docs/porting.md` to replicate HEDB's performance results on Intel SGX (using SGX SDK) and ARM TrustZone (using OP-TEE). Note that the mode switch, namely, cross-domain VM fork, on these architectures is not implemented and remains an open question.

## Security

This proof-of-concept should not be used in any products, as it has some insecure implementations:

1. The prototype uses a hard-coded key and iv for data en/decryption; see `src/privacy_zone/crypto.cpp`. A good practice would harness cliend-side en/decryption.
2. The prototype contains encryption and decryption operators (e.g., `enc_int4_encrypt`) for the purpose of debugging. They should be all eliminated. A DBA should only see ciphertexts in the `base64` form, by turning on `SELECT disable_client_mode()`.

## Maintainers

- Xuyang Zhao: https://github.com/zhaoxuyang13
- Le Chen: https://github.com/Casieee
- Mingyu Li: https://github.com/Maxul

## Notes

HEDB is also an educational EDB project. It helps students understand the internal of EDB.

## Anecdotes

Why name HEDB (Helium Database)?

Helium or HE is the most lightweight neutral gas, meaning HEDB's isolation from the rest yet being simple to use. He is also the 2nd element, denoting HEDB's dual modes.

HEDB is pronounced [haɪdiːbiː] or 嗨嘀哔.

## Acknowledgement

- Database and Storage Lab@Alibaba DAMO Academy, who provides insights of real-world DBA tasks.
- [StealthDB](https://github.com/cryptograph/stealthdb), who provides the initial version of EDB for PostgreSQL.
