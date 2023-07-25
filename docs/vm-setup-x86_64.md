# How to set up a 2-VM environment for HEDB

We recommend using Ubuntu cloud images, as they provide ready-to-use images.

Download an amd64 image to use:
```sh
$ wget https://cloud-images.ubuntu.com/releases/23.04/release/ubuntu-23.04-server-cloudimg-amd64.img
```

Install the tool:
```sh
$ sudo apt install -y qemu-kvm cloud-image-utils
```

Prepare the authentication info:
```sh
$ cat > cloud.yaml << EOF
#cloud-config
password: ubuntu
chpasswd: { expire: False }
ssh_pwauth: True
EOF

$ cloud-localds --disk-format qcow2 cloud.img cloud.yaml
```
With `cloud.img`, both DBMS VM and Operator VM can be logged into by username `ubuntu` and password `ubuntu`.

Setup the host shared memory:
```sh
$ fallocate -l 16M /dev/shm/ivshmem
$ chmod a+rwx /dev/shm/ivshmem
```

# Privacy Zone

Set up the privacy-zone VM for Operators:
```sh
$ cp ubuntu-23.04-server-cloudimg-amd64.img ops.img
$ qemu-system-x86_64 \
    -cpu host -enable-kvm -m 1G -nographic \
    -drive if=virtio,format=qcow2,file=ops.img \
    -drive if=virtio,format=raw,file=cloud.img \
    -device ivshmem-plain,memdev=hostmem \
    -object memory-backend-file,size=16M,share=on,mem-path=/dev/shm/ivshmem,id=hostmem \
    -device virtio-net-pci,netdev=net0 -netdev user,id=net0
```
We recommend you to wait a while till the message `ci-info: no authorized SSH keys fingerprints found for user ubuntu.` displays, or you may fail to login.

After you log into the privacy-zone VM, install the dependencies:
```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake libmbedtls-dev
```

Then pull the HEDB repo:
```sh
git clone https://github.com/SJTU-IPADS/HEDB
cd HEDB
make
```

Install the HEDB uio-ivshmem kernel driver:
```sh
cd HEDB/tools/drivers/ivshmem-driver
make
sudo insmod uio-<kernel-version>.ko
sudo insmod uio-ivshmem.ko
sudo chmod a+rwx /dev/uio0
```
You may need to replace <kernel-version> with a right one.

In the end, run the ops inside the privacy zone:
```sh
cd HEDB
./build/ops_server
```

# Integrity Zone

Make up the integrity-zone VM for DBMS:
```sh
$ cp ubuntu-23.04-server-cloudimg-amd64.img dbms.img
$ qemu-img resize dbms.img 10G
$ cloud-localds --disk-format qcow2 cloud2.img cloud.yaml
$ qemu-system-x86_64 \
    -cpu host -smp 2 -enable-kvm -m 4G -nographic \
    -drive if=virtio,format=qcow2,file=dbms.img \
    -drive if=virtio,format=raw,file=cloud2.img \
    -device ivshmem-plain,memdev=hostmem,master=on \
    -object memory-backend-file,size=16M,share=on,mem-path=/dev/shm/ivshmem,id=hostmem \
    -device virtio-net-pci,netdev=net0 -netdev user,id=net0,hostfwd=tcp::8000-:8000 \
```
(If you wish to experiment with local shmem, at least 2 vcpus must be assigned, or otherwise the busy-polling shmem will be blocked.)
Note that the `hostfwd` is for HEDB's template-hotfix server, and `master=on` is for HEDB's CVM fork (via QEMU VM snapshot even with ivshmem).

After you log into the integrity-zone VM, install the postgresql server:
```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake libmbedtls-dev \
    postgresql postgresql-contrib postgresql-server-dev-all
```

Then pull the HEDB repo, build and install:
```sh
git clone https://github.com/SJTU-IPADS/HEDB
cd HEDB
make
sudo make install
```

Install the HEDB uio-ivshmem kernel driver:
```sh
cd HEDB/tools/drivers/ivshmem-driver
make
sudo insmod uio-<kernel-version>.ko
sudo insmod uio-ivshmem.ko
sudo chmod a+rwx /dev/uio0
```
You may need to replace <kernel-version> with a right one.

Now you can try the 1st SQL:
``` sh
sudo -u postgres psql

# psql
DROP EXTENSION IF EXISTS hedb CASCADE;
CREATE EXTENSION hedb;
SELECT 1024::enc_int4 * 4096::enc_int4;
```

# How to do Mode Switch?

Mode switch forks a confidential VM from integrity zone to normal zone.
The initial prototype of HEDB uses TwinVisor@SOSP'2021 to implement mode switch, which moves VM from Secure to Normal by modifying the TZASC controller.
This repo does not provide this migration code (maybe in the future).
Implementing migration between CVM to VM atop SEV and TDX is future work, and you may even publish your own work for these!

Let's assume you have such migration code already.
One way to implement CVM fork is to leverage the mature QEMU feature: VM snapshots.

First, append this line to qemu command line of integrity zone:
```
    -monitor stdio -serial telnet:localhost:4321,server,nowait
```

Second, log into the integrity zone VM via telnet:
```sh
telnet localhost 4321
```

Third, you can issue commands to the qemu monitor.
For example, `savevm <checkpoint name>` creates a VM snapshot. Acting as a DBA, you can try as many times as you wish.
To re-execute SQLs, simply try HEDB replay mode.
After whatever inspections and fixes, issue `loadvm <checkpoint name>` to unwind, which discards the side effects.
To navigate the snapshot list, use `info snapshots`. `delvm <checkpoint name>` deletes unwanted ones.

In brief, VM snapshots provides temporal fork, rather than spatial fork.

Note that the migration back to CVM phase should check the VM integrity carefully, somehow like runtime attestation.

# How to Apply HotFix?

Please refer to the README of `tools/hotfix`.

# Misc

To shrink the VM image size, you can use:
```
qemu-img convert -c -O qcow2 dbms.img dbms-new.img
```
