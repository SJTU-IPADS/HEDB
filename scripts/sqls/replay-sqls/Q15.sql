select enable_debug_mode(1);
select enable_replay_mode('Q15', 'seq');
\timing
WITH revenue (supplier_no, total_revenue) as (
    SELECT
        l_suppkey,SUM(l_extendedprice * ('1'-l_discount))
    FROM
        lineitem
    WHERE
        l_shipdate >= pg_enc_timestamp_encrypt('1995-05-01') 
        AND l_shipdate < pg_enc_timestamp_encrypt('1995-08-01')
    GROUP BY l_suppkey
    )
SELECT
s_suppkey,s_name,s_address,s_phone,total_revenue
FROM
    supplier, revenue
WHERE
    s_suppkey = supplier_no AND total_revenue = (
    SELECT MAX(total_revenue)FROM revenue)
ORDER BY
    s_suppkey;
