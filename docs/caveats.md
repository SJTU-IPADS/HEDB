# Caveats

The proof-of-concept of HEDB has some insecure implementations:

1. The prototype contains encryption and decryption operators (e.g., `enc_int4_encrypt`) for the purpose of development and debugging. They should be all eliminated when released. A DBA should only see ciphertexts in the `base64` form, by turning on `SELECT enable_server_mode()`.
2. The operators uses a hard-coded key and iv for data en/decryption; see [src/privacy_zone/crypto.cpp](https://github.com/SJTU-IPADS/HEDB/blob/main/src/privacy_zone/crypto.cpp). A good practice would harness cliend-side encryption. See [Acra](https://github.com/cossacklabs/acra).
3. The anonymized replay does not support troubleshooting cryptographic algorithms (e.g., mbedtls), because concolic executors such as KLEE cannot scale over them. You may be interested in this [research direction](https://ieeexplore.ieee.org/document/8023121).
4. The authenticated replay does not support query rewriting. Replay should use search instead of match. Improve them in [src/integrity_zone/rr](https://github.com/SJTU-IPADS/HEDB/tree/main/src/integrity_zone/rr).

# Limitations

HEDB's current implementation has limitations.

1. HEDB relies on deterministic record-and-replay of operator interfaces to reproduce the DBMS bugs, hence falling short in providing read-write transactional workloads such as TPC-C. To maintain TPC-C, only a subset of HEDB operators (e.g., comparison) need to be exposed to both users and DBAs. See Figure-5 in [Azure AEv2](https://dl.acm.org/doi/abs/10.1145/3318464.3386141).
2. HEDB depends on KLEE to reproduce the operators bugs. The official version of KLEE cannot support floating-point numbers. HEDB inherits this limitation. You may use [KLEE-Float](https://github.com/srg-imperial/klee-float).
3. HEDB leverages an ARMv8.4 server with S-EL2 (TrustZone Virtualization) to support its dual-mode design. This design can be ported to other platforms. Refer to [porting.md](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/porting.md) to replicate HEDB's performance results on Intel SGX (using SGX SDK) and ARM TrustZone (using OP-TEE). Note that the feature of mode switch on these platforms is not implemented.