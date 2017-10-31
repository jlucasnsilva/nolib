#define _NOLIB_INCLUDE_IMPL_
#include "../nolib.h"


static SDL_Rect SPACE_SHIP_TEX       = SDL_Rect(.x = 0,  .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_1           = SDL_Rect(.x = 32, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_2           = SDL_Rect(.x = 64, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_3           = SDL_Rect(.x = 96, .y = 0,  .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_1 = SDL_Rect(.x = 0,  .y = 32, .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_2 = SDL_Rect(.x = 32, .y = 32, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_1      = SDL_Rect(.x = 32, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_2      = SDL_Rect(.x = 64, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_3      = SDL_Rect(.x = 96, .y = 64, .w = 32, .h = 32);


void Init(void);
void Step(void);
void Finalize(void);
void EventHandler(const SDL_Event *restrict e);


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


static SDL_Texture* tex;
static n_Camera     cam;


void Init(void)
{
    tex = n_LoadTexture("space.png");
    cam = n_Camera();
}

void Step(void)
{
    n_Rect d = n_Rect(.x = 0, .y = 1, .w = 1, .h = 1);

    n_DrawTexture(&cam, tex, &SPACE_SHIP_TEX,       &d, 0.0f, SDL_FLIP_NONE); d.x = 3;
    n_DrawTexture(&cam, tex, &BOMB_TEX_1,           &d, 0.0f, SDL_FLIP_NONE); d.y = 3;
    n_DrawTexture(&cam, tex, &BOMB_TEX_2,           &d, 0.0f, SDL_FLIP_NONE); d.x = 5;
    n_DrawTexture(&cam, tex, &BOMB_TEX_3,           &d, 0.0f, SDL_FLIP_NONE); d.y = 6;
    n_DrawTexture(&cam, tex, &DIGLET_INVADER_TEX_1, &d, 0.0f, SDL_FLIP_NONE); d.x = 10;
    n_DrawTexture(&cam, tex, &DIGLET_INVADER_TEX_2, &d, 0.0f, SDL_FLIP_NONE); d.y = 13;
    n_DrawTexture(&cam, tex, &EXPLOSION_TEX_1,      &d, 0.0f, SDL_FLIP_NONE); d.x = 5;
    n_DrawTexture(&cam, tex, &EXPLOSION_TEX_2,      &d, 0.0f, SDL_FLIP_NONE); d.y = 17;
    n_DrawTexture(&cam, tex, &EXPLOSION_TEX_3,      &d, 0.0f, SDL_FLIP_NONE);
}

void Finalize(void)
{
    n_DestroyTexture(&tex);
}

void EventHandler(const SDL_Event *restrict e)
{
    // TODO
}
