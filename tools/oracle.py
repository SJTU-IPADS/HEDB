#!/usr/bin/env python
# -*- coding: utf-8 -*-

# $ python3 -m pip install psycopg2

import psycopg2
import sys

con = psycopg2.connect(database='secure_test', user='postgres', password='postgres', host='127.0.0.1', port='5432')

def main():
    with con:
        cur = con.cursor()
        cur.execute("SELECT enc_int4_decrypt('%s');" % sys.argv[1])
        print("%s" % cur.fetchall()[0])

if __name__ == '__main__':
    main()