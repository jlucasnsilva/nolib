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
// * Allocator
// * Graphics
// * Joystick
// * Loader
// * Nine Patch
// * Physics
// * Runtime
// * Screen
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
// INITIALIZATION AND FINALIZATION
//
// ========================================================


bool n_Init(const char* gameName, int windowWidth, int windowHeight, float ppm);
void n_Finalize(void);


// ========================================================
//
// ALLOCATORS
//
// ========================================================


typedef struct n_CircularAllocatorNode n_CircularAllocatorNode;

struct n_CircularAllocatorNode {
    n_CircularAllocatorNode* next;
};

typedef struct {
    n_CircularAllocatorNode* firstFree;
    n_CircularAllocatorNode* lastFree;
    uint64_t*                buffer;
    size_t                   bufferSize;
    size_t                   objSize;
    bool                     isStatic;
} n_CircularAllocator;

// - Stack allocator ----------------------------------------------------------
//
// Memory layout:
//
// 15 |  .  |
// 14 |  .  |
// 13 |  .  |
// 12 |_____|
// 11 |__6__| <- index of block 1 first position
// 10 |     | <- end of block 1
//  9 |     |
//  8 |     |
//  7 |     |
//  6 |_____|
//  5 |__1__| <- beggining of block 1
//  4 |     | <- index of block 0's first position
//  3 |     | <- end of block 0
//  2 |_____|
//  1 |__0__| <- beggining of block 0
//  0 |     | <- dead byte
//       #
//
// ----------------------------------------------------------------------------
typedef struct {
    uint64_t* buffer;     // a 8-bytes-aligned chunck of memory
    size_t    bufferSize; // the length of the buffer
    size_t    index;      // current position at the buffer
    bool      isStatic;   // is it a global static array?
} n_StackAllocator;


// Initializes a new Circular Allocator.
//
// \param a a existing n_CircularAllocator.
// \param buffer an array to be used as a back buffer.
// \param bufferSize the size of the buffer in bytes.
// \param objSize size of the object to be stored.
// \param isStatic if 'false', when the n_CircularAllocator is unmade
//                 or deleted the back buffer will be freed.
bool n_MakeCircularAllocator(
    n_CircularAllocator *restrict a,
    uint64_t*                     buffer,
    size_t                        bufferSize,
    size_t                        objSize,
    bool                          isStatic
);

void n_UnmakeCircularAllocator(n_CircularAllocator *restrict a);

n_CircularAllocator* n_NewCircularAllocator(
    uint64_t* buffer,
    size_t    bufferSize,
    size_t    objSize,
    bool      isStatic
);

void n_DeleteCircularAllocator(n_CircularAllocator **restrict a);

void* n_CircularAllocatorAlloc(n_CircularAllocator *restrict a);

bool n_CircularAllocatorFree(n_CircularAllocator *restrict a, void *restrict ptr);


bool n_MakeStackAllocator(
    n_StackAllocator *restrict a,
    uint64_t*                  buffer,
    size_t                     bufferLength,
    bool                       isStatic
);

void n_UnmakeStackAllocator(n_StackAllocator *restrict a);

// Allocates a stack allocator handle.
//
// \param buffer a 8-byte-aligned chunk of memory (global static or
//               malloced) that will be used as memory space for this
//               stack memory handle;
// \param bufferLength the length of the buffer/array passed in (not
//                     in bytes, but in '8-bytes');
// \param isStatic tells if the buffer is a global static array or
//                 (false) it was malloced. This flag is later used
//                 by 'n_StackAllocator_delete': if it is false, it
//                 will free;
n_StackAllocator* n_NewStackAllocator(
    uint64_t* buffer,
    size_t    bufferLength,
    bool      isStatic
);

// Frees the memory allocated by 'n_StackAllocator_new'. If 'is_static'
// was set to false, stdlib/free will be called on the buffer.
//
// \param a a stack allocator handle;
void n_DeleteStackAllocator(n_StackAllocator **restrict a);

// Allocates 'size' bytes and returns a pointer to the first. Internally
// it rounds 'size' to the next 8-byte block (for instance, for 13 bytes 
// internally it'll be allocated 18 bytes, 21 will be 24 and so on) plus
// 8 bytes for stack's internals.
//
// \param a a stack allocator handle;
// \param size the size in bytes of the chuck to be allocated;
void* n_StackAllocatorAlloc(n_StackAllocator *restrict a, size_t size);

// Frees the last allocated block of memory.
//
// \param a a stack allocator handle;
void n_StackAllocatorFree(n_StackAllocator *restrict a);


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
void n_CameraFollow(n_Camera *restrict cam, n_Vec2 point);

// TODO
// Moves the camera so that <point> is within the camera's viewport
// bounds.
void n_CameraJail(n_Camera *restrict cam, const n_Rect *restrict cage, n_Vec2 point);


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
// PHYSICS
//
// ========================================================


#ifndef nG_DEFAULT_VELOCITY_DEAD_ZONE
    #define nG_DEFAULT_VELOCITY_DEAD_ZONE 0.2f
#endif

#ifndef nG_DEFAULT_VELOCITY_DAMPING
    #define nG_DEFAULT_VELOCITY_DAMPING 0.9f
#endif

typedef struct {
    // The category which this body belongs to.
    uint16_t mask;
    // Categories which this body can collide with.
    uint16_t category;
} n_CollisionFilter;

typedef enum {
    n_BodyType_Sensor  = 1,
    n_BodyType_Dynamic = 1 << 1,
    n_BodyType_Kinetic = 1 << 2,
    n_BodyType_Static  = 1 << 3
} n_BodyType;

typedef enum {
    n_SensorActivity_None   = 0,
    n_SensorActivity_Above  = 1,
    n_SensorActivity_Bellow = 1 << 1,
    n_SensorActivity_Left   = 1 << 2,
    n_SensorActivity_Right  = 1 << 3
} n_SensorActivity;

typedef struct {
    n_Rect            hitbox;
    n_Rect            sensor;
    n_Vec2            velocity;
    n_CollisionFilter filter;
    n_BodyType        type;
    uint16_t          data;
} n_Body;

typedef struct {
    n_Body*          a;
    n_Body*          b;
    n_SensorActivity aSensorActivity;
} n_Collision;

typedef void (* n_CollisionHandlerFn)(n_Collision coll);


void n_Accelerate(n_Body *restrict body, n_Vec2 acceleration, float dt);

void n_DeleteBody(n_Body **restrict b);

n_Body* n_NewBody(
    n_Vec2            pos,
    n_Vec2            size,
    n_Vec2            sensorPadding,
    n_CollisionFilter filter,
    n_BodyType        type,
    uint16_t          data
);

void n_PhysicsStep(float dt);

void n_SetCollisionHandler(n_CollisionHandlerFn handler);
void n_SetCollisionResolutionHandler(n_CollisionHandlerFn handler);
void n_SetSensorCollisionHandler(n_CollisionHandlerFn handler);
void n_SetVelocityDamping(float damping);
void n_SetVelocityDeadzone(float v);

void n_DrawBodies(n_Camera *restrict cam);


// ========================================================
//
// RUNTIME
//
// ========================================================


typedef struct {
    float deltaTime;
    float totalTime;
} n_GameTime;

typedef void (* n_GameEventHandler)(const SDL_Event *restrict e);
typedef void (* n_GameRuntimeFn)(n_GameTime gameTime);

typedef struct {
    n_GameRuntimeFn    init;
    n_GameRuntimeFn    step;
    n_GameRuntimeFn    finalize;
    n_GameEventHandler ehandler;
} n_IGame;


void n_Quit(void);

void n_Run(uint32_t fps, n_IGame *restrict game);

void n_SetBackgroundColor(const SDL_Color *restrict color);


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
// ALLOCATORS
//
// ========================================================


// CIRCULAR ALLOCATOR -------------------------------------


bool n_MakeCircularAllocator(
    n_CircularAllocator *restrict a,
    uint64_t            *restrict buffer,
    size_t bufferSize,
    size_t objSize,
    bool   isStatic
) {
    if (!a || !buffer || bufferSize < objSize) {
        return false;
    }

    if (a && objSize < bufferSize) {
        n_CircularAllocatorNode* first = Ptr(buffer);
        n_CircularAllocatorNode* last  = NULL;
        char*                    buff  = Ptr(buffer);

        for (int i = 1; i < (bufferSize / objSize); i++) {
            last = Ptr(&buff[i - 1]);
            last->next = Ptr(&buff[i]);
        }

        last->next->next = NULL;

        *a = (n_CircularAllocator) {
            .buffer     = buffer,
            .bufferSize = bufferSize,
            .objSize    = objSize,
            .isStatic   = isStatic,
            .firstFree  = first,
            .lastFree   = last
        };
    }

    return true;
}

void n_UnmakeCircularAllocator(n_CircularAllocator *restrict a)
{
    if (a && !a->isStatic) {
        n_Delete(a->buffer);
    }
}

n_CircularAllocator* n_NewCircularAllocator(
    uint64_t* buffer,
    size_t    bufferSize,
    size_t    objSize,
    bool      isStatic
) {
    if (!buffer || bufferSize < objSize) {
        return NULL;
    }

    n_CircularAllocator* a = n_New(n_CircularAllocator, 1);
    n_MakeCircularAllocator(a, buffer, bufferSize, objSize, isStatic);

    return a;
}

void n_DeleteCircularAllocator(n_CircularAllocator **restrict a)
{
    if (a && *a) {
        if ( !(*a)->isStatic ) {
            n_Delete((*a)->buffer);
        }

        n_Delete(*a);
    }
}

void* n_CircularAllocatorAlloc(n_CircularAllocator *restrict a)
{
    void* ptr = NULL;

    if (a && a->firstFree) {
        ptr = a->firstFree;
        a->firstFree = a->firstFree->next;
    }

    return ptr;
}

bool n_CircularAllocatorFree(n_CircularAllocator *restrict a, void *restrict ptr)
{
    char*  cbuff   = Ptr(a->buffer);
    size_t lastIdx = a->bufferSize - a->objSize;

    if (a && ptr) {
        if (ptr >= Ptr(&cbuff[0]) && ptr <= Ptr(&cbuff[lastIdx])) {
            a->lastFree->next = ptr;
            a->lastFree       = ptr;
        } else {
            // puts("circular allocator error: trying to free chuck of memory that doesn't belong to this pool!");
            return false;
        }
    }

    return true;
}


// STACK ALLOCATOR ------------------------------------------------------------


bool n_MakeStackAllocator(
    n_StackAllocator *restrict a,
    uint64_t*                  buffer,
    size_t                     bufferLength,
    bool                       isStatic
){
    if (!a || !buffer || bufferLength < 1) {
        return false;
    }

    if (a) {
        *a = (n_StackAllocator) {
            .buffer     = buffer,
            .bufferSize = bufferLength - 1,
            .index      = 0,
            .isStatic   = isStatic,
        };
    }

    return true;
}

void n_UnmakeStackAllocator(n_StackAllocator *restrict a)
{
    if (a && !a->isStatic) {
        n_Delete(a->buffer);
    }
}

n_StackAllocator* n_NewStackAllocator(
    uint64_t* buffer,
    size_t    bufferLength,
    bool      isStatic
) {
    if (!buffer || bufferLength < 1) {
        return NULL;
    }

    n_StackAllocator* a = n_New(n_StackAllocator, 1);
    n_MakeStackAllocator(a, buffer, bufferLength, isStatic);

    return a;
}

void n_DeleteStackAllocator(n_StackAllocator **restrict a)
{
    if (a && *a) {
        if ( !(*a)->isStatic ) {
            n_Delete((*a)->buffer);
        }

        n_Delete(*a);
    }
}

static inline size_t n_RoundUp(float x)
{
    return (size_t) (x + 0.99f);
}


