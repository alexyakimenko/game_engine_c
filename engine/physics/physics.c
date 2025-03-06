#include "../physics.h"

#include <linmath.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../global.h"
#include "../array_list.h"
#include "physics_internal.h"

static Physics_State_Internal state;

void physics_init(void) {
    state.body_list = array_list_create(sizeof(Body), 0);
}

void physics_update(void) {
    for (usize i = 0; i < state.body_list->length; i++) {
        Body *body = array_list_get(state.body_list, i);
        body->velocity[0] += body->acceleration[0] * global.time.delta;
        body->velocity[1] += body->acceleration[1] * global.time.delta;
        body->aabb.position[0] += body->velocity[0] * global.time.delta;
        body->aabb.position[1] += body->velocity[1] * global.time.delta;
    }
}

usize physics_body_create(vec2 position, const vec2 size) {
    const Body body = {
        .aabb = {
            .position = {position[0], position[1]},
            .half_size = {size[0] * 0.5f, size[1] * 0.5f},
        },
        .velocity = {0, 0},
    };

    if (array_list_append(state.body_list, &body) == -1) {
        printf("Failed to add body\n");
        exit(EXIT_FAILURE);
    }

    return state.body_list->length - 1;
}
Body* physics_body_get(const usize index) {
    return array_list_get(state.body_list, index);
}
