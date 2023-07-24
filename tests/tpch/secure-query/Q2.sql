select
    s_acctbal, s_name, n_name, p_partkey, p_mfgr, s_address, s_phone, s_comment
from
    part, supplier, partsupp, nation, region
where
    p_partkey = ps_partkey
    and s_suppkey = ps_suppkey
    and p_size = enc_int4_encrypt('25')
    and p_type like enc_text_encrypt('%PROMO BURNISHED COPPER')
    and s_nationkey = n_nationkey
    and n_regionkey = r_regionkey
    and r_name = enc_text_encrypt('AMERICA')
    and ps_supplycost = (
        select
            min(ps_supplycost)
        from
            partsupp, supplier, nation, region
        where
            p_partkey = ps_partkey
            and s_suppkey = ps_suppkey
            and s_nationkey = n_nationkey
            and n_regionkey = r_regionkey
            and r_name = enc_text_encrypt('AMERICA')
    )order by
    s_acctbal desc,
    n_name,
    s_name,
    p_partkey;
