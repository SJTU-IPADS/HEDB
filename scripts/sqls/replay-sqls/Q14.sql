select enable_debug_mode(1);
select enable_replay_mode('Q14', 'seq');
\timing
select
    '100.00' * sum(case
        when p_type like 'PROMO%' 
        then l_extendedprice*('1' - l_discount) 
        else '0' end) / sum(l_extendedprice * ('1' - l_discount))
    as promo_revenue
from
    lineitem, part
where
    l_partkey = p_partkey
    and l_shipdate >= pg_enc_timestamp_encrypt('1995-06-01') 
    and l_shipdate < pg_enc_timestamp_encrypt('1995-07-01');
