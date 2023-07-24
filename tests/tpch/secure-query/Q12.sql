select
    l_shipmode,
    sum(case 
        when o_orderpriority = enc_text_encrypt('1-URGENT') or o_orderpriority = enc_text_encrypt('2-HIGH')
        then enc_int4_encrypt('1') else enc_int4_encrypt('0') end) as high_line_count,
    sum(case 
        when o_orderpriority <> enc_text_encrypt('1-URGENT')
        and o_orderpriority <> enc_text_encrypt('2-HIGH')
        then enc_int4_encrypt('1') else enc_int4_encrypt('0') end) as low_line_count
from
    orders,lineitem
where
    o_orderkey = l_orderkey
    and l_shipmode in (enc_text_encrypt('TRUCK'), enc_text_encrypt('REG AIR'))
    and l_commitdate < l_receiptdate
    and l_shipdate < l_commitdate
    and l_receiptdate >= enc_timestamp_encrypt('1995-01-01')
    and l_receiptdate < enc_timestamp_encrypt('1996-01-01')
group by l_shipmode
order by l_shipmode;
