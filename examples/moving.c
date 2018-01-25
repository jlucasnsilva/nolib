#define _NOLIB_INCLUDE_IMPL_
#include "../nolib.h"
#include <stdio.h>

n_Sprite* gSprite;
n_Camera  gCam = n_Camera();

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
        SDL_Texture* tex = n_LoadTexture("space.png");
        SDL_Color    bgColor;

        bgColor = SDL_Color(.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF);
        n_SetBackgroundColor(&bgColor);

        gSprite = n_New(n_Sprite, 1);

        if (gSprite) {
            *gSprite = n_Sprite(
                .tex = tex,
                .src = SDL_Rect(.w = 32, .h = 32),
                .dest = n_Rect(.w = 1.0f, .h = 1.0f),
            );
            
            n_Run(30, &game);

            n_Delete(gSprite);
        }

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
    // WASP to move the sprite
    const uint8_t* kbds = SDL_GetKeyboardState(NULL);

    if (kbds[SDL_SCANCODE_W] && gSprite->dest.y < 8.9f) {
        gSprite->dest.y += 0.001;
    } else if (kbds[SDL_SCANCODE_S] && gSprite->dest.y > 0.1f) {
        gSprite->dest.y -= 0.001;
    }

    if (kbds[SDL_SCANCODE_A] && gSprite->dest.x > 0.1f) {
        gSprite->dest.x -= 0.001;
    } else if (kbds[SDL_SCANCODE_D] && gSprite->dest.x < 8.9f) {
        gSprite->dest.x += 0.001;
    }


    n_DrawSprite(&gCam, gSprite);
}

void Finalize(n_IGame *restrict game, n_GameTime gameTime)
{
    puts("The game was finalized");
}

void EventHandler(n_IGame *restrict game, const SDL_Event *restrict e)
{
    // Do nothing
}
