-- noinspection SqlNoDataSourceInspectionForFile

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION hedb" to load this file. \quit

CREATE FUNCTION enable_server_mode()
RETURNS void AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enable_client_mode()
RETURNS void AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enable_record_mode(cstring, cstring)
RETURNS void AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enable_replay_mode(cstring, cstring, cstring)
RETURNS void AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_int4;
CREATE TYPE enc_text;
CREATE TYPE enc_float4;
CREATE TYPE enc_timestamp;

-------------------------------------------------------------------------------
--ENCRYPTED INTEGER TYPE (randomized)
-------------------------------------------------------------------------------

CREATE FUNCTION enc_int4_encrypt(integer)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_int4_decrypt(enc_int4)
RETURNS integer AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_int4_in(cstring)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_int4_out(enc_int4)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_int4 (
    INPUT          = enc_int4_in,
    OUTPUT         = enc_int4_out,
    INTERNALLENGTH = 32, -- 4 + 12 + 16
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);
COMMENT ON TYPE enc_int4 IS 'ENCRYPTED INTEGER';

CREATE FUNCTION enc_int4_eq(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE FUNCTION enc_int4_ne(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <> (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION enc_int4_lt(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR < (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_int4_le(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <= (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_int4_gt(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR > (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_int4_ge(enc_int4, enc_int4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR >= (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_int4_cmp(enc_int4, enc_int4)
RETURNS integer AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR CLASS btree_enc_int4_ops
DEFAULT FOR TYPE enc_int4 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       enc_int4_cmp(enc_int4, enc_int4);

CREATE FUNCTION enc_int4_add(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR + (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_add
);

CREATE FUNCTION enc_int4_sub(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR - (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_sub
);

CREATE FUNCTION enc_int4_mult(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR * (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_mult
);

CREATE FUNCTION enc_int4_div(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR / (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_div
);

CREATE FUNCTION enc_int4_mod(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR % (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_mod
);

CREATE FUNCTION enc_int4_pow(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR ^ (
  LEFTARG = enc_int4,
  RIGHTARG = enc_int4,
  PROCEDURE = enc_int4_pow
);

CREATE FUNCTION enc_int4_sum_bulk(enc_int4[])
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE sum (enc_int4)
(
   sfunc = array_append,
   stype = enc_int4[],
   PARALLEL = SAFE,
   COMBINEFUNC = array_cat,
   finalfunc = enc_int4_sum_bulk  
);
-- CREATE FUNCTION enc_int4_sum(enc_int4, enc_int4)
-- RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
-- CREATE AGGREGATE sum (enc_int4)
-- (
--    sfunc = enc_int4_sum,
--    stype = enc_int4,
--    PARALLEL = safe,
--    combinefunc = enc_int4_sum
-- );

CREATE FUNCTION enc_int4_avg_bulk(enc_int4[])
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE avg (enc_int4)
(
   sfunc = array_append,
   stype = enc_int4[],
   finalfunc = enc_int4_avg_bulk
);

CREATE FUNCTION enc_int4_min(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE min (enc_int4)
(  
   sfunc = enc_int4_min,
   stype = enc_int4,
   PARALLEL = safe, 
   combinefunc = enc_int4_min
);

CREATE FUNCTION enc_int4_max(enc_int4, enc_int4)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE max (enc_int4)
(
   sfunc = enc_int4_max,
   stype = enc_int4,
   PARALLEL = safe, 
   combinefunc = enc_int4_max
);

--------------------------------------------------------------------------------
-- ENCRYPTED STRING TYPE (randomized)
--------------------------------------------------------------------------------

CREATE FUNCTION enc_text_encrypt(cstring)
RETURNS enc_text AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_text_decrypt(enc_text)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_text_in(cstring)
RETURNS enc_text AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_text_out(enc_text)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_text (
    INPUT          = enc_text_in,
    OUTPUT         = enc_text_out,
    INTERNALLENGTH = VARIABLE,
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);
COMMENT ON TYPE enc_text IS 'ENCRYPTED STRING';

CREATE FUNCTION enc_text_eq(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE FUNCTION enc_text_ne(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <> (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION enc_text_lt(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR < (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_text_le(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <= (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_text_gt(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR > (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_text_ge(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR >= (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_text_cmp(enc_text, enc_text)
RETURNS integer AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR CLASS btree_enc_text_ops
DEFAULT FOR TYPE enc_text USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       enc_text_cmp(enc_text, enc_text);

CREATE FUNCTION enc_text_concatenate(enc_text, enc_text)
RETURNS enc_text AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR || (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_concatenate
);

CREATE FUNCTION enc_text_like(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR ~~ (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_like
);

CREATE FUNCTION enc_text_notlike(enc_text, enc_text)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR !~~ (
  LEFTARG = enc_text,
  RIGHTARG = enc_text,
  PROCEDURE = enc_text_notlike
);

CREATE FUNCTION pg_catalog.substring(enc_text, enc_int4, enc_int4)
RETURNS enc_text AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- sets the order in the text field in enc_text
CREATE FUNCTION enc_text_set_order(enc_text, int4)
RETURNS enc_text AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


--------------------------------------------------------------------------------
-- ENCRYPTED FLOAT4 TYPE (randomized)
--------------------------------------------------------------------------------

CREATE FUNCTION enc_float4_encrypt(float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_float4_decrypt(enc_float4)
RETURNS float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_float4_in(cstring)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_float4_out(enc_float4)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_float4 (
    INPUT          = enc_float4_in,
    OUTPUT         = enc_float4_out,
    INTERNALLENGTH = 32,
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);

CREATE FUNCTION enc_float4_eq(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE FUNCTION enc_float4_ne(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <> (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION enc_float4_lt(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR < (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_float4_le(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <= (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_float4_gt(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR > (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_float4_ge(enc_float4, enc_float4)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR >= (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_float4_cmp(enc_float4, enc_float4)
RETURNS integer AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR CLASS btree_enc_float4_ops
DEFAULT FOR TYPE enc_float4 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       enc_float4_cmp(enc_float4, enc_float4);

CREATE FUNCTION enc_float4_add(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR + (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_add
);
CREATE AGGREGATE sum_simple (enc_float4)
(
   sfunc = enc_float4_add,
   stype = enc_float4,
   PARALLEL = SAFE,
   COMBINEFUNC = enc_float4_add 
);

CREATE FUNCTION enc_float4_subs(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR - (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_subs
);

CREATE FUNCTION enc_float4_mult(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR * (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_mult
);

CREATE FUNCTION enc_float4_div(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR / (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_div
);

CREATE FUNCTION enc_float4_mod(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR % (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_mod
);

CREATE FUNCTION enc_float4_exp(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR ^ (
  LEFTARG = enc_float4,
  RIGHTARG = enc_float4,
  PROCEDURE = enc_float4_exp
);

CREATE FUNCTION enc_float4_max(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE max (enc_float4)
(
   sfunc = enc_float4_max,
   stype = enc_float4,
   PARALLEL = SAFE,
   combinefunc = enc_float4_max
);

CREATE FUNCTION enc_float4_min(enc_float4, enc_float4)
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE min (enc_float4)
(
   sfunc = enc_float4_min,
   stype = enc_float4,
   PARALLEL = SAFE,
   combinefunc = enc_float4_min
);

CREATE FUNCTION enc_float4_avg_bulk(enc_float4[])
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE avg (enc_float4)
(
   sfunc = array_append,
   stype = enc_float4[],
   finalfunc = enc_float4_avg_bulk
);

CREATE FUNCTION enc_float4_sum_bulk(enc_float4[])
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE AGGREGATE sum (enc_float4)
(
   sfunc = array_append,
   stype = enc_float4[],
   PARALLEL = SAFE,
   COMBINEFUNC = array_cat,
   finalfunc = enc_float4_sum_bulk  
);

CREATE FUNCTION enc_float4_eval_expr(VARIADIC "any")
RETURNS enc_float4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

--------------------------------------------------------------------------------
-- ENCRYPTED TIMESTAMP TYPE (randomized)
--------------------------------------------------------------------------------

CREATE FUNCTION enc_timestamp_encrypt(cstring)
RETURNS enc_timestamp AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_timestamp_decrypt(enc_timestamp)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_timestamp_in(cstring)
RETURNS enc_timestamp AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION enc_timestamp_out(enc_timestamp)
RETURNS cstring AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE enc_timestamp (
    INPUT          = enc_timestamp_in,
    OUTPUT         = enc_timestamp_out,
    INTERNALLENGTH = 36,
    ALIGNMENT      = int4,
    STORAGE        = PLAIN
);

CREATE FUNCTION enc_timestamp_eq(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  MERGES
);

CREATE FUNCTION enc_timestamp_ne(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <> (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION enc_timestamp_lt(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR < (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_timestamp_le(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <= (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION enc_timestamp_gt(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR > (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_timestamp_ge(enc_timestamp, enc_timestamp)
RETURNS boolean AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR >= (
  LEFTARG = enc_timestamp,
  RIGHTARG = enc_timestamp,
  PROCEDURE = enc_timestamp_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION enc_timestamp_cmp(enc_timestamp, enc_timestamp)
RETURNS integer AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR CLASS btree_enc_timestamp_ops
DEFAULT FOR TYPE enc_timestamp USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       enc_timestamp_cmp(enc_timestamp, enc_timestamp);

CREATE FUNCTION pg_catalog.date_part(text, enc_timestamp)
RETURNS enc_int4 AS 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- sets order info in EncStr.order field
-- if order info not available, order is set to -1, and comparison goes to TEE
-- Usage: SELECT enc_text_add_order($COLUMN_NAME, $TABLE_NAME);
-- Hash Join must be disabled, since no hash function available for type EncText
--    (or maybe we should implement one?
CREATE FUNCTION enc_text_add_order(p_column text, p_table text)
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
    SET %1$s = enc_text_set_order(o_res.%1$s, o_res.order::int4)
	FROM o_res
    WHERE o_res.%1$s = r.%1$s;',
p_column, p_table);
END;
$$
LANGUAGE plpgsql;
