#define _NOLIB_INCLUDE_IMPL_
#include "../nolib.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>


#define WIN_WIDTH  15
#define WIN_HEIGHT 20


typedef struct {
    n_IGame      interface;

    n_Camera     cam;

    SDL_TimerID  spawnTimerID;

    SDL_Texture* tex;
    n_Animation* explosionAnim;
    n_Animation* bombAnim;
} Game;


void Init(n_IGame *restrict game, n_GameTime gameTime);
void Step(n_IGame *restrict game, n_GameTime gameTime);
void Finalize(n_IGame *restrict game, n_GameTime gameTime);
void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e);

uint32_t SpawnInvadersCallback(uint32_t interval, void* param);
void SpawnInvaders(float y);
void UpdateInvaders(n_GameTime gt);
void DrawInvaders(SDL_Texture* tex, const n_Camera *restrict cam);

void DrawBombs(n_Animation *restrict bombAnim, const n_Camera *restrict cam);
void DrawPlayer(SDL_Texture* tex, const n_Camera *restrict cam);
void Shoot(float x, float y, float dy);
void UpdateBombs(n_Animation *restrict bombAnim, n_GameTime gt);
void UpdatePlayer(n_GameTime gt);



int main(void)
{
    Game game;

    game.interface.init     = &Init;
    game.interface.step     = &Step;
    game.interface.finalize = &Finalize;
    game.interface.ehandler = &EventHandler;

    if (n_Init("Space Invader", WIN_WIDTH*32, WIN_HEIGHT*32, 32)) {
        SDL_Color bg = SDL_Color(.r = 0xFF, .g = 0xFF, .b = 0xFF);

        n_SetBackgroundColor(&bg);

        n_Run(30, &game.interface);
        n_Finalize();
    }

    return 0;
}


// --------------------------------------------------------
//
// GAME
//
// --------------------------------------------------------


#define USER_EVENT_SPAWN_CODE   1


enum {
    CollFilter_Player     = 1,
    CollFilter_Diglet     = 1 << 1,
    CollFilter_PlayerBomb = 1 << 2,
    CollFilter_DigletBomb = 1 << 3
};


