/*
    zlib License

    Copyright (C) 2017 João Lucas Nunes e Silva

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/


// NoLib:
// * SDL Utils
//
// * Math
// * Util
//
// * Graphics
// * Joystick
// * Loader
// * Runtime
//
// * Initialization and Finalization
#ifndef _NOLIB_H_
#define _NOLIB_H_


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ========================================================
//
// SDL
//
// ========================================================


#ifndef nG_RENDERER_FLAGS
    #define nG_RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#endif // !nG_RENDERER_FLAGS

#ifndef nG_WINDOW_FLAGS
    #define nG_WINDOW_FLAGS SDL_WINDOW_SHOWN
#endif // !n_G_WINDOW_FLAGS


#define SDL_Rect(...)  ((SDL_Rect)  {.x = 0, .y = 0, .w = 0, .h = 0, __VA_ARGS__})
#define SDL_Color(...) ((SDL_Color) {.r = 0, .g = 0, .b = 0, .a = 0xFF, __VA_ARGS__})


// ========================================================
//
// MATH
//
// ========================================================


typedef struct {
    float x, y;
} n_Vec2;

typedef struct {
    float x, y, w, h;
} n_Rect;


#define n_Vec2(...) ((n_Vec2) {.x = 0.0f, .y = 0.0f, __VA_ARGS__})

#define n_Rect(...) ((n_Rect) {.x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f, __VA_ARGS__})


bool n_IsValidRect(const n_Rect *restrict r);
bool n_RectsOverlap(n_Rect *restrict a, n_Rect *restrict b);


// ========================================================
//
// UTIL
//
// ========================================================


#ifndef nG_LOG_BUFFER
    #define nG_LOG_BUFFER stderr
#endif // !nG_LOG_BUFFER


#define n_Delete(obj) { \
    free(obj);          \
    obj = NULL;         \
}

#define n_Logf(...) fprintf(nG_LOG_BUFFER, __VA_ARGS__)

#define n_New(T, n) ((T *) calloc(n, sizeof(T)))


#define Short(x)  ((short)    (x))
#define Int(x)    ((int)      (x))
#define Long(x)   ((long)     (x))
#define Int8(x)   ((int8_t)   (x))
#define Int16(x)  ((int16_t)  (x))
#define Int32(x)  ((int32_t)  (x))
#define Int64(x)  ((int64_t)  (x))
#define UInt8(x)  ((uint8_t)  (x))
#define UInt16(x) ((uint16_t) (x))
#define UInt32(x) ((uint32_t) (x))
#define UInt64(x) ((uint64_t) (x))

#define Float(x)  ((float)    (x))
#define Double(x) ((double)   (x))

#define Ptr(x)    ((void *)   (x))


// ========================================================
//
// GRAPHICS
//
// ========================================================


#ifndef nG_IMG_FLAGS
    #define nG_IMG_FLAGS IMG_INIT_PNG
#endif // !nG_IMG_FLAGS


typedef struct {
    SDL_Texture*     tex;
    SDL_Rect*        frames;
    n_Rect           dest;
    uint32_t         size;
    uint32_t         index;
    float            frameDuration;
    float            totalTime;
    float            angle;
    float            angleInc;
    SDL_RendererFlip flip;
} n_Animation;

typedef struct {
    n_Vec2 center;
    n_Vec2 acceleration;
    n_Vec2 velocity;
    float  x;
    float  y;
    float  zoom;
} n_Camera;

typedef struct {
    SDL_Texture*     tex;
    SDL_Rect         src;
    n_Rect           dest;
    float            angle;
    SDL_RendererFlip flip;
} n_Sprite;


#define n_Animation(...) ((n_Animation) {    \
    .tex           = NULL,                   \
    .frames        = NULL,                   \
    .dest          = n_Rect(.w = 1, .h = 1), \
    .size          = 0,                      \
    .index         = 0,                      \
    .frameDuration = 0.0f,                   \
    .totalTime     = 0.0f,                   \
    .angle         = 0.0f,                   \
    .angleInc      = 0.0f,                   \
    .flip          = SDL_FLIP_NONE,          \
    __VA_ARGS__                              \
})

#define n_Sprite(...) ((n_Sprite) { \
    .tex   = NULL,                  \
    .src   = SDL_Rect(),            \
    .dest  = n_Rect(),              \
    .angle = 0.0f,                  \
    .flip  = SDL_FLIP_NONE,         \
    __VA_ARGS__                     \
})

#define n_Camera(...) ((n_Camera) { \
    .center       = n_Vec2(),       \
    .acceleration = n_Vec2(),       \
    .velocity     = n_Vec2(),       \
    .x            = 0.0f,           \
    .y            = 0.0f,           \
    .zoom         = 1.0f,           \
    __VA_ARGS__                     \
})


void n_Animate(n_Animation *restrict a, float totalTime);


// TODO
// Makes the camera follow a certain point. The Camera position
// is so that <point> is at the center of the screen.
// void n_CameraFollow(n_Camera *restrict cam, n_Vec2 point);

// TODO
// Moves the camera so that <point> is within the camera's viewport
// bounds.
// void n_CameraJail(n_Camera *restrict cam, const n_Rect *restrict cage, n_Vec2 point);


void n_ClearBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


void n_DeleteAnimation(n_Animation** a);


void n_DrawAnimation(const n_Camera *restrict cam, const n_Animation *restrict a);

void n_DrawFilledRect(const n_Camera *restrict cam, const n_Rect *restrict rect);

void n_DrawRect(const n_Camera *restrict cam, const n_Rect *restrict rect);

void n_DrawSprite(const n_Camera *restrict cam, const n_Sprite *restrict sprite);

void n_DrawTexture(
    const n_Camera *restrict cam,
    SDL_Texture* tex,
    const SDL_Rect *restrict src,
    const n_Rect *restrict  dest,
    float angle,
    SDL_RendererFlip flip
);

n_Animation* n_NewAnimation(
    SDL_Texture* tex,
    SDL_Rect*    frames,
    uint32_t     nOfFrames,
    float        frameDuration
);

// Renders the scenes.
void n_Present(void);


void n_SetRendererDrawColor(SDL_Color color);


SDL_Rect n_Unproject(const n_Camera *restrict cam, const n_Rect *restrict r);


// ========================================================
//
// JOYSTICK
//
// ========================================================


typedef enum {
    // AXIS -----------------------------

    n_DualShock_LeftX  = 0,
    n_DualShock_LeftY  = 1,
    n_DualShock_RightX = 2,
    n_DualShock_RightY = 3,

    // Buttons --------------------------
    
    n_DualShock_L1       = 4,
    n_DualShock_L2       = 6,
    n_DualShock_L3       = 10,
    n_DualShock_R1       = 5,
    n_DualShock_R2       = 7,
    n_DualShock_R3       = 11,
    n_DualShock_Triangle = 0,
    n_DualShock_X        = 2,
    n_DualShock_Square   = 3,
    n_DualShock_O        = 1,
    n_DualShock_Start    = 9,
    n_DualShock_Select   = 8,

    // Hat ------------------------------
    
    n_DualShock_Centered  = SDL_HAT_CENTERED,
    n_DualShock_Up        = SDL_HAT_UP,
    n_DualShock_Down      = SDL_HAT_DOWN,
    n_DualShock_Left      = SDL_HAT_LEFT,
    n_DualShock_LeftUp    = SDL_HAT_LEFTUP,
    n_DualShock_LeftDown  = SDL_HAT_LEFTDOWN,
    n_DualShock_Right     = SDL_HAT_RIGHT,
    n_DualShock_RightUp   = SDL_HAT_RIGHTUP,
    n_DualShock_RightDown = SDL_HAT_RIGHTDOWN
} n_DualShock;


// ========================================================
//
// LOADER
//
// ========================================================


static inline void n_DeleteTexture(SDL_Texture** tex)
{
    if (tex && *tex) {
        SDL_DestroyTexture(*tex);
        *tex = NULL;
    }
}


SDL_Texture* n_LoadTexture(const char *restrict name);

bool n_SetLoaderSearchPath(const char *restrict path);


// ========================================================
//
// RUNTIME
//
// ========================================================


typedef struct n_IGame n_IGame;

typedef struct {
    float deltaTime;
    float totalTime;
} n_GameTime;

typedef void (* n_GameEventHandler)(n_IGame *restrict self, const SDL_Event *restrict e);
typedef void (* n_GameRuntimeFn)(n_IGame *restrict self, n_GameTime gameTime);

struct n_IGame {
    n_GameRuntimeFn    init;
    n_GameRuntimeFn    step;
    n_GameRuntimeFn    finalize;
    n_GameEventHandler ehandler;
};


void n_Quit(void);

void n_Run(uint32_t fps, n_IGame *restrict game);

void n_SetBackgroundColor(const SDL_Color *restrict color);


// ========================================================
//
// INITIALIZATION AND FINALIZATION
//
// ========================================================


bool n_Init(const char* gameName, int windowWidth, int windowHeight, float ppm);
void n_Finalize(void);


#endif // !_NOLIB_H_




// ========================================================
// ****                                                ****
// ****                                                ****
// ****               IMPLEMENTATION                   ****
// ****                                                ****
// ****                                                ****
// ========================================================




#ifdef _NOLIB_INCLUDE_IMPL_


// ========================================================
//
// SDL
//
// ========================================================


static SDL_Renderer* nG_Renderer;

static SDL_Window* nG_Window;


// ========================================================
//
// MATH
//
// ========================================================


bool n_IsValidRect(const n_Rect *restrict r)
{
    return !r ? false : (r->w > 0.0f && r->h > 0.0f);
}

bool n_RectsOverlap(n_Rect *restrict a, n_Rect *restrict b)
{
    if (a && b) {
        return a->x < (b->x + b->w)
            && b->x < (a->x + a->w)
            && a->y < (b->y + b->h)
            && b->y < (a->y + a->h);
    }
    return false;
}


// ========================================================
//
// GRAPHICS
//
// ========================================================


static float nG_PPM;


void n_Animate(n_Animation *restrict a, float totalTime)
{
    if (!a) {
        return;
    }

    float frameTime = a->frameDuration;
    int   animSize  = a->size;
    float duration  = animSize * frameTime;
    float dt        = totalTime - a->totalTime;

    if (dt <= duration) {
        a->index  = UInt16(totalTime / frameTime) % animSize;
        a->angle += a->angleInc;
    } else {
        // if a long time has passed without animating, restart
        // the animation.
        a->index = 0;
    }

    a->totalTime = totalTime;
}

void n_CenterCamera(n_Camera *restrict cam, n_Vec2 center)
{
    if (!cam) {
        return;
    }
}

void n_ClearBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor(nG_Renderer, r, g, b, a);
    SDL_RenderClear(nG_Renderer);
}

void n_DeleteAnimation(n_Animation** a)
{
    if (a && *a) {
        n_Delete((*a)->frames);
        n_Delete(*a);
    }
}

void n_DrawAnimation(const n_Camera *restrict cam, const n_Animation *restrict a) {
    if (!cam || !a) {
        return;
    }

    uint32_t i = a->index;

    n_DrawTexture(cam, a->tex, &a->frames[i], &a->dest, a->angle, a->flip);
}

void n_DrawFilledRect(const n_Camera *restrict cam, const n_Rect *restrict rect)
{
    if (cam && n_IsValidRect(rect)) {
        SDL_Rect r = n_Unproject(cam, rect);
        SDL_RenderFillRect(nG_Renderer, &r);
    }
}

void n_DrawRect(const n_Camera *restrict cam, const n_Rect *restrict rect)
{
    if (cam && n_IsValidRect(rect)) {
        SDL_Rect r = n_Unproject(cam, rect);
        SDL_RenderDrawRect(nG_Renderer, &r);
    }
}

void n_DrawSprite(const n_Camera *restrict cam, const n_Sprite *restrict sprite)
{
    if (cam && sprite) {
        n_DrawTexture(
            cam,
            sprite->tex,
            &sprite->src,
            &sprite->dest,
            sprite->angle,
            sprite->flip
        );
    }
}

void n_DrawTexture(
    const n_Camera *restrict cam,
    SDL_Texture* tex,
    const SDL_Rect *restrict src,
    const n_Rect *restrict dest,
    float angle,
    SDL_RendererFlip flip
) {
    if (!cam || !tex) {
        return;
    }

    SDL_Rect d = n_Unproject(cam, dest);

    SDL_RenderCopyEx(
        nG_Renderer,
        tex,
        src,
        dest ? &d : NULL,
        angle,
        NULL,
        flip
    );
}

n_Animation* n_NewAnimation(
    SDL_Texture* tex,
    SDL_Rect*    frames,
    uint32_t     nOfFrames,
    float        frameDuration
) {
    if (!tex || !frames || nOfFrames < 2) {
        // TODO error message
        return NULL;
    }

    SDL_Rect* fs = n_New(SDL_Rect, nOfFrames);

    if (!fs) {
        // TODO error message
        return NULL;
    }

    n_Animation* a = n_New(n_Animation, 1);

    if (!a) {
        // TODO error message
        n_Delete(fs);
        return NULL;
    }

    memcpy(fs, frames, nOfFrames * sizeof(SDL_Rect));

    *a = n_Animation(
        .tex           = tex,
        .frames        = fs,
        .dest          = n_Rect(.x = 1.0f, .y = 1.0f, .w = 1.0f, .h = 1.0f),
        .size          = nOfFrames,
        .index         = 0,
        .frameDuration = frameDuration,
        .totalTime     = 0,
        .angle         = 0,
        .angleInc      = 0,
        .flip          = SDL_FLIP_NONE
    );

    return a;
}

void n_Present(void)
{
    SDL_RenderPresent(nG_Renderer);
}

void n_SetRendererDrawColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(nG_Renderer, color.r, color.g, color.b, color.a);
}

SDL_Rect n_Unproject(const n_Camera *restrict cam, const n_Rect *restrict r)
{
    SDL_Rect out = SDL_Rect();

    if (cam && n_IsValidRect(r)) {
        float ppm = nG_PPM;
        float cx  = cam->x;
        float cy  = cam->y;
        float z   = cam->zoom;
        int   h   = Int(z * r->h * ppm);
        int   maxH;

        SDL_GetWindowSize(nG_Window, NULL, &maxH);

        out.x =        Int(z * ppm * (r->x + cx));
        out.y = maxH - Int(z * ppm * (r->y + cy)) - h;
        out.w = Int(cam->zoom * r->w * ppm);
        out.h = h;
    }

    return out;
}


// ========================================================
//
// LOADER
//
// ========================================================


#define nG_BaseLoaderPathMaxLen 255


static char nG_BaseLoaderPath[nG_BaseLoaderPathMaxLen + 1] = "";


SDL_Texture* n_LoadTexture(const char *restrict path)
{
    if (strlen(path) > nG_BaseLoaderPathMaxLen) {
        n_Logf("path is too long (> %d).", nG_BaseLoaderPathMaxLen - 1);
    }

    char filepath[2 * nG_BaseLoaderPathMaxLen + 1];
    
    snprintf(filepath, 2 * nG_BaseLoaderPathMaxLen, "%s%s", nG_BaseLoaderPath, path);
    
    SDL_Surface* s = IMG_Load(filepath);
    SDL_Texture* t = NULL;

    
    if (s) {
        t = SDL_CreateTextureFromSurface(nG_Renderer, s);
        SDL_FreeSurface(s);
    }

    if (!t) {
        n_Logf("Unable to load texture '%s': %s\n", filepath, SDL_GetError());
    }

    return t;
}

bool n_SetLoaderSearchPath(const char *restrict path)
{
    long len = strlen(path);
 
    if (!path) {
        n_Logf("NULL path.");
        return false;
    }

    if (len >= nG_BaseLoaderPathMaxLen) {
        n_Logf("path is too long (> %d).", nG_BaseLoaderPathMaxLen);
        return false;
    }

    if (nG_BaseLoaderPath[len - 1] != '/') {
        snprintf(
            nG_BaseLoaderPath,
            nG_BaseLoaderPathMaxLen - 1,
            "%s/",
            path
        );
    } else {
        strncpy(nG_BaseLoaderPath, path, nG_BaseLoaderPathMaxLen);
    }

    return true;
}


// ========================================================
//
// RUNTIME
//
// ========================================================


static bool n_ShouldQuit = false;

static SDL_Color n_DefaultBGColor = SDL_Color(
    .r = 0x00,
    .g = 0x00,
    .b = 0x00,
    .a = 0xFF,
);


void n_Quit(void)
{
    n_ShouldQuit = true;
}

void n_Run(uint32_t fps, n_IGame *restrict game)
{
    const int FRAME_TIME = Int(1000.0 / fps);
    n_GameTime gt = {0.0f, 0.0f};
    uint32_t  curr  = 0;
    uint32_t  prev  = 0;
    uint32_t  delta = 0;
    SDL_Event e;

    game->init(game, gt);

    while (!n_ShouldQuit) {
        curr  = SDL_GetTicks();
        delta = curr - prev;

        if (delta >=  FRAME_TIME) {
            n_ClearBackground(
                n_DefaultBGColor.r,
                n_DefaultBGColor.g,
                n_DefaultBGColor.b,
                n_DefaultBGColor.a
            );

            while (!n_ShouldQuit && SDL_PollEvent(&e) > 0) {
                switch(e.type)
                {
                case SDL_QUIT:
                    n_ShouldQuit = true;
                    break;
                default:
                    game->ehandler(game, &e);
                    break;
                }
            }

            gt.deltaTime = Float(delta) / 1000.0f;
            gt.totalTime = Float(curr) / 1000.0f;
            prev = curr;
            game->step(game, gt);

            n_Present();
        }
    }

    game->finalize(game, gt);
}

void n_SetBackgroundColor(const SDL_Color *restrict color)
{
    n_DefaultBGColor = SDL_Color(
        .r = color->r,
        .g = color->g,
        .b = color->b,
        .a = color->a
    );
}


// ========================================================
//
// INITIALIZATION AND FINALIZATION
//
// ========================================================


bool n_Init(const char* gameName, int windowWidth, int windowHeight, float ppm)
{
    nG_PPM = ppm;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        n_Logf("Error while initializing SDL: %s\n", SDL_GetError());
        n_Finalize();
        return false;
    }

    if ((IMG_Init(nG_IMG_FLAGS) & nG_IMG_FLAGS) != nG_IMG_FLAGS) {
        n_Logf("Error while initializing SDL_image: %s\n", IMG_GetError());
        n_Finalize();
        return false;
    }

    if (TTF_Init() < 0) {
        n_Logf("Error while initializing SDL_ttf: %s\n", TTF_GetError());
        n_Finalize();
        return false;
    }

    nG_Window = SDL_CreateWindow(
        gameName,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        nG_WINDOW_FLAGS
    );
    if (!nG_Window) {
        n_Logf("Error while creating the window: %s\n", SDL_GetError());
        n_Finalize();
        return false;
    }

    nG_Renderer = SDL_CreateRenderer(nG_Window, -1, nG_RENDERER_FLAGS);
    if (!nG_Renderer) {
        n_Logf("Error while creating the renderer: %s\n", SDL_GetError());
        n_Finalize();
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    return true;
}

void n_Finalize(void)
{
    SDL_DestroyRenderer(nG_Renderer);
    SDL_DestroyWindow(nG_Window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


#endif // !_NOLIB_INCLUDE_IMPL_
