#pragma once

extern "C"
{
#include <defs.h>
#include <enc_types.h>
#include <request_types.h>
}


#define IS_ENCSTR(a) (sizeof(*a) == sizeof(EncStr))
#define ENCSTR_LEN(a) (((EncStr*)a)->len)
#define ENCSTR_DATA(a) (&((EncStr*)a)->enc_cstr)
#define COPY_ENC(to,from) { \
    ENCSTR_LEN(to) = ENCSTR_LEN(from);                        \
    memcpy(ENCSTR_DATA(to),ENCSTR_DATA(from), ENCSTR_LEN(to)); }

#define IS_STR(a) (sizeof(*a) == sizeof(Str))
#define STR_LEN(a) (((Str*)a)->len)
#define STR_DATA(a) (((Str*)a)->data)
#define COPY_PLAIN(to,from) { \
    STR_LEN(to) = STR_LEN(from);                        \
    memcpy(STR_DATA(to),STR_DATA(from), STR_LEN(to)); }

#define COPY(to, from) { \
    if      (IS_STR(from)) { COPY_PLAIN(to,from); } \
    else if (IS_ENCSTR(from)) {COPY_ENC(to, from);} \
    else    *to = *from; }

#define TYPESIZE(a,size) { \
    if      (IS_STR(a)) { size = sizeof(STR_LEN(a)) + STR_LEN(a); } \
    else if (IS_ENCSTR(a)) { size = sizeof(ENCSTR_LEN(a)) + ENCSTR_LEN(a); } \
    else    {size = sizeof(*a);} }

class Request {
public:
    virtual void serializeTo(void *buffer) const = 0;

    virtual inline void copyResultFrom(void *buffer) const = 0;

    // virtual void genLog(void *buffer) const = 0;
    virtual inline int size()  const { return 0; };

private:
};

/* DataType should be one of CMP DATA */

template <typename EncType, int reqType>
class CmpRequest : public Request {
public:
    DEFINE_ENCTYPE_CMP_ReqData(EncType);
    CmpRequest(EncType *left, EncType *right, int *cmp) : left(left), right(right), cmp(cmp) {}

    EncType *left;
    EncType *right;
    int *cmp;

    void serializeTo(void *buffer) const override  {
        auto *req = (EncTypeCmpRequestData *) buffer;
        req->common.reqType = reqType;
        COPY(&req->left, left);
        COPY(&req->right, right);
        // req->left = *left;
        // req->right = *right;
    }
    
    inline void copyResultFrom(void *buffer)const override  {
        auto *req = (EncTypeCmpRequestData *) buffer;
        *cmp = req->cmp;
    }

    inline int size() const override{
        int size;
        TYPESIZE(left,size);
        return sizeof(BaseRequest) + 2 * size + sizeof(int);
    };
    // void genLog(void *buffer) const override{
    
    // }
};


template <typename EncType>
class CalcRequest : public Request {
public:
    DEFINE_ENCTYPE_CALC_ReqData(EncType);
    int op;
    EncType *left;
    EncType *right;
    EncType *res;

    CalcRequest(int op, EncType *left, EncType *right, EncType *res) : op(op), left(left), right(right), res(res) {}

    void serializeTo(void *buffer) const override {
        auto *req = (EncTypeCalcRequestData *) buffer;
        req->common.reqType = op;
        req->op = op;
        COPY(&req->left, left);
        COPY(&req->right, right);
    }

    inline void copyResultFrom(void *buffer)const override  {
        auto *req = (EncTypeCalcRequestData *) buffer;
        COPY(res, &req->res);
        // *res = req->res;
    }

    inline int size() const override {
        int size;
        TYPESIZE(left,size);
        return sizeof(BaseRequest) + sizeof(op) + 3 * size;
    };
};
// template<>  // specialization
// void CalcRequest<EncStr>::copyResultFrom(void *buffer) const override1{
//     auto *req = (EncTypeCalcRequestData *) buffer;
//     // EncStr *estr = (EncStr *) res;
//     *res = req->res;
// }



template<typename EncType>
class BulkRequest : public Request {
public:
    DEFINE_ENCTYPE_BULK_ReqData(EncType)
    int bulk_type;
    int bulk_size;
    EncType *items; //begin of items
    EncType *res;   // Maybe EncFloat in average, use union that-wise

    BulkRequest(int bulkType, int bulkSize, EncType *items, EncType *res) : bulk_type(bulkType),
                                                                                bulk_size(bulkSize), items(items),
                                                                                res(res) {}

    void serializeTo(void *buffer) const override {
        auto *req = (EncTypeBulkRequestData *)buffer;
        req->common.reqType = bulk_type;
        req->bulk_size = bulk_size;
        memcpy(req->items, items, sizeof(EncType) * bulk_size);
    }

    inline void copyResultFrom(void *buffer)const override  {
        auto *req = (EncTypeBulkRequestData *)buffer;
        *res = req->res;
    }
    inline int size() const override {
        return sizeof(BaseRequest) + sizeof(int) * 2 + (bulk_size + 1) * sizeof(EncType);
    };
};

template<typename EncType>
class EvalExprRequest : public Request {
public:
    DEFINE_ENCTYPE_EVALEXPR_ReqData(EncType)
    int eval_type;
    int arg_cnt;
    Str expr;
    EncType **items;
    EncType *res;

