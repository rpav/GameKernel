#pragma once

#include <chrono>

#include <rpav/str/gk.hpp>
#include <rpav/log.hpp>

#include "gk/gk.h"

using namespace rpav;

#define WIDTH 1280
#define HEIGHT 720

#define checkrc(x)                                                                       \
    if((x) != 0) {                                                                       \
        printf("Error: '%s'\n", SDL_GetError());                                         \
        exit(1);                                                                         \
    }
#define checknull(x)                                                                     \
    if((x) == NULL) {                                                                    \
        printf("Error: '%s'\n", SDL_GetError());                                         \
        exit(1);                                                                         \
    }

void        swap();
void        wait();
bool        check_input();
extern void example_main(int argc, const char* argv[]);

/* Note these do not alloc/free the *passed* object, only its *direct*
   contents. */
void init_bundle(gk_bundle* bundle, unsigned int start, int nlists);
void free_bundle(gk_bundle* bundle);

void init_list(gk_list* list, gk_subsystem type, int ncmds, gk_cmd** cmds = nullptr);
void free_list(gk_list* list);

/* This does not allocate anything */
void init_path(gk_cmd_path* path, float* pathdef, size_t len);

/* Utility */
typedef std::chrono::steady_clock    steady_clock;
typedef std::chrono::duration<float> float_dur;
typedef std::chrono::milliseconds    time_ms;

class Clock {
    std::chrono::time_point<steady_clock> _start;

public:
    void start() { reset(); }

    float delta()
    {
        auto      now  = steady_clock::now();
        float_dur diff = now - _start;
        return std::chrono::duration_cast<time_ms>(diff).count();
    }

    void reset() { _start = steady_clock::now(); }
};

class FPS {
    Clock        _clock;
    unsigned int _reports;
    unsigned int _frames;

public:
    FPS() : _reports(1000), _frames(0) {}

    void start() { _clock.start(); }

    void frame()
    {
        ++_frames;

        auto ms = _clock.delta();
        if(ms >= _reports) {
            say("FPS: ", (float)_frames / ((float)ms / 1000.0), "   ",
                "ms/F: ", ((double)ms) / _frames, " (avg over ", _frames, ")");
            _frames = 0;
            _clock.reset();
        }
    }
};
