# Native Install

This tutorial gives you a short guidance on how to run HEDB without VM support.

Set up the host shared memory:
```sh
$ fallocate -l 16M /dev/shm/ivshmem
$ sudo chmod a+rwx /dev/shm/ivshmem
```

Install the dependencies:
```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake libmbedtls-dev \
    postgresql postgresql-contrib postgresql-server-dev-all
```

Pull the HEDB repo, build and install:
```sh
git clone https://github.com/SJTU-IPADS/HEDB
make
sudo make install
./build/ops_server &
sudo -u postgres psql
```

Run your 1st SQL:
```sql
CREATE EXTENSION hedb;
SELECT enc_int4_encrypt(1024) * enc_int4_encrypt(4096);
```

# Run TPC-H

TPC-H is a decision support benchmark. It consists of a suite of business-oriented ad hoc queries. The queries and the data populating the database have been chosen to have broad industry-wide relevance. This benchmark illustrates decision support systems that examine large volumes of data, execute queries with a high degree of complexity, and give answers to critical business questions.

## (Optional) Install TPC-H DBGEN

```sh
wget https://github.com/electrum/tpch-dbgen/archive/refs/heads/master.zip
unzip master.zip
cd tpch-dbgen-master/
echo "#define EOL_HANDLING 1" >> config.h # remove the tail '|'
make
./dbgen -s 1
chmod +rw *.tbl
```

## Using HEDB scripts

TODO

# Unit Test

## (Optional) Install PGTAP

```sh
wget https://github.com/theory/pgtap/releases/download/v1.2.0/pgTAP-1.2.0.zip
unzip pgTAP-1.2.0.zip
cd pgTAP-1.2.0
make && sudo make install
sudo apt install libtap-parser-sourcehandler-pgtap-perl
```

## Using HEDB scripts

```sh
cd tests/unit-test
sudo -u postgres pg_prove unit-test.sql
```

# Security

1. IV

The IV should be a random number when inserting the data.

For anonymized replay, you can add the masking rule id as part of the IV.

2. Encryption Key

Likewise, you should use the random number as the encryption key.
