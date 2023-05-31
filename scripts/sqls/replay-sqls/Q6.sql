select enable_debug_mode(1);
select enable_replay_mode('Q6', 'seq');
\timing
select
    sum(l_extendedprice*l_discount) as revenue 
from
    lineitem
where
    l_shipdate >= pg_enc_timestamp_encrypt('1995-01-01') 
and l_shipdate < pg_enc_timestamp_encrypt('1996-01-01')
and l_discount between '0.07' and '0.09' 
and l_quantity < '24';
