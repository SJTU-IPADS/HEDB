DROP DATABASE IF EXISTS secure_test CASCADE;
CREATE DATABASE secure_test;
\c secure_test

DROP TABLE IF EXISTS nation CASCADE;
DROP TABLE IF EXISTS region CASCADE;
DROP TABLE IF EXISTS part CASCADE;
DROP TABLE IF EXISTS supplier CASCADE;
DROP TABLE IF EXISTS partsupp CASCADE;
DROP TABLE IF EXISTS orders CASCADE;
DROP TABLE IF EXISTS customer CASCADE;
DROP TABLE IF EXISTS lineitem CASCADE;

DROP EXTENSION IF EXISTS hedb CASCADE;
CREATE EXTENSION hedb;
ALTER SYSTEM SET max_parallel_workers_per_gather = 0; -- FIXME: avoid replay bug

CREATE TABLE nation  ( n_nationkey  INTEGER NOT NULL,
                       n_name       enc_text NOT NULL,
                       n_regionkey  INTEGER NOT NULL,
                       n_comment    enc_text);

CREATE TABLE region  ( r_regionkey  INTEGER NOT NULL,
                       r_name       enc_text NOT NULL,
                       r_comment    enc_text);

CREATE TABLE part  ( p_partkey     INTEGER NOT NULL,
                     p_name        enc_text NOT NULL,
                     p_mfgr        enc_text NOT NULL,
                     p_brand       enc_text NOT NULL,
                     p_type        enc_text NOT NULL,
                     p_size        enc_int4 NOT NULL,
                     p_container   enc_text NOT NULL,
                     p_retailprice enc_float4 NOT NULL,
                     p_comment     enc_text NOT NULL );

CREATE TABLE supplier ( s_suppkey     INTEGER NOT NULL,
                        s_name        enc_text NOT NULL,
                        s_address     enc_text NOT NULL,
                        s_nationkey   INTEGER NOT NULL,
                        s_phone       enc_text NOT NULL,
                        s_acctbal     enc_float4 NOT NULL,
                        s_comment     enc_text NOT NULL);

CREATE TABLE partsupp ( ps_partkey     INTEGER NOT NULL,
                        ps_suppkey     INTEGER NOT NULL,
                        ps_availqty    enc_float4 NOT NULL,
                        ps_supplycost  enc_float4  NOT NULL,
                        ps_comment     enc_text NOT NULL );

CREATE TABLE customer ( c_custkey     INTEGER NOT NULL,
                        c_name        enc_text NOT NULL,
                        c_address     enc_text NOT NULL,
                        c_nationkey   INTEGER NOT NULL,
                        c_phone       enc_text NOT NULL,
                        c_acctbal     enc_float4   NOT NULL,
                        c_mktsegment  enc_text NOT NULL,
                        c_comment     enc_text NOT NULL);

CREATE TABLE orders  ( o_orderkey       INTEGER NOT NULL,
                       o_custkey        INTEGER NOT NULL,
                       o_orderstatus    enc_text NOT NULL,
                       o_totalprice     enc_float4 NOT NULL,
                       o_orderdate      enc_timestamp NOT NULL,
                       o_orderpriority  enc_text NOT NULL,
                       o_clerk          enc_text NOT NULL,
                       o_shippriority   enc_int4 NOT NULL,
                       o_comment        enc_text NOT NULL);

CREATE TABLE lineitem ( l_orderkey    INTEGER NOT NULL,
                        l_partkey     INTEGER NOT NULL,
                        l_suppkey     INTEGER NOT NULL,
                        l_linenumber  enc_int4 NOT NULL,
                        l_quantity    enc_float4 NOT NULL,
                        l_extendedprice  enc_float4 NOT NULL,
                        l_discount    enc_float4 NOT NULL,
                        l_tax         enc_float4 NOT NULL,
                        l_returnflag  enc_text NOT NULL,
                        l_linestatus  enc_text NOT NULL,
                        l_shipdate    enc_timestamp NOT NULL,
                        l_commitdate  enc_timestamp NOT NULL,
                        l_receiptdate enc_timestamp NOT NULL,
                        l_shipinstruct enc_text NOT NULL,
                        l_shipmode     enc_text NOT NULL,
                        l_comment      enc_text NOT NULL);

create unique index c_ck on customer (c_custkey asc) ;
create index c_nk on customer (c_nationkey asc) ;
create unique index p_pk on part (p_partkey asc) ;
create unique index s_sk on supplier (s_suppkey asc) ;
create index s_nk on supplier (s_nationkey asc) ;
create index ps_pk on partsupp (ps_partkey asc) ;
create index ps_sk on partsupp (ps_suppkey asc) ;
create unique index ps_pk_sk on partsupp (ps_partkey asc, ps_suppkey asc) ;
create unique index ps_sk_pk on partsupp (ps_suppkey asc, ps_partkey asc) ;
create unique index o_ok on orders (o_orderkey asc) ;
create index o_ck on orders (o_custkey asc) ;
create index o_od on orders (o_orderdate asc) ;
create index l_ok on lineitem (l_orderkey asc) ;
create index l_pk on lineitem (l_partkey asc) ;
create index l_sk on lineitem (l_suppkey asc) ;
--create index l_ln on lineitem (l_linenumber asc) ;
create index l_sd on lineitem (l_shipdate asc) ;
create index l_cd on lineitem (l_commitdate asc) ;
create index l_rd on lineitem (l_receiptdate asc) ;
--create unique index l_ok_ln on lineitem (l_orderkey asc, l_linenumber asc) ;
--create unique index l_ln_ok on lineitem (l_linenumber asc, l_orderkey asc) ;
create index l_pk_sk on lineitem (l_partkey asc, l_suppkey asc) ;
create index l_sk_pk on lineitem (l_suppkey asc, l_partkey asc) ;
create unique index n_nk on nation (n_nationkey asc) ;
create index n_rk on nation (n_regionkey asc) ;
create unique index r_rk on region (r_regionkey asc) ;