static SDL_Rect SPACE_SHIP_TEX       = SDL_Rect(.x = 0,  .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_1           = SDL_Rect(.x = 32, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_2           = SDL_Rect(.x = 64, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_3           = SDL_Rect(.x = 96, .y = 0,  .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_1 = SDL_Rect(.x = 0,  .y = 32, .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_2 = SDL_Rect(.x = 32, .y = 32, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_1      = SDL_Rect(.x =  0, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_2      = SDL_Rect(.x = 32, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_3      = SDL_Rect(.x = 64, .y = 64, .w = 32, .h = 32);


void Init(n_IGame *restrict game, n_GameTime gameTime)
{
    Game*    g                    = Ptr(game);
    SDL_Rect explositionFrames[3] = {EXPLOSION_TEX_1, EXPLOSION_TEX_2, EXPLOSION_TEX_3};
    SDL_Rect bombFrames[3]        = {BOMB_TEX_1, BOMB_TEX_2, BOMB_TEX_3};
    
    g->cam           = n_Camera();
    g->tex           = n_LoadTexture("space.png");
    g->explosionAnim = n_NewAnimation(g->tex, explositionFrames, 3, 0.1f);
    g->bombAnim      = n_NewAnimation(g->tex, bombFrames, 3, 0.2f);

    g->explosionAnim->dest = n_Rect(.x = 0, .y = 0, .w = 1.0f, .h = 1.0f);
    g->bombAnim->dest      = n_Rect(.x = 0, .y = 0, .w = 0.5f, .h = 0.5f);

    g->spawnTimerID = SDL_AddTimer(1000, &SpawnInvadersCallback, NULL);

    for (float y = WIN_HEIGHT / 2; y < (WIN_HEIGHT - 1); y += 2.0f) {
        SpawnInvaders(y);
    }

    srand(time(NULL));
}

void Step(n_IGame *restrict game, n_GameTime gameTime)
{
    Game* g = Ptr(game);

    n_Animate(g->explosionAnim, gameTime.totalTime);
    n_DrawAnimation(&g->cam, g->explosionAnim);

    UpdateInvaders(gameTime);
    UpdatePlayer(gameTime);
    UpdateBombs(g->bombAnim, gameTime);
    
    DrawInvaders(g->tex, &g->cam);
    DrawPlayer(g->tex, &g->cam);
    DrawBombs(g->bombAnim, &g->cam);
}

void Finalize(n_IGame *restrict game, n_GameTime gameTime)
{
    Game* g = Ptr(game);

    SDL_RemoveTimer(g->spawnTimerID);

    n_DeleteAnimation(&g->bombAnim);
    n_DeleteAnimation(&g->explosionAnim);
    n_DeleteTexture(&g->tex);
}

void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e)
{
    switch(e->type) {
    case SDL_USEREVENT:
        SpawnInvaders(WIN_HEIGHT);
        break;
    }
}


// --------------------------------------------------------
//
// INVADERS
//
// --------------------------------------------------------


#define SPAWN_TIMER_TIME 4000

#define DIGLET_INVADER_MAX_DX  10
#define DIGLET_INVADER_MAX_NUM 50


typedef struct {
    n_Body* body;
    n_Vec2  acceleration;

    int     dir;
    float   x0;
    float   x;
    float   y;
} DigletInvader;


static DigletInvader invaders[DIGLET_INVADER_MAX_NUM];
static size_t        nOfInvaders;


void DrawInvaders(SDL_Texture* tex, const n_Camera *restrict cam)
{
    n_Rect d = n_Rect(.x = 0, .y = 0, .w = 1, .h = 1);

    for (int i = 0; i < nOfInvaders; i++) {
        DigletInvader* di = &invaders[i];

        d.x = di->x;
        d.y = di->y;

        if (di->dir > 0) {
            n_DrawTexture(cam, tex, &DIGLET_INVADER_TEX_2, &d, 0.0f, SDL_FLIP_NONE);
        } else {
            n_DrawTexture(cam, tex, &DIGLET_INVADER_TEX_1, &d, 0.0f, SDL_FLIP_NONE);
        }
    }
}

void UpdateInvaders(n_GameTime gt)
{
    const float MAX_SHIFT    = 3.0f;
    const int   SHOOT_CHANCE = 2;
    int         chance;

    for (int i = 0; i < nOfInvaders; i++) {
        DigletInvader* di = &invaders[i];

        if (di->dir > 0 && di->x < (di->x0 + MAX_SHIFT)) {
            di->x += 0.02f;
        } else if (di->dir < 0 && di->x > di->x0) {
            di->x -= 0.02f;
        } else {
            di->y   -= 0.5;
            di->dir *= -1;
        }

        chance = rand() % 1000;
        if (chance <= SHOOT_CHANCE) {
            Shoot(di->x, di->y, -0.05);
        }
    }
}

void SpawnInvaders(float y)
{
    static bool flag = 1;
    float       x    = flag ? 1.0f : 2.0f;
    int         i    = 0;

    flag = !flag;

    for (i = 0; i < 5 && (i + nOfInvaders) < DIGLET_INVADER_MAX_NUM; i++) {
        invaders[i + nOfInvaders].body = n_NewBody(
            n_Vec2(.x = x + 2 * i, .y = y),
            n_Vec2(.x = 1, .y = 1),
            n_Vec2(.x = 1, .y = 1),
            n_CollisionFilter(
                .mask     = CollFilter_Diglet,
                .category = CollFilter_Player | CollFilter_PlayerBomb
            ),
            n_BodyType_Dynamic,
            0
        );
        invaders[i + nOfInvaders].acceleration = n_Vec2(.x = 1.0f, .y = -0.1f);

        invaders[i + nOfInvaders].x0  = x + 2 * i;
        invaders[i + nOfInvaders].x   = invaders[i + nOfInvaders].x0;
        invaders[i + nOfInvaders].y   = y;
        invaders[i + nOfInvaders].dir = 1;
    }

    nOfInvaders += i;
}

uint32_t SpawnInvadersCallback(uint32_t interval, void* param)
{
    SDL_Event     e;
    SDL_UserEvent ue;

    ue.type  = SDL_USEREVENT;
    ue.code  = USER_EVENT_SPAWN_CODE;
    ue.data1 = NULL;
    ue.data2 = NULL;

    e.type = SDL_USEREVENT;
    e.user = ue;

    SDL_PushEvent(&e);
    return SPAWN_TIMER_TIME;
}


// --------------------------------------------------------
//
// Player
//
// --------------------------------------------------------


#define BULLET_MAX_NUM 100


const float SHOOT_COOL_DOWN = 500.0f / 1000.0f;


typedef struct BulletList BulletList;

struct BulletList {
    BulletList* next;
    n_Body*     body;
    n_Vec2      acceleration;

    n_Vec2      pos;
    float       dy;
};


static struct {
    n_Body* body;
    float   x;
    float   y;
    float   lastShotT;
} player = {.x = WIN_WIDTH / 2.0f - 0.5f, .y = 1.0f, .lastShotT = 0.0f};

static BulletList bullets = {.next = NULL};


void DrawBombs(n_Animation *restrict bombAnim, const n_Camera *restrict cam)
{
    for (BulletList *b = bullets.next; b; b = b->next) {
        bombAnim->dest.x = b->pos.x;
        bombAnim->dest.y = b->pos.y;
        n_DrawAnimation(cam, bombAnim);
    }
}

void DrawPlayer(SDL_Texture* tex, const n_Camera *restrict cam)
{
    n_Rect d = n_Rect(.x = player.x, .y = player.y, .w = 1, .h = 1);
    n_DrawTexture(cam, tex, &SPACE_SHIP_TEX, &d, 0.0f, SDL_FLIP_NONE);
}

void Shoot(float x, float y, float dy)
{
    BulletList* b = n_New(BulletList, 1);

    if (b) {
        b->body = n_NewBody(
            n_Vec2(.x = x, .y = y),
            n_Vec2(.x = 1, .y = 1),
            n_Vec2(.x = 1, .y = 1),
            n_CollisionFilter(
                .mask     = CollFilter_PlayerBomb,
                .category = CollFilter_Diglet | CollFilter_DigletBomb
            ),
            n_BodyType_Dynamic,
            0
        )

        b->dy        = dy;
        b->pos.x     = x;
        b->pos.y     = y;
        b->next      = bullets.next;
        bullets.next = b;
    }
}

void UpdateBombs(n_Animation *restrict bombAnim, n_GameTime gt)
{
    n_Animate(bombAnim, gt.totalTime);

    for (BulletList *b = bullets.next; b; b = b->next) {
        b->pos.y += b->dy;
    }
}

void UpdatePlayer(n_GameTime gt)
{
    const uint8_t* ks = SDL_GetKeyboardState(NULL);
    float          dx = 0.07;
    float          dy = 0.09f;

    if (ks[SDL_SCANCODE_UP] && player.y < (WIN_HEIGHT - 2)) {
        player.y += dy;
    } else if (ks[SDL_SCANCODE_DOWN] && player.y > 0.5f) {
        player.y -= dy;
    }

    if (ks[SDL_SCANCODE_LEFT] && player.x > 0.0f) {
        player.x -= dx;
    } else if (ks[SDL_SCANCODE_RIGHT] && player.x < (WIN_WIDTH - 1)) {
        player.x += dx;
    }

    if (ks[SDL_SCANCODE_SPACE] && (gt.totalTime - player.lastShotT) >= SHOOT_COOL_DOWN) {
        player.lastShotT = gt.totalTime;
        Shoot(player.x, player.y, 0.05f);
    }
}
