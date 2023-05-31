#include <dirent.h>
#include <extension.hpp>
#include <stdafx.hpp>
#include <stdarg.h>
#include <sys/types.h>

extern "C" {
PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(launch);
PG_FUNCTION_INFO_V1(enable_debug_mode);
PG_FUNCTION_INFO_V1(enable_record_mode);
PG_FUNCTION_INFO_V1(enable_replay_mode);
// void close_write_file_ptr();
}

#define MAX_NAME_LENGTH 100
#define MAX_PARALLEL_WORKER_SIZE 16
#define MAX_RECORDS_NUM (MAX_PARALLEL_WORKER_SIZE + 1)

bool debugMode = true;
extern bool recordMode;
extern bool replayMode;
extern bool sequence_replay;
extern char record_name_prefix[MAX_NAME_LENGTH];
extern char record_names[MAX_RECORDS_NUM][MAX_NAME_LENGTH];
extern int records_cnt;

// void print_info(const char *str,...)
// {
//     ereport(INFO, (errmsg(str)));
// }
// void print_error(const char *fmt,...)
// {
//     ereport(ERROR, (errmsg(fmt)));
// }

Datum enable_debug_mode(PG_FUNCTION_ARGS)
{
    debugMode = true;
    PG_RETURN_INT32(0);
}
Datum enable_record_mode(PG_FUNCTION_ARGS)
{
    recordMode = true;
    char* s = PG_GETARG_CSTRING(0);
    strncpy(record_name_prefix, s, strlen(s));
    // print_info("%s\n", s);
    PG_RETURN_INT32(0);
}
Datum enable_replay_mode(PG_FUNCTION_ARGS)
{
    if (recordMode) {
        recordMode = false;
        // close_write_file_ptr();
    }
    replayMode = true;
    char* s = PG_GETARG_CSTRING(0);
    strncpy(record_name_prefix, s, strlen(s));
    strcat(record_name_prefix, "-");
    // print_info("%s\n", s);

    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir("/usr/local/pgsql/data")) != NULL) {
        // print_info("open directory success\n");
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (0 == strncmp(record_name_prefix, ent->d_name, strlen(record_name_prefix))) { // if prefix match add to names list
                strncpy(record_names[records_cnt], ent->d_name, strlen(ent->d_name));
                records_cnt++;
            }
        }
        char tmp[256];
        sprintf(tmp, "find %d log files\n", records_cnt);
        for (int i = 0; i < records_cnt; i++) {
            sprintf(tmp + strlen(tmp), "%d: %s\n", i, record_names[i]);
        }
        // print_info("%s\n",tmp);
        closedir(dir);
    } else {
        /* could not open directory */
        // print_info("could not open directory\n");
        return EXIT_FAILURE;
    }

    char* mode = PG_GETARG_CSTRING(1);
    if (strcmp(mode, "random") == 0) {
        sequence_replay = false;
    }

    // print_info("mode: %s, seq: %d", mode, (int)sequence_replay);

    PG_RETURN_INT32(0);
}
Datum launch(PG_FUNCTION_ARGS)
{
    ereport(LOG, (errmsg("launch was called")));
    // recordMode = true;

    // char* s = "tpcc";
    // strncpy(record_name_prefix, s, strlen(s));
    int64_t ret = 0;

    PG_RETURN_INT32(ret);
}
