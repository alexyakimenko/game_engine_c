#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/config.h"
#include "engine/global.h"
#include "engine/physics.h"
#include "engine/util.h"

constexpr u32 body_count = 100;

AABB test_aabb;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    config_init();
    physics_init();
    render_init(1280, 720);

    SDL_HideCursor();

    const AABB aabb = {
        .position = {(f32)global.render.width * 0.5f, (f32)global.render.height * 0.5f},
        .half_size = {50, 50}
    };

    test_aabb = aabb;

    return SDL_APP_CONTINUE;
}

vec2 pos;
void input_handle(void) {
    f32 x, y;
    SDL_GetMouseState(&x, &y);
    pos[0] = x;
    pos[1] = (f32)global.render.height - y;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    time_update();
    input_update();
    input_handle();
    physics_update();

    render_update();

    render_aabb((f32*)&test_aabb, (vec4){1, 1, 1, 0.5f});

    if (physics_point_intersects_aabb(pos, test_aabb)) {
        render_quad(pos, (vec2){5, 5}, RED);
    } else {
        render_quad(pos, (vec2){5, 5}, CYAN);
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
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { }