DROP EXTENSION IF EXISTS pgtap CASCADE;
CREATE EXTENSION pgtap;

DROP EXTENSION IF EXISTS hedb CASCADE;
CREATE EXTENSION hedb;

\unset ECHO

select plan(46);

DROP TABLE IF EXISTS test_table;
CREATE TABLE test_table (id int, num_i enc_int4, num_f enc_float4, str enc_text, time enc_timestamp);

select ok(enc_int4_decrypt(enc_int4_encrypt(1)) = 1::int4, 'enc_int4: encryption/decryption test');
select ok(enc_float4_decrypt(enc_float4_encrypt(1.1)) = 1.1::float4, 'enc_float4: encryption/decryption test');

INSERT INTO test_table VALUES (1, '1', '1.1', 'hello', '2020-01-01');
INSERT INTO test_table VALUES (2, '2', '2.1', 'world', '2021-01-01');
INSERT INTO test_table VALUES (3, '3', '3.1', 'from', '2022-01-01');
INSERT INTO test_table VALUES (3, '3', '3.1', 'hedb', '2023-01-01');

SELECT results_eq(
   'select SUM(num_i) from test_table',
    $$VALUES (9::enc_int4)$$,
    'enc_int4: SUM function '
);

SELECT results_eq(
   'select MIN(num_i) from test_table',
    $$VALUES (1::enc_int4)$$,
    'enc_int4: MIN function '
);

SELECT results_eq(
   'select MAX(num_i) from test_table',
    $$VALUES (3::enc_int4)$$,
    'enc_int4: MAX function '
);

SELECT results_eq(
   'select AVG(num_i) from test_table',
    $$VALUES (2::enc_int4)$$,
    'enc_int4: AVG function (with rounding)'
);

SELECT results_eq(
   'select SUM(num_f) from test_table',
    $$VALUES (9.4::enc_float4)$$,
    'enc_float4: SUM function'
);

SELECT results_eq(
   'select AVG(num_f) from test_table',
    $$VALUES (2.35::enc_float4)$$,
    'enc_float4: AVG function'
);

select ok(1::enc_int4 = 1::enc_int4, 'enc_int4: inequality test, operator =');
select ok(0::enc_int4 != 1::enc_int4, 'enc_int4: inequality test, operator !=');
select ok(0::enc_int4 <> 1::enc_int4, 'enc_int4: inequality test, operator <>');
select ok(1::enc_int4 <= 2::enc_int4, 'enc_int4: inequality test, operator <=');
select ok(1::enc_int4 <= 1::enc_int4, 'enc_int4: inequality test, operator <=');
select ok(3::enc_int4 >= 2::enc_int4, 'enc_int4: inequality test, operator >=');
select ok(1::enc_int4 >= 1::enc_int4, 'enc_int4: inequality test, operator >=');
select ok(2::enc_int4 < 3::enc_int4, 'enc_int4: inequality test, operator <');
select ok(3::enc_int4 > 2::enc_int4, 'enc_int4: inequality test, operator >');

select ok(2::enc_int4 + 1::enc_int4 = 3::enc_int4, 'enc_int4: operator +');
select ok(2::enc_int4 - 1::enc_int4 = 1::enc_int4, 'enc_int4: operator -');
select ok(2::enc_int4 * 2::enc_int4 = 4::enc_int4, 'enc_int4: operator *');
select ok(6::enc_int4 / 2::enc_int4 = 3::enc_int4, 'enc_int4: operator /');

select ok(1.1::enc_float4 = 1.1::enc_float4, 'enc_float4: inequality test, operator =');
select ok(0.2::enc_float4 != 1.1::enc_float4, 'enc_float4: inequality test, operator !=');
select ok(0.2::enc_float4 <> 1.1::enc_float4, 'enc_float4: inequality test, operator <>');
select ok(1.1::enc_float4 <= 2.3::enc_float4, 'enc_float4: inequality test, operator <=');
select ok(1.1::enc_float4 <= 1.1::enc_float4, 'enc_float4: inequality test, operator <=');
select ok(3.4::enc_float4 >= 2.3::enc_float4, 'enc_float4: inequality test, operator >=');
select ok(1.1::enc_float4 >= 1.1::enc_float4, 'enc_float4: inequality test, operator >=');
select ok(2.3::enc_float4 < 3.4::enc_float4, 'enc_float4: inequality test, operator <');
select ok(3.4::enc_float4 > 2.3::enc_float4, 'enc_float4: inequality test, operator >');
select ok(2.3::enc_float4 + 1.1::enc_float4 = 3.4::enc_float4, 'enc_float4: operator +');
select ok(9.9::enc_float4 / 3.3::enc_float4 = 3::enc_float4, 'enc_float4: operator /');

select ok('11/11/12'::enc_timestamp = '11/11/12'::enc_timestamp, 'enc_timestamp: inequality test, operator =');
select ok('11/11/12 00:00:01'::enc_timestamp != '11/11/12 00:00:02'::enc_timestamp, 'enc_timestamp: inequality test, operator !=');
select ok('11/11/12 00:00:01'::enc_timestamp <> '11/11/12 00:00:02'::enc_timestamp, 'enc_timestamp: inequality test, operator <>');
select ok('11/11/12 00:00:01'::enc_timestamp <= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: inequality test, operator <=');
select ok('11/11/12 00:00:01'::enc_timestamp <= '11/11/12 00:00:03'::enc_timestamp, 'enc_timestamp: inequality test, operator <=');
select ok('11/11/12 00:00:01'::enc_timestamp >= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: inequality test, operator >=');
select ok('11/11/12 00:00:02'::enc_timestamp >= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: inequality test, operator >=');
select ok('11/11/12 00:00:01'::enc_timestamp < '11/11/12 00:00:03'::enc_timestamp, 'enc_timestamp: inequality test, operator <');
select ok('11/11/12 00:00:03'::enc_timestamp > '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: inequality test, operator >');

select ok('test1'::enc_text = 'test1'::enc_text, 'enc_text: inequality test, operator =');
select ok('test1'::enc_text != 'test2'::enc_text, 'enc_text: inequality test, operator !=');
select ok('test1'::enc_text <> 'test2'::enc_text, 'enc_text: inequality test, operator <>');
select ok('hello'::enc_text || 'world'::enc_text = 'helloworld'::enc_text, 'enc_text: operator ||');
select ok(substring('hedb'::enc_text, 2, 3) = 'edb'::enc_text, 'enc_text: operator substring');

select * from finish();
DROP TABLE IF EXISTS test_table;
