#include "../io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

File io_file_read(const char* path) {
    File file = { .is_valid = false };

    FILE* rfile = fopen(path, "r");

    if (!rfile) {
        printf("Error opening file!\n");
        return file;
    }

    usize size = 0;
    usize capacity = 1 << 11; // 2048

    char* buffer = malloc(capacity);

    if (!buffer) {
        printf("Error allocating memory!\n");
        fclose(rfile);
        return file;
    }

    while (fgets(buffer + size, (int)capacity - (int)size, rfile)) {
        size += strlen(buffer + size);

        if (size + 1 >= capacity) {
            capacity *= 2;
            char* temp = realloc(buffer, capacity);

            if (!temp) {
                printf("Error allocating memory!\n");
                free(buffer);
                fclose(rfile);
                return file;
            }

            buffer = temp;
        }
    }

    fclose(rfile);

    file.data = buffer;
    file.length = size;
    file.is_valid = true;

    return file;
}
i32 io_file_write(void* buffer, usize size, const char* path);
