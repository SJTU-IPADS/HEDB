select
    l_orderkey,sum(l_extendedprice*('1'::enc_float4-l_discount)) as revenue,
    o_orderdate,
    o_shippriority
from
    customer, orders, lineitem
where
    c_mktsegment = 'AUTOMOBILE'::enc_text
    and c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and o_orderdate < '1995-03-15'::enc_timestamp
    and l_shipdate > '1995-03-15'::enc_timestamp
group by
    l_orderkey,
    o_orderdate,
    o_shippriority
order by
    revenue desc,
    o_orderdate;

