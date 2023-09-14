# HEDB For Other Platforms

HEDB is an extension turning existing RDBMS systems into an encrypted style. Its current form is based on PostgreSQL as the RDBMS.

Theoretically, HEDB can be ported to any of the following trusted execution environments (TEEs):
- ARM TrustZone using OP-TEE
- Intel SGX using SGX SDK
- Confidential VMs such as AMD SEV, Intel TDX, Hygon HyperEnclave, Amazon Nitro, ARMv9 CCA Realm using VMs

## Port Guidance

HEDB, as a Type-II EDB, relies on the advanced extension capabilities provided by modern RDBMSes. Type-II operators are basically implemented through user-defined datatypes (UDT) and functions (UDFs).

Our UDT and UDFs for PostgreSQL can be found in the `src/integrity_zone` directory. If you plan to port HEDB to a different database engine like MySQL and DuckDB, you have the flexibility to modify the UDF construction in `src/integrity_zone` to adhere to MySQL or DuckDB's specifications. Additionally, you can reuse the `src/privacy_zone` directory, which executes the confidential operators within the protected domain, regardless of the database engine being used.

## Some Prior Efforts

The HEDB official repo does not contain TrustZone and SGX support, you may refer to [here](https://github.com/zhaoxuyang13/hedb).

1. Install PostgreSQL:

```shell
sudo apt-get install postgresql postgresql-server-dev-all
```
or build from source: https://www.postgresql.org/docs/current/install-short.html

### ARM OP-TEE

2. Run HEDB Pg-extension as TrustZone TA [(see here)](https://optee.readthedocs.io/en/latest/building/gits/build.html):

  
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

### Intel SGX

   1. Install HEDB extension:

   ```bash
   make configure_sgx
   make 
   make install
   ```

### ARM CCA

   The procedures are exactly the same as how you do in a 2-VM setting, see [here](https://github.com/SJTU-IPADS/HEDB/blob/main/docs/vm-setup-aarch64.md).
