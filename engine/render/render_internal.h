#pragma once
#include <SDL3/SDL_video.h>
#include <linmath.h>

#include "../types.h"

typedef struct render_state_internal {
    u32 vao_quad;
    u32 vbo_quad;
    u32 ebo_quad;
    u32 vao_line;
    u32 vbo_line;
    u32 texture_color;
    u32 shader_default;
    mat4x4 projection;
} Render_State_Internal;

SDL_Window* render_init_window(i32 width, i32 height, const char* title);
void render_init_quad(u32* vao, u32* vbo, u32* ebo);
void render_init_color_texture(u32* texture);
void render_init_shaders(Render_State_Internal* state);
void render_init_line(u32* vao, u32* vbo);
u32 render_shader_create(const char* path_vert, const char* path_frag);
