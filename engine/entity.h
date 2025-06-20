#pragma once

#include "types.h"
#include "linmath.h"
#include "physics.h"

typedef struct entity {
    usize body_id;
    usize animation_id;
    bool is_active;
} Entity;

void entity_init();
usize entity_create(vec2 position, vec2 size, vec2 velocity, u8 collision_layer, u8 collision_mask, On_Hit on_hit, On_Hit_Static on_hit_static);
Entity* entity_get(usize id);
usize entity_count();
