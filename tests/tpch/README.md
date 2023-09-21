# TPC-H test

## Directory Structure
```
.
├── dbgen                # data generation tool
├── run.py               # python script to run TPC-H
├── README.md
├── tpch-config.json     # config file
├── tpch-schema-enc.sql  # cipher schema for TPC-H
├── tpch-schema.sql      # normal schema for TPC-H
└── util_py3             # utils
```

## Run

### Step 1
Config your environment in tpch-config.json.
``` json
{
    "test_name": "tpch",
    "pg_ip": "127.0.0.1",
    "pg_port": "5432",
    "pg_user": "postgres",
    "pg_password": "postgres",
    "data_size": "0.01",
    "secure": "y",    // encryted or not, 'y' for yes
    "secure_query_dir": "secure-query",
    "insecure_query_dir": "insecure-query",
    "output_dir": "output"
}
```

### Step 2
Run the script (you may need to install python package `psycopg2`).

``` sh
$ python3 run.py -l
$ python3 run.py -sg
```

### Step 3

Record:
``` sh
$ python3 run.py -sg -rr record
```

Replay:
``` sh
$ python3 run.py -sg -rr replay
```

Use `-h` to see more options.
