select
    s_name, s_address
from
    supplier, nation
where
    s_suppkey in (
        select ps_suppkey from partsupp
        where ps_partkey in (
                select p_partkey from part where p_name like 'chocolate%'::enc_text
                )
            and ps_availqty > (
                select '0.5'::enc_float4 * sum(l_quantity) from lineitem
                where
                    l_partkey = ps_partkey
                    and l_suppkey = ps_suppkey
                    and l_shipdate >= '1995-1-1'::enc_timestamp
                    and l_shipdate < '1996-1-1'::enc_timestamp
                ))
   and s_nationkey = n_nationkey
   and n_name = 'JAPAN'::enc_text
order by s_name;
