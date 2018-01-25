#define _NOLIB_INCLUDE_IMPL_
#include "../nolib.h"
#include <stdio.h>

n_Animation* gAnimation;
n_Camera     gCam = n_Camera();

void Init(n_IGame *restrict game, n_GameTime gameTime);
void Step(n_IGame *restrict game, n_GameTime gameTime);
void Finalize(n_IGame *restrict game, n_GameTime gameTime);
void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e);

int main(void)
{
    n_IGame game;

    game.init     = &Init;
    game.step     = &Step;
    game.finalize = &Finalize;
    game.ehandler = &EventHandler;

    if (n_Init("nolib example", 320, 320, 32.0f)) {
        SDL_Texture* tex       = n_LoadTexture("space.png");
        SDL_Rect     frames[3] = {{0, 64, 32, 32}, {32, 64, 32, 32}, {64, 64, 32, 32}};

        gAnimation = n_NewAnimation(tex, frames, 3, 0.1f);
        n_Run(30, &game);
        n_DeleteAnimation(&gAnimation);

        n_DeleteTexture(&tex);
        n_Finalize();
    }
    return 0;
}


void Init(n_IGame *restrict game, n_GameTime gameTime)
{
    puts("The game was initalized");
}

void Step(n_IGame *restrict game, n_GameTime gameTime)
{
    n_Animate(gAnimation, gameTime.totalTime);
    n_DrawAnimation(&gCam, gAnimation);
}

void Finalize(n_IGame *restrict game, n_GameTime gameTime)
{
    puts("The game was finalized");
}

void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e)
{
    // WASD to move the animation
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_w: gAnimation->dest.y += 1; break;
            case SDLK_s: gAnimation->dest.y -= 1; break;
            case SDLK_a: gAnimation->dest.x -= 1; break;
            case SDLK_d: gAnimation->dest.x += 1; break;
        }
    }
}
