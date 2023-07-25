#!/usr/bin/env python3

import os, json
from http.server import BaseHTTPRequestHandler, HTTPServer

HOST = '0.0.0.0'
PORT = 8000

cwd = os.getcwd()

actions = [
    "os_instance_failure",
    "os_hugepage_update [value] between 64 and 1024",
    "os_coredump_space_full",
    "db_space_full",
    "db_query_hung",
    "db_buffer_update [value] between 64 and 2048 MB (requires restart)",
]

def Maintenance_Template(action, value):
    if action == actions[0]:
        return os.popen(cwd + "/" + "service_restart").read()
    
    elif action == actions[1]:
        if 64 <= int(value) and int(value) <= 1024:
            return os.popen(cwd + "/" + "hugepage_update " + value).read()
        else:
            return "os_hugepage_update [value] invalid: " + value
    
    elif action == actions[2]:
        return os.popen("rm /var/crash/*.crash").read()
    
    elif action == actions[3]:
        return os.popen("psql -c 'VACUUM FULL;'").read()
    
    elif action == actions[4]:
        # no check: let the dbms do the check
        return os.popen("psql -c 'SELECT pg_cancel_backend(" + value + ");'").read()
    
    elif action == actions[5]:
        if 64 <= int(value) and int(value) <= 2048:
            return os.popen("psql -c 'ALTER SYSTEM SET shared_buffers TO \"" + value + "MB\";'").read()
        else:
            return "db_buffer_update [value] invalid: " + value
    
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

