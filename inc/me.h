#ifndef __ME_H__
#define __ME_H__

#include <SDL.h>

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

#endif // __ME_H__
