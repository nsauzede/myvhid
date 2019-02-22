#include <stdio.h>
#include <stdlib.h>

#ifdef SDL
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef SDL
#else
void myvhid_mouseMove(int x, int y)
{
	Display *display = XOpenDisplay(0);

	if(display == 0) {
		printf("XOpenDisplay failed !\n");
		exit(1);
	}
	XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
	XCloseDisplay(display);
}
#endif

int main() {
	myvhid_mouseMove(128, 128);
	return 0;
}
