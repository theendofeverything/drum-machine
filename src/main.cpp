#include <SDL.h>
#include <cstdlib>
#include "window_info.h"
#include "color.h"

SDL_Window *win;
SDL_Renderer *ren;

void shutdown(void)
{
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
}

struct Me
{
    SDL_FRect rect;
    Uint8 r,g,b,a;
    float size, x, y;
    Me(float size, float x, float y);
    void set_color(SDL_Color);
    void recenter(void);
    void shrink(void);
    void expand(void);
    void relax(void);
};
Me::Me(float size_pixels, float x_center, float y_center)
{
    // Abstract notion of my "relaxed" size and location
    size = size_pixels;                                         // size : I'm a square
    x = x_center;                                               // x : center
    y = y_center;                                               // y : center
    // Implement my size and location as an SDL_Rect
    rect.w = size; rect.h = size;
    rect.x = x-(rect.w/2.f);
    rect.y = y-(rect.h/2.f);
}
void Me::recenter(void)
{
    rect.x = x-(rect.w/2.f);
    rect.y = y-(rect.h/2.f);
}
void Me::shrink(void)
{ // Shrink to a skinny short guy
    rect.w = size/4.f; rect.h = size/2.f; recenter();
}
void Me::expand(void)
{ // Expand to a fat guy
    rect.w = size*2.f; rect.h = size/1.2f; recenter();
}
void Me::relax(void)
{ // Return to my default size
    rect.w = size; rect.h = size; recenter();
}
void Me::set_color(SDL_Color c)
{
    r=c.r;
    g=c.g;
    b=c.b;
    a=c.a;
}

int main(int argc, char* argv[])
{
    for (int i=0; i<argc; i++) puts(argv[i]);

    WindowInfo wI; wI.setup(argc, argv);

    { // SDL Setup
        SDL_Init(SDL_INIT_VIDEO);
        win = SDL_CreateWindow(argv[0], wI.x, wI.y, wI.w, wI.h, wI.flags);
        Uint32 ren_flags = 0;
        ren_flags |= SDL_RENDERER_PRESENTVSYNC;                     // 60 fps! No SDL_Delay()
        ren_flags |= SDL_RENDERER_ACCELERATED;                      // Hardware acceleration
        ren = SDL_CreateRenderer(win, -1, ren_flags);
    }

    // Initial game state
    Me me(40, wI.w/2.f, wI.h/2.f);                              // me : 40x40 center square

    bool quit = false;
    while(!quit)
    {
        // Game state
        { // Return to my inactive state
            me.set_color(medium_gray);                          // Inactive : gray
            me.relax();
        }

        // UI
        SDL_Keymod kmod = SDL_GetModState();                    // kmod : OR'd modifiers
        SDL_PumpEvents();
        const Uint8 *k = SDL_GetKeyboardState(NULL);
        if (  (kmod&KMOD_ALT) && k[SDL_SCANCODE_Q]  ) quit = true; // quit : Alt-Q

        if (k[SDL_SCANCODE_J])
        {
            me.set_color(orange);
            me.shrink();
        }
        if (k[SDL_SCANCODE_SPACE])
        {
            me.set_color(purple);
            me.expand();
        }

        // Render
        { // Background
            SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
            SDL_RenderClear(ren);
        }
        { // Me
            SDL_SetRenderDrawColor(ren, me.r, me.g, me.b, me.a);
            SDL_RenderFillRectF(ren, &(me.rect));
        }
        SDL_RenderPresent(ren);                                 // Display to screen
    }

    shutdown();
    return EXIT_SUCCESS;
}
