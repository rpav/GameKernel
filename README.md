# GameKernel

GameKernel is a command-driven "kernel" of game functionality.  Its
goal is to be a fast, portable, *language-friendly* group of
pure C API functionality:

* Minimal FFI
* Data/command-driven structure
* Bridge to useful C++ libs

One can render an entire scene with as few as *1* C call per frame.
Right now, this includes the following:

* NanoVG for drawing, text, etc
* Sprite/quad buffering
* Transformation/matrices
* Spritesheets, TexturePacker json parsing
* A nicer C++ API called `gk++` that wraps the C API

This is *not* an engine---there is no scene handling, animation,
render loop.  It will not even create windows and context; use SDL2 or
similar.  Rather, if you are *writing* an engine, or you want to write
a game with a great deal of control, you would want GameKernel as your
core.

In the works, no particular order:

* Box2D integration
* More complete NanoVG support
* RenderTexture
* 3D
* More language wrappers

# Work In Progress

This is, quite obviously, a work-in-progress.  It started as a
week-long project in preparation for the [Spring 2016 Lisp Game
Jam](https://itch.io/jam/spring-2016-lisp-game-jam), and has continued
from there.

Some things are likely to change wildly---data structure layouts and
constants, for instance.  However, the actual names are relatively
stable, though this may not be the case always.

# Example Code

Currently there are a few examples in the `examples/` directory.  The
original examples 1–3 are more-or-less plain C with a bit of C++.  This
isn't very friendly.

However, with the new `gk++` API, this is changing.  An updated
version of `example1` called `example1a` shows the differences.  New
examples will be created this way, because they're easier to read and
write.

# Building

GLM and GLEW are required to be installed.

0. Make sure to `git submodule update --init`.  This depends on numerous submodules.
0. Create a subdirectory for building.
0. Run `cmake -DCMAKE_BUILD_TYPE="RelWithDebInfo" ..` (or "Release").  Note GLM performs *significantly* faster with at least some optimization.  Building "Debug" will be very slow.
0. Run `make; make install`.

```console
vcs $ git clone ... gamekernel
vcs $ cd gamekernel
gamekernel $ git submodule update --init
gamekernel $ mkdir build; cd build
build $ cmake -DCMAKE_BUILD_TYPE="RelWithDebInfo"
build $ make ; make install
```

Examples are also built and should be in `build/examples`.

# Basic Design

## API

Part of the idea is to minimize function calls, since FFI from other
languages tends to be expensive, and merely setting data structures
hypothetically less so.  Thus, there are a total of *three* functions:

* `gk_context* gk_create(gk_impl)`: Return a GK context. Currently only GK_GL3 is implemented.  This requires an OpenGL 3.3 Core profile.
* `void gk_process(gk_context *gk, gk_bundle *bundle)`: This is the function you call to get things done.  This takes the context, and a bundle.  On return, the bundle contains any relevant error code.
* `void gk_destroy(gk_context*)`: This cleans up the context.

There is a single header:

* `gk/gk.h`:  This contains all definitions.

## Bundles, Passes, Lists, Commands

Conceptually, you should think of processing a series of *passes*.
For instance, you might have a pass that renders sprites, and a pass
that renders UI, and maybe a pass that composites them (once
render-to-texture is supported!).  How passes work is entirely
determined by you.  This affords a great deal of flexibility in game
rendering.

Each of these passes processes a *list* of *commands*.  The `gk_list`
struct holds 3 things:

* The `gk_subsystem`: currently GK_SUB_CONFIG, GK_SUB_GL, GK_SUB_NVG; this
  is which subsystem processes the list.  Some subsystems may process
  commands differently.
* The number of commands, and an array of pointers to those commands.

It is assumed that commands may be statically held and reused.  It is
generally safe to have a command on multiple lists, and call it
multiple times in a row.  It is generally safe to process the same
list multiple times in a row.  It is *not* generally safe to process
the same command (or list) with two different contexts, or from two
different threads with the same context.  Exceptions will be
explicitly noted in either case.

A *pass* is a specific kind of command, which references a *list
index*, and whether the list should be pre-sorted.  Note that sorting
modifies the list in-place.

A *bundle* holds an array of the lists you wish processed in a single
call, as well as a "starting pass" which determines the first list to
be processed.  The "list index" of a pass refers to this array.
Because a pass is just another command, to process multiple passes in
a single call, you would create a list of pass commands, and process
this as the first pass.

*Commands* are the fundamental unit of "getting something done".
These may be anything from loading or freeing a sprite sheet to
actually rendering something on the screen.  Any command has either a
`gk_cmd` or another command with `gk_cmd` as its first member, and may
be safely cast (or treated as) a `gk_cmd`.  `gk_cmd` itself has a few
basic fields:

* `gk_cmd_type type`: This specifies what the command is
* `int key`: This is an arbitrary integer key.  If a pass specifies
  sorting, this is the sort key.

For a reference of all the commands, see `gk/gk.h`, and the examples
for demonstration.

# License

GameKernel
Copyright (c) 2016 Ryan Pavlik

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
