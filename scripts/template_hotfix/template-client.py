#!/usr/bin/env python3

import http.client, json, os, sys, time

HOST = 'localhost'
PORT = 8000

actions = [
    "os_instance_failure",
    "os_hugepage_update [value] between 64 and 1024",
    "os_coredump_space_full",
    "db_space_full",
    "db_query_hung",
    "db_buffer_update [value] between 64 and 2048 MB (requires restart)",
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
