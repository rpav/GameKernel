#include <GL/glew.h>
#include <GL/gl.h>

#include "gk/gk.hpp"
#include "gk/gl.hpp"
#include "gk/log.hpp"

#include <vector>

const GLenum gk_filter_to_gl[] = {
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

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
        char *log = new char[res+1];

        GL_CHECK(glGetShaderInfoLog(shader, res, nullptr, log));
        LOG(log);
		delete log;
        goto gl_error;
    }

    return shader;

 gl_error:
    glDeleteShader(shader);
    return 0;
}

GLuint gk_gl_link_program(int numshaders, GLuint *shaders) {
    auto prog = glCreateProgram(); GL_CHECKERR(glCreateProgram);

    for(int i = 0; i < numshaders; ++i)
        GL_CHECK(glAttachShader(prog, shaders[i]));

    GL_CHECK(glLinkProgram(prog));

    GLint res;
    GL_CHECK(glGetProgramiv(prog, GL_LINK_STATUS, &res));
    if(res != GL_TRUE) {
        LOG("GLSL error linking program:");
        GL_CHECK(glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &res));
        char *log = new char[res+1];

        GL_CHECK(glGetProgramInfoLog(prog, res, nullptr, log));
        LOG(log);
		delete log;
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

    gk->nvg_inframe = false;
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

    if(list_gl->width && list_gl->height)
        glViewport(0, 0, list_gl->width, list_gl->height);

    for(int j = 0; j < list->ncmds; ++j) {
        auto cmd = list->cmds[j];
        auto type = GK_CMD_TYPE(cmd);

        if(gk->gl.last_cmd != type && gk_gl_process_should_transition(gk->gl.last_cmd, type)) {
            gk_gl_process_end(gk, bundle, gk->gl.last_cmd);
            gk_gl_process_begin(gk, bundle, type, cmd);
            gk->gl.last_cmd = type;
        }

        switch(type) {
            case GK_CMD_CLEAR:
                gl_cmd_clear(gk, (gk_cmd_clear*)cmd);
                break;
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

void gl_cmd_clear(gk_context *gk, gk_cmd_clear *cmd) {
	LOG("was clear");
	return;
    GLenum flags = 0;

    if(cmd->flags & GK_CLEAR_COLOR) {
        flags |= GL_COLOR_BUFFER_BIT;
        GL_CHECK(glClearColor(cmd->color.x, cmd->color.y, cmd->color.z, cmd->color.w));
    }

    if(cmd->flags & GK_CLEAR_DEPTH) {
        flags |= GL_DEPTH_BUFFER_BIT;
        GL_CHECK(glClearDepth(cmd->depth));
    }

    if(cmd->flags & GK_CLEAR_STENCIL) {
        flags |= GL_STENCIL_BUFFER_BIT;
        GL_CHECK(glClearStencil(cmd->stencil));
    }

    GL_CHECK(glClear(flags));
    return;

 gl_error:
    return;
}

static const GLenum gk_to_gl_shader[] = {
    GL_VERTEX_SHADER,
    GL_TESS_CONTROL_SHADER,
    GL_TESS_EVALUATION_SHADER,
    GL_GEOMETRY_SHADER,
    GL_FRAGMENT_SHADER,
    GL_COMPUTE_SHADER,

    0
};

static void build_one_program(gk_context *gk, gk_program_source *progsrc) {
    std::vector<GLuint> shaders;
    shaders.reserve(GK_SHADER_MAX);

    for(int i = 0; i < progsrc->nsources; ++i) {
        auto *src = progsrc->source[i];
        auto s = gk_gl_compile_shader(gk_to_gl_shader[src->type],
                                      src->source);

        if(s) {
            shaders.push_back(s);
        } else {
            gk_seterror(gk, GK_ERROR_COMPILING_SHADER);
            return;
        }
    }

    progsrc->program = gk_gl_link_program(shaders.size(), shaders.data());
}

void gl_cmd_program_create(gk_context *gk, gk_cmd_program_create *cmd) {
    for(int i = 0; i < cmd->nsources; ++i) {
        build_one_program(gk, cmd->source[i]);
        if(gk_haserror(gk)) return;
    }
}

void gl_cmd_program_destroy(gk_context *gk, gk_cmd_program_destroy *cmd) {
    for(int i = 0; i < cmd->nprograms; ++i) {
        GL_CHECK(glDeleteProgram(cmd->program[i]));
    }
    return;

 gl_error:
    gk_seterror(gk, GK_ERROR_DESTROYING_PROGRAM);
    return;
}

void gl_cmd_uniform_query(gk_context *gk, gk_cmd_uniform_query *cmd) {
    auto &program = *cmd->program;

    for(size_t i = 0; i < cmd->nuniforms; ++i) {
        cmd->uniforms[i] = glGetUniformLocation(program, cmd->names[i]);
    }
}
