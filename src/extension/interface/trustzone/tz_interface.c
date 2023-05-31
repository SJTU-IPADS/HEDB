#include <tee_interface.hpp>
#include <tz_interface.h>

#include <defs.h>
#include <pthread.h>
#include <request_types.h>
#include <tee_client_api.h>
#include <tee_client_api_extensions.h>

bool status = false;

TEEC_Session sess;
TEEC_SharedMemory teec_shm;
TEEC_Context ctx;

int init = false;
void* call_ta(void* arg)
{

    TEEC_Result res;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_OPS_UUID;
    uint32_t err_origin;

    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /*
     * Open a session to the "hello world" TA, the TA will print "hello
     * world!" in the log when the session is created.
     */
    res = TEEC_OpenSession(&ctx, &sess, &uuid,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
            res, err_origin);

    /*
     * Execute a function in the TA by invoking it, in this case
     * we're incrementing a number.
     *
     * The value of command ID part and how the parameters are
     * interpreted is part of the interface provided by the TA.
     */

    teec_shm.size = sizeof(EncIntBulkRequestData); // buffer contains one request.
    teec_shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT; // input & output

    res = TEEC_AllocateSharedMemory(&ctx, &teec_shm);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x\n", res);
    }

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    /*
     * Prepare the argument. Pass a value in the first parameter,
     * the remaining three parameters are unused.
     */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    op.params[0].memref.parent = &teec_shm;
    op.params[0].memref.offset = 0;
    op.params[0].memref.size = teec_shm.size;

    BaseRequest* req = (BaseRequest*)teec_shm.buffer;
    req->status = NONE;
    init = true;
    res = TEEC_InvokeCommand(&sess, TA_OPS_CMD_OPS_PROCESS, &op,
        &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
            res, err_origin);

    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);

    return 0;
}

/* --------------------------------------------- */
void* getSharedBuffer(size_t size)
{

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &call_ta, 0); //创建线程
    pthread_detach(thread_id); // 线程分离，结束时自动回收资源
    while (!init)
        ;

    void* buffer = teec_shm.buffer;

    return buffer;
}
void freeBuffer(void* buffer)
{
    ((BaseRequest*)teec_shm.buffer)->status = EXIT; // TODO
    TEEC_ReleaseSharedMemory(&teec_shm);
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    /* stop enclave first TODO */
}
