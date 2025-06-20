#include "../array_list.h"
#include "../entity.h"

#include "../physics.h"

static Array_List* entity_list;

void entity_init() {
    entity_list = array_list_create(sizeof(Entity), 0);
};

usize entity_create(vec2 position, vec2 size, vec2 velocity, const u8 collision_layer, const u8 collision_mask, const On_Hit on_hit, const On_Hit_Static on_hit_static) {
    const Entity entity = {
        .body_id = physics_body_create(position, size, velocity, collision_layer, collision_mask, on_hit, on_hit_static),
        .animation_id = (usize)-1,
        .is_active = true
    };

    return array_list_append(entity_list, &entity);
};

Entity* entity_get(const usize id) {
    return array_list_get(entity_list, id);
};
usize entity_count() {
    return entity_list->length;
};
