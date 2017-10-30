#define _NOLIB_INCLUDE_IMPL_
#include "nolib.h"


n_Camera  cam = n_Camera();

float zoomInc = 0.1f;

n_Rect r1  = n_Rect(.x = 1, .y = 1, .w = 5, .h = 5);
n_Rect r2  = n_Rect(.x = 6, .y = 6, .w = 5, .h = 5);
n_Rect r3  = n_Rect(.x = 11, .y = 11, .w = 5, .h = 5);
n_Rect r4  = n_Rect(.x = 16, .y = 16, .w = 5, .h = 5);


void step(const SDL_Event *restrict e)
{
    n_SetRendererDrawColor(SDL_Color(.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF));
    n_DrawFilledRect(&cam, &r1);

    n_SetRendererDrawColor(SDL_Color(.r = 0xFF, .g = 0x00, .b = 0x00, .a = 0xFF));
    n_DrawFilledRect(&cam, &r2);

    n_SetRendererDrawColor(SDL_Color(.r = 0x00, .g = 0xFF, .b = 0x00, .a = 0xFF));
    n_DrawFilledRect(&cam, &r3);

    n_SetRendererDrawColor(SDL_Color(.r = 0x00, .g = 0x00, .b = 0xFF, .a = 0xFF));
    n_DrawFilledRect(&cam, &r4);

    const uint8_t* kbStt = SDL_GetKeyboardState(NULL);

    if (kbStt[SDL_SCANCODE_UP]) {
        cam.y -= 0.25f;
    } else if (kbStt[SDL_SCANCODE_DOWN]) {
        cam.y += 0.25f;
    }
    
    if (kbStt[SDL_SCANCODE_LEFT]) {
        cam.x += zoomInc;
    } else if (kbStt[SDL_SCANCODE_RIGHT]) {
        cam.x -= zoomInc;
    }

    if (kbStt[SDL_SCANCODE_KP_PLUS]) {
        cam.zoom += zoomInc;
        printf("cam.zoom = %.2f\n", cam.zoom);
    } else if (kbStt[SDL_SCANCODE_KP_MINUS] && cam.zoom > 0.5f) {
        cam.zoom -= zoomInc;
        printf("cam.zoom = %.2f\n", cam.zoom);
    }
}


void handler(const SDL_Event *restrict e)
{
    switch(e->type)
    {
    case SDL_MOUSEWHEEL:
        if (e->wheel.y == 1) {
            cam.zoom += 0.2f;
            printf("cam.zoom = %.2f\n", cam.zoom);
        } else if (e->wheel.y == -1 && cam.zoom > 0.5f) {
            cam.zoom -= 0.2f;
            printf("cam.zoom = %.2f\n", cam.zoom);
        }
        break;
    }
}

int main(void)
{
    if (n_Init("Hello World", 1024, 768, 32.0f)) {
        n_Run(30, &step, &handler);
        n_Finalize();
    }
    return 0;
}
