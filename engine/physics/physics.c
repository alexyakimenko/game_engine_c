#include "../physics.h"

#include <linmath.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../global.h"
#include "../array_list.h"
#include "physics_internal.h"

static Physics_State_Internal state;

static u8 iterations = 4;
static f32 tick_rate;

void aabb_min_max(vec2 min, vec2 max, const AABB aabb) {
    vec2_sub(min, aabb.position, aabb.half_size);
    vec2_add(max, aabb.position, aabb.half_size);
}

bool physics_point_intersects_aabb(const vec2 point, const AABB aabb) {
    vec2 min, max;
    aabb_min_max(min, max, aabb);
    return point[0] >= min[0] &&
        point[0] <= max[0] &&
        point[1] >= min[1] &&
        point[1] <= max[1];
}

bool physics_aabb_intersect_aabb(const AABB a, const AABB b) {
    vec2 min, max;
    aabb_min_max(min, max, aabb_minkowski_difference(a, b));

    return min[0] <= 0 && max[0] >= 0 && min[1] <= 0 && max[1] >= 0;
}

AABB aabb_minkowski_difference(const AABB a, const AABB b) {
    AABB result;
    vec2_sub(result.position, a.position, b.position);
    vec2_add(result.half_size, a.half_size, b.half_size);

    return result;
}

void aabb_penetration_vector(vec2 r, const AABB aabb) {
    vec2 min, max;
    aabb_min_max(min, max, aabb);

    f32 min_dist = fabsf(min[0]);
    r[0] = min[0];
    r[1] = 0;

    if (fabsf(max[0]) < min_dist) {
        r[0] = max[0];
        min_dist = fabsf(max[0]);
    }

    if (fabsf(min[1]) < min_dist) {
        r[0] = 0;
        r[1] = min[1];
        min_dist = fabsf(min[1]);
    }

    if (fabsf(max[1]) < min_dist) {
        r[0] = 0;
        r[1] = max[1];
    }
}

Hit ray_intersect_aabb(const vec2 position, const vec2 magnitude, const AABB aabb) {
    Hit hit = {0};
    vec2 min, max;
    aabb_min_max(min, max, aabb);

    f32 last_entry = -INFINITY;
    f32 first_exit =  INFINITY;

    for (u8 i = 0; i < 2; i++) {
        if (magnitude[i] != 0) {
            const f32 t1 = (min[i] - position[i]) / magnitude[i];
            const f32 t2 = (max[i] - position[i]) / magnitude[i];

            last_entry = fmaxf(last_entry, fminf(t1, t2));
            first_exit = fminf(first_exit, fmaxf(t1, t2));
        } else if (position[i] <= min[i] || position[i] >= max[i]) {
            return hit;
        }
    }

    if (first_exit > last_entry && first_exit > 0 && last_entry < 1) {
        hit.position[0] = position[0] + magnitude[0] * last_entry;
        hit.position[1] = position[1] + magnitude[1] * last_entry;

        hit.is_hit = true;
        hit.time = last_entry;

        const f32 dx = hit.position[0] - aabb.position[0];
        const f32 dy = hit.position[1] - aabb.position[1];
        const f32 px = aabb.half_size[0] - fabsf(dx);
        const f32 py = aabb.half_size[1] - fabsf(dy);

        if (px < py) {
            hit.normal[0] = (f32)(dx > 0) - (f32)(dx < 0);
        } else {
            hit.normal[1] = (f32)(dy > 0) - (f32)(dy < 0);
        }
    }

    return hit;
}

void physics_init(void) {
    state.body_list = array_list_create(sizeof(Body), 0);
    state.static_body_list = array_list_create(sizeof(Static_Body), 0);

    state.gravity = -100;
    state.terminal_velocity = -7000;

    tick_rate = 1.0f / (f32)iterations;
}


static void update_sweep_result(Hit* result, const usize other_id, const AABB a, const AABB b, vec2 velocity, const u8 a_collision_mask, const u8 b_collision_layer) {
    if ((a_collision_mask & b_collision_layer) == 0) {
        return;
    }

    AABB sum_aabb = b;
    vec2_add(sum_aabb.half_size, sum_aabb.half_size, a.half_size);

    const Hit hit = ray_intersect_aabb(a.position, velocity, sum_aabb);
    if (hit.is_hit) {
        if (hit.time < result->time) {
            *result = hit;
        } else if (hit.time == result->time) {
            if (fabsf(velocity[0]) > fabsf(velocity[1]) && hit.normal[0] != 0 || fabsf(velocity[1]) > fabsf(velocity[0]) && hit.normal[1] != 0) {
                *result = hit;
            }
        }
        result->other_id = other_id;
    }
}
static Hit sweep_static_bodies(const Body *body, vec2 velocity) {
    Hit result = {.time = 0xBEEF};

    for (usize i = 0; i < state.static_body_list->length; i++) {
        const Static_Body* static_body = physics_static_body_get(i);

        update_sweep_result(&result, i, body->aabb, static_body->aabb, velocity, body->collision_mask, static_body->collision_layer);
    }

    return result;
}

static Hit sweep_bodies(const Body *body, vec2 velocity) {
    Hit result = {.time = 0xBEEF};

    for (usize i = 0; i < state.body_list->length; i++) {
        const Body* other = physics_body_get(i);

        if (body == other) {
            continue;
        }

        update_sweep_result(&result, i, body->aabb, other->aabb, velocity, body->collision_mask, other->collision_layer);
    }

    return result;
}
static void sweep_response(Body* body, vec2 velocity) {
    const Hit hit = sweep_static_bodies(body, velocity);
    const Hit hit_moving = sweep_bodies(body, velocity);

    if (hit_moving.is_hit) {
        if (body->on_hit != NULL) {
            body->on_hit(body, physics_body_get(hit_moving.other_id), hit_moving);
        }
    }

    if (hit.is_hit) {
        body->aabb.position[0] = hit.position[0];
        body->aabb.position[1] = hit.position[1];

        if (hit.normal[0] != 0) {
            body->aabb.position[1] += velocity[1];
            body->velocity[0] = 0;
        } else if (hit.normal[1] != 0) {
            body->aabb.position[0] += velocity[0];
            body->velocity[1] = 0;
        }

        if (body->on_hit_static != NULL) {
            body->on_hit_static(body, physics_static_body_get(hit.other_id), hit);
        }
    } else {
        vec2_add(body->aabb.position, body->aabb.position, velocity);
    }
}

static void stationary_response(Body* body) {
    for (usize i = 0; i < state.static_body_list->length; i++) {
        const Static_Body* static_body = physics_static_body_get(i);

        const AABB aabb = aabb_minkowski_difference(static_body->aabb, body->aabb);
        vec2 min, max;
        aabb_min_max(min, max, aabb);

        if (physics_aabb_intersect_aabb(static_body->aabb, body->aabb)) {
            vec2 penetration_vector;
            aabb_penetration_vector(penetration_vector, aabb);

            body->aabb.position[0] += penetration_vector[0];
            body->aabb.position[1] += penetration_vector[1];
        }
    }
}

void physics_update(void) {
    for (usize i = 0; i < state.body_list->length; i++) {
        Body *body = array_list_get(state.body_list, i);
        body->velocity[1] += state.gravity;

        if (state.terminal_velocity > body->velocity[1]) {
            body->velocity[1] = state.terminal_velocity;
        }

        body->velocity[0] += body->acceleration[0];
        body->velocity[1] += body->acceleration[1];

        vec2 scaled_velocity;
        vec2_scale(scaled_velocity, body->velocity, global.time.delta * tick_rate);

        for (u8 j = 0; j < iterations; j++) {
            sweep_response(body, scaled_velocity);
            stationary_response(body);
        }
    }
}

usize physics_body_create(vec2 position, const vec2 size, vec2 velocity, const u8 collision_layer, const u8 collision_mask, const On_Hit on_hit, const On_Hit_Static on_hit_static) {
    const Body body = {
        .aabb = {
            .position = {position[0], position[1]},
            .half_size = {size[0] * 0.5f, size[1] * 0.5f},
        },
        .velocity = {velocity[0], velocity[1]},
        .collision_layer = collision_layer,
        .collision_mask = collision_mask,
        .on_hit = on_hit,
        .on_hit_static = on_hit_static,
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

usize physics_static_body_create(vec2 position, const vec2 size, const u8 collision_layer) {
    const Static_Body static_body = {
        .aabb = {
            .position = {position[0], position[1]},
            .half_size = {size[0] * 0.5f, size[1] * 0.5f},
        },
        .collision_layer = collision_layer,
    };

    if (array_list_append(state.static_body_list, &static_body) == -1) {
        printf("Failed to add body\n");
        exit(EXIT_FAILURE);
    }

    return state.static_body_list->length - 1;
}

Static_Body* physics_static_body_get(const usize index) {
    return array_list_get(state.static_body_list, index);
}
