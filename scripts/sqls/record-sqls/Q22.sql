select enable_debug_mode(1);
select enable_record_mode('Q22');
\timing
select
    cntrycode,
    count(*) as numcust,
    sum(c_acctbal) as totacctbal
from (
    select
        substring(c_phone from '1' for '2') as cntrycode,
        c_acctbal
    from customer
    where
        substring(c_phone from '1' for '2') in ('28','14','22','10','26','33','30')
        and c_acctbal > (
            select avg(c_acctbal) from customer
            where
                c_acctbal > '0.00'
                and substring (c_phone from '1' for '2') 
                                in ('28','14','22','10','26','33','30'))
        and not exists (
            select * from orders where  o_custkey = c_custkey)
        ) as custsale
group by
    cntrycode
order by
    cntrycode;