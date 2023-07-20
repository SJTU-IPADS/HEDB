select
    '100.00'::enc_float4 * sum(case
        when p_type like 'PROMO%'::enc_text
        then l_extendedprice*('1'::enc_float4 - l_discount) 
        else '0'::enc_float4 end) / sum(l_extendedprice * ('1'::enc_float4 - l_discount))
    as promo_revenue
from
    lineitem, part
where
    l_partkey = p_partkey
    and l_shipdate >= '1995-06-01'::enc_timestamp
    and l_shipdate < '1995-07-01'::enc_timestamp;
