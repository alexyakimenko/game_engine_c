#include <stdio.h>
#include <string.h>

#include "engine/io.h"

int main(void) {
    const char* str = "Hello World!";

    io_file_write("text.txt", str, strlen(str));

    const File file = io_file_read("text.txt");

    if (file.is_valid) {
        puts(file.data);
    }

    return 0;
}
