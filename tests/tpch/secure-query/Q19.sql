select
    sum(l_extendedprice * (enc_float4_encrypt('1') - l_discount) ) as revenue
from
    lineitem, part
where(
    p_partkey = l_partkey
    and p_brand = enc_text_encrypt('Brand#15')
    and p_container in ( enc_text_encrypt('SM CASE'), enc_text_encrypt('SM BOX'), enc_text_encrypt('SM PACK'), enc_text_encrypt('SM PKG'))
    and l_quantity >= enc_float4_encrypt('5') and l_quantity <= enc_float4_encrypt('15')
    and p_size between enc_int4_encrypt('1') and enc_int4_encrypt('5')
    and l_shipmode in (enc_text_encrypt('AIR'), enc_text_encrypt('AIR REG'))
    and l_shipinstruct = enc_text_encrypt('DELIVER IN PERSON'))
or(
    p_partkey = l_partkey
    and p_brand = enc_text_encrypt('Brand#25')
    and p_container in (enc_text_encrypt('MED BAG'), enc_text_encrypt('MED BOX'), enc_text_encrypt('MED PKG'), enc_text_encrypt('MED PACK'))
    and l_quantity >= enc_float4_encrypt('15') and l_quantity <= enc_float4_encrypt('25')
    and p_size between enc_int4_encrypt('1') and enc_int4_encrypt('10')
    and l_shipmode in (enc_text_encrypt('AIR'), enc_text_encrypt('AIR REG'))
    and l_shipinstruct = enc_text_encrypt('DELIVER IN PERSON'))
or(
    p_partkey = l_partkey
    and p_brand = enc_text_encrypt('Brand#35')
    and p_container in ( enc_text_encrypt('LG CASE'), enc_text_encrypt('LG BOX'), enc_text_encrypt('LG PACK'), enc_text_encrypt('LG PKG'))
    and l_quantity >= enc_float4_encrypt('25') and l_quantity <= enc_float4_encrypt('35')
    and p_size between enc_int4_encrypt('1') and enc_int4_encrypt('15')
    and l_shipmode in (enc_text_encrypt('AIR'), enc_text_encrypt('AIR REG'))
    and l_shipinstruct = enc_text_encrypt('DELIVER IN PERSON'));
