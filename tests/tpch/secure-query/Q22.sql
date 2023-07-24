select
    cntrycode,
    count(*) as numcust,
    sum(c_acctbal) as totacctbal
from (
    select
        substring(c_phone from 1 for 2) as cntrycode,
        c_acctbal
    from customer
    where
        substring(c_phone from 1 for 2) in (enc_text_encrypt('28'),enc_text_encrypt('14'),enc_text_encrypt('22'),enc_text_encrypt('10'),enc_text_encrypt('26'),enc_text_encrypt('33'),enc_text_encrypt('30'))
        and c_acctbal > (
            select avg(c_acctbal) from customer
            where
                c_acctbal > enc_float4_encrypt('0.00')
                and substring (c_phone from 1 for 2)
                                in (enc_text_encrypt('28'),enc_text_encrypt('14'),enc_text_encrypt('22'),enc_text_encrypt('10'),enc_text_encrypt('26'),enc_text_encrypt('33'),enc_text_encrypt('30')))
        and not exists (
            select * from orders where  o_custkey = c_custkey)
        ) as custsale
group by
    cntrycode
order by
    cntrycode;
