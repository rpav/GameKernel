#pragma once

#include <vector>

struct gk_context;

namespace gk {
    // This tracks all "global state"
    struct GLGlobalState {
        GLuint active_program;
        GLenum active_texture;

        GLuint vertex_array;
        GLuint array_buffer;
    };

    // This (or a subclass at least) tracks one specific thing
    class GLState {
    public:
        virtual ~GLState() {}

        // Subclasses should implement this; check/update the GLState and
        // make the appropriate GL call if necessary.
        virtual void stateUpdate(GLState*) = 0;
    };
    typedef std::vector<GLState*> StateVector;

    // This is one configuration of GL states.  Subclass it, make members,
    // add them to the vector.
    class GLStateConfig {
    protected:
        StateVector states;
    public:

        void apply(GLGlobalState*);
    };
}
// State-tracking API
void gk_gl_reset_state(gk_context *gk);

void gk_glActiveTexture(gk_context *gk, GLenum texture);
void gk_glUseProgram(gk_context *gk, GLuint program);

void gk_glBindVertexArray(gk_context *gk, GLuint array);
void gk_glBindBuffer(gk_context *gk, GLenum target, GLuint buffer);
