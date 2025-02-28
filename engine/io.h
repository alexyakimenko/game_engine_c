#pragma once
#include "types.h"

typedef struct file {
    usize length;
    bool is_valid;
    char* data;
} File;

File io_file_read(const char* path);
i32 io_file_write(void* buffer, usize size, const char* path);
