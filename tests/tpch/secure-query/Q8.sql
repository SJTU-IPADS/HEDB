select
o_year, 
sum(case when nation = 'JAPAN'::enc_text then volume else '0'::enc_float4 end)
    / sum(volume) as mkt_share
from
    (select 
        date_part('year', o_orderdate) as o_year,
        l_extendedprice * ('1'::enc_float4-l_discount) as volume,
        n2.n_name as nation
    from
        part,supplier,lineitem,orders,customer,nation n1,nation n2,region 
    where
        p_partkey = l_partkey
        and s_suppkey = l_suppkey
        and l_orderkey = o_orderkey
        and o_custkey = c_custkey
        and c_nationkey = n1.n_nationkey
        and n1.n_regionkey = r_regionkey
        and r_name = 'ASIA'::enc_text
        and s_nationkey = n2.n_nationkey
        and o_orderdate between '1995-01-01'::enc_timestamp and '1996-12-31'::enc_timestamp
        and p_type = 'PROMO BURNISHED COPPER'::enc_text
        ) as all_nations
group by o_year 
order by o_year;
