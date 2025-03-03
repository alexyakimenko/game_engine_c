#include "../array_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Array_List* array_list_create(const usize item_size, const usize initial_capacity) {
    Array_List* list = malloc(sizeof(Array_List));

    if (!list) {
        printf("Memory allocation error\n");
        return nullptr;
    }

    list->item_size = item_size;
    list->capacity = initial_capacity;
    list->length = 0;

    list->items = malloc(item_size * initial_capacity);

    if (!list->items) {
        printf("Memory allocation error\n");
        return nullptr;
    }

    return list;
}

usize array_list_append(Array_List* list, const void* item) {
    if (list->length >= list->capacity) {
        list->capacity = list->capacity ? list->capacity * 2 : 1;

        void* items = realloc(list->items, list->capacity * list->item_size);

        if (!items) {
            printf("Memory reallocation error\n");
            return -1;
        }

        list->items = items;
    }

    const usize index = list->length++;

    memcpy(list->items + index * list->item_size, item, list->item_size);

    return index;
}

void* array_list_get(const Array_List* array_list, const usize index) {
    if (index >= array_list->length) {
        printf("Index out of bounds\n");
        return nullptr;
    }

    return array_list->items + index * array_list->item_size;
}

u8 array_list_remove(Array_List* list, const usize index) {
    if (list->length == 0) {
        printf("Array list is empty\n");
    }

    if (index >= list->length) {
        printf("Index out of bounds\n");
        return 1;
    }

    if (list->length == 1) {
        list->length = 0;
        return 0;
    }

    list->length--;

    u8* item_ptr = list->items + index * list->item_size;
    const u8* end_ptr = list->items + list->length * list->item_size;

    memcpy(item_ptr, end_ptr, list->item_size); // does not save order

    // in case I'll want to save order after removal
    // memcpy(item_ptr, item_ptr + list->item_size, list->item_size * list->length);

    return 0;
}
