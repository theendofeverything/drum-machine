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
/* *************Random Numbers***************
 *
 * I don't generate a unique seed for the RNG in this game.
 *
 * rand() and srand() are already in cstdlib. So it is only two more lines of code to
 * generate a unique seed based on time:
 *
 *  #include <ctime>
 *  srand(time(NULL));
 *
 * So why did I decide not to seed?
 *
 * I do not seed because this game only uses random numbers for visual effects.
 * No one will see (or care) that the visual effect use the same sequence of random values.
 * So there's no reason to seed. Not seeding avoids linking in the ctime lib. Less libs
 * means smaller footprint.
 * *******************************/
/* *************Float Quotient***************
 * A float quotient requires all operands are type float.
 * The operands must individually be cast to type float!
 *
 * It does not suffice to cast the result as float.
 * By that point, it is too late because the integer conversion
 * has already happened, causing the quotient to be zero
 * (assuming numerator < denominator, like it is in my case).
 *
 * RIGHT: <--------- Both numerator and denominator must be floats!
 *  printf( "rand()/RAND_MAX: %f\n",
 *          static_cast<float>(rand())/static_cast<float>(RAND_MAX) );
 *
 * WRONG: <--------- If only one is a float, quotient is always zero
 *  printf( "rand()/RAND_MAX: %f\n",
 *          static_cast<float>(rand()/RAND_MAX) );
 * *******************************/
float RAND_MAX_F = static_cast<float>(RAND_MAX);                // Quotient denominator : RAND_MAX_F 

SDL_Window *win;
SDL_Renderer *ren;

void shutdown(void)
{
    Mix_CloseAudio();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void music_volume(void)
{
    // I can change the volume of "music" whatever that means.
    // But it does not change the volume of my drum samples.
    // Get the current volume : -1 leaves volume unchanged
    int volume = Mix_VolumeMusic(-1);
    printf("Volume before change: %d\n", volume);
    // Change the volume -- returns *original* volume, not new volume
    volume = Mix_VolumeMusic(static_cast<int>(128.f*0.1f));
    printf("Volume returned when I change: %d\n", volume);
    // Query new volume by calling again with -1
    volume = Mix_VolumeMusic(-1);
    printf("Volume query after change: %d\n", volume);
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
    /* music_volume(); */
    bool quit = false;
    while(!quit)
    {

        // UI
        SDL_Keymod kmod = SDL_GetModState();                    // kmod : OR'd modifiers
        SDL_Event e; while(  SDL_PollEvent(&e)  )
        {
            if(  e.type == SDL_KEYDOWN  )
            {
                /* if(  kmod&KMOD_SHIFT  ) */
                /* { */
                /*     Mix_VolumeMusic(static_cast<int>(128*0.1)); // Shift : play soft */
                /* } */
                if(  kmod&KMOD_SHIFT  )
                {
                    Mix_VolumeChunk(snare, static_cast<int>(128) );
                }
                else
                {
                    Mix_VolumeChunk(snare, static_cast<int>(128*0.1) );
                }
                switch(  e.key.keysym.sym  )
                {
                    case SDLK_q:
                        if(  (kmod&KMOD_ALT) || (kmod&KMOD_CTRL)  ) quit = true; // quit : Alt-Q
                        break;
                    // TODO: Fix this: Me::shrink() and Me::expand() are undone by Renderer jiggle.
                    //
                    // Separate Me::shrink() and Me::expand() from the random
                    // jiggling I do in the Renderer section. The random jiggle is undoing
                    // the size change set up by shrink and expand.
                    //
                    // I think I need to create some indirection to prevent random jiggle
                    // from directly changing the w and h.
                    case SDLK_j:
                        if (!is_snare_j)
                        {
                            is_snare_j = true;
                            // channel = -1 : play on first free channel
                            // Mix_Chunk * snare : (snare points to the snare sample)
                            // loops = 0 : do not loop
                            // int ticks (ms) = -1 : play full sound
                            int channel = Mix_PlayChannelTimed(-1, snare, 0, -1);
                            // TODO: Display channel number
                            (void) channel;
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
            /* *************Jiggly effect***************
             * Alter width by a random amount that is at most +/- some fraction of the
             * width.
             * *******************************/
            float frac = 1/16.f;                                // jiggle by up to this fraction of the width
            float delta = me.size*frac;                         // max change
            { // Random change : me width
                // New value is me original size +/- delta
                me.rect.w = me.size + 2*delta*(  static_cast<float>((rand())/RAND_MAX_F)  ) - delta;
            }
            { // Random change : me height
                // New value is me original size +/- delta
                me.rect.h = me.size + 2*delta*(  static_cast<float>((rand())/RAND_MAX_F)  ) - delta;
            }
            // Directly changing w and h anchors the size change to me top-left corner.
            // Recenter to make it appear as if the size-change anchors to me center.
            me.recenter();                                      // Anchor w and h change to me center
            SDL_RenderFillRectF(ren, &(me.rect));
        }
        SDL_RenderPresent(ren);                                 // Display to screen
    }

    shutdown();
    return EXIT_SUCCESS;
}
