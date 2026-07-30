#include <kos.h>
#include <SDL/SDL.h>

int main(int arc, char *argv[])
{
    // Initialize SDL video and joystick and quit if none fond
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        return -1;
    }
    // Set up standard resolution and 16bit color
    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE);
    if(!screen)
    {
        SDL_Quit();
        return -1;
    }

    // Setup controllers
    // Use P1 if exists, and quit if none
    SDL_Joystick *controller = NULL;
    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_JoystickOpen(0);
    }
    else
    {
        SDL_Quit();
        return -1;
    }


    // Setup screen
    SDL_LockSurface(screen);
    /*
        Go to top left corner in 16-bit mode,
        Go down 100, right 50,
        Define writing 640 px wide,
        Opaque text
    */
    bfont_draw_str((uint16_t *)screen->pixels + (100 * 640 + 50), 640, 1, "Hello world!\nPress A to quit...");
    SDL_UnlockSurface(screen);
    SDL_Flip(screen);

    
    // Game loop
    int running = 1;
    SDL_Event event;
    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                // Loop for buttons
                case SDL_JOYBUTTONDOWN:
                    // If A, quit
                    if (event.jbutton.button == 2)
                    {
                        running = 0;
                    }
                    break;
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        SDL_Delay(20);
    }

    // Cleanup
    if(controller)
    {
        SDL_JoystickClose(controller);
    }
    SDL_Quit();

    // Return to system menu
    arch_menu();

    return 0;
}
