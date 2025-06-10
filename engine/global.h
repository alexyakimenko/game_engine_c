#pragma once

#include "config.h"
#include "render.h"
#include "time.h"

typedef struct global {
    Time_State time;
    Config_State config;
    Input_State input;
} Global;

extern Global global;
