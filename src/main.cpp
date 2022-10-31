#include <cstdlib>
#include <SDL.h>
/* *************SDL Audio ***************
 * Built in lib is SDL_audio
 *
 * Easier to use third-party lib SDL_mixer:
 *  MSYS: Install SDL2_mixer, not SDL_mixer!
 *  pacman -S mingw-w64-x86_64-SDL2_mixer
 *
 * Get free drum sounds from here: https://freewavesamples.com/
 * *******************************/
#include <SDL_mixer.h>
#include "window_info.h"
#include "color.h"
#include "me.h"

SDL_Window *win;
SDL_Renderer *ren;

void shutdown(void)
{
    Mix_CloseAudio();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}


int main(int argc, char* argv[])
{
    for (int i=0; i<argc; i++) puts(argv[i]);

    WindowInfo wI; wI.setup(argc, argv);

    { // SDL Setup
        SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
        win = SDL_CreateWindow(argv[0], wI.x, wI.y, wI.w, wI.h, wI.flags);
        Uint32 ren_flags = 0;
        ren_flags |= SDL_RENDERER_PRESENTVSYNC;                     // 60 fps! No SDL_Delay()
        ren_flags |= SDL_RENDERER_ACCELERATED;                      // Hardware acceleration
        ren = SDL_CreateRenderer(win, -1, ren_flags);
    }

    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        shutdown();
        return EXIT_FAILURE;
    }
    // Preload WAV files
    Mix_Chunk * kick = Mix_LoadWAV("data/Dry-Kick.wav");
    if (kick == NULL)
    {
        fprintf(stderr, "Unable to load wave file: %s\n", SDL_GetError());
    }
    Mix_Chunk * snare = Mix_LoadWAV("data/Ensoniq-ESQ-1-Snare.wav");
    if (snare == NULL)
    {
        fprintf(stderr, "Unable to load wave file: %s\n", SDL_GetError());
    }

    // Initial game state
    Me me(40, wI.w/2.f, wI.h/2.f);                              // me : 40x40 center square
    { // inactive state
        me.set_color(medium_gray);                          // Inactive : gray
        me.relax();
    }
    bool is_snare_j = false;
    bool is_snare_i = false;
    bool is_kick = false;

    bool quit = false;
    while(!quit)
    {

        // UI
        SDL_Keymod kmod = SDL_GetModState();                    // kmod : OR'd modifiers
        SDL_Event e; while(  SDL_PollEvent(&e)  )
        {
            if(  e.type == SDL_KEYDOWN  )
            {
                switch(  e.key.keysym.sym  )
                {
                    case SDLK_q:
                        if(  (kmod&KMOD_ALT) || (kmod&KMOD_CTRL)  ) quit = true; // quit : Alt-Q
                        break;
                    case SDLK_j:
                        if (!is_snare_j)
                        {
                            is_snare_j = true;
                            // channel = -1 : play on first free channel
                            // Mix_Chunk * snare : (snare points to the snare sample)
                            // loops = 0 : do not loop
                            Mix_PlayChannel(-1, snare, 0);
                            me.set_color(orange);
                            me.shrink();
                        }
                        break;
                    case SDLK_i:
                        if (!is_snare_i)
                        {
                            is_snare_i = true;
                            Mix_PlayChannel(-1, snare, 0);
                            me.set_color(orange);
                            me.shrink();
                        }
                        break;
                    case SDLK_SPACE:
                        if (!is_kick)
                        {
                            is_kick = true;
                            Mix_PlayChannel(-1, kick, 0);
                            me.set_color(purple);
                            me.expand();
                        }
                        break;
                    default: break;
                }
            }
            if(  e.type == SDL_KEYUP  )
            {
                switch(  e.key.keysym.sym  )
                {
                    case SDLK_j:
                        if (is_snare_j)
                        {
                            is_snare_j = false;
                            me.set_color(medium_gray);
                            me.relax();
                        }
                        break;
                    case SDLK_i:
                        if (is_snare_i)
                        {
                            is_snare_i = false;
                            me.set_color(medium_gray);
                            me.relax();
                        }
                        break;
                    case SDLK_SPACE:
                        if (is_kick)
                        {
                            is_kick = false;
                            me.set_color(medium_gray);
                            me.relax();
                        }
                        break;
                    default: break;
                }
            }
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
