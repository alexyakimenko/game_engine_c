#pragma once

#include "types.h"
#include "input.h"

typedef struct config_state {
    u8 key_binds[5];
} Config_State;

void config_init(void);
void config_key_bind(Input_Key key, const char* key_name);
