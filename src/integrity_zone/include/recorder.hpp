#pragma once

#include <fcntl.h> // open
#include <string>
#include <sys/mman.h> // mmap
#include <unistd.h> // pid

class Recorder {
    int write_fd;
    std::string filename;
    /* variables to operate write_fd */
    const int DATA_LENGTH = 16 * 1024 * 1024UL;
    unsigned long file_length;
    unsigned long file_cursor;
    char* write_addr;
    /* return mmaped write buffer from write_fd,
    extened the file if file size is not large enough */
    char* get_write_buffer(unsigned long length);

    Recorder(std::string filename_prefix)
        : file_length(0)
        , file_cursor(0)
        , write_addr(nullptr)
        , write_fd(0)
    {
        update_write_fd(filename_prefix);
    }

    ~Recorder()
    {
        close(write_fd);
    }

public:
    static Recorder& getInstance(char* filename_prefix)
    {
        static Recorder recorder(filename_prefix);
        return recorder;
    }
    Recorder(const Recorder& recorder) = delete;
    Recorder& operator=(const Recorder& recorder) = delete;

    void record(void* request_buffer);
    void update_write_fd(std::string prefix);
};

/* use different Recorder subclass to represent different rr strategy*/
