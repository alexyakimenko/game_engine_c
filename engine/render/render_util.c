#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <SDL3/SDL_messagebox.h>

#include "../io.h"

#include "render_internal.h"

u32 render_shader_create(const char* path_vert, const char* path_frag) {
    i32 success;
    char info[1024];

    const File vertex_file = io_file_read(path_vert);
    if (!vertex_file.is_valid) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error reading vertex shader", path_vert, nullptr);
    }

    const u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_file.data, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, sizeof(info), nullptr, info);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error compiling vertex shader", info, nullptr);
    }

    const File fragment_file = io_file_read(path_frag);
    if (!fragment_file.is_valid) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error reading fragment shader", path_frag, nullptr);
    }

    const u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_file.data, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, sizeof(info), nullptr, info);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error compiling fragment shader", info, nullptr);
    }

    const u32 program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(info), nullptr, info);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error linking program", info, nullptr);
    }
    free(vertex_file.data);
    free(fragment_file.data);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}
