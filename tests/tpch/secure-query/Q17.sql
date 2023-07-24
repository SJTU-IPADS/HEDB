select
    sum(l_extendedprice) / enc_float4_encrypt('7.0') as avg_yearly
from
    lineitem, part
where
    p_partkey = l_partkey
    and p_brand = enc_text_encrypt('Brand#15')
    and p_container = enc_text_encrypt('JUMBO CASE')
    and l_quantity < (
        select
            enc_float4_encrypt('0.2') * avg(l_quantity)
        from
            lineitem
        where
            l_partkey = p_partkey
    );
