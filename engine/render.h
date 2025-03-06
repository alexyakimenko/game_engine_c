#pragma once

#include <SDL3/SDL.h>
#include "linmath.h"

#include "types.h"

typedef struct render_state {
    SDL_Window* window;
    u32 width;
    u32 height;
} Render_State;

void render_init(i32 width, i32 height);
void render_update(void);
void render_end(void);
void render_quad(vec2 pos, vec2 size, vec4 color);
void render_line_segment(vec2 start, const vec2 end, vec4 color);
void render_quad_line(vec2 pos, const vec2 size, vec4 color);
void render_aabb(f32* aabb, vec4 color);
