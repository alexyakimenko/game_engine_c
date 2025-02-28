#pragma once

#include "types.h"

typedef struct array_list {
    usize length;
    usize capacity;
    usize item_size;
    void* items;
} Array_List;

Array_List* array_list_create(usize item_size, usize initial_capacity);
usize array_list_append(Array_List* array_list, void* item);
void* array_list_get(Array_List* array_list, usize index);
u8 array_list_remove(Array_List* array_list, usize index);