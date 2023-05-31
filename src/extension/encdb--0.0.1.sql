-- noinspection SqlNoDataSourceInspectionForFile

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION encdb" to load this file. \quit

CREATE FUNCTION launch() RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- CREATE OR REPLACE FUNCTION generate_key()
-- RETURNS int
-- AS 'MODULE_PATHNAME'
-- LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- CREATE OR REPLACE FUNCTION load_key(int)
-- RETURNS int
-- AS 'MODULE_PATHNAME'
-- LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION enable_debug_mode(int)
RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION enable_record_mode(cstring)
RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION enable_replay_mode(cstring, cstring)
RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-------------------------------------------------------------------------------
--ENCRYPTED INTEGER TYPE (randomized)
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION pg_enc_int4_in(cstring)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_out(enc_int4)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_eq(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_ne(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_lt(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_le(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_gt(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_ge(enc_int4, enc_int4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_encrypt(integer)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_decrypt(enc_int4)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_add(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_sub(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_mult(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_div(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_mod(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_pow(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_int4_cmp(enc_int4, enc_int4)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

--CREATE OR REPLACE FUNCTION pg_enc_int4_recv(internal)
--RETURNS enc_int4
--AS 'MODULE_PATHNAME'
--LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

--CREATE OR REPLACE FUNCTION pg_enc_int4_send(enc_int4)
--RETURNS bytea
--AS 'MODULE_PATHNAME'
--LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_int4 (
    INPUT          = pg_enc_int4_in,
    OUTPUT         = pg_enc_int4_out,
--    RECEIVE        = pg_enc_int4_recv,
--    SEND           = pg_enc_int4_send,
    INTERNALLENGTH = 32, -- 4 + 12 + 16
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);
COMMENT ON TYPE enc_int4 IS 'ENCRYPTED INTEGER';

-- CREATE FUNCTION pg_enc_int4_addfinal(enc_int4[])
-- RETURNS enc_int4
-- AS 'MODULE_PATHNAME'
-- LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_sum_bulk(enc_int4[])
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- CREATE FUNCTION pg_enc_int4_avgfinal(enc_int4[])
-- RETURNS enc_float4
-- AS 'MODULE_PATHNAME'
-- LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_avg_bulk(enc_int4[])
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_min_bulk(enc_int4[])
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE FUNCTION pg_enc_int4_max_bulk(enc_int4[])
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_min(enc_int4,enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_int4_max(enc_int4, enc_int4)
RETURNS enc_int4
AS 'MODULE_PATHNAME'

LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE OPERATOR <> (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);
CREATE OPERATOR < (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

-- CREATE AGGREGATE sum (enc_int4)
-- (
--    sfunc = pg_enc_int4_add,
--    stype = enc_int4,
--    PARALLEL = safe, 
--    combinefunc = pg_enc_int4_add
-- );

CREATE AGGREGATE sum (enc_int4)
(
   sfunc = array_append,
   stype = enc_int4[],
   PARALLEL = SAFE,
   COMBINEFUNC = array_cat,
   finalfunc = pg_enc_int4_sum_bulk  
);

CREATE AGGREGATE avg (enc_int4)
(
   sfunc = array_append,
   stype = enc_int4[],
   finalfunc = pg_enc_int4_avg_bulk

);

-- CREATE AGGREGATE avg_simple (enc_int4)
-- (
--    sfunc = pg_enc_int4_accum,
--    stype = enc_int4[],
--    initcond = '{0,0}',
--    finalfunc = pg_enc_int4_avgfinal

-- );

CREATE AGGREGATE min (enc_int4)
(  
   sfunc = pg_enc_int4_min,
   stype = enc_int4,
   PARALLEL = safe, 
   combinefunc = pg_enc_int4_min

);

CREATE AGGREGATE max (enc_int4)
(
   sfunc = pg_enc_int4_max,
   stype = enc_int4,
   PARALLEL = safe, 
   combinefunc = pg_enc_int4_max

);

CREATE OPERATOR + (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_add
);


CREATE OPERATOR - (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_sub
);

CREATE OPERATOR * (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_mult
);

CREATE OPERATOR / (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_div
);

CREATE OPERATOR % (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_mod
);

CREATE OPERATOR ^ (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = pg_enc_int4_pow
);


CREATE OPERATOR CLASS btree_pg_enc_int4_ops
DEFAULT FOR TYPE enc_int4 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       pg_enc_int4_cmp(enc_int4, enc_int4);

CREATE OR REPLACE FUNCTION enc_int4(int4)
    RETURNS enc_int4
    AS 'MODULE_PATHNAME', 'pg_int4_to_enc_int4'
    LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION enc_int4(int8)
    RETURNS enc_int4
    AS 'MODULE_PATHNAME', 'pg_int8_to_enc_int4'
    LANGUAGE C STRICT IMMUTABLE;

CREATE CAST (int4 AS enc_int4) WITH FUNCTION enc_int4(int4) AS IMPLICIT;
CREATE CAST (int8 AS enc_int4) WITH FUNCTION enc_int4(int8) AS IMPLICIT;

--------------------------------------------------------------------------------
--ENCRYPTED STRING TYPE (randomized)
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION pg_enc_text_in(cstring)
RETURNS enc_text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_text_out(enc_text)
RETURNS cstring
--LANGUAGE internal IMMUTABLE AS 'textout';
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_eq(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_ne(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_lt(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_le(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_gt(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_ge(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_cmp(enc_text, enc_text)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_concatenate(enc_text, enc_text)
RETURNS enc_text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_catalog.substring(enc_text, enc_int4, enc_int4)
RETURNS enc_text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_like(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_notlike(enc_text, enc_text)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- sets the order in the text field in enc_text
CREATE OR REPLACE FUNCTION pg_enc_text_set_order(enc_text, int4)
RETURNS enc_text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_encrypt(cstring)
RETURNS enc_text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_text_decrypt(enc_text)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_text (
    INPUT          = pg_enc_text_in,
    OUTPUT         = pg_enc_text_out,
--      LIKE       = text,
    INTERNALLENGTH = VARIABLE,
--    CATEGORY = 'S',
--    PREFERRED = false
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);
COMMENT ON TYPE enc_text IS 'ENCRYPTED STRING';

CREATE OPERATOR = (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE OPERATOR <> (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);
CREATE OPERATOR < (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR || (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_concatenate
);

CREATE OPERATOR ~~ (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_like
);

CREATE OPERATOR !~~ (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = pg_enc_text_notlike
);

CREATE OPERATOR CLASS btree_pg_enc_text_ops
DEFAULT FOR TYPE enc_text USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       pg_enc_text_cmp(enc_text, enc_text);



CREATE OR REPLACE FUNCTION enc_text(varchar)
    RETURNS enc_text
    AS 'MODULE_PATHNAME', 'varchar_to_enc_text'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE CAST (varchar AS enc_text) WITH FUNCTION enc_text(varchar) AS IMPLICIT;

--------------------------------------------------------------------------------
--ENCRYPTED FLOAT4 TYPE (randomized)
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION pg_enc_float4_in(cstring)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_float4_out(enc_float4)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_float4 (
    INPUT          = pg_enc_float4_in,
    OUTPUT         = pg_enc_float4_out,
    -- INTERNALLENGTH = 45,
    INTERNALLENGTH = 32,
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);

CREATE FUNCTION pg_enc_float4_encrypt(float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_decrypt(enc_float4)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_eq(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_ne(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_lt(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_le(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_gt(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_ge(enc_float4, enc_float4)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_cmp(enc_float4, enc_float4)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_add(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_subs(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_mult(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_div(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_exp(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_sum_bulk(enc_float4[])
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE FUNCTION pg_enc_float4_max(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_min(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_avg_bulk(enc_float4[])
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_avg_simple(enc_float4[])
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_mod(enc_float4, enc_float4)
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_float4_eval_expr(VARIADIC "any")
RETURNS enc_float4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR = (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE OPERATOR <> (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);
CREATE OPERATOR < (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR + (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_add
);

CREATE OPERATOR - (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_subs
);

CREATE OPERATOR * (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_mult
);

CREATE OPERATOR / (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_div
);

CREATE OPERATOR % (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_mod
);

CREATE OPERATOR ^ (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = pg_enc_float4_exp
);


CREATE OPERATOR CLASS btree_pg_enc_float4_ops
DEFAULT FOR TYPE enc_float4 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       pg_enc_float4_cmp(enc_float4, enc_float4);


CREATE AGGREGATE sum_simple (enc_float4)
(
   sfunc = pg_enc_float4_add,
   stype = enc_float4,
   PARALLEL = SAFE,
   COMBINEFUNC = pg_enc_float4_add 
);

CREATE AGGREGATE sum (enc_float4)
(
   sfunc = array_append,
   stype = enc_float4[],
   PARALLEL = SAFE,
   COMBINEFUNC = array_cat,
   finalfunc = pg_enc_float4_sum_bulk  
);

CREATE AGGREGATE avg (enc_float4)
(
   sfunc = array_append,
   stype = enc_float4[],
   finalfunc = pg_enc_float4_avg_bulk
);

CREATE AGGREGATE avg_simple (enc_float4)
(
   sfunc = array_append,
   stype = enc_float4[],
   finalfunc = pg_enc_float4_avg_simple
);

CREATE AGGREGATE max (enc_float4)
(
   sfunc = pg_enc_float4_max,
   stype = enc_float4,
   PARALLEL = SAFE,
   combinefunc = pg_enc_float4_max
);

CREATE AGGREGATE min (enc_float4)
(
   sfunc = pg_enc_float4_min,
   stype = enc_float4,
   PARALLEL = SAFE,
   combinefunc = pg_enc_float4_min
);

CREATE OR REPLACE FUNCTION enc_float4(float4)
    RETURNS enc_float4
    AS 'MODULE_PATHNAME', 'float4_to_enc_float4'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE OR REPLACE FUNCTION enc_float4(double precision)
    RETURNS enc_float4
    AS 'MODULE_PATHNAME', 'double_to_enc_float4'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE OR REPLACE FUNCTION enc_float4(numeric)
    RETURNS enc_float4
    AS 'MODULE_PATHNAME', 'numeric_to_enc_float4'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE OR REPLACE FUNCTION enc_float4(int8)
    RETURNS enc_float4
    AS 'MODULE_PATHNAME', 'int8_to_enc_float4'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE OR REPLACE FUNCTION enc_float4(int4)
    RETURNS enc_float4
    AS 'MODULE_PATHNAME', 'int4_to_enc_float4'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE CAST (float4 AS enc_float4) WITH FUNCTION enc_float4(float4) AS IMPLICIT;
CREATE CAST (double precision AS enc_float4) WITH FUNCTION enc_float4(double precision) AS IMPLICIT;
CREATE CAST (numeric AS enc_float4) WITH FUNCTION enc_float4(numeric) AS IMPLICIT;
CREATE CAST (int8 AS enc_float4) WITH FUNCTION enc_float4(int8) AS IMPLICIT;
CREATE CAST (int4 AS enc_float4) WITH FUNCTION enc_float4(int4) AS IMPLICIT;
--------------------------------------------------------------------------------
--ENCRYPTED TIMESTAMP TYPE (randomized)
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION pg_enc_timestamp_in(cstring)
RETURNS enc_timestamp
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_enc_timestamp_out(enc_timestamp)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_timestamp (
    INPUT          = pg_enc_timestamp_in,
    OUTPUT         = pg_enc_timestamp_out,
    -- INTERNALLENGTH = 49,
    INTERNALLENGTH = 36,
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);

CREATE FUNCTION pg_enc_timestamp_encrypt(cstring)
RETURNS enc_timestamp
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_decrypt(enc_timestamp)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_eq(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_ne(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_lt(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_le(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_catalog.date_part(text, enc_timestamp)
RETURNS enc_int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_gt(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_ge(enc_timestamp, enc_timestamp)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION pg_enc_timestamp_cmp(enc_timestamp, enc_timestamp)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR = (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE OPERATOR <> (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);
CREATE OPERATOR < (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = pg_enc_timestamp_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS btree_enc_timestamp_ops
DEFAULT FOR TYPE enc_timestamp USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       pg_enc_timestamp_cmp(enc_timestamp, enc_timestamp);


CREATE OR REPLACE FUNCTION enc_timestamp(timestamp)
    RETURNS enc_timestamp
    AS 'MODULE_PATHNAME', 'pg_enc_timestamp_encrypt'
    LANGUAGE C STRICT IMMUTABLE ;

CREATE CAST (timestamp AS enc_timestamp) WITH FUNCTION enc_timestamp(timestamp) AS IMPLICIT;

-- CREATE CAST (varchar AS enc_timestamp) WITH FUNCTION enc_timestamp(varchar) AS IMPLICIT;
-- CREATE OR REPLACE FUNCTION enc_text(varchar)
--     RETURNS enc_text
--     AS 'MODULE_PATHNAME', 'varchar_to_enc_timestamp'
--     LANGUAGE C STRICT IMMUTABLE ;

-- sets order info in EncStr.order field
-- if order info not available, order is set to -1, and comparison goes to TEE
-- Usage: SELECT enc_text_add_order($COLUMN_NAME, $TABLE_NAME);
-- Hash Join must be disabled, since no hash function available for type EncText
--    (or maybe we should implement one?
CREATE OR REPLACE FUNCTION enc_text_add_order(p_column text, p_table text)
	RETURNS void
AS
$$
BEGIN
EXECUTE FORMAT(
'SET enable_hashjoin=off;
WITH o_res AS
(SELECT s.%1$s, ROW_NUMBER() OVER (ORDER BY s.%1$s ASC) AS order 	
    FROM (SELECT DISTINCT (%1$s) FROM %2$s) s)
UPDATE %2$s r
    SET %1$s = pg_enc_text_set_order(o_res.%1$s, o_res.order::int4)
	FROM o_res
    WHERE o_res.%1$s = r.%1$s;',
p_column, p_table);
END;
$$
LANGUAGE plpgsql;
