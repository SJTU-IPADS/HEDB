# Template Hotfixes

According to the insights, we find that DBAs has a finite whitelist of DBA actions.

HEDB provides a template server running inside the integrity zone, and allow DBAs in the normal zone to interact with it.

## How to Build & Run

In the integrity zone, run `make all`, then `python3 template-server.py &`.

In the normal zone, run `python3 template-client.py`.

## Security Caution

To avoid using sudo when executing DBA's fixes, we use the sticky bit, i.e., SUID.

## DBA Guidance

To learn more about DBA skills, you can refer to `https://roadmap.sh/postgresql-dba`.

For sure, you can add your own templates.
