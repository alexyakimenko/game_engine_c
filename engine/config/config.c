#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_keyboard.h>

#include "../io.h"
#include "../global.h"

static const char* CONFIG_DEFAULT =
    "[controls]\n"
    "left = A\n"
    "right = D\n"
    "up = W\n"
    "down = S\n"
    "escape = Escape\n"
    "\n";

static const char* CONFIG_PATH = "assets/config/config.ini";

static char tmp_buffer[20] = {0};

static char* config_get_value(const char* config_buffer, const char* value) {
    char* line = strstr(config_buffer, value);

    if (!line) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Could not find config value", value, nullptr);
        exit(EXIT_FAILURE);
    }

    const usize len = strlen(line);
    const char* end = line + len;

    const char* curr = line;
    char* tmp_ptr = &tmp_buffer[0];

    // move pointer till = or eol
    while (*curr != '=' && curr != end) {
        curr++;
    }

    curr++;

    // move till first character
    while (*curr == ' ') {
       curr++;
    }

    while (*curr != '\n' && *curr != '\r' && *curr != 0 && curr != end) {
        *tmp_ptr++ = *curr++;
    }

    *tmp_ptr = 0;

    return tmp_buffer;
}

static void load_controls(const char* config_buffer) {
    config_key_bind(INPUT_KEY_LEFT, config_get_value(config_buffer, "left"));
    config_key_bind(INPUT_KEY_RIGHT, config_get_value(config_buffer, "right"));
    config_key_bind(INPUT_KEY_UP, config_get_value(config_buffer, "up"));
    config_key_bind(INPUT_KEY_DOWN, config_get_value(config_buffer, "down"));
    config_key_bind(INPUT_KEY_ESCAPE, config_get_value(config_buffer, "escape"));
}

static int config_load(void) {
    const File config_file = io_file_read(CONFIG_PATH);

    if (!config_file.is_valid) {
        printf("Error reading config file, using default\n");
        load_controls(CONFIG_DEFAULT);
        return 1;
    }

    load_controls(config_file.data);

    free(config_file.data);

    return 0;
}

void config_init(void) {
    if (config_load() == 0) {
        return;
    }

    io_file_write(CONFIG_PATH, CONFIG_DEFAULT, strlen(CONFIG_DEFAULT));
}

void config_key_bind(const Input_Key key, const char* key_name) {
    const SDL_Scancode scancode = SDL_GetScancodeFromName(key_name);
    if (scancode == SDL_SCANCODE_UNKNOWN) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Invalid scan code for key", key_name, nullptr);
        return;
    }

    global.config.key_binds[key] = scancode;
}
