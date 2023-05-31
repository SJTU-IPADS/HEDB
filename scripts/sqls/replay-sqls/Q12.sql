select enable_debug_mode(1);
select enable_replay_mode('Q12', 'seq');
\timing
select
    l_shipmode,
    sum(case 
        when o_orderpriority ='1-URGENT' or o_orderpriority ='2-HIGH'
        then 1 else 0 end) as high_line_count,
    sum(case 
        when o_orderpriority <> '1-URGENT'
        and o_orderpriority <> '2-HIGH' 
        then 1 else 0 end) as low_line_count
from
    orders,lineitem
where
    o_orderkey = l_orderkey
    and l_shipmode in ('TRUCK', 'REG AIR')
    and l_commitdate < l_receiptdate
    and l_shipdate < l_commitdate
    and l_receiptdate >= pg_enc_timestamp_encrypt('1995-01-01')
    and l_receiptdate < pg_enc_timestamp_encrypt('1996-01-01')
group by l_shipmode
order by l_shipmode;
