select
    enc_float4_encrypt('100.00') * sum(case
        when p_type like enc_text_encrypt('PROMO%')
        then l_extendedprice*(enc_float4_encrypt('1') - l_discount) 
        else enc_float4_encrypt('0') end) / sum(l_extendedprice * (enc_float4_encrypt('1') - l_discount))
    as promo_revenue
from
    lineitem, part
where
    l_partkey = p_partkey
    and l_shipdate >= enc_timestamp_encrypt('1995-06-01')
    and l_shipdate < enc_timestamp_encrypt('1995-07-01');
