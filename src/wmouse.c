#include <stdio.h>
#include <stdlib.h>

#ifdef SDL
#include <SDL.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

void myvhid_mouseMove(int x, int y) {
#ifdef SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WarpMouseGlobal(x, y);
	SDL_Delay(1000);
	SDL_Quit();
#else
	Display *display = XOpenDisplay(0);

	if(display == 0) {
		printf("XOpenDisplay failed !\n");
		exit(1);
	}
	XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
	XCloseDisplay(display);
#endif
}

int main(int argc, char *argv[]) {
	myvhid_mouseMove(128, 128);
	return 0;
}