void* n_StackAllocatorAlloc(n_StackAllocator *restrict a, size_t size)
{
    uint64_t* block     = NULL;
    // the +1 is the spot to hold the index where the block starts
    size_t    realSize  = 1 + n_RoundUp(Float(size) / sizeof(uint64_t));

    if (a && realSize <= a->bufferSize) {
        size_t index = a->index + 1;
        
        block = &a->buffer[index];
        
        a->index            += realSize;
        a->buffer[a->index]  = index;
        a->bufferSize       -= realSize;
    }

    return Ptr(block);
}

void n_StackAllocatorFree(n_StackAllocator *restrict a)
{
    if (a && a->buffer[a->index] > 0) {
        size_t s       = a->index - a->buffer[a->index] + 1;
        a->index       = a->buffer[a->index] - 1;
        a->bufferSize += s;
    }
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


static char nG_BaseLoaderPath[nG_BaseLoaderPathMaxLen + 1];


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
// PHYSICS
//
// ========================================================


#define nG_MEM_BUFFER_LEN 4096


static void n_PassCollissionHandler(n_Collision coll);
static void n_PassResolutionHandler(n_Collision coll);


typedef struct n_BodyNode n_BodyNode;

struct n_BodyNode {
    n_BodyNode* next;
    n_BodyNode* prev;
    n_Body      body;
};


static float nG_MotionDamping    = nG_DEFAULT_VELOCITY_DAMPING;
static float nG_VelocityDeadZone = nG_DEFAULT_VELOCITY_DEAD_ZONE;

static uint64_t            nG_MemBuffer[nG_MEM_BUFFER_LEN]; // 32kBytes, or 512 bodies
static n_CircularAllocator nG_PhyAllocator;

static n_CollisionHandlerFn nG_CollisionHandler       = &n_PassCollissionHandler;
static n_CollisionHandlerFn nG_SensorCollisionHandler = &n_PassCollissionHandler;
static n_CollisionHandlerFn nG_ResolutionHandler      = &n_PassResolutionHandler;


static struct {
    n_BodyNode* first;
    n_BodyNode* last;
} nG_BodyList;


static inline bool n_CanCollide(n_Body *restrict a, n_Body *restrict b)
{
    return (a->filter.mask & b->filter.category)
        && (b->filter.mask & a->filter.category);
}

static inline n_BodyNode* n_DerefBody(n_Body* restrict b)
{
    char* ptr = Ptr(b);
    return Ptr(ptr - sizeof(n_BodyNode) + sizeof(n_Body));
}

static inline n_SensorActivity n_GetSensorActivity(n_Body *restrict a, n_Body *restrict b)
{
    n_SensorActivity activity = n_SensorActivity_None;

    // TODO coll.aSensorActivity

    return activity;
}

static inline bool n_IsDeadVelocity(float v)
{
    return v < nG_VelocityDeadZone && v > -nG_VelocityDeadZone;
}


static void n_InitPhy();

static void n_MotionStep(float dt);
static void n_ResolutionStep();

static n_Vec2 n_Shift(n_Rect *restrict a, n_Rect *restrict b);


void n_InitPhy()
{
    n_MakeCircularAllocator(&nG_PhyAllocator, nG_MemBuffer, sizeof(nG_MemBuffer), sizeof(n_BodyNode), true);

    nG_BodyList.first = NULL;
    nG_BodyList.last  = NULL;
}


void n_Accelerate(n_Body *restrict body, n_Vec2 acceleration, float dt)
{
    uint16_t movable = n_BodyType_Dynamic | n_BodyType_Kinetic;

    if (body && (body->type & movable) == body->type) {
        body->velocity.x += acceleration.x * dt;
        body->velocity.y += acceleration.y * dt;
    }
}

void n_DeleteBody(n_Body **restrict b)
{
    if (b && *b){
        n_BodyNode* bn = n_DerefBody(*b);

        if (bn == nG_BodyList.first) {
            nG_BodyList.first = nG_BodyList.first->next;
        } else if (bn == nG_BodyList.last) {
            nG_BodyList.last = nG_BodyList.last->prev;
        } else {
            bn->next->prev = bn->prev;
            bn->prev->next = bn->next;
        }

        n_CircularAllocatorFree(&nG_PhyAllocator, bn);
        *b = NULL;
    }
}

n_Body* n_NewBody(
    n_Vec2            pos,
    n_Vec2            size,
    n_Vec2            sensorPadding,
    n_CollisionFilter filter,
    n_BodyType        type,
    uint16_t          data
) {
    n_BodyNode* bn = n_CircularAllocatorAlloc(&nG_PhyAllocator);

    if (!bn) {
        return NULL;
    }

    bn->body = (n_Body) {
        .hitbox = n_Rect(
            .x = pos.x,
            .y = pos.y,
            .w = size.x,
            .h = size.y
        ),
        .sensor = n_Rect(
            .x = pos.x  - sensorPadding.x,
            .y = pos.y  - sensorPadding.y,
            .w = size.x + 2 * sensorPadding.x,
            .h = size.y + 2 * sensorPadding.y
        ),
        .velocity = n_Vec2(.x = 0.0f, .y = 0.0f),
        .filter   = filter,
        .type     = type,
        .data     = data
    };

    bn->prev = nG_BodyList.last;
    bn->next = NULL;

    if (!nG_BodyList.first) {
        nG_BodyList.first = bn;
        nG_BodyList.last  = bn;
    } else {
        nG_BodyList.last->next = bn;
        nG_BodyList.last       = bn;
    }

    return &bn->body;
}

void n_PhysicsStep(float dt)
{
    n_MotionStep(dt);
    n_ResolutionStep();
} // n_Step


void n_SetCollisionHandler(n_CollisionHandlerFn handler)
{
    nG_CollisionHandler = handler;
}

void n_SetCollisionResolutionHandler(n_CollisionHandlerFn handler)
{
    nG_ResolutionHandler = handler;
}

void n_SetSensorCollisionHandler(n_CollisionHandlerFn handler)
{
    nG_SensorCollisionHandler = handler;
}

void n_SetVelocityDamping(float damping)
{
    nG_MotionDamping = damping;
}

void n_SetVelocityDeadzone(float v)
{
    nG_VelocityDeadZone = v;
}

static void n_PassCollissionHandler(n_Collision coll)
{
    // printf("body[%d] collided with body [%d]\n", a->Data, b->Data);
}

static void n_PassResolutionHandler(n_Collision coll)
{
    // printf("sensor[%d] touched sensor [%d]\n", a->Data, b->Data);
}

static n_Vec2 n_Shift(n_Rect *restrict a, n_Rect *restrict b)
{
    n_Vec2 shift;

    if (b->x > a->x) {
        shift.x = -(a->x + a->w - b->x);
    } else if (b->x < a->x) {
        shift.x = b->x + b->w - a->x;
    } else {
        shift.x = 0.0f;
    }

    if (b->y > a->y) {
        shift.y =  -(a->y + a->h - b->y);
    } else if (b->y < a->y) {
        shift.y =  b->y + b->h - a->y;
    } else {
        shift.y = 0.0f;
    }

    float dx = fabsf(shift.x);
    float dy = fabsf(shift.y);

    if (dx > dy) {
        shift.x = 0.0f;
    } else {
        shift.y = 0.0f;
    }

    return shift;
}

static void n_MotionStep(float dt)
{
    n_BodyNode* bn = nG_BodyList.first;
    n_Body*     b  = NULL;

    while (bn) {
        b  = &bn->body;

        b->hitbox.x += b->velocity.x * dt;
        b->hitbox.y += b->velocity.y * dt;
        b->sensor.x += b->velocity.x * dt;
        b->sensor.y += b->velocity.y * dt;

        b->velocity.x *= nG_MotionDamping;
        b->velocity.y *= nG_MotionDamping;

        if (n_IsDeadVelocity(b->velocity.x)) {
            b->velocity.x = 0.0f;
        }

        if (n_IsDeadVelocity(b->velocity.y)) {
            b->velocity.y = 0.0f;
        }

        bn = bn->next;
    }
}

static void n_ResolutionStep()
{
    n_BodyNode* bn1 = nG_BodyList.first;

    if (!bn1 || !bn1->next) {
        return;
    }

    n_BodyNode* bn2 = nG_BodyList.first->next;
    n_Body*     a   = NULL;
    n_Body*     b   = NULL;

    while (bn1) {
        a = &bn1->body;
        while (bn2) {
            b = &bn2->body;

            if (n_CanCollide(a, b)) {
                n_Collision coll = {
                    .a               = a,
                    .b               = b,
                    .aSensorActivity = n_SensorActivity_None
                };

                if (a->type == n_BodyType_Sensor || a->type == n_BodyType_Sensor) {
                    coll.aSensorActivity = n_GetSensorActivity(a, b);
                    nG_SensorCollisionHandler(coll);
                    continue;
                }

                if (n_RectsOverlap(&a->hitbox, &b->sensor) ||
                    n_RectsOverlap(&a->sensor, &b->hitbox))
                {
                    coll.aSensorActivity = n_GetSensorActivity(a, b);
                    nG_SensorCollisionHandler(coll);
                }

                if (n_RectsOverlap(&a->hitbox, &b->hitbox)) {
                    n_Vec2 shift = n_Shift(&a->hitbox, &b->hitbox);

                    nG_CollisionHandler(coll);

                    switch (a->type)
                    {
                    case n_BodyType_Sensor:
                        break;
                    case n_BodyType_Dynamic:
                        if (b->type == n_BodyType_Dynamic) {
                            a->hitbox.x += (shift.x / 2.0f);
                            a->hitbox.y += (shift.y / 2.0f);
                            a->sensor.x += (shift.x / 2.0f);
                            a->sensor.y += (shift.y / 2.0f);

                            b->hitbox.x -= (shift.x / 2.0f);
                            b->hitbox.y -= (shift.y / 2.0f);
                            b->sensor.x -= (shift.x / 2.0f);
                            b->sensor.y -= (shift.y / 2.0f);
                        } else {
                            a->hitbox.x += shift.x;
                            a->hitbox.y += shift.y;
                            a->sensor.x += shift.x;
                            a->sensor.y += shift.y;
                        }
                        break;
                    case n_BodyType_Kinetic:
                        if (b->type == n_BodyType_Dynamic) {
                            // kinectic body wins over dynamic bodies
                            b->hitbox.x -= shift.x;
                            b->hitbox.y -= shift.y;
                            b->sensor.x -= shift.x;
                            b->sensor.y -= shift.y;
                        } else {
                            // kinectic body loses to static bodies
                            a->hitbox.x += shift.x;
                            a->hitbox.y += shift.y;
                            a->sensor.x += shift.x;
                            a->sensor.y += shift.y;
                        }
                        break;
                    case n_BodyType_Static:
                        // static bodies always wins
                        b->hitbox.x -= shift.x;
                        b->hitbox.y -= shift.y;
                        b->sensor.x -= shift.x;
                        b->sensor.y -= shift.y;
                        break;
                    } // swicth

                    nG_ResolutionHandler(coll);
                } // if[overlap]
            } // if[can collide]

            bn2 = bn2->next;
        } // while

        bn1 = bn1->next;
    } // while
} // function

void n_DrawBodies(n_Camera *restrict cam)
{
    SDL_Color   sensorColor = {0xFF, 0x00, 0x00, 0xFF};
    SDL_Color   bodyColor   = {0x00, 0x00, 0x00, 0xFF};
    n_BodyNode* bn          = nG_BodyList.first;

    while (bn) {
        n_SetRendererDrawColor(sensorColor);
        n_DrawRect(cam, &bn->body.sensor);
        n_SetRendererDrawColor(bodyColor);
        n_DrawRect(cam, &bn->body.hitbox);
        bn = bn->next;
    }
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

    game->init(gt);

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
                    game->ehandler(&e);
                    break;
                }
            }

            gt.deltaTime = Float(delta) / 1000.0f;
            gt.totalTime = Float(curr) / 1000.0f;
            game->step(gt);

            n_Present();
        }
    }

    game->finalize(gt);
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

    n_InitPhy();
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
