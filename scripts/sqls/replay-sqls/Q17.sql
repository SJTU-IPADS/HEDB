select enable_debug_mode(1);
select enable_replay_mode('Q17', 'seq');
\timing
select
    sum(l_extendedprice) / '7.0' as avg_yearly
from
    lineitem, part
where
    p_partkey = l_partkey
    and p_brand = 'Brand#15' 
    and p_container = 'JUMBO CASE'
    and l_quantity < (
        select
            '0.2' * avg(l_quantity)
        from
            lineitem
        where
            l_partkey = p_partkey
    );
