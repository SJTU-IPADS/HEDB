#pragma once 

#include <request.hpp>
#include <request_types.h>

class TEEInvoker {
private:
    TEEInvoker();
    static TEEInvoker* invoker;
    void *req_buffer;
public: 
    ~TEEInvoker();
    static inline TEEInvoker *getInstance(){
        if (invoker == nullptr){
            invoker = new TEEInvoker;
        }
        return invoker;
    }
    /* send request, get result in req->res, get resp as return value.*/
    int sendRequest(Request *req);

};


