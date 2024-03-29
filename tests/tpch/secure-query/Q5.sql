select
    n_name,sum(l_extendedprice * (enc_float4_encrypt('1') - l_discount)) as revenue
from
    customer,orders,lineitem,supplier,nation,region
where
    c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and l_suppkey = s_suppkey
    and c_nationkey = s_nationkey
    and s_nationkey = n_nationkey
    and n_regionkey = r_regionkey
    and r_name = enc_text_encrypt('AMERICA')
    and o_orderdate >= enc_timestamp_encrypt('1994-01-01')
    and o_orderdate < enc_timestamp_encrypt('1995-01-01')
group by
    n_name
order by
    revenue desc;

