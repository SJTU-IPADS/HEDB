#!/usr/bin/env python3

import http.client, json, os, sys, time

HOST = 'localhost'
PORT = 8000

actions = [
    "none"
    "os_service_restart",
    "os_hugepage_update [value] between 64 and 1024",
    "os_coredump_remove",

    "db_space_reclaim [table]",
    "db_table_reindex [table]",

    "db_query_cancel [process]",
    "db_query_kill [process]",

    "db_shared_buffer_update [value] between 64 and 2048 MB (requires restart)",
    "db_work_mem_update [value] between 1 and 16 MB (requires restart)",
    "db_maintenance_work_mem_update [value] between 16 and 128 MB (requires restart)",

    "db_max_connections_update [value] between 10 and 500 (requires restart)",
    "db_statement_timeout_update [value] between 1 and 10800000 (requires restart)",
]

if __name__== '__main__':
    while 1:
        print("Supported Actions:")
        i = 1
        for action in actions:
            print("%d. %s" % (i, action))
            i = i + 1

        action = input("Type action number (e.g., 1): ")
        value = input("Type action value (e.g., 10, type any if no need): ")

        BODY = json.dumps( { "action": actions[int(action)-1], "value": value, } )
        HEAD = {'Content-type': 'application/json'}

        conn = http.client.HTTPConnection(HOST, PORT)
        conn.request("GET", "/dba", BODY, HEAD)

        response = conn.getresponse()
        print(response.status, response.reason)
        print(response.read().decode())
        print("\n")
        time.sleep(1)
