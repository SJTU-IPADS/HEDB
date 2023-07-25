# Template Hotfixes

According to the insights, we find that DBAs has a finite whitelist of DBA actions.

HEDB provides a hotfix server running inside the integrity zone, and allow DBAs in the normal zone to interact with it.

## How to Build & Run

In the integrity zone, run `make all`, then `python3 hotfix-server.py &`.

In the normal zone, run `python3 hotfix-client.py`.

## Security Caution

To avoid using sudo when executing DBA's fixes, we use the sticky bit, i.e., SUID.

## DBA Guidance

To learn more about DBA skills, you can refer to `https://roadmap.sh/postgresql-dba`.

For sure, you can add your own templates.

### Other Resources

- https://optimizdba.com/top-database-optimization-tips-from-an-expert-dba/
- https://www.prisma.io/dataguide/managing-databases/database-troubleshooting
- https://www.mssqltips.com/sql-server-tip-category/60/database-administration/
- https://github.com/pgexperts/pgx_scripts/
- https://github.com/oraclesean/oracle
