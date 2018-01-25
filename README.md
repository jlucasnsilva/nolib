# No Lib

> What kind of lib throws everything in the header file?
> Nolib. Nolib at all.
>
> -- Gus Fring

# Usage

In this section the organization of the library will be presented and hopefuly
you'll be able to use it. The functions' parameters won't be shown here, since
you can just take look at the code (there aren't many LOC and use the power of
Ctrl+f).

## Initialization and Finalization

The function `n_Init()` must be called as the first statement in your main function.
It'll initialize SDL, SDL\_image, and SDL\_ttf and create the window and the renderer.
By default the `SDL_HINT_RENDER_SCALE_QUALITY` is set to nearest pixel sampling.
If initialization succeeds `true` will be returned -- and `false` otherwise. This is also
the function where the `ppm` (pixels per meter) is set.

Your last statement (before the return) should be n_Finalize(), which will call
{SDL,TTF,IMG}_Quit() and destroy the window and the renderers.

## Common functions

The following function will used once in while:

* `n_SetRendererDrawColor()`: set the color for drawing rects;
* `n_New()`: that's acctually a macro, it use instead of `malloc()` (and `n_Delete()` instead of `free()`);

## Types

The library has few data types you should care about:

* `n_Vec2`
* `n_Rect`: unlike SDL_Rect, this one has `float` fields (x, y, w, h); 
* `n_IGame`: an interface that will be used to initialized, finalize, handle events and call the step function;
* `n_GameTime`: delta time and total time as `float`s;
* `n_Camera`: all the graphics (rects, textures, sprites and animations) para rendered relative to a camera;
* `n_Animation`: sprite animation;
* `n_Sprite`: a SDL_Texture section;

### n_IGame and runtime

The `n_IGame` is a struct that you must the values (function pointers):

* `init`: called by the runtime (`n_Run()`) right before the game loop;
* `finalize`: called after the game loop has stopped;
* `step`: called inside the game loop. This place where you'll place your game logic;
* `ehandler`: called during the game loop when there is a SDL_Event in the queue;

The following functions are related to the game execution:

* `n_Run()`: executes your n_IGame. That's the place where the game loop resides;
* `n_Quit()`: forces the end of the game loop;
* `n_SetBackgroundColor()`: set the background color;

### n_Animation

These are the functions to help you create and destroy animtions:

* `n_NewAnimation()`
* `n_DeleteAnimation() // note that it won't call SDL_DestroyTexture() for you`

And these are the ones to animate and draw animations:

* `n_Animate()`
* `n_DrawAnimation()`

### n_Rect

The following functions are similar to their SDL counterparts, however
work on `n_Rect` and also need a `n_Camera`:

* `n_DrawFilledRect()`
* `n_DrawRect()`

### n_Sprite

Draw sprite using:

```c
n_DrawSprite()
```

### SDL_Texture

Some functions to help you load and destroy `SDL_Texture`s:

* `n_LoadTexture()`: loads a `SDL_Texture`;
* `n_SetLoaderSearchPath()`: set the path where `n_LoadTexture()` will look for textures;
* `n_DeleteTexture()`: destroys a `SDL_Texture`;
* `n_DrawTexture()`: base function used by `n_DrawAnimation()` and `n_DrawSprite()`.

## Constructor macros

There are also a few macros to help you set the value of `struct`s:

* `SDL_Rect(...)`
* `SDL_Color(...)`
* `n_Vec2(...)`
* `n_Rect(...)`
* `n_Animation(...)`
* `n_Camera(...)`

If they are called with no values (for instance `n_Rect()`, instead of
`n_Rect(.x = 0, .y = 1, .w = 1, .h = 1)`) the struct will be initialized with 0 values
(`NULL` for pointers, `0` for numeric types and `false` to `bool`).

## Other macros

There are also macros used for initilization:

* `nG_RENDERER_FLAGS`
* `nG_WINDOW_FLAGS`
* `nG_IMG_FLAGS`
* `nG_LOG_BUFFER`
* `nG_BaseLoaderPathMaxLen`

If you want to change their default value, just `#define` before you `#include "nolib.h"`

## Joystick Mappings

To help you use the joystick API the `n_DualShock` enumeration is provided. Its values
will assist you when you call `SDL_JoystickGetAxis()`, `SDL_JoystickGetButton` and 
`SDL_JoystickGetHat`.

Currently only mappings for PS's Dual Shock is provided.

# License

The code in this repository is available under the zlib license.

> zlib License
> 
> Copyright (C) 2017 João Lucas Nunes e Silva
> 
> This software is provided 'as-is', without any express or implied
> warranty.  In no event will the authors be held liable for any damages
> arising from the use of this software.
> 
> Permission is granted to anyone to use this software for any purpose,
> including commercial applications, and to alter it and redistribute it
> freely, subject to the following restrictions:
> 
> 1. The origin of this software must not be misrepresented; you must not
>     claim that you wrote the original software. If you use this software
>     in a product, an acknowledgment in the product documentation would be
>     appreciated but is not required.
> 2. Altered source versions must be plainly marked as such, and must not be
>     misrepresented as being the original software.
> 3. This notice may not be removed or altered from any source distribution.