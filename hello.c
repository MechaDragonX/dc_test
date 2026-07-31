#include <dc/biosfont.h>
#include <kos.h>
#include <SDL/SDL.h>

// Use default init settings
KOS_INIT_FLAGS(INIT_DEFAULT);

int main(int arc, char *argv[])
{
    // Initialize SDL video and joystick and quit if none fond
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        return -1;
    }
    // Prevent the cursor from showing up by default
    SDL_ShowCursor(SDL_DISABLE);
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
    
    // Set encoding to EUC-JP
    bfont_set_encoding(BFONT_CODE_EUC);
    /*
        Set o to top left corner,
        Go down 100, right 50,
        Define writing 640 px wide,
        Black background, white foreground,
        16bits per pixel,
        Opaque text
    */
    bfont_draw_str_ex((uint16_t *)screen->pixels + (100 * 640 + 50), 640, 
                        0xFFFF, 0x0000, 16, 1,
                        "ハロー・ワールド！\nHello world!\n\n終了・Quit: A Button");

    // Update screen
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
