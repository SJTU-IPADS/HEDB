#include <defs.h>
#include <limits.h>
#include <plain_timestamp_ops.h>

int plain_timestamp_cmp(TIMESTAMP left, TIMESTAMP right)
{
    return (left == right) ? 0 : (left < right) ? -1
                                                : 1;
}

#define TMODULO(t, q, u)        \
    do {                        \
        (q) = ((t) / (u));      \
        if ((q) != 0)           \
            (t) -= ((q) * (u)); \
    } while (0)

#define INT64CONST(x) (x##L)
#define USECS_PER_DAY INT64CONST(86400000000)
#define POSTGRES_EPOCH_JDATE 2451545

int plain_timestamp_extract_year(int64_t timestamp)
{

    int64_t date;
    unsigned int quad;
    unsigned int extra;
    int year;

    TMODULO(timestamp, date, USECS_PER_DAY);
    if (timestamp < INT64CONST(0)) {
        timestamp += USECS_PER_DAY;
        date -= 1;
    }

    /* add offset to go from J2000 back to standard Julian date */
    date += POSTGRES_EPOCH_JDATE;

    /* Julian day routine does not work for negative Julian days */
    if (date < 0 || date > (int64_t)INT_MAX)
        return -1;

    date += 32044;
    quad = date / 146097;
    extra = (date - quad * 146097) * 4 + 3;

    date += 60 + quad * 3 + extra / 146097;
    quad = date / 1461;
    date -= quad * 1461;

    year = date * 4 / 1461;
    year += quad * 4;
    return year - 4800;
}
