#!/usr/bin/env python3

import os, json
from http.server import BaseHTTPRequestHandler, HTTPServer

HOST = '0.0.0.0'
PORT = 8000

cwd = os.getcwd()

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

def Maintenance_Template(action, value):
    if action == actions[1]:
        return os.popen(cwd + "/" + "service_restart").read()
    
    elif action == actions[2]:
        if 64 <= int(value) and int(value) <= 1024:
            return os.popen(cwd + "/" + "hugepage_update " + value).read()
        else:
            return "os_hugepage_update [value] invalid: " + value
    
    elif action == actions[3]:
        return os.popen("rm /var/crash/*.crash").read()
    
    elif action == actions[4]:
        # no check: let DBMS do the check
        return os.popen("psql -c 'VACUUM FULL " + value + ";'").read()

    elif action == actions[5]:
        # no check: let DBMS do the check
        return os.popen("psql -c 'REINDEX INDEX " + value + ";'").read()

    elif action == actions[6]:
        # no check: let DBMS do the check
        return os.popen("psql -c 'SELECT pg_cancel_backend(" + value + ");'").read()

    elif action == actions[7]:
        # no check: let DBMS do the check
        return os.popen("psql -c 'SELECT pg_terminate_backend(" + value + ");'").read()

    elif action == actions[8]:
        if 64 <= int(value) and int(value) <= 2048:
            return os.popen("psql -c 'ALTER SYSTEM SET shared_buffers TO \"" + value + "MB\";'").read()
        else:
            return "db_shared_buffers_update [value] invalid: " + value

    elif action == actions[9]:
        if 1 <= int(value) and int(value) <= 16:
            return os.popen("psql -c 'ALTER SYSTEM SET work_mem TO \"" + value + "MB\";'").read()
        else:
            return "db_work_mem_update [value] invalid: " + value

    elif action == actions[10]:
        if 16 <= int(value) and int(value) <= 128:
            return os.popen("psql -c 'ALTER SYSTEM SET maintenance_work_mem TO \"" + value + "MB\";'").read()
        else:
            return "db_maintenance_work_mem_update [value] invalid: " + value

    elif action == actions[11]:
        if 10 <= int(value) and int(value) <= 500:
            return os.popen("psql -c 'ALTER SYSTEM SET max_connections TO " + value + ";'").read()
        else:
            return "db_max_connections_update [value] invalid: " + value

    elif action == actions[12]:
        if 1 <= int(value) and int(value) <= 10800000:
            return os.popen("psql -c 'ALTER SYSTEM SET statement_timeout TO " + value + ";'").read()
        else:
            return "db_statement_timeout_update [value] invalid: " + value

    else:
        print("Unsupported actions...")


class Template_Handler(BaseHTTPRequestHandler):

    def respond_status(self, opts):
        status_code = opts['status']
        self.send_response(status_code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        paths = {
            '/dba': {'status': 200},
        }
        if self.path in paths:
            self.respond_status(paths[self.path])
        else:
            self.respond_status({'status': 404})

        content_length = int(self.headers['Content-Length'])
        content = self.rfile.read(content_length)
        print(content.decode("UTF-8"))
        
        body = json.loads(content)
        result = Maintenance_Template(body["action"], body["value"])
        self.wfile.write(result.encode())
        

if __name__== '__main__':
    with HTTPServer((HOST, PORT), Template_Handler) as server:
        server.serve_forever()
