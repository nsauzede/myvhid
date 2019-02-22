#include <stdlib.h>
#include <stdio.h>

#ifdef SDL
#include <SDL.h>
#else
#include <string.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

// -lX11 -lXtst
//libx11-dev libxcb-xtest0-dev (i think)
#endif

void *myvhid_init() {
	void *opaque = 0;
#ifdef WIN32
	setbuf(stdout, 0);
#endif
#ifdef SDL
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
#else
	Display *display = XOpenDisplay(0);
	if(!display){
		printf("XOpenDisplay failed !\n");
		exit(1);
	}
	opaque = display;
#endif
	return opaque;
}

void myvhid_close(void *opaque) {
#ifdef SDL
#else
	Display *display = opaque;
	//close display
	XCloseDisplay(display);
#endif
}

int myvhid_delayms(void *opaque, int ms) {
#ifdef SDL
	SDL_Delay(ms);
#else
	struct timespec tv;
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = ms / 1000;
	tv.tv_nsec = 1000000 * (ms - tv.tv_sec * 1000);
	nanosleep(&tv, 0);
#endif
	return 0;
}

int myvhid_read_mouse(void *opaque, int *x, int *y, int *buttons) {
	int result = 0;
#ifdef SDL
	Uint32 _buttons = SDL_GetGlobalMouseState(x, y);
	*buttons = _buttons;
#else
	Display *display = opaque;
	//actual movement
	int ret = XTestFakeMotionEvent(display, -1, *x, *y, 0);
	printf("ret=%d\n");
	*buttons = 0; // ??
#endif
	return result;
}

int main(int argc, char *argv[]) {
	void *ctx = myvhid_init();
	while (1) {
		int x = -1, y = -1, buttons = -1;
		myvhid_read_mouse(ctx, &x, &y, &buttons);
		printf("x=%d y=%d b=%d\n", x, y, buttons);
		myvhid_delayms(ctx, 100);
	}

	return 0;
}
