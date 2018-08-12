#include <stdio.h>
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <rpav/log.hpp>
#include "example.hpp"

SDL_Window *w = NULL;
SDL_GLContext gl;

void swap() {
    SDL_GL_SwapWindow(w);
}

void wait() {
    SDL_Event ev;

    do {
        SDL_WaitEvent(&ev);
    } while(!(ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN));
}

bool check_input() {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        switch(ev.type) {
            case SDL_KEYDOWN:
            case SDL_MOUSEBUTTONDOWN:
                return true;

            case SDL_WINDOWEVENT:
                if(ev.window.event == SDL_WINDOWEVENT_CLOSE)
                    return true;

            default: break;
        }
    }
    return false;
}

#undef main
int main(int argc, const char* argv[]) {
    checkrc(SDL_Init(SDL_INIT_EVERYTHING));

    checkrc(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
    checkrc(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
    checkrc(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
    checkrc(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));

    checknull(w = SDL_CreateWindow("Example",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WIDTH, HEIGHT,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL));
    checknull(gl = SDL_GL_CreateContext(w));
    checkrc(SDL_GL_MakeCurrent(w, gl));

    checkrc(SDL_GL_SetSwapInterval(0));

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    say(glGetString(GL_VENDOR), " - ", glGetString(GL_RENDERER));
    say("GL ", major, ".", minor);

    example_main(argc, argv);
              
    SDL_Quit();
}

void init_bundle(gk_bundle *bundle, unsigned int start, int nlists) {
    GK_CMD_TYPE(&bundle->start) = GK_CMD_PASS;
    bundle->start.sort = GK_PASS_SORT_NONE;
    bundle->start.list_index = start;

    bundle->nlists = nlists;
    bundle->lists = new gk_list*[nlists];
}

void free_bundle(gk_bundle *bundle) {
    delete[] bundle->lists;
}

void init_list(gk_list *list, gk_subsystem sub, int ncmds, gk_cmd **cmds) {
    list->sub = sub;
    list->ncmds = ncmds;
    list->in_use = 0;

    if(!cmds)
        list->cmds = new gk_cmd*[ncmds];
    else
        list->cmds = cmds;
}

void free_list(gk_list *list) {
    delete[] list->cmds;
}

void init_path(gk_cmd_path *path, float *pathdef, size_t len) {
    GK_CMD_TYPE(path) = GK_CMD_PATH;
    path->pathdef = pathdef;
    path->pathlen = len;
}
