WITH revenue (supplier_no, total_revenue) as (
    SELECT
        l_suppkey, SUM(l_extendedprice * (enc_float4_encrypt('1') - l_discount))
    FROM
        lineitem
    WHERE
        l_shipdate >= enc_timestamp_encrypt('1995-05-01')
        AND l_shipdate < enc_timestamp_encrypt('1995-08-01')
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
