#include "../time.h"

#include <SDL3/SDL_timer.h>

#include "../global.h"

constexpr float SECOND = 1000.0f;

void time_init(const u32 frame_rate) {
    global.time.frame_rate = frame_rate;
    global.time.frame_delay = SECOND / (f32)frame_rate;
}

void time_update(void) {
    global.time.now = (f32)SDL_GetTicks();
    global.time.delta = (global.time.now - global.time.last) / SECOND;
    global.time.last = global.time.now;
    global.time.frame_count++;

    if (global.time.now - global.time.frame_last >= SECOND) {
        global.time.frame_rate = global.time.frame_count;
        global.time.frame_count = 0;
        global.time.frame_last = global.time.now;
    }
}

void time_update_late(void) {
    global.time.frame_time = (f32)SDL_GetTicks() - global.time.now;

    if (global.time.frame_delay > global.time.frame_time) {
        SDL_Delay((u32)(global.time.frame_delay - global.time.frame_time));
    }
}
