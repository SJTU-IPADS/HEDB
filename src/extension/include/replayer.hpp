#pragma once 
#include <string>
#include <vector>
#include <unistd.h> // fopen

class Replayer{
    FILE *file;
    std::vector<std::string> filenames;
    int previous_op;
    int replay_request(void *req_buffer);
    Replayer(const std::vector<std::string>& fileList): file(nullptr), filenames(fileList) {}
    ~Replayer(){}
public:
    static Replayer& getInstance(const std::vector<std::string>& fileList){
        static Replayer replayer(fileList);
        return replayer;
    }
    Replayer(const Replayer& replayer) = delete;
    Replayer & operator=(const Replayer& replayer) = delete;

    static const int RETRY_FAILED = -10086;
    /* this request type is not for replay */
    static const int NOT_REPLAY = -10087;


    /* process request on the request buffer, base on repay file 
    return value: status */
    int replay(void *request_buffer);
};


                                              

