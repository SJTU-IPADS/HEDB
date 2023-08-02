# How to build a secure Type-I EDB?

The basic idea is to place a DBMS inside the confidential VM or CVM.

First you need to have a CVM in hand, like AMD SEV or Intel TDX.

Then you can install DBMSes like PostgreSQL, MySQL, SQLite or DuckDB inside the CVM.

Finally, make sure you are using an SSH tunnel to communicate with the CVM and the DBMS engine.

In order to prevent data breach from the untrusted storage, you may use Linux `dm-crypt` for convenience.

## How to use Linux dm-crypt

```bash
sudo apt install cryptsetup
sudo modprobe brd rd_nr=1 rd_size=$((4*1024*1024*1024))
ls /dev/ram0
fallocate -l 2M crypthdr.img
sudo cryptsetup luksFormat /dev/ram0 --header crypthdr.img
sudo cryptsetup open --header crypthdr.img /dev/ram0 encrypted-ram0
```

After that, you can use `/dev/mapper/encrypted-ram0` for the storage of the DBMS engine.

Here is how to benchmark the `dm-crypt` block layer:
```bash
sudo fio --filename=/dev/ram0 --readwrite=readwrite --bs=4k --direct=1 --loops=10 --name=plain
sudo fio --filename=/dev/mapper/encrypted-ram0 --readwrite=readwrite --bs=4k --direct=1 --loops=1 --name=crypt
```