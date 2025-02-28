#include <stdio.h>

#include "engine/array_list.h"

typedef struct {
    const char* text;
} Test;

int main(void) {
    const Test test1 = {
       .text = "Hello World!"
    };

    const Test test2 = {
        .text = "Another Text!"
    };

    const Test test3 = {
        .text = "Third Text!"
    };

    Array_List* array_list = array_list_create(sizeof(Test), 0);

    array_list_append(array_list, &test1);
    array_list_append(array_list, &test2);
    array_list_append(array_list, &test3);

    array_list_remove(array_list, 0);

    for (usize i = 0; i < array_list->length; i++) {
        const Test* test_from_list = array_list_get(array_list, i);
        printf("%s\n", test_from_list->text);
    }
}