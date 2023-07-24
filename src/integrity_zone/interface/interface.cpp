#include <ctype.h>
#include <extension.hpp>
#include <interface.hpp>
#include <pthread.h>
#include <recorder.hpp>
#include <replayer.hpp>
#include <stdio.h>
#include <stdlib.h> // at exit
#include <sync.h>
#include <tee_interface.hpp>
#include <timer.hpp>
#include <unistd.h>
#include "barrier.h"

TEEInvoker *TEEInvoker::invoker = nullptr;

bool recordMode = false;
bool replayMode = false;
bool updateRecordFile = false;
bool updateReplayFile = false;
bool sequence_replay = true;
bool performance_replay = false;
int records_cnt = 0;
char record_name_prefix[MAX_NAME_LENGTH];
char record_names[MAX_RECORDS_NUM][MAX_NAME_LENGTH];

uint64_t current_log_size = 0;

void exit_handler()
{
    TEEInvoker *invoker = TEEInvoker::getInstance();
    delete invoker;
}

#define SHM_SIZE (16 * 1024 * 1024) // TODO: merge this into one header

TEEInvoker::TEEInvoker()
{
    if (!replayMode)
    {
        req_buffer = getSharedBuffer(sizeof(EncIntBulkRequestData));
    }
    else
    {
        req_buffer = calloc(1, SHM_SIZE);
    }
    BaseRequest *req_control = static_cast<BaseRequest *>(req_buffer);
    req_control->status = NONE;
    atexit(exit_handler);
}

TEEInvoker::~TEEInvoker()
{
    if (!replayMode)
    {
        freeBuffer(req_buffer);
    }
    else
    {
        free(req_buffer);
    }
}
#define RETRY_FAILED 10086

int TEEInvoker::sendRequest(Request *req)
{
    int resp;

    /* 1. serialize request data structure to shared memory buffer */
    req->serializeTo(req_buffer);

    /*
    Treat buffer as BaseRequest structure, which locates in first several bytes of requests structure, e.g.:
    xxxRequest {
        BaseRequest : reqType, status, resp
        xxxRequest-specific parameters...
    }
    */
    BaseRequest *req_control = static_cast<BaseRequest *>(req_buffer);

    if (replayMode)
    {
        /* replay_request will answer request written to req_buffer */
        std::vector<std::string> filenames;
        for (int i = 0; i < records_cnt; i++)
        {
            filenames.push_back(record_names[i]);
        }
        static Replayer &replayer = Replayer::getInstance(filenames);
        if (updateReplayFile)
        {
            replayer.update_replay_files(filenames);
            updateReplayFile = false;
        }
        int resp = replayer.replay(req_buffer);
        if (resp != Replayer::NOT_REPLAY)
        {
            /* then copy result from req_buffer to destination buffer */
            req->copyResultFrom(req_buffer);
            return resp;
        }
    }

    STORE_BARRIER;
    req_control->status = SENT;
    /* wait for status */
    while (req_control->status != DONE)
        YIELD_PROCESSOR;
    LOAD_BARRIER;
    /* memcpy results in req_buffer to result_buffer
        result buffer is determined when request is constructed.
    */
    req->copyResultFrom(req_buffer);
    resp = req_control->resp;

    /* record */
    if (recordMode)
    {
        static Recorder &recorder = Recorder::getInstance(record_name_prefix);
        if (updateRecordFile)
        {
            recorder.update_write_fd(record_name_prefix);
            updateRecordFile = false;
        }
        recorder.record(req_buffer);
    }

    /* read-write barrier, no read move after this barrier, no write move before this barrier */

    req_control->status = NONE;

    return resp;
}

/* currently useless */
void enter_replay_mode()
{
    if (recordMode)
    {
        recordMode = false;
        // print_info("record file could be not fully written\n");
        /* should add flush */
    }
    replayMode = true;
}

