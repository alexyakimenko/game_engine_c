#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "engine/config.h"
#include "engine/global.h"
#include "engine/physics.h"
#include "engine/util.h"
#include "engine/entity.h"

typedef enum collision_layer {
    COLLISION_LAYER_PLAYER = 1,
    COLLISION_LAYER_ENEMY = 1 << 1,
    COLLISION_LAYER_TERRAIN = 1 << 2
} Collision_Layer;

usize static_body_ids[5];
usize enemies_ids[2];

vec4 player_color = {0, 1, 1, 1};
bool player_is_grounded = false;

void player_on_hit(Body* self, const Body* other, const Hit hit) {
    if (other->collision_layer == COLLISION_LAYER_ENEMY) {
        player_color[0] = 1;
        player_color[2] = 0;
    }
}

void player_on_hit_static(Body* self, const Static_Body* other, const Hit hit) {
    if (hit.normal[1] > 0) {
        player_is_grounded = true;
    }
}

void enemy_on_hit_static(Body* self, const Static_Body* other, const Hit hit) {
    if (hit.normal[0] > 0) {
        self->velocity[0] = 700;
    }
    if (hit.normal[0] < 0) {
        self->velocity[0] = -700;
    }
}

constexpr u8 enemy_mask = COLLISION_LAYER_PLAYER | COLLISION_LAYER_TERRAIN;
constexpr u8 player_mask = COLLISION_LAYER_ENEMY | COLLISION_LAYER_TERRAIN;

usize player_id;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    time_init(60);
    config_init();
    physics_init();
    render_init(1280, 720);
    entity_init();

    player_id = entity_create((vec2){100, 300}, (vec2){50, 50}, (vec2){0, 0}, COLLISION_LAYER_PLAYER, player_mask, player_on_hit, player_on_hit_static);

    const f32 width = (f32)global.render.width;
    const f32 height = (f32)global.render.height;

    static_body_ids[0] = physics_static_body_create((vec2){width * 0.5f - 25, height - 25}, (vec2){width - 50, 50}, COLLISION_LAYER_TERRAIN);
    static_body_ids[1] = physics_static_body_create((vec2){width - 25, height * 0.5f + 25}, (vec2){50, height - 50}, COLLISION_LAYER_TERRAIN);
    static_body_ids[2] = physics_static_body_create((vec2){width * 0.5f + 25, 25}, (vec2){width - 50, 50}, COLLISION_LAYER_TERRAIN);
    static_body_ids[3] = physics_static_body_create((vec2){25, height * 0.5f - 25}, (vec2){50, height - 50}, COLLISION_LAYER_TERRAIN);
    static_body_ids[4] = physics_static_body_create((vec2){width * 0.5f, height * 0.5f}, (vec2){150, 150}, COLLISION_LAYER_TERRAIN);

    enemies_ids[0] = entity_create((vec2){600, 600}, (vec2){50, 50}, (vec2){900, 0}, COLLISION_LAYER_ENEMY, enemy_mask, nullptr, enemy_on_hit_static);
    enemies_ids[1] = entity_create((vec2){600, 600}, (vec2){90, 90}, (vec2){-900, 0}, COLLISION_LAYER_ENEMY, enemy_mask, nullptr, enemy_on_hit_static);

    return SDL_APP_CONTINUE;
}

static void input_handle(Body* player_body) {
    vec2 vel = {0, player_body->velocity[1]};

    f32 accel = 0;

    if (global.input.right > 0) {
        vel[0] += 1000;
        accel += 50;
    }

    if (global.input.left > 0) {
        vel[0] -= 1000;
        accel -= 50;
    }

    if (global.input.up > 0 && player_is_grounded) {
        player_is_grounded = false;
        vel[1] = 1000;
    }

    if (global.input.down > 0) {
        vel[1] -= 800;
    }

    player_body->velocity[0] = vel[0];
    player_body->acceleration[0] = accel;
    player_body->velocity[1] = vel[1];
}

Static_Body* static_bodies[5];

char title_buffer[50];


SDL_AppResult SDL_AppIterate(void *appstate)
{
    if ((f32)SDL_GetTicks() - global.time.frame_last >= 1000.0f) {
        sprintf(title_buffer, "%s - %d fps", "Current FPS", global.time.frame_rate);
        SDL_SetWindowTitle(global.render.window, title_buffer);
    }

    time_update();

    const Entity* player = entity_get(player_id);
    Body* player_body = physics_body_get(player->body_id);
    for (u8 i = 0; i < 5; i++) {
        static_bodies[i] = physics_static_body_get(static_body_ids[i]);
    }

    input_update();
    input_handle(player_body);
    physics_update();

    render_begin();

    for (u8 i = 0; i < 5; i++) {
        render_aabb((f32*)static_bodies[i], WHITE);
    }
    render_aabb((f32*)player_body, player_color);

    for (u8 i = 0; i < 2; i++) {
       render_aabb((f32*)physics_body_get(entity_get(enemies_ids[i])->body_id), WHITE);
    }

    render_end();

    player_color[0] = 0;
    player_color[2] = 1;
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