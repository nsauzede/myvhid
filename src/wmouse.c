#include <unistd.h>	// usleep
#include <stdio.h>
#include <stdlib.h>

#ifdef SDL
#include <SDL.h>
#else
#ifdef WIN32
#define WINVER 0x500
#include<windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#endif

void myvhid_init() {
#ifdef SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	atexit(SDL_Quit);
#endif
}

void myvhid_getDims(int *_maxx, int *_maxy) {
#ifdef SDL
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		exit(1);
	}
	*_maxx = dm.w;
	*_maxy = dm.h;
#else
#ifdef WIN32
	int maxX = GetSystemMetrics(SM_CXSCREEN), maxY = GetSystemMetrics(SM_CYSCREEN);
	double factorX = 65536.0 / maxX,factorY = 65536.0 / maxY;
	*_maxx = maxX;
	*_maxy = maxY;
#endif
#endif
}

void myvhid_mouseMove(int x, int y) {
#ifdef SDL
	SDL_WarpMouseGlobal(x, y);
#else
#ifdef WIN32
	int maxX = GetSystemMetrics(SM_CXSCREEN), maxY = GetSystemMetrics(SM_CYSCREEN);
	double factorX = 65536.0 / maxX,factorY = 65536.0 / maxY;
	INPUT ip;
	ZeroMemory(&ip,sizeof(ip));
	ip.type = INPUT_MOUSE;
	ip.mi.mouseData = 0;
	ip.mi.dx = x * factorX;
	ip.mi.dy = y * factorY;
	ip.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1,&ip,sizeof(ip));
#else
	Display *display = XOpenDisplay(0);

	if(display == 0) {
		printf("XOpenDisplay failed !\n");
		exit(1);
	}
	XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
	XCloseDisplay(display);
#endif
#endif
}

void myvhid_msleep(int ms) {
#ifdef SDL
	SDL_Delay(ms);
#else
	usleep(ms * 1000);
#endif
}

int main(int argc, char *argv[]) {
	myvhid_init();
	int maxX, maxY;
	myvhid_getDims(&maxX, &maxY);
	int x = maxX/2, y = maxY/2;
	while(x > 5 || y < maxY-5) {
		if(x>5)
			x-=1;
		if(y<maxY-5)
			y+=1;
		myvhid_mouseMove(x, y);
		myvhid_msleep(10);
	}
	return 0;
}
