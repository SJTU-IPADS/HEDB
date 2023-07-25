# Non-VM Install (Development Only)

This tutorial gives you a short guidance on how to run HEDB without VM support.

Set up the host shared memory:
```sh
$ fallocate -l 16M /dev/shm/ivshmem
$ sudo chmod a+rwx /dev/shm/ivshmem
```

Install the dependencies:
```sh
$ sudo apt-get update
$ sudo apt-get install -y build-essential cmake libmbedtls-dev \
    postgresql postgresql-contrib postgresql-server-dev-all
$ sudo service postgresql restart
```

Pull the HEDB repo, build and install:
```sh
$ git clone https://github.com/SJTU-IPADS/HEDB
$ cd HEDB
$ make
$ sudo make install
$ ./build/ops_server &

$ sudo -u postgres psql
```

Run your 1st SQL:
```sql
CREATE EXTENSION hedb;
SELECT '1024'::enc_int4 * '4096'::enc_int4;
```

To peek into the secret values, try out the client mode:
```sql
SELECT enable_client_mode();
SELECT '1024'::enc_int4 * '4096'::enc_int4;
```

# Tests

## Unit Test

### (Optional) Manually Install PGTAP

```sh
$ wget https://github.com/theory/pgtap/releases/download/v1.2.0/pgTAP-1.2.0.zip
$ unzip pgTAP-1.2.0.zip
$ cd pgTAP-1.2.0
$ make
$ sudo make install
```

### Using HEDB scripts

```sh
$ sudo apt-get install pgtap libtap-parser-sourcehandler-pgtap-perl
$ cd tests/unit-test
$ sudo -u postgres pg_prove unit-test.sql
```

## Run TPC-H

TPC-H is a decision support benchmark. It consists of a suite of business-oriented ad hoc queries. The queries and the data populating the database have been chosen to have broad industry-wide relevance. This benchmark illustrates decision support systems that examine large volumes of data, execute queries with a high degree of complexity, and give answers to critical business questions.

### (Optional) Manually Install TPC-H DBGEN

```sh
$ wget https://github.com/electrum/tpch-dbgen/archive/refs/heads/master.zip
$ unzip master.zip
$ cd tpch-dbgen-master/
$ echo "#define EOL_HANDLING 1" >> config.h # remove the tail '|'
$ make
```

### Using HEDB scripts

```sh
## install dependencies
$ pip3 install psycopg2 tqdm

$ cd tests/tpch
## generate and load data
$ python3 run.py -l
## transform TPC-H queries
$ python3 run.py -t
## record TPC-H
$ python3 run.py -sg -rr record
## replay TPC-H
$ python3 run.py -sg -rr replay
```

If you encounter `FATAL:  password authentication failed for user "postgres"`,
then update DBA's password with:
```sql
ALTER USER postgres WITH PASSWORD 'postgres';
```

### Notes

TPC-H may not be a suitable benchmark for evaluating EDB's performance.

Since we were not able to obtain the real-world traces, we resort to using TPC-H to simulate the realistic financial workloads.
Note that our tests involved encrypting all data types. However, in real-world scenarios, it is common practice for customers to selectively encrypt only the data that is crucial for security reasons, in order to maintain optimal performance.

# Security Concerns

1. IV

The IV should be a random value when inserting the data.

For anonymized replay, you can add the masking rule ID as part of the IV.

2. Encryption Key

Likewise, you should use the random value as the encryption key, not hard-coded one, see `src/privacy_zone/crypto.cpp`.

3. Prevent Admin login

Replace admin's password with a random long key.
