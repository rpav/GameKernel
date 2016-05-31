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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "gk/coreconst.h"
#include "gk/core.h"
#include "gk/math.h"
#include "gk/transform.h"
#include "gk/gl.h"
#include "gk/path.h"
#include "gk/nanovg.h"
#include "gk/box2d.h"
#include "gk/spritesheet.h"

/******************************************************************
 * API
 ******************************************************************/
gk_context* gk_create(gk_impl impl);
void gk_destroy(gk_context *gk);
void gk_process(gk_context *gk, gk_bundle *bundle);

#ifdef __cplusplus
}
#endif

#endif /* __GAMEKERNEL_CAPI_H__ */
