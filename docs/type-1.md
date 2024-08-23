# How about building a secure Type-I EDB?

The core idea is to place a DBMS directly into a confidential VM or CVM. To begin with, you need to have access to a CVM, such as AMD SEV or Intel TDX. Once obtained, you can proceed with installing popular DBMSes like PostgreSQL, MySQL, SQLite, or DuckDB within the CVM. It is crucial to ensure secure communication between the CVM and the DBMS engine by utilizing an SSH secure channel, and carefully protect your password.

To prevent data breaches from untrusted storage, consider using Linux `dm-crypt` for convenience.

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

Benchmark the `dm-crypt` block layer:
```bash
sudo fio --filename=/dev/ram0 --readwrite=readwrite --bs=4k --direct=1 --loops=10 --name=plain
sudo fio --filename=/dev/mapper/encrypted-ram0 --readwrite=readwrite --bs=4k --direct=1 --loops=1 --name=crypt
```