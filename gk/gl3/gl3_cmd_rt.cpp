#include <GL/glew.h>

#include <GL/gl.h>
#include <rpav/log.hpp>

#include "gk/gk.hpp"
#include "gk/gl.hpp"

using namespace rpav;

void gl3_rt_init(gk_context* gk)
{
    gk->gl.gl_cmd_rt_create  = gl3_cmd_rt_create;
    gk->gl.gl_cmd_rt_destroy = gl3_cmd_rt_destroy;
    gk->gl.gl_cmd_rt_bind    = gl3_cmd_rt_bind;
    gk->gl.gl_cmd_rt_unbind  = gl3_cmd_rt_unbind;
}

void gl3_cmd_rt_create(gk_context* gk, gk_cmd_rt_create* cmd)
{
    GLenum format     = GL_RGB;
    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
    GLenum wrap_s     = GL_CLAMP_TO_EDGE;
    GLenum wrap_t     = GL_CLAMP_TO_EDGE;
    auto   gl3        = (gl3_impl*)gk->impl_data;
    auto&  config     = gl3->state;

    glGenFramebuffers(1, &cmd->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, cmd->framebuffer);

    if(cmd->rt_flags & GK_RT_ALPHA) format = GL_RGBA;
    if(cmd->tex_flags & GK_TEX_REPEATX) wrap_s = GL_REPEAT;
    if(cmd->tex_flags & GK_TEX_REPEATY) wrap_t = GL_REPEAT;

    glGenTextures(1, &cmd->tex);
    config.tex.set(cmd->tex, config);
    config.tex.apply(gl3->glstate);

    glTexImage2D(GL_TEXTURE_2D, 0, format, cmd->size.x, cmd->size.y, 0, format, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gk_filter_to_gl[cmd->tex_min_filter]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gk_filter_to_gl[cmd->tex_mag_filter]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cmd->tex, 0);
    glDrawBuffers(1, buffers);

    if((cmd->rt_flags & GK_RT_DEPTH) && (cmd->rt_flags & GK_RT_STENCIL)) {
        glGenRenderbuffers(1, &cmd->dsbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, cmd->dsbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, cmd->size.x, cmd->size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cmd->dsbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, cmd->dsbuffer);
    } else if(cmd->rt_flags & GK_RT_STENCIL) {
        glGenRenderbuffers(1, &cmd->dsbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, cmd->dsbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, cmd->size.x, cmd->size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, cmd->dsbuffer);
    } else if(cmd->rt_flags & GK_RT_DEPTH) {
        glGenRenderbuffers(1, &cmd->dsbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, cmd->dsbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cmd->size.x, cmd->size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cmd->dsbuffer);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        say("GK: unexpected incomplete framebuffer");
        goto gl_error;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return;

gl_error:
    if(cmd->tex) glDeleteTextures(1, &cmd->tex);
    if(cmd->dsbuffer) glDeleteRenderbuffers(1, &cmd->dsbuffer);
    if(cmd->framebuffer) glDeleteFramebuffers(1, &cmd->framebuffer);
    return;
}

void gl3_cmd_rt_destroy(gk_context*, gk_cmd_rt_destroy* cmd)
{
    if(cmd->tex) glDeleteTextures(1, &cmd->tex);
    if(cmd->dsbuffer) glDeleteRenderbuffers(1, &cmd->dsbuffer);
    if(cmd->framebuffer) glDeleteFramebuffers(1, &cmd->framebuffer);
}

void gl3_cmd_rt_bind(gk_context*, gk_cmd_rt_bind* cmd)
{
    auto v = cmd->viewport;

    glBindFramebuffer(GL_FRAMEBUFFER, cmd->framebuffer);
    glViewport(v.pos.x, v.pos.y, v.size.x, v.size.y);
}

void gl3_cmd_rt_unbind(gk_context*, gk_cmd_rt_unbind* cmd)
{
    auto v = cmd->viewport;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(v.pos.x, v.pos.y, v.size.x, v.size.y);
}
