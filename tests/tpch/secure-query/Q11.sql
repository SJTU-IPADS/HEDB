select
    ps_partkey,sum(ps_supplycost * ps_availqty) as value 
from
    partsupp, supplier, nation
where
    ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = enc_text_encrypt('JAPAN')
group by
    ps_partkey 
having 
    sum(ps_supplycost * ps_availqty) > 
( select sum(ps_supplycost * ps_availqty) * enc_float4_encrypt('0.0001')
  from
    partsupp, supplier, nation 
  where
    ps_suppkey = s_suppkey 
    and s_nationkey = n_nationkey 
    and n_name = enc_text_encrypt('JAPAN'))
order by value desc;