    EvalExprRequest(int eval_type, int argCnt, Str expr, EncType **items, EncType *res) : eval_type(eval_type), arg_cnt(argCnt),
                                                                            expr(expr), items(items),
                                                                            res(res) {}

    void serializeTo(void *buffer) const override {
        auto *req = (EncTypeEvalExprRequestData *)buffer;
        req->common.reqType = eval_type;
        req->arg_cnt = arg_cnt;
        // memcpy(req->items, items, sizeof(EncType) * arg_cnt);
        for (int i = 0; i < arg_cnt; ++i) {
            memcpy(&(req->items[i]), items[i], sizeof(EncType));
        }
        memcpy(&(req->expr), &expr, sizeof(expr));
    }

    inline void copyResultFrom(void *buffer)const override  {
        auto *req = (EncTypeEvalExprRequestData *)buffer;
        *res = req->res;
    }
};

/* TODO: define = operator for encstr type, and other types
         change encstr type to {size, char[]}
*/
template<typename PlainType, typename EncType, int reqType>
class EncRequest : public Request {
public:
    DEFINE_ENCTYPE_ENC_ReqData(EncType,PlainType);

    PlainType *plaintext;
    EncType *res;

    EncRequest(PlainType *plaintext, EncType *res) : plaintext(plaintext), res(res) {}

    void serializeTo(void *buffer) const override {
        auto *req = (EncTypeEncRequestData *) buffer;
        req->common.reqType = reqType;
        // req->plaintext = *plaintext;
        COPY(&req->plaintext, plaintext);

        // if(reqType == CMD_STRING_ENC){
        //     char ch[100];
        //     sprintf(ch, "after serialize %d", ((Str *) plaintext)->len);
        //     print_info(ch);
        // }
    }

    inline void copyResultFrom(void *buffer)const override {
        auto *req = (EncTypeEncRequestData *) buffer;
        COPY(res, &req->ciphertext);

    }
    inline int size() const override {
        int size1,size2;
        TYPESIZE(plaintext,size1);
        TYPESIZE(res,size2);
        return sizeof(BaseRequest) + size1 + size2;
    };
};

template<typename EncType,typename PlainType,int reqType>
class DecRequest : public Request {
public:
    DEFINE_ENCTYPE_DEC_ReqData(EncType, PlainType);
    EncType *ciphertext;
    PlainType *res;
    
    DecRequest(EncType *ciphertext, PlainType *res) : ciphertext(ciphertext), res(res) {}

    void serializeTo(void *buffer) const override{
        auto *req = (EncTypeDecRequestData *) buffer;
        req->common.reqType = reqType;

        COPY(&req->ciphertext, ciphertext);
        // req->ciphertext = *ciphertext;
    }

    inline void copyResultFrom(void *buffer)const override {
        auto *req = (EncTypeDecRequestData *) buffer;

        COPY(res, &req->plaintext);
        // *res = req->plaintext;
    }
    inline int size() const override {
        int size1,size2;
        TYPESIZE(ciphertext,size1);
        TYPESIZE(res,size2);
        return sizeof(BaseRequest) + size1 + size2;
    };
};

template<typename Type1, typename Type2,int reqType>
class OneArgRequest: public Request {
public:
    DEFINE_ENCTYPE_1ARG_ReqData(Type1, Type2,OneArgRequestData);
    Type1 *in;
    Type2 *res;
    
    OneArgRequest(Type1 *in, Type2 *res) : in(in), res(res) {}

    void serializeTo(void *buffer) const override{
        auto *req = (OneArgRequestData *) buffer;
        req->common.reqType = reqType;
        COPY(&req->in, in);
    }

    inline void copyResultFrom(void *buffer)const override {
        auto *req = (OneArgRequestData *) buffer;
        *res = req->res;
    }
    inline int size() const override {
        int size;
        TYPESIZE(in, size);
        return sizeof(BaseRequest) + size + sizeof(Type2);
    };
};


template<typename Type1, 
         typename Type2,
         typename Type3,
         typename resType,
         int reqType>
class ThreeArgRequest: public Request {
public:
    DEFINE_ENCTYPE_3ARG_ReqData(Type1,arg1,Type2,arg2, Type3,arg3, resType, ThreeArgRequestData);
    Type1 *arg1;
    Type2 *arg2;
    Type3 *arg3;
    resType *res;
    
    ThreeArgRequest(Type1 *arg1, Type2 *arg2,Type3 *arg3, resType *res) : arg1(arg1),arg2(arg2),arg3(arg3), res(res) {}

    void serializeTo(void *buffer) const override{
        auto *req = (ThreeArgRequestData *) buffer;
        req->common.reqType = reqType;
        COPY(&req->arg1, arg1);
        // req->arg1 = *arg1;
        req->arg2 = *arg2;
        req->arg3 = *arg3;
    }

    inline void copyResultFrom(void *buffer)const override {
        auto *req = (ThreeArgRequestData *) buffer;

        COPY(res, &req->res);
        // *res = req->res;
    }
    inline int size() const override {
        int size;
        TYPESIZE(arg1,size);
        return sizeof(BaseRequest) + size + sizeof(Type2) + sizeof(Type3) + sizeof(res);  
    };
};
