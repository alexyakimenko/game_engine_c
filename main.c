#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/config.h"
#include "engine/global.h"
#include "engine/physics.h"

constexpr u32 body_count = 100;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    config_init();
    physics_init();
    render_init(1280, 720);


    for (u32 i = 0; i < body_count; i++) {
        const usize body_index = physics_body_create(
            (vec2){(f32)(rand() % global.render.width), (f32)(rand() % global.render.height)},
            (vec2){(f32)(rand() % 100), (f32)(rand() % 100)}
            );

        Body* body = physics_body_get(body_index);
        body->acceleration[0] += (f32)(rand() % 200 - 100);
        body->acceleration[1] += (f32)(rand() % 200 - 100);
    }

    return SDL_APP_CONTINUE;
}

void input_handle(void) {

}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    time_update();
    input_update();
    input_handle();
    physics_update();

    render_update();

    for (u32 i = 0; i < body_count; i++) {
        Body* body = physics_body_get(i);
        vec2 size;
        vec2_add(size, body->aabb.half_size, body->aabb.half_size);
        render_quad(body->aabb.position, size, (vec4){1, 0, 0, 1});

        if (body->aabb.position[0] > global.render.width || body->aabb.position[0] < 0)
            body->velocity[0] = -body->velocity[0];
        if (body->aabb.position[1] > global.render.height || body->aabb.position[1] < 0)
            body->velocity[1] = -body->velocity[1];

        if (body->velocity[0] > 500)
            body->velocity[0] = 500;
        if (body->velocity[0] < -500)
            body->velocity[0] = -500;
        if (body->velocity[1] > 500)
            body->velocity[1] = 500;
        if (body->velocity[1] < -500)
            body->velocity[1] = -500;
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