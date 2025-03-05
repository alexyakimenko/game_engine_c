#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/global.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    render_init(1280, 720);
    return SDL_APP_CONTINUE;
}

float posX = 0.0f;
SDL_AppResult SDL_AppIterate(void *appstate)
{
    time_update();
    render_update();

    render_quad_line(
        (vec2){posX, (f32)global.render.height * 0.5f},
        (vec2){50, 50},
        (vec4){0, 0.5f, 0.8f, 1});

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
            if (event->key.key == SDLK_D) {
                posX += 10.0f;
            } else if (event->key.key == SDLK_A) {
                posX -= 10.0f;
            }
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { }