/*
* https://www.badprog.com/c-sdl-simple-directmedia-layer-hello-world
*/

#include <SDL.h>

int main(int argc, char *argv[]) {
    int gogogo = 1;
    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_WM_SetCaption("Hello World! :D", NULL);
    SDL_SetVideoMode(320, 240, 8, SDL_HWSURFACE);
    SDL_ShowCursor(0);

    while (gogogo) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            gogogo = 0;
    }
    SDL_Quit();
    return 0;
}
