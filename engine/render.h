#pragma once

#include <SDL3/SDL.h>
#include "linmath.h"

#include "types.h"

typedef struct batch_vertex {
    vec2 position;
    vec2 uvs;
    vec4 color;
} Batch_Vertex;

typedef struct sprite_sheet {
    f32 width;
    f32 height;
    f32 cell_width;
    f32 cell_height;
    u32 texture_id;
} Sprite_Sheet;

#define MAX_BATCH_QUADS 10000
#define MAX_BATCH_VERTICES 40000
#define MAX_BATCH_ELEMENTS 60000

SDL_Window* render_init(void);
void render_begin(void);
void render_end(SDL_Window* window, u32 batch_texture_id);
void render_quad(vec2 pos, vec2 size, vec4 color);
void render_line_segment(const vec2 start, const vec2 end, vec4 color);
void render_quad_line(vec2 pos, const vec2 size, vec4 color);
void render_aabb(f32* aabb, vec4 color);
f32 render_get_scale();

void render_sprite_sheet_init(Sprite_Sheet* sprite_sheet, const char* path, f32 cell_width, f32 cell_height);
void render_sprite_sheet_frame(const Sprite_Sheet* sprite_sheet, f32 row, f32 column, vec2 position, bool is_flipped);