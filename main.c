#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/config.h"
#include "engine/global.h"
#include "engine/physics.h"
#include "engine/util.h"

constexpr u32 body_count = 100;

AABB test_aabb;
AABB cursor_aabb;
AABB sum_aabb;
AABB start_aabb;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    config_init();
    physics_init();
    render_init(1280, 720);

    SDL_HideCursor();

    const AABB test = {
        .position = {(f32)global.render.width * 0.5f, (f32)global.render.height * 0.5f},
        .half_size = {50, 50}
    };

    const AABB cursor = {
        .half_size = {75, 75},
    };

    const AABB start = cursor;

    const AABB sum = {
        .position = {test.position[0], test.position[1]},
        .half_size = {test.half_size[0] + cursor.half_size[0], test.half_size[1] + cursor.half_size[1]},
    };

    test_aabb = test;
    cursor_aabb = cursor;
    sum_aabb = sum;
    start_aabb = start;

    return SDL_APP_CONTINUE;
}

vec2 pos;
void input_handle(void) {
    f32 x, y;
    SDL_GetMouseState(&x, &y);
    pos[0] = x;
    pos[1] = (f32)global.render.height - y;
}

char title_buffer[50];
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if ((f32)SDL_GetTicks() - global.time.frame_last >= 1000.0f) {
        sprintf(title_buffer, "%s - %d fps", "Current FPS", global.time.frame_rate);
        SDL_SetWindowTitle(global.render.window, title_buffer);
    }

    time_update();
    input_update();
    input_handle();
    physics_update();

    render_begin();

    cursor_aabb.position[0] = pos[0];
    cursor_aabb.position[1] = pos[1];

    render_aabb((f32*)&test_aabb, WHITE);

    if (physics_aabb_intersect_aabb(test_aabb, cursor_aabb)) {
        render_aabb((f32*)&cursor_aabb, RED);
    } else {
        render_aabb((f32*)&cursor_aabb, WHITE);
    }

    vec4 faded = {1, 1, 1, 0.3f};

    render_aabb((f32*)&start_aabb, faded);
    render_line_segment(start_aabb.position, cursor_aabb.position, faded);

    const f32 x = sum_aabb.position[0];
    const f32 y = sum_aabb.position[1];
    const f32 size = sum_aabb.half_size[0];

    render_line_segment((vec2){x - size, 0}, (vec2){x - size, (f32)global.render.height}, faded);
    render_line_segment((vec2){x + size, 0}, (vec2){x + size, (f32)global.render.height}, faded);
    render_line_segment((vec2){0, y - size}, (vec2){(f32)global.render.width, y - size}, faded);
    render_line_segment((vec2){0, y + size}, (vec2){(f32)global.render.width, y + size}, faded);

    vec2 min, max;
    aabb_min_max(min, max, sum_aabb);

    vec2 magnitude;
    vec2_sub(magnitude, pos, start_aabb.position);
    const Hit hit = ray_intersect_aabb(start_aabb.position, magnitude, sum_aabb);

    if (hit.is_hit) {
        AABB hit_aabb = {
            .position = {hit.position[0], hit.position[1]},
            .half_size = {start_aabb.half_size[0], start_aabb.half_size[1]},
        };
        render_aabb((f32*)&hit_aabb, CYAN);
        render_quad(hit.position, (vec2){5, 5}, CYAN);
    }

    for (u8 i = 0; i < 2; i++) {
        if (magnitude[i] != 0) {
            const f32 t1 = (min[i] - pos[i]) / magnitude[i];
            const f32 t2 = (max[i] - pos[i]) / magnitude[i];

            vec2 point;
            vec2_scale(point, magnitude, t1);
            vec2_add(point, point, pos);
            if (min[i] < start_aabb.position[i]) {
                render_quad(point, (vec2){5, 5}, ORANGE);
            } else {
                render_quad(point, (vec2){5, 5}, CYAN);
            }

            vec2_scale(point, magnitude, t2);
            vec2_add(point, point, pos);
            if (max[i] < start_aabb.position[i]) {
                render_quad(point, (vec2){5, 5}, CYAN);
            } else {
                render_quad(point, (vec2){5, 5}, ORANGE);
            }
        }
    }

    render_end();
    time_update_late();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, const SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
                return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
            {
                return SDL_APP_SUCCESS;
            }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            start_aabb = cursor_aabb;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { }