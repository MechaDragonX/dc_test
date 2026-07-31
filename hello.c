#include <dc/biosfont.h>
#include <kos.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

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
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE);
    if(!screen)
    {
        SDL_Quit();
        return -1;
    }
    // Setup controllers
    // Use P1 if exists, and quit if none
    SDL_Joystick* controller = NULL;
    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_JoystickOpen(0);
    }
    else
    {
        SDL_Quit();
        return -1;
    }

    if(TTF_Init() < 0)
    {
        SDL_Quit();
        return -1;
    }


    // Import and define font
    // Represents height of lines and font size, cuz that's the same
    int lineHeight = 18;
    TTF_Font* font = TTF_OpenFont("/rd/font.ttf", lineHeight);
    // White
    SDL_Color color = { 255, 255, 255, 255 };

    // All lines as an array
    const char *lines[] =
    {
        "ハロー・ワールド！",
        "Hello world!",
        "",
        "終了・Quit: A Button"
    };
    int numLines = 4;
    // Starting draw positions
    int xPos = 50;
    int yPos = 100;

    // Define surfaces and recetanges for drawing
    SDL_Surface* rawText = NULL;
    SDL_Surface* textSurface = NULL;
    SDL_Rect textRect;
    // Loop through all lines to draw them
    for(int i = 0; i < numLines; i++)
    {
        // If it's a blank line, then just move down the lineHeight
        if(strlen(lines[i]) == 0)
        {
            yPos += lineHeight;
            continue;
        }

        // Setup the text
        rawText = TTF_RenderUTF8_Blended(font, lines[i], color);
        if(rawText)
        {
            // Create it with 16bit and such
            textSurface = SDL_DisplayFormatAlpha(rawText);
            SDL_FreeSurface(rawText);

            // Draw it in the specified positions
            if(textSurface)
            {
                textRect.x = (int16_t)xPos;
                textRect.y = (int16_t)yPos;
                SDL_BlitSurface(textSurface, NULL, screen, &textRect);
                SDL_FreeSurface(textSurface);
            }
        }

        // Move further down
        yPos += lineHeight;
    }

    // Update screen
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
    if(font)
    {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    if(controller)
    {
        SDL_JoystickClose(controller);
    }
    SDL_Quit();

    // Return to system menu
    arch_menu();

    return 0;
}
