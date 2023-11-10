#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>  
#include <SDL2/SDL_timer.h>

int DELAY_TIME = 10;

Mix_Music *bounceSound;  

void clear(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void draw(SDL_Renderer *renderer, SDL_Rect *rect)
{
    SDL_RenderFillRect(renderer, rect);
}

int main(void)
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  // Initialisation de l'audio en plus de la vidéo

    window = SDL_CreateWindow(
        "Fenêtre SDL2",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        800,
        SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL)
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("./Black_Future.otf", 24);

    // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    bounceSound = Mix_LoadMUS("./sound.mp3");

    if (bounceSound == NULL)
    {
        printf("Failed to load bounce sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    SDL_Rect balle = {.x = 400, .y = 400, .w = 12, .h = 12};
    SDL_Rect rectangle = {.x = 20, .y = 20, .w = 10, .h = 100};
    SDL_Rect carre = {.x = 780, .y = 20, .w = 10, .h = 100};
    SDL_Rect losage = {.x = 399, .y = 50, .w = 2, .h = 700};

    int quit = 0;
    int score = 0;

    int ball_velocity_x = 2;
    int ball_velocity_y = 2;

    int checkCollision(SDL_Rect rectangle, SDL_Rect carre)
    {
        return rectangle.x < carre.x + carre.w &&
               rectangle.x + rectangle.w > carre.x &&
               rectangle.y < carre.y + carre.h &&
               rectangle.y + rectangle.h > carre.y;
    }

    int player1_score = 0;
    int player2_score = 0;

    SDL_Surface *scoreSurfacePlayer1 = NULL;
    SDL_Texture *scoreTexturePlayer1 = NULL;

    SDL_Surface *scoreSurfacePlayer2 = NULL;
    SDL_Texture *scoreTexturePlayer2 = NULL;

    SDL_Color ballColor = {255, 255, 255, 255}; // Couleur initiale de la balle

    while (!quit)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = 1;
                puts("Fin du programme.");
                break;
            }
        }

        const uint8_t *keystates = SDL_GetKeyboardState(NULL);

        if (keystates[SDL_SCANCODE_Q] && rectangle.y > 0)
            rectangle.y -= 7;
        else if (keystates[SDL_SCANCODE_A] && rectangle.y < 700)
            rectangle.y += 7;
        if (keystates[SDL_SCANCODE_L] && carre.y > 0)
            carre.y -= 7;
        else if (keystates[SDL_SCANCODE_Y] && carre.y < 700)
            carre.y += 7;

        if (balle.x <= 0)
        {
            player2_score++;
            balle.x = 400;
            balle.y = 400;
            ball_velocity_x = 2;
            ball_velocity_y = 2;
            ballColor.r = rand() % 256;
            ballColor.g = rand() % 256;
            ballColor.b = rand() % 256;
            Mix_PlayMusic(bounceSound, 0); // Jouer la musique lors de la collision
        }
        else if (balle.x >= 800 - balle.w)
        {
            player1_score++;
            balle.x = 400;
            balle.y = 400;
            ball_velocity_x = -2;
            ball_velocity_y = 2;
            ballColor.r = rand() % 256;
            ballColor.g = rand() % 256;
            ballColor.b = rand() % 256;
            Mix_PlayMusic(bounceSound, 0); // Jouer la musique lors de la collision
        }

        if (checkCollision(balle, rectangle))
        {
            ball_velocity_x = -ball_velocity_x;
            ball_velocity_x *= 1.5;
        }

        if (checkCollision(balle, carre))
        {
            ball_velocity_x = -ball_velocity_x;
            ball_velocity_x *= 1.5;
        }

        balle.x += ball_velocity_x;
        balle.y += ball_velocity_y;

        if (balle.x <= 0 || balle.x >= 800 - balle.w)
        {
            ball_velocity_x = -ball_velocity_x;
        }

        if (balle.y <= 0 || balle.y >= 800 - balle.h)
        {
            ball_velocity_y = -ball_velocity_y;
        }

        clear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_SetRenderDrawColor(renderer, ballColor.r, ballColor.g, ballColor.b, 255);
        draw(renderer, &balle);

        draw(renderer, &rectangle);
        draw(renderer, &carre);
        draw(renderer, &losage);

        SDL_Color textColor = {255, 255, 255, 255};
        char scoreText[20];
        sprintf(scoreText, "ETHAN");
        SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
        SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = {370, 20, scoreSurface->w, scoreSurface->h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_DestroyTexture(scoreTexture);
        SDL_FreeSurface(scoreSurface);

        char scoreTextPlayer1[20];
        sprintf(scoreTextPlayer1, "Player 1: %d", player1_score);
        scoreSurfacePlayer1 = TTF_RenderText_Solid(font, scoreTextPlayer1, textColor);
        scoreTexturePlayer1 = SDL_CreateTextureFromSurface(renderer, scoreSurfacePlayer1);

        char scoreTextPlayer2[20];
        sprintf(scoreTextPlayer2, "Player 2: %d", player2_score);
        scoreSurfacePlayer2 = TTF_RenderText_Solid(font, scoreTextPlayer2, textColor);
        scoreTexturePlayer2 = SDL_CreateTextureFromSurface(renderer, scoreSurfacePlayer2);

        SDL_Rect scoreRectPlayer1 = {100, 20, scoreSurfacePlayer1->w, scoreSurfacePlayer1->h};
        SDL_Rect scoreRectPlayer2 = {700 - 20 - scoreSurfacePlayer2->w, 20, scoreSurfacePlayer2->w, scoreSurfacePlayer2->h};

        SDL_RenderCopy(renderer, scoreTexturePlayer1, NULL, &scoreRectPlayer1);
        SDL_RenderCopy(renderer, scoreTexturePlayer2, NULL, &scoreRectPlayer2);

        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(scoreTexturePlayer1);
        SDL_FreeSurface(scoreSurfacePlayer1);

        SDL_DestroyTexture(scoreTexturePlayer2);
        SDL_FreeSurface(scoreSurfacePlayer2);

        SDL_Delay(DELAY_TIME);
    }

    // Libération des ressources SDL_mixer
    Mix_FreeMusic(bounceSound);
    Mix_CloseAudio();

    printf("Resultat final :\n");
    printf("Player 1: %d | Player 2: %d\n", player1_score, player2_score);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
