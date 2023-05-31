select enable_debug_mode(1);
\timing
select
    l_orderkey,sum(pg_enc_float4_eval_expr('1*(2-3)', l_extendedprice, '1'::enc_float4, l_discount)) as revenue,
    o_orderdate,
    o_shippriority
from
    customer, orders, lineitem
where
    c_mktsegment = 'AUTOMOBILE'
    and c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and o_orderdate < pg_enc_timestamp_encrypt('1995-03-15')
    and l_shipdate > pg_enc_timestamp_encrypt('1995-03-15')
group by
    l_orderkey,
    o_orderdate,
    o_shippriority
order by
    revenue desc,
    o_orderdate;

