#include "../input.h"

#include <SDL3/SDL_keyboard.h>

#include "../types.h"
#include "../global.h"

static void update_key_state(const u8 current_state, Key_State* key_state) {
    if (current_state) {
        if (*key_state) {
            *key_state = KS_HELD;
        } else {
            *key_state = KS_PRESSED;
        }
    } else {
        *key_state = KS_UNPRESSED;
    }
}

void input_update(void) {
    const u8* keyboard_state = (u8*)SDL_GetKeyboardState(nullptr);

    update_key_state(keyboard_state[global.config.key_binds[INPUT_KEY_LEFT]], &global.input.left);
    update_key_state(keyboard_state[global.config.key_binds[INPUT_KEY_RIGHT]], &global.input.right);
    update_key_state(keyboard_state[global.config.key_binds[INPUT_KEY_UP]], &global.input.up);
    update_key_state(keyboard_state[global.config.key_binds[INPUT_KEY_DOWN]], &global.input.down);
    update_key_state(keyboard_state[global.config.key_binds[INPUT_KEY_ESCAPE]], &global.input.escape);
}
