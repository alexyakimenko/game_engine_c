#pragma once

#include "render.h"
#include "time.h"

typedef struct global {
    Render_State render;
    Time_State time;
} Global;

extern Global global;
