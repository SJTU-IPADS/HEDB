#pragma once
#include <sgx_error.h>
typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char* msg;
    const char* sug; /* Suggestion */
} sgx_errlist_t;
/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
   {
       SGX_ERROR_INVALID_KEYNAME,
       "The key name is an unsupported value",
       NULL
   },
   {
       SGX_ERROR_INVALID_ISVSVN,
       "The isv svn is greater than the enclave's isv svn",
       NULL
   },
   {
       SGX_ERROR_MAC_MISMATCH,
       "Indicates verification error for reports, sealed datas, MAC checks and etc",
       NULL
   },
   {
       SGX_ERROR_INVALID_ATTRIBUTE,
       "The cpu svn is beyond platform's cpu svn value",
       NULL
   },

    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
    {
        SGX_ERROR_SERVICE_UNAVAILABLE,
        "Indicates aesm didn't respond or the requested service is not supported",
        NULL
    },
};
