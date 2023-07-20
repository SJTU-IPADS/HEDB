select
    sum(l_extendedprice*l_discount) as revenue 
from
    lineitem
where
    l_shipdate >= '1995-01-01'::enc_timestamp
and l_shipdate < '1996-01-01'::enc_timestamp
and l_discount between '0.07'::enc_float4 and '0.09'::enc_float4
and l_quantity < '24'::enc_float4;
