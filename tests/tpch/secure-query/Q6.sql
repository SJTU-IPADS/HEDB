select
    sum(l_extendedprice*l_discount) as revenue 
from
    lineitem
where
    l_shipdate >= enc_timestamp_encrypt('1995-01-01')
and l_shipdate < enc_timestamp_encrypt('1996-01-01')
and l_discount between enc_float4_encrypt('0.07') and enc_float4_encrypt('0.09')
and l_quantity < enc_float4_encrypt('24');
