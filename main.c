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
char buffer[50];
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (SDL_GetTicks() - global.time.frame_last >= 1000.0f) {
        sprintf(buffer, "%s - %d fps", "Current FPS", global.time.frame_rate);
        SDL_SetWindowTitle(global.render.window, buffer);
    }

    time_update();
    input_update();
    input_handle();
    physics_update();

    render_begin();

    cursor_aabb.position[0] = pos[0];
    cursor_aabb.position[1] = pos[1];

    render_aabb((f32*)&test_aabb, WHITE);
    render_aabb((f32*)&sum_aabb, (vec4){1, 1, 1, 0.5f});

    AABB minkowski_difference = aabb_minkowski_difference(test_aabb, cursor_aabb);
    render_aabb((f32*)&minkowski_difference, ORANGE);

    vec2 pv;
    aabb_penetration_vector(pv, minkowski_difference);

    AABB collision_aabb = cursor_aabb;
    collision_aabb.position[0] += pv[0];
    collision_aabb.position[1] += pv[1];

    if (physics_aabb_intersects_aabb(test_aabb, cursor_aabb)) {
        render_aabb((f32*)&collision_aabb, CYAN);
        render_line_segment(cursor_aabb.position, collision_aabb.position, PURPLE);
        render_aabb((f32*)&cursor_aabb, RED);
    } else {
        render_aabb((f32*)&cursor_aabb, WHITE);
    }

    render_aabb((f32*)&start_aabb, WHITE);
    render_line_segment(start_aabb.position, cursor_aabb.position, PURPLE);


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