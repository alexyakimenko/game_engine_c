#pragma once
#include "types.h"

typedef struct file {
    usize length;
    bool is_valid;
    char* data;
} File;

File io_file_read(const char* path);
i8 io_file_write(const char* path, const void* buffer, usize size);
