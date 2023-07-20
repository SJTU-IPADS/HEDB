select
    l_shipmode,
    sum(case 
        when o_orderpriority ='1-URGENT'::enc_text or o_orderpriority ='2-HIGH'::enc_text
        then 1::enc_int4 else 0::enc_int4 end) as high_line_count,
    sum(case 
        when o_orderpriority <> '1-URGENT'::enc_text
        and o_orderpriority <> '2-HIGH'::enc_text
        then 1::enc_int4 else 0::enc_int4 end) as low_line_count
from
    orders,lineitem
where
    o_orderkey = l_orderkey
    and l_shipmode in ('TRUCK'::enc_text, 'REG AIR'::enc_text)
    and l_commitdate < l_receiptdate
    and l_shipdate < l_commitdate
    and l_receiptdate >= '1995-01-01'::enc_timestamp
    and l_receiptdate < '1996-01-01'::enc_timestamp
group by l_shipmode
order by l_shipmode;
