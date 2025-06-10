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

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
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

void render_init_shaders(u32* shader_default, u32* shader_batch,  const f32 render_width, const f32 render_height) {
    mat4x4 projection;
    *shader_default = render_shader_create("assets/shaders/default.vert", "assets/shaders/default.frag");
    *shader_batch = render_shader_create("assets/shaders/batch_quad.vert", "assets/shaders/batch_quad.frag");

    // creates orthogonal matrix
    mat4x4_ortho(projection, 0, render_width, 0, render_height, -2, 2);

    glUseProgram(*shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(*shader_default, "projection"),
        1, GL_FALSE,
        &projection[0][0]
        );

    glUseProgram(*shader_batch);
    glUniformMatrix4fv(
        glGetUniformLocation(*shader_batch, "projection"),
        1, GL_FALSE,
        &projection[0][0]
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

void render_init_batch_quads(u32* vao, u32* vbo, u32* ebo) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    u32 indices[MAX_BATCH_ELEMENTS];
    for (u32 i = 0, offset = 0; i < MAX_BATCH_ELEMENTS; i+= 6, offset += 4) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
    }

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_VERTICES * sizeof(Batch_Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Batch_Vertex), (void*) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Batch_Vertex), (void*)offsetof(Batch_Vertex, uvs));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Batch_Vertex), (void*)offsetof(Batch_Vertex, color));

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_BATCH_ELEMENTS * sizeof(u32), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
