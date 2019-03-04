#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

int main(int argc, char *argv[]) {
#ifdef SDL1
#define SDLV 1
#else
#define SDLV 2
#endif
	printf("hello SDL %d\n", SDLV);
	int w = 320;
	int h = 200;
	int bpp = 32;
	SDL_Surface *screen = 0;
#ifdef SDL1
#else
	SDL_Window *sdlWindow = 0;
	SDL_Renderer *sdlRenderer = 0;
	SDL_Texture *sdlTexture = 0;
#endif

	SDL_Init(SDL_INIT_VIDEO);
#ifdef SDL1
	screen = SDL_SetVideoMode(w, h, bpp, 0);
#else
	SDL_CreateWindowAndRenderer(w, h, 0, &sdlWindow, &sdlRenderer);
	screen = SDL_CreateRGBSurface(0, w, h, bpp,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);
	int maxw = dm.w;
	int maxh = dm.h;
#endif
	if (!screen) {
		printf("failed to init SDL\n");
		exit(1);
	}
	atexit(SDL_Quit);
	SDL_bool grab = SDL_FALSE;
	int quit = 0;
	while (!quit) {
		static int last_x = -1, last_y = -1;
		int x = last_x, y = last_y;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
				break;
			}
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = 1;
					break;
				}
				if (event.key.keysym.sym == SDLK_SPACE) {
					grab = !grab;
#if 1
					SDL_SetRelativeMouseMode(grab);
#else
					SDL_ShowCursor(!grab);
					SDL_SetWindowGrab(sdlWindow, grab);
#endif
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				x = event.motion.x;
				y = event.motion.y;
				int xrel, yrel;
				xrel = event.motion.xrel;
				yrel = event.motion.yrel;
				static int count = 0;
				if (!grab) {
					grab = SDL_TRUE;
					SDL_SetRelativeMouseMode(grab);
				} else {
					printf("mouse motion @%d:%d +%d:%d #%d\n", x, y, xrel, yrel, count++);
//					printf("%d,%d #%d\n", xrel, yrel, count++);
					int xw = (float)maxw * xrel / 65535;
					int yw = (float)maxh * yrel / 65535;
					SDL_SetWindowPosition(sdlWindow, xw, yw);
				}
			}
		}
		if (quit)
			break;
		if (x != last_x || y != last_y) {
//		if (x != 0 || y != 0) {
//			printf("mouse pos %d %d\n", x, y);
			last_x = x;
			last_y = y;
		}
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = w;
		rect.h = h;
		Uint32 col = SDL_MapRGB(screen->format, 128, 0, 0);
		SDL_FillRect(screen, &rect, col);
#ifdef SDL1
		SDL_UpdateRect(screen, 0, 0, 0, 0);
#else
		SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
#endif
		SDL_Delay(20);
	}
	printf("bye\n");

	return 0;
}
