#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

bool gk_gl_checkerror(const char *expr, const char *file, int line) {
    auto err = glGetError();
    if(err != GL_NO_ERROR) {
        LOG("GL error ", err, " at ", file, ":", line, " ", expr);
        return true;
    }
    return false;
}

GLuint gk_gl_compile_shader(GLenum type, const char *text) {
    auto shader = glCreateShader(type);
    GL_CHECK(glShaderSource(shader, 1, &text, nullptr));
    GL_CHECK(glCompileShader(shader));

    GLint res;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &res));
    if(res != GL_TRUE) {
        LOG("GLSL error:");
        GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res));
        char log[res+1];

        GL_CHECK(glGetShaderInfoLog(shader, res, nullptr, log));
        LOG(log);
        goto gl_error;
    }
    
    return shader;

 gl_error:
    glDeleteShader(shader);
    return 0;
}

GLuint gk_gl_link_program(int numshaders, GLuint *shaders) {
    auto prog = glCreateProgram(); GL_CHECK();

    for(int i = 0; i < numshaders; ++i)
        GL_CHECK(glAttachShader(prog, shaders[i]));

    GL_CHECK(glLinkProgram(prog));

    GLint res;
    GL_CHECK(glGetProgramiv(prog, GL_LINK_STATUS, &res));
    if(res != GL_TRUE) {
        LOG("GLSL error linking program:");
        GL_CHECK(glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &res));
        char log[res+1];

        GL_CHECK(glGetProgramInfoLog(prog, res, nullptr, log));
        LOG(log);
        goto gl_error;
    }

    for(int i = 0; i < numshaders; ++i) {
        GL_CHECK(glDetachShader(prog, shaders[i]));
        GL_CHECK(glDeleteShader(shaders[i]));
    }

    return prog;

 gl_error:
    if(prog) glDeleteProgram(prog);
    for(int i = 0; i < numshaders; ++i)
        glDeleteShader(shaders[i]);
    return 0;
}

bool gk_init_gl(gk_context *gk) {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOG("Error initializing GLEW: ", glewGetErrorString(err));
        return false;
    }
    /* GLEW can return INVALID_ENUM */
    while(glGetError() != GL_NO_ERROR);

    switch(gk->impl) {
        case GK_GL2:
            gk_create_gl2(gk);
            break;

        case GK_GL3:
            gk_create_gl3(gk);
            break;

        default:
            LOG("GK error: Unknown impl: ", gk->impl);
            return false;
    };

    gk->gl.last_cmd = GK_CMD_NULL;
    gk_gl_reset_state(gk);

    return true;
}

void gk_fini_gl(gk_context *gk) {
    switch(gk->impl) {
        case GK_GL2:
            gk_destroy_gl2(gk);
            break;

        case GK_GL3:
            gk_destroy_gl3(gk);
            break;

        default:
            LOG("GK warning: Unknown impl: ", gk->impl);
    };
}

static void gk_gl_process_begin(gk_context *gk, gk_bundle *bundle, gk_cmd_type type, gk_cmd *cmd) {
    switch(type) {
        case GK_CMD_QUAD:
        case GK_CMD_QUADSPRITE:
            gk->gl.gl_begin_quad(gk, bundle, (gk_cmd_quad*)cmd);
            return;

        default: return;
    }
}

static bool gk_gl_process_should_transition(gk_cmd_type last, gk_cmd_type next) {
    switch(last) {
        case GK_CMD_QUAD:
            switch(next) {
                case GK_CMD_QUADSPRITE: return false;
                default: break;
            }
            break;
        case GK_CMD_QUADSPRITE:
            switch(next) {
                case GK_CMD_QUAD: return false;
                default: break;
            }
            break;
        default: break;
    }
    return true;
}

static void gk_gl_process_end(gk_context *gk, gk_bundle *bundle, gk_cmd_type type) {
    switch(type) {
        case GK_CMD_QUAD:
        case GK_CMD_QUADSPRITE:            
            gk->gl.gl_end_quad(gk);
            return;

        default: return;
    }
}

void gk_process_gl(gk_context *gk, gk_bundle *bundle, gk_list_gl *list_gl) {
    auto list = GK_LIST(list_gl);

    for(int j = 0; j < list->ncmds; ++j) {
        auto cmd = list->cmds[j];
        auto type = GK_CMD_TYPE(cmd);

        if(gk->gl.last_cmd != type && gk_gl_process_should_transition(gk->gl.last_cmd, type)) {
            gk_gl_process_end(gk, bundle, gk->gl.last_cmd);
            gk_gl_process_begin(gk, bundle, type, cmd);
            gk->gl.last_cmd = type;
        }

        switch(type) {
            case GK_CMD_QUAD:
                gk->gl.gl_cmd_quad(gk, bundle, (gk_cmd_quad*)cmd);
                break;
            case GK_CMD_QUADSPRITE:
                gk->gl.gl_cmd_quadsprite(gk, bundle, (gk_cmd_quadsprite*)cmd);
                break;                
            default:
                gk_process_cmd_general("GK_LIST_GL", gk, bundle, cmd);
                break;
        }
    }

    gk_gl_process_end(gk, bundle, gk->gl.last_cmd);
    gk->gl.last_cmd = GK_CMD_NULL;
    glFlush();
}
