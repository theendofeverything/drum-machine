#ifndef __WINDOW_INFO_H
#define __WINDOW_INFO_H

struct WindowInfo
{
    int x, y, w, h;
    Uint32 flags;
    void setup(int argc, char* argv[]);
};

void WindowInfo::setup(int argc, char* argv[])
{ // Window size, location, and behavior to act like a Vim window

    // Set defaults
    x = 50;                                                     // Default x
    y = 50;                                                     // Default y
    w = 2*320;                                                  // Default w
    h = 2*180;                                                  // Default h

    // Overwrite with values, if provided
    if(argc>1) x = atoi(argv[1]);
    if(argc>2) y = atoi(argv[2]);
    if(argc>3) w = atoi(argv[3]);
    if(argc>4) h = atoi(argv[4]);

    // Only do a borderless, always-on-top window if I pass window args
    if(argc>1)
    {
        flags = SDL_WINDOW_BORDERLESS |                         // Look pretty
                SDL_WINDOW_ALWAYS_ON_TOP |                      // Stay on top
                SDL_WINDOW_INPUT_GRABBED;                       // Really stay on top
    }
    else
    {
        flags = SDL_WINDOW_RESIZABLE                            // Click drag to resize
                | SDL_WINDOW_BORDERLESS 
                ;
    }
}
#endif // __WINDOW_INFO_H
