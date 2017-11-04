#define _NOLIB_INCLUDE_IMPL_
#include "../nolib.h"
#include <stdlib.h>


#define KEEP_ALIVE_TIMER 1


#define DIGLET_INVADER_MAX_DX  10
#define DIGLET_INVADER_MAX_NUM 50
#define USR_EVENT_SPAWN_CODE   1


typedef struct {
    float x0;
    float x;
    float y;
} DigletInvader;


static SDL_Rect SPACE_SHIP_TEX       = SDL_Rect(.x = 0,  .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_1           = SDL_Rect(.x = 32, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_2           = SDL_Rect(.x = 64, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_3           = SDL_Rect(.x = 96, .y = 0,  .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_1 = SDL_Rect(.x = 0,  .y = 32, .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_2 = SDL_Rect(.x = 32, .y = 32, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_1      = SDL_Rect(.x =  0, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_2      = SDL_Rect(.x = 32, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_3      = SDL_Rect(.x = 64, .y = 64, .w = 32, .h = 32);


void Init(n_GameTime gameTime);
void Step(n_GameTime gameTime);
void Finalize(n_GameTime gameTime);
void EventHandler(const SDL_Event *restrict e);

uint32_t SpawnCallback(uint32_t interval, void* param);
void Spawn(void);

void GetInput();


int main(void)
{
    n_IGame game = {
        .init     = &Init,
        .step     = &Step,
        .finalize = &Finalize,
        .ehandler = &EventHandler
    };

    if (n_Init("Space Invader", 10*32, 20*32, 32)) {
        SDL_Color bg = SDL_Color(.r = 0xFF, .g = 0xFF, .b = 0xFF);

        n_SetBackgroundColor(&bg);
        
        n_Run(30, &game);
        n_Finalize();
    }
    return 0;
}


static n_Camera     cam;

static SDL_Texture* tex;

static n_Animation* explosionAnim;
static n_Animation* bombAnim;

SDL_TimerID spawnTimerID;

static DigletInvader invaders[DIGLET_INVADER_MAX_NUM];
static size_t        nOfInvaders;


void Init(n_GameTime gameTime)
{
    SDL_Rect explositionFrames[3] = {EXPLOSION_TEX_1, EXPLOSION_TEX_2, EXPLOSION_TEX_3};
    SDL_Rect bombFrames[3]        = {BOMB_TEX_1, BOMB_TEX_2, BOMB_TEX_3};
    
    cam = n_Camera();

    tex = n_LoadTexture("space.png");

    explosionAnim = n_NewAnimation(tex, explositionFrames, 3, 0.1f);
    bombAnim      = n_NewAnimation(tex, bombFrames, 3, 0.2f);

    explosionAnim->dest.y = 5.0f;
    bombAnim->dest.y = 10.0f;
    bombAnim->dest.w = 0.5f;
    bombAnim->dest.h = 0.5f;

    spawnTimerID = SDL_AddTimer(3000, &SpawnCallback, NULL);
}

void Step(n_GameTime gameTime)
{
    n_Rect d = n_Rect(.x = 0, .y = 1, .w = 1, .h = 1);

    n_Animate(bombAnim, gameTime.totalTime);
    n_Animate(explosionAnim, gameTime.totalTime);

    n_DrawAnimation(&cam, bombAnim);
    n_DrawAnimation(&cam, explosionAnim);

    n_DrawTexture(&cam, tex, &SPACE_SHIP_TEX,       &d, 0.0f, SDL_FLIP_NONE); d.x = 3;
    n_DrawTexture(&cam, tex, &DIGLET_INVADER_TEX_1, &d, 0.0f, SDL_FLIP_NONE); d.x = 10;
    n_DrawTexture(&cam, tex, &DIGLET_INVADER_TEX_2, &d, 0.0f, SDL_FLIP_NONE); d.y = 13;
}

void Finalize(n_GameTime gameTime)
{
    n_DeleteAnimation(&bombAnim);
    n_DeleteAnimation(&explosionAnim);
    n_DeleteTexture(&tex);
}

void EventHandler(const SDL_Event *restrict e)
{
    switch(e->type) {
    case SDL_USEREVENT:
        break;
    }
}

uint32_t SpawnCallback(uint32_t interval, void* param)
{
    SDL_Event     e;
    SDL_UserEvent ue;

    ue.type  = SDL_USEREVENT;
    ue.code  = USR_EVENT_SPAWN_CODE;
    ue.data1 = NULL;
    ue.data2 = NULL;

    e.type = SDL_USEREVENT;
    e.user = ue;

    SDL_PushEvent(&e);
    return KEEP_ALIVE_TIMER;
}

void Spawn(void)
{
    static bool flag = 1;
    float x = flag ? 1.0f : 2.0f;

    flag = !flag;

    if (nOfInvaders < DIGLET_INVADER_MAX_NUM) {
        
    }
}
