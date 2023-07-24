select
    p_brand,p_type,p_size,
    count(distinct ps_suppkey) as supplier_cnt
from
    partsupp,part
where
    p_partkey = ps_partkey
    and p_brand <> enc_text_encrypt('Brand#12')
    and p_type not like enc_text_encrypt('PROMO%')
    and p_size in (enc_int4_encrypt('10'), enc_int4_encrypt('12'), enc_int4_encrypt('14'), enc_int4_encrypt('16'), enc_int4_encrypt('18'), enc_int4_encrypt('20') , enc_int4_encrypt('22'), enc_int4_encrypt('24'))
    and ps_suppkey not in (
        select
            s_suppkey
        from
            supplier
        where
            s_comment like enc_text_encrypt('%Customer%Complaints%')
    )
group by
    p_brand,p_type,p_size
order by
    supplier_cnt desc,p_brand,p_type,p_size;

