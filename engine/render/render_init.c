#include "render_internal.h"

#include <glad/glad.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_messagebox.h>

#include "../global.h"

SDL_Window* render_init_window(const i32 width, const i32 height, const char* title) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), nullptr);
    }

    SDL_Window* window = SDL_CreateWindow(
            title,
            width,
            height,
            SDL_WINDOW_OPENGL
        );

    if (window == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't create window/renderer!", SDL_GetError(), nullptr);
    }

    SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize OpenGL!", SDL_GetError(), nullptr);
    }

    return window;
}

void render_init_quad(u32* vao, u32* vbo, u32* ebo) {
    // quad vertices
    constexpr f32 vertices[] = {
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
    };

    // quad indices
    const u32 indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // generate buffers
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    // unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_init_color_texture(u32* texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    constexpr u8 solid_white[4] = {255, 255, 255, 255}; // creates 1px texture with solid white color
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, solid_white);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_init_shaders(Render_State_Internal* state) {
    state->shader_default = render_shader_create("assets/shaders/default.vert", "assets/shaders/default.frag");

    // creates orthogonal matrix
    mat4x4_ortho(state->projection, 0, (f32)global.render.width, 0, (f32)global.render.height, -2, 2);

    glUseProgram(state->shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(state->shader_default, "projection"),
        1, GL_FALSE,
        &state->projection[0][0]
        );
}

void render_init_line(u32* vao, u32* vbo) {
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);

    // bind buffers
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);

    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
