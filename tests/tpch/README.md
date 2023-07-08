# TPCH test

## Directory Structure
```
.
├── dbgen     # data generation tool
├── doit.py   # python script to run TPCH
├── README.md
├── tpch-config.json   # config file
├── tpch-schema-enc.sql  # encrypted schema for TPCH
├── tpch-schema.sql      # normal schema for TPCH
└── util_py3
```

## Run
### Step 1
Config your environment in tpch-config.json.
``` json
{
    "test_name": "tpch",
    "pg_ip": "127.0.0.1",
    "pg_port": "5432",
    "pg_user": "ubuntu",
    "pg_password": "ubuntu",
    "data_size": "0.01",
    "secure": "y",    // encryted or not, 'y' for yes
    "record_replay": "replay",    // 'replay', 'record', 'none'
    "secure_query_dir": "secure-query",
    "insecure_query_dir": "insecure-query",
    "output_dir": "output"
}
```

### Step 2
Run the script (you may need to install python package `psycopg2`).

``` shell
$ python3 doit.py
```

Use `-h` to see more options.