DROP EXTENSION IF EXISTS pgtap CASCADE;
CREATE EXTENSION pgtap;

DROP EXTENSION IF EXISTS hedb CASCADE;
CREATE EXTENSION hedb;

SELECT enable_client_mode();

\unset ECHO

SELECT plan(48);

SELECT ok('2'::enc_int4 + '1'::enc_int4 = '3'::enc_int4, 'enc_int4: operator +');
SELECT ok('2'::enc_int4 - '1'::enc_int4 = '1'::enc_int4, 'enc_int4: operator -');
SELECT ok('2'::enc_int4 * '2'::enc_int4 = '4'::enc_int4, 'enc_int4: operator *');
SELECT ok('6'::enc_int4 / '2'::enc_int4 = '3'::enc_int4, 'enc_int4: operator /');
SELECT ok('1'::enc_int4 = '1'::enc_int4, 'enc_int4: operator =');
SELECT ok('0'::enc_int4 != '1'::enc_int4, 'enc_int4: operator !=');
SELECT ok('0'::enc_int4 <> '1'::enc_int4, 'enc_int4: operator <>');
SELECT ok('1'::enc_int4 <= '2'::enc_int4, 'enc_int4: operator <=');
SELECT ok('1'::enc_int4 <= '1'::enc_int4, 'enc_int4: operator <=');
SELECT ok('3'::enc_int4 >= '2'::enc_int4, 'enc_int4: operator >=');
SELECT ok('1'::enc_int4 >= '1'::enc_int4, 'enc_int4: operator >=');
SELECT ok('2'::enc_int4 < '3'::enc_int4, 'enc_int4: operator <');
SELECT ok('3'::enc_int4 > '2'::enc_int4, 'enc_int4: operator >');

SELECT ok('2.3'::enc_float4 + '1.1'::enc_float4 = '3.4'::enc_float4, 'enc_float4: operator +');
SELECT ok('2.0'::enc_float4 - '1.1'::enc_float4 = '0.9'::enc_float4, 'enc_float4: operator -');
SELECT ok('2.0'::enc_float4 * '1.1'::enc_float4 = '2.2'::enc_float4, 'enc_float4: operator *');
SELECT ok('9.9'::enc_float4 / '3.3'::enc_float4 = '3'::enc_float4, 'enc_float4: operator /');
SELECT ok('1.1'::enc_float4 = '1.1'::enc_float4, 'enc_float4: operator =');
SELECT ok('0.2'::enc_float4 != '1.1'::enc_float4, 'enc_float4: operator !=');
SELECT ok('0.2'::enc_float4 <> '1.1'::enc_float4, 'enc_float4: operator <>');
SELECT ok('1.1'::enc_float4 <= '2.3'::enc_float4, 'enc_float4: operator <=');
SELECT ok('1.1'::enc_float4 <= '1.1'::enc_float4, 'enc_float4: operator <=');
SELECT ok('3.4'::enc_float4 >= '2.3'::enc_float4, 'enc_float4: operator >=');
SELECT ok('1.1'::enc_float4 >= '1.1'::enc_float4, 'enc_float4: operator >=');
SELECT ok('2.3'::enc_float4 < '3.4'::enc_float4, 'enc_float4: operator <');
SELECT ok('3.4'::enc_float4 > '2.3'::enc_float4, 'enc_float4: operator >');

SELECT ok('11/11/12'::enc_timestamp = '11/11/12'::enc_timestamp, 'enc_timestamp: operator =');
SELECT ok('11/11/12 00:00:01'::enc_timestamp != '11/11/12 00:00:02'::enc_timestamp, 'enc_timestamp: operator !=');
SELECT ok('11/11/12 00:00:01'::enc_timestamp <> '11/11/12 00:00:02'::enc_timestamp, 'enc_timestamp: operator <>');
SELECT ok('11/11/12 00:00:01'::enc_timestamp <= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: operator <=');
SELECT ok('11/11/12 00:00:01'::enc_timestamp <= '11/11/12 00:00:03'::enc_timestamp, 'enc_timestamp: operator <=');
SELECT ok('11/11/12 00:00:01'::enc_timestamp >= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: operator >=');
SELECT ok('11/11/12 00:00:02'::enc_timestamp >= '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: operator >=');
SELECT ok('11/11/12 00:00:01'::enc_timestamp < '11/11/12 00:00:03'::enc_timestamp, 'enc_timestamp: operator <');
SELECT ok('11/11/12 00:00:03'::enc_timestamp > '11/11/12 00:00:01'::enc_timestamp, 'enc_timestamp: operator >');

SELECT ok('test1'::enc_text = 'test1'::enc_text, 'enc_text: operator =');
SELECT ok('test1'::enc_text != 'test2'::enc_text, 'enc_text: operator !=');
SELECT ok('test1'::enc_text <> 'test2'::enc_text, 'enc_text: operator <>');
SELECT ok('hello'::enc_text || 'world'::enc_text = 'helloworld'::enc_text, 'enc_text: operator ||');
SELECT ok('edb'::enc_text = substring('hedb'::enc_text, '2'::enc_int4, '3'::enc_int4), 'enc_text: operator substring');

DROP TABLE IF EXISTS test_table;
CREATE TABLE test_table (id int, num_i enc_int4, num_f enc_float4, str enc_text, time enc_timestamp);

INSERT INTO test_table VALUES (1, '1', '1.1', 'hello', '2020-01-01');
INSERT INTO test_table VALUES (2, '2', '2.1', 'world', '2021-01-01');
INSERT INTO test_table VALUES (3, '3', '3.1', 'from', '2022-01-01');
INSERT INTO test_table VALUES (3, '4', '3.1', 'hedb', '2023-01-01');

SELECT results_eq(
   'SELECT SUM(num_i) FROM test_table',
    $$VALUES ('10'::enc_int4)$$,
    'enc_int4: SUM function '
);
SELECT results_eq(
   'SELECT AVG(num_i) FROM test_table',
    $$VALUES ('2'::enc_int4)$$,
    'enc_int4: AVG function (with rounding)'
);
SELECT results_eq(
   'SELECT MAX(num_i) FROM test_table',
    $$VALUES ('4'::enc_int4)$$,
    'enc_int4: MAX function '
);
SELECT results_eq(
   'SELECT MIN(num_i) FROM test_table',
    $$VALUES ('1'::enc_int4)$$,
    'enc_int4: MIN function '
);

SELECT results_eq(
   'SELECT SUM(num_f) FROM test_table',
    $$VALUES ('9.4'::enc_float4)$$,
    'enc_float4: SUM function'
);
SELECT results_eq(
   'SELECT AVG(num_f) FROM test_table',
    $$VALUES ('2.35'::enc_float4)$$,
    'enc_float4: AVG function'
);
SELECT results_eq(
   'SELECT MAX(num_f) FROM test_table',
    $$VALUES ('3.1'::enc_float4)$$,
    'enc_float4: MAX function'
);
SELECT results_eq(
   'SELECT MIN(num_f) FROM test_table',
    $$VALUES ('1.1'::enc_float4)$$,
    'enc_float4: MIN function'
);

SELECT * FROM finish();
DROP TABLE IF EXISTS test_table;
