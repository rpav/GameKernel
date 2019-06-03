#pragma once

#if 0
#define GL_CHECK(x) do { x; if(gk_gl_checkerror(#x,__FILE__,__LINE__)) goto gl_error; } while(0)
#define GL_CHECKERR(x) do { if(gk_gl_checkerror(#x,__FILE__,__LINE__)) goto gl_error; } while(0)
#else
#define GL_CHECK(x) (x)
#define GL_CHECKERR(x)
#endif

bool gk_gl_checkerror(const char *expr, const char *file, int line);

GLuint gk_gl_compile_shader(GLenum type, const char *text);
GLuint gk_gl_link_program(int numshaders, GLuint *shaders);
