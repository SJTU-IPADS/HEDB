select
    l_orderkey,sum(l_extendedprice*(enc_float4_encrypt('1')-l_discount)) as revenue,
    o_orderdate,
    o_shippriority
from
    customer, orders, lineitem
where
    c_mktsegment = enc_text_encrypt('AUTOMOBILE')
    and c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and o_orderdate < enc_timestamp_encrypt('1995-03-15')
    and l_shipdate > enc_timestamp_encrypt('1995-03-15')
group by
    l_orderkey,
    o_orderdate,
    o_shippriority
order by
    revenue desc,
    o_orderdate;

