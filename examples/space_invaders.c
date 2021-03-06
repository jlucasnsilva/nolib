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
    n_Animation* digletAnim;
    n_Sprite*    playerSprite;
} Game;


void Init(n_IGame *restrict game, n_GameTime gameTime);
void Step(n_IGame *restrict game, n_GameTime gameTime);
void Finalize(n_IGame *restrict game, n_GameTime gameTime);
void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e);

void DrawBombs(n_Animation *restrict bombAnim, const n_Camera *restrict cam);
void Shoot(float x, float y, float dy);
void UpdateBombs(float dt);

uint32_t SpawnInvadersCallback(uint32_t interval, void* param);
void SpawnInvaders(float y);
void UpdateInvaders(n_GameTime gt);
void DrawInvaders(SDL_Texture* tex, const n_Camera *restrict cam);

void DrawPlayer(SDL_Texture* tex, const n_Camera *restrict cam);
void InitPlayer(void);
void UpdatePlayer(n_GameTime gt);

void MakeWalls(void);


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
// ENTITY
//
// --------------------------------------------------------


static SDL_Rect SPACE_SHIP_TEX       = SDL_Rect(.x = 0,  .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_1           = SDL_Rect(.x = 32, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_2           = SDL_Rect(.x = 64, .y = 0,  .w = 32, .h = 32);
static SDL_Rect BOMB_TEX_3           = SDL_Rect(.x = 96, .y = 0,  .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_1 = SDL_Rect(.x = 0,  .y = 32, .w = 32, .h = 32);
static SDL_Rect DIGLET_INVADER_TEX_2 = SDL_Rect(.x = 32, .y = 32, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_1      = SDL_Rect(.x =  0, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_2      = SDL_Rect(.x = 32, .y = 64, .w = 32, .h = 32);
static SDL_Rect EXPLOSION_TEX_3      = SDL_Rect(.x = 64, .y = 64, .w = 32, .h = 32);

#define VELOCITY_DAMPING 0.95

typedef struct Entity Entity;

struct Entity {
    Entity*      next;
    Entity*      prev;
    union {
        n_Animation* anim;
        n_Sprite*    sprite;
    } graphic;
    n_Rect       hitbox;
    void         (*update)(Entity *restrict self, float dt);
    void         (*draw)(Entity *restrict self, const n_Camera *restrict cam);
    n_Vec2       velocity;
    int          type;
    int          collidesWith;
};

enum {
    type_Player     = 1,
    type_Diglet     = 1 << 1,
    type_PlayerBomb = 1 << 2,
    type_DigletBomb = 1 << 3,
    type_Wall       = 1 << 4,
};


void DrawAnimation(Entity *restrict self, const n_Camera *restrict cam)
{
    if (!self || !cam || !self->graphic.anim) {
        return;
    }

    self->graphic.anim->dest.x = self->hitbox.x;
    self->graphic.anim->dest.y = self->hitbox.y;
    n_DrawAnimation(cam, self->graphic.anim);
}

void DrawSprite(Entity *restrict self, const n_Camera *restrict cam)
{
    if (!self || !cam || !self->graphic.sprite) {
        return;
    }

    self->graphic.sprite->dest.x = self->hitbox.x;
    self->graphic.sprite->dest.y = self->hitbox.y;
    n_DrawSprite(cam, self->graphic.sprite);
}

void DrawHitbox(Entity *restrict self, const n_Camera *restrict cam)
{
    if (!self) {
        return;
    }

    n_SetRendererDrawColor(SDL_Color(.g = 100, .b = 100));
    n_DrawFilledRect(cam, &self->hitbox);
}


// --------------------------------------------------------
//
// GAME
//
// --------------------------------------------------------


#define USER_EVENT_SPAWN_CODE   1


void Init(n_IGame *restrict game, n_GameTime gameTime)
{
    Game*    g                    = Ptr(game);
    SDL_Rect explositionFrames[3] = {EXPLOSION_TEX_1, EXPLOSION_TEX_2, EXPLOSION_TEX_3};
    SDL_Rect bombFrames[3]        = {BOMB_TEX_1, BOMB_TEX_2, BOMB_TEX_3};
    SDL_Rect digletFrames[2]      = {DIGLET_INVADER_TEX_1, DIGLET_INVADER_TEX_2};
    
    g->cam           = n_Camera();
    g->tex           = n_LoadTexture("space.png");
    
    g->explosionAnim = n_NewAnimation(g->tex, explositionFrames, 3, 0.1f);
    g->bombAnim      = n_NewAnimation(g->tex, bombFrames, 3, 0.2f);
    g->digletAnim    = n_NewAnimation(g->tex, digletFrames, 2, 0.4f);
    g->playerSprite  = n_New(n_Sprite, 1);

    *g->playerSprite  = (n_Sprite) {
        .tex   = g->tex,
        .src   = SPACE_SHIP_TEX,
        .dest  = n_Rect(.w = 1.0f, .h = 1.0f),
        .angle = 0.0f,
        .flip  = SDL_FLIP_HORIZONTAL
    };
    
    g->spawnTimerID  = SDL_AddTimer(1000, &SpawnInvadersCallback, NULL);

    g->explosionAnim->dest = n_Rect(.x = 0, .y = 0, .w = 1.0f, .h = 1.0f);
    g->bombAnim->dest      = n_Rect(.x = 0, .y = 0, .w = 0.5f, .h = 0.5f);

    for (float y = WIN_HEIGHT / 2; y < (WIN_HEIGHT - 1); y += 2.0f) {
        SpawnInvaders(y);
    }

    InitPlayer();
    MakeWalls();

    srand(time(NULL));
}

void Step(n_IGame *restrict game, n_GameTime gameTime)
{
    Game* g = Ptr(game);

    n_Animate(g->explosionAnim, gameTime.totalTime);
    n_Animate(g->bombAnim, gameTime.totalTime);

    UpdateBombs(gameTime.deltaTime);
    UpdateInvaders(gameTime);
    UpdatePlayer(gameTime);
}

void Finalize(n_IGame *restrict game, n_GameTime gameTime)
{
    Game* g = Ptr(game);

    SDL_RemoveTimer(g->spawnTimerID);

    n_DeleteAnimation(&g->bombAnim);
    n_DeleteAnimation(&g->explosionAnim);
    n_DeleteAnimation(&g->digletAnim);
    n_Delete(g->playerSprite);
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


void UpdateBomb(Entity *restrict self, float dt)
{
    self->velocity.x *= VELOCITY_DAMPING;
    self->velocity.y *= VELOCITY_DAMPING;
}

void UpdateInvader(n_GameTime gt)
{
    const float Y_ACC        = 5.0f;
    const float MAX_SHIFT    = 3.0f;
    const int   SHOOT_CHANCE = 2;
    int         chance;

    for (int i = 0; i < nOfInvaders; i++) {
        DigletInvader* di = &invaders[i];

        if (di->body->hitbox.x > (di->x0 + MAX_SHIFT)
            || di->body->hitbox.x < (di->x0 - MAX_SHIFT)) {
            di->acceleration.x *= -1;
            di->acceleration.y = Y_ACC;
        } else {
            di->acceleration.y = 0;
        }

        n_Accelerate(di->body, di->acceleration, gt.deltaTime);

        chance = rand() % 1000;
        if (chance <= SHOOT_CHANCE) {
            Shoot(di->body->hitbox.x, di->body->hitbox.y, -1);
        }
    }
}

void UpdatePlayer(n_GameTime gt)
{
    const float    X_ACC = 0.01f;
    const float    Y_ACC = 0.01f;
    const uint8_t* ks  = SDL_GetKeyboardState(NULL);
    n_Vec2         acc = n_Vec2();

    if (ks[SDL_SCANCODE_UP]) {
        acc.y = Y_ACC;
    } else if (ks[SDL_SCANCODE_DOWN]) {
        acc.y = -Y_ACC;
    } else {
        acc.y = 0.0f;
    }

    if (ks[SDL_SCANCODE_LEFT]) {
        acc.x = -X_ACC;
    } else if (ks[SDL_SCANCODE_RIGHT]) {
        acc.x = X_ACC;
    } else {
        acc.x = 0.0f;
    }

    if (ks[SDL_SCANCODE_SPACE] && (gt.totalTime - player.lastShotT) >= SHOOT_COOL_DOWN) {
        player.lastShotT = gt.totalTime;
        Shoot(player.body->hitbox.x, player.body->hitbox.y, 1);
    }

    n_Accelerate(player.body, acc, gt.deltaTime);
}

void Shoot(float x, float y, float dy)
{
    const float BOMB_ACC = 0.01f;
    BombList*   b        = n_New(BombList, 1);

    if (b) {
        b->body = n_NewBody(
            n_Vec2(.x = x, .y = y),
            n_Vec2(.x = 0.5f, .y = 0.5f),
            n_Vec2(.x = 0.1f, .y = 0.1f),
            n_CollisionFilter(
                .mask     = CollFilter_PlayerBomb,
                .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Wall
            ),
            n_BodyType_Dynamic,
            0
        );

        if (b->body) {
            b->next         = bullets.next;
            b->acceleration = n_Vec2(.y = dy * BOMB_ACC);
            bullets.next    = b;
        } else {
            n_Delete(b);
        }
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
            n_Vec2(.x = 0.1f, .y = 0.1f),
            n_CollisionFilter(
                .mask     = CollFilter_Diglet,
                .category = CollFilter_Player | CollFilter_PlayerBomb | CollFilter_Wall
            ),
            n_BodyType_Dynamic,
            0
        );

        invaders[i + nOfInvaders].acceleration = n_Vec2(.x = 1.0f);
        invaders[i + nOfInvaders].x0  = x + 2 * i;
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


const float SHOOT_COOL_DOWN = 500.0f / 1000.0f;


static struct {
    n_Body* body;
    float   lastShotT;
} player;


void InitPlayer(void)
{
    player.body = n_NewBody(
        n_Vec2(.x = WIN_WIDTH / 2.0f - 0.5f, .y = 1.0f),
        n_Vec2(.x = 1.0f, .y = 1.0f),
        n_Vec2(.x = 0.1f, .y = 0.1f),
        n_CollisionFilter(
            .mask     = CollFilter_Player,
            .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Wall
        ),
        n_BodyType_Dynamic,
        0
    );

    player.lastShotT = 0.0f;
}




// --------------------------------------------------------
//
// Wall
//
// --------------------------------------------------------


void MakeWalls(void)
{
    // Left Wall
    n_NewBody(
        n_Vec2(.x = -1.0f, .y = 0.0f),
        n_Vec2(.x =  1.2f, .y = WIN_HEIGHT),
        n_Vec2(.x =  0.1f, .y = 0.1f),
        n_CollisionFilter(
            .mask     = CollFilter_Wall,
            .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Player | CollFilter_PlayerBomb
        ),
        n_BodyType_Static,
        0
    );

    // Right Wall
    n_NewBody(
        n_Vec2(.x = WIN_WIDTH - 0.2f, .y = 0.0f),
        n_Vec2(.x =  1.0f, .y = WIN_HEIGHT),
        n_Vec2(.x =  0.1f, .y = 0.1f),
        n_CollisionFilter(
            .mask     = CollFilter_Wall,
            .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Player | CollFilter_PlayerBomb
        ),
        n_BodyType_Static,
        0
    );

    // Upper Wall
    n_NewBody(
        n_Vec2(.x =  0.0f, .y = WIN_HEIGHT * 2.0),
        n_Vec2(.x =  WIN_WIDTH, .y = 1.0f),
        n_Vec2(.x =  0.1f, .y = 0.1f),
        n_CollisionFilter(
            .mask     = CollFilter_Wall,
            .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Player | CollFilter_PlayerBomb
        ),
        n_BodyType_Static,
        0
    );

    // Bottom Wall
    n_NewBody(
        n_Vec2(.x =  0.0f, .y = -1.0f),
        n_Vec2(.x =  WIN_WIDTH, .y = 1),
        n_Vec2(.x =  0.1f, .y = 0.1f),
        n_CollisionFilter(
            .mask     = CollFilter_Wall,
            .category = CollFilter_Diglet | CollFilter_DigletBomb | CollFilter_Player | CollFilter_PlayerBomb
        ),
        n_BodyType_Static,
        0
    );
}
