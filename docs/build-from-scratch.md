# HEDB Build Instructions

HEDB is a novel encrypted database system. Its current form is based on PostgreSQL as the RDBMS.

The implemented prototype can be run on any of the three kinds of trusted execution environments:

- ARM TrustZone (OP-TEE based)
- Intel SGX (SGX SDK based)
- Confidential VMs such as ARMv9 CCA and Intel TDX (process based)

## How to Build?

1. Install Postgresql:

```shell
sudo apt-get install postgresql postgresql-server-dev-all
```
or build from source: https://www.postgresql.org/docs/current/install-short.html

### Choice-1: ARM OP-TEE

2. Run HEDB Pg-extension as TrustZone TA [(see here)]( https://optee.readthedocs.io/en/latest/building/gits/build.html):

  
   1. Install OPTEE prerequisite:

      ```bash
      sudo apt-get install android-tools-adb android-tools-fastboot autoconf \
              automake bc bison build-essential ccache codespell \
              cscope curl device-tree-compiler expect flex ftp-upload gdisk iasl \
              libattr1-dev libcap-dev libcap-ng-dev \
              libfdt-dev libftdi-dev libglib2.0-dev libgmp-dev libhidapi-dev \
              libmpc-dev libncurses5-dev libpixman-1-dev libssl-dev libtool make \
              mtools netcat ninja-build python3-crypto \
              python3-pycryptodome python3-pyelftools python3-serial \
              rsync unzip uuid-dev xdg-utils xterm xz-utils zlib1g-dev
      ```

   2. Install REPO:

      ```bash
      mkdir ~/bin
      PATH=~/bin:$PATH
      curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
      chmod a+x ~/bin/repo
      git config --global user.name "Your Name" #repo need git name/email config.
      git config --global user.email "you@example.com" 
      ```

   3. Get OPTEE source code:

      ``` bash
      mkdir <work-dir>
      cd <work-dir>
      repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml
      repo sync
      # apply our own code.
      mkdir -p share/edb
      tar -xzf edb.tar.gz -C share/edb
      cp share/edb/patch/qemu_v8.mk build/qemu_v8.mk
      mkdir -p img
      mv ramdisk img/ramdisk
      ```

   4. Build and run OP-TEE:

      ```bash
      cd build
      make toolchains
      make QEMU_VIRTFS_ENABLE=y QEMU_USERNET_ENABLE=y CFG_WITH_PAGER=y run
      ```

   5. Build QEMU:

   ```bash
   mkdir mnt
   mount /dev/vda mnt
   ./mnt/mnt.sh
   ./init.sh 
   ```

   6. Install HEDB extension:

   ```bash
   make configure_tz
   make build 
   sudo make install
   ```

### Choice-2: Intel SGX

   1. Install HEDB extension:

   ```bash
   make configure_sgx
   make 
   make install
   ```

### Choice-3: ARM CCA

   1. Install HEDB extension:

   ```bash
   sudo apt install libmedtls-dev
   make configure_sim
   make 
   make install
   ```

## How to Benchmark?

1. Run psql:

   ```bash
   psql -U postgres -p 5432 -h localhost
   ```

   ```psql
   CREATE USER test WITH PASSWORD 'password';
   CREATE database test;
   \c test
   CREATE extension encdb;
   SELECT pg_enc_int4_encrypt(1) + pg_enc_int4_encrypt(2);
   SELECT pg_enc_int4_decrypt(pg_enc_int4_encrypt(1) + pg_enc_int4_encrypt(2));
   ```

2. Run TPCC benchmark:

   ```bash
   cd benchmark 
   java -Dlog4j.configuration=log4j.properties -jar bin/oltp.jar -b tpcc -o output -s 100 --config config/tpcc_config.xml --load true --execute false
   java -Dlog4j.configuration=log4j.properties -jar bin/oltp.jar -b tpcc -o output -s 100 --config config/tpcc_config.xml --load false --execute true
   ```

3. Run TPCH benchmarks:

   ```bash
   cd benchmark 
   ./tool/dbgen -s 2
   java -Dlog4j.configuration=log4j.properties -jar bin/tpch.jar -b tpch -o output -s 10 --config config/tpch_config.xml --load true --execute false
   java -Dlog4j.configuration=log4j.properties -jar bin/tpch.jar -b tpch -o output -s 10 --config config/tpch_config.xml --load false --execute true
   ```

## How to Port?

HEDB is a Type-II EDB that depends on the extension or plugin capability offered by RDBMS. This is usually achieved via user-defined datatypes and functions (UDFs).

Our UDFs for PostgreSQL can be found in the `src/extension` folder. If you want to port HEDB to another database engine, such as MySQL, you can modify `src/extension` to use MySQL's UDF construction, and reuse `src/enclave` that runs the confidential operators in the protected domain.
