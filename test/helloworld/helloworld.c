/*
* https://www.badprog.com/c-sdl-simple-directmedia-layer-hello-world
*/

#include <SDL.h>

static const unsigned short buffer_size = 256;

int main(int argc, char *argv[]) {
    int gogogo = 1;
    SDL_Event event;

    char text_buffer[buffer_size];
  	memset(text_buffer, 0, buffer_size);

    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
      sprintf(text_buffer, "Couldn't initialize SDL: %s", SDL_GetError());
      SDL_SetError(text_buffer);
      for(;;);
    }

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
