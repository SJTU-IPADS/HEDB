# Template-based Hotfixes

According to the paper's insights, DBAs has a finite allow list of DBA actions.

HEDB provides a hotfix server running inside the integrity zone, and allow DBAs in Management Zone to interact with it.

## How to Build & Run

In Integrity Zone, run `make all`, then `python3 hotfix-server.py &`.

In Management Zone, run `python3 hotfix-client.py`.

## Security Caution

To avoid using `sudo` when executing DBA's hotfixes, the sticky bit (SUID) is used.

## DBA Guidance

To learn more about DBA skills, we recommend you to refer to https://roadmap.sh/postgresql-dba.

For sure, you can add your own templates.

### Other Resources

- https://optimizdba.com/top-database-optimization-tips-from-an-expert-dba/
- https://www.prisma.io/dataguide/managing-databases/database-troubleshooting
- https://www.mssqltips.com/sql-server-tip-category/60/database-administration/
- https://github.com/pgexperts/pgx_scripts/
- https://github.com/oraclesean/oracle
