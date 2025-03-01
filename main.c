#include <stdio.h>

#include "engine/io.h"

int main(void) {
    const File file = io_file_read("text.txt");

    if (file.is_valid) {
        puts(file.data);
    }

    return 0;
}
