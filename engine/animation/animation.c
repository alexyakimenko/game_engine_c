#include "../animation.h"

#include <assert.h>
#include <stdio.h>

#include "../util.h"
#include "../array_list.h"

Array_List* animation_definition_storage;
Array_List* animation_storage;

void animation_init(void) {
    animation_definition_storage = array_list_create(sizeof(Animation_Definition), 0);
    animation_storage = array_list_create(sizeof(Animation), 0);
}

usize animation_definition_create(Sprite_Sheet* sprite_sheet, const f32* durations, const u8* rows, const u8* columns, const u8 frame_count) {
    assert(frame_count <= MAX_FRAMES);

	Animation_Definition def = {0};

	def.sprite_sheet = sprite_sheet;
	def.frame_count = frame_count;

	for (u8 i = 0; i < frame_count; ++i) {
		def.frames[i] = (Animation_Frame){
			.column = columns[i],
			.row = rows[i],
			.duration = durations[i],
		};
	}

	return array_list_append(animation_definition_storage, &def);
}

usize animation_create(const usize animation_definition_id, bool does_loop) {
    usize id = animation_storage->length;
    Animation_Definition* def = array_list_get(animation_definition_storage, animation_definition_id);
    if (def == NULL) {
        printf("Animation definition with id %zu not found", animation_definition_id);
    }

    for (usize i = 0; i < animation_storage->length; i++) {
        const Animation* animation = array_list_get(animation_storage, i);
        if (!animation->is_active) {
            id = i;
            break;
        }
    }

    if (id == animation_storage->length) {
        array_list_append(animation_storage, &(Animation){nullptr});
    }

    Animation* animation = array_list_get(animation_storage, id);

    *animation = (Animation){
        .definition = def,
        .does_loop = does_loop,
        .is_active = true,
    };

    return id;
}

void animation_destroy(const usize id) {
    Animation* animation = array_list_get(animation_storage, id);
    animation->is_active = false;
}
Animation *animation_get(const usize id) {
    return array_list_get(animation_storage, id);
}
void animation_update(const f32 dt) {
    for (usize i = 0; i < animation_storage->length; i++) {
        Animation* animation = array_list_get(animation_storage, i);
        const Animation_Definition* def = animation->definition;
        animation->current_frame_time -= dt;

        if (animation->current_frame_time < 0.0f) {
            animation->current_frame_index++;

            if (animation->current_frame_index >= def->frame_count) {
                if (animation->does_loop) {
                    animation->current_frame_index = 0;
                } else {
                    animation->current_frame_index -= 1;
                }
            }

            animation->current_frame_time = def->frames[animation->current_frame_index].duration;
        }
    }
}
