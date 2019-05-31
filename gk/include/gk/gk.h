/******************************************************************
 * GameKernel
 *
 * This is the only header you need to include.  Note that while some
 * dependencies are optional, various support structs and constants
 * are not optionalized in this header.  Simply don't use what you
 * don't want; it will otherwise not exist at runtime.
 ******************************************************************/

#ifndef __GAMEKERNEL_GK_H__
#define __GAMEKERNEL_GK_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(WIN32)
#    ifdef GK_EXPORT
#        define GK_API __declspec(dllexport)
#    elif defined(GK_IMPORT)
#        define GK_API __declspec(dllimport)
#    else
#        define GK_API
#    endif
#else
#    define GK_API
#endif

#ifndef __cplusplus
#    include "gk/core.h"
#else
#    include "gk/core.hpp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
#include "gk/constants.h"
#include "gk/base.h"
#include "gk/transform.h"
#include "gk/gl.h"
#include "gk/path.h"
#include "gk/nanovg.h"
#include "gk/box2d.h"
#include "gk/spritesheet.h"
//clang-format on

/******************************************************************
 * API
 ******************************************************************/
GK_API gk_context* gk_create(gk_impl impl);
GK_API void        gk_destroy(gk_context* gk);
GK_API void        gk_process(gk_context* gk, gk_bundle* bundle);
GK_API void        gk_process_one(gk_context* gk, gk_subsystem sys, gk_cmd* cmd, gk_error* error);

#ifdef __cplusplus
}
#endif

#endif /* __GAMEKERNEL_CAPI_H__ */
