select enable_debug_mode(1);
select enable_record_mode('Q20');
\timing
select
    s_name, s_address
from
    supplier, nation
where
    s_suppkey in (
        select ps_suppkey from partsupp
        where ps_partkey in (
                select p_partkey from part where p_name like 'chocolate%'
                )
            and ps_availqty > (
                select '0.5' * sum(l_quantity) from lineitem
                where
                    l_partkey = ps_partkey
                    and l_suppkey = ps_suppkey
                    and l_shipdate >= pg_enc_timestamp_encrypt('1995-1-1')
                    and l_shipdate < pg_enc_timestamp_encrypt('1996-1-1') 
                ))
   and s_nationkey = n_nationkey
   and n_name = 'JAPAN' 
order by s_name;
