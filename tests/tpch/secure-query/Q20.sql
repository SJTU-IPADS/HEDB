select
    s_name, s_address
from
    supplier, nation
where
    s_suppkey in (
        select ps_suppkey from partsupp
        where ps_partkey in (
                select p_partkey from part where p_name like enc_text_encrypt('chocolate%')
                )
            and ps_availqty > (
                select enc_float4_encrypt('0.5') * sum(l_quantity) from lineitem
                where
                    l_partkey = ps_partkey
                    and l_suppkey = ps_suppkey
                    and l_shipdate >= enc_timestamp_encrypt('1995-1-1')
                    and l_shipdate < enc_timestamp_encrypt('1996-1-1')
                ))
   and s_nationkey = n_nationkey
   and n_name = enc_text_encrypt('JAPAN')
order by s_name;
