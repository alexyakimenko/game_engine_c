#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/config.h"
#include "engine/global.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    config_init();
    render_init(1280, 720);
    return SDL_APP_CONTINUE;
}

float posX = 0.0f;
float posY = 0.0f;

void input_handle(void) {
    if (global.input.right == KS_PRESSED || global.input.right == KS_HELD) {
        posX += global.time.delta * 350;
    }
    if (global.input.left == KS_PRESSED || global.input.left == KS_HELD) {
        posX -= global.time.delta * 350;
    }
    if (global.input.up == KS_PRESSED || global.input.up == KS_HELD) {
        posY += global.time.delta * 350;
    }
    if (global.input.down == KS_PRESSED || global.input.down == KS_HELD) {
        posY -= global.time.delta * 350;
    }
}
SDL_AppResult SDL_AppIterate(void *appstate)
{
    time_update();
    input_update();
    input_handle();

    render_update();

    render_quad_line(
        (vec2){posX, posY},
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
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { }