#include "../io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

File io_file_read(const char* path) {
    File file = {.is_valid = false};

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        printf("File not found\n");
        return file;
    }

    fseek(fp, 0, SEEK_END);
    const usize size = ftell(fp);

    rewind(fp);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        printf("Could not allocate memory\n");
        return file;
    }

    fread(buffer, 1, size, fp);
    fclose(fp);

    buffer[size] = '\0';

    file.data = buffer;
    file.length = size;
    file.is_valid = true;

    return file;
}

i8 io_file_write(const char* path, const void* buffer, const usize size) {
    FILE* fp = fopen(path, "wb");

    if (!fp) {
        printf("File not found\n");
        return 1;
    }

    fwrite(buffer, 1, size, fp);
    fclose(fp);

    return 0;
}
