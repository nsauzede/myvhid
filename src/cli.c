#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <SDL.h>

int main(int argc, char *argv[]) {
	int port = 10001;
	char *host = "127.0.0.1";
	int n;
	int s;
	if (argc > 1) {
		host = argv[1];
		if (argc > 2) {
			sscanf(argv[2], "%d", &port);
		}
	}
	SSL_library_init();
	SSL_load_error_strings();
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		perror("socket");
		exit(1);
	}
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(host);
	printf("connecting on port %d..\n", port);
	if (connect(s, (struct sockaddr *)&sa, sizeof(sa))) {
		perror("connect");
		exit(1);
	}
	SSL_CTX *ssl_ctx = 0;
	SSL *ssl = 0;
	ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	if (!ssl_ctx) {
		printf("couldn't create server ctx\n");
		exit(1);
	}
	ssl = SSL_new(ssl_ctx);
	if (!ssl) {
		printf("couldn't create server ssl\n");
		exit(1);
	}
	if (!SSL_set_fd(ssl, s)) {
		printf("failed to SSL set fd\n");
		exit(1);
	}
	printf("SSL connecting..\n");
	n = SSL_connect(ssl);
	if (n != 1) {
		unsigned long err = ERR_get_error();
		printf("failed to SSL connect (%d, %d) (%ld, %s)\n", n, SSL_get_error(ssl, n), err, ERR_error_string(err, NULL));
		exit(1);
	}
	char buf[1024];
	printf("ssl initiated\n");
	n = snprintf(buf, sizeof(buf), "hello ssl\n");
	n = SSL_read(ssl, buf, sizeof(buf));
	if (n > sizeof(buf) - 1)
		n = sizeof(buf) - 1;
	printf("SSL_read %d bytes : %s\n", n, buf);

	int w = 320;
	int h = 200;
	int bpp = 32;
	SDL_Surface *screen = 0;
	SDL_Window *sdlWindow = 0;
	SDL_Renderer *sdlRenderer = 0;
	SDL_Texture *sdlTexture = 0;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(w, h, 0, &sdlWindow, &sdlRenderer);
	screen = SDL_CreateRGBSurface(0, w, h, bpp,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!screen) {
		printf("failed to init SDL\n");
		exit(1);
	}
	atexit(SDL_Quit);
	SDL_bool grab = SDL_FALSE;
	int quit = 0;
	while (!quit) {
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
					SDL_SetRelativeMouseMode(grab);
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				int x, y;
				int xrel, yrel;
				x = event.motion.x;
				y = event.motion.y;
				xrel = event.motion.xrel;
				yrel = event.motion.yrel;
				static int count = 0;
				if (!grab) {
					grab = SDL_TRUE;
					SDL_SetRelativeMouseMode(grab);
				} else {
					printf("mouse motion @%d:%d +%d:%d #%d\n", x, y, xrel, yrel, count++);
					int coord[2];
					coord[0] = xrel;
					coord[1] = yrel;
					n = SSL_write(ssl, coord, sizeof(coord));
					if (n != sizeof(coord)) {
						printf("ssl write failed\n");
						exit(1);
					}
				}
			}
		}
		if (quit)
			break;
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = w;
		rect.h = h;
		Uint32 col = SDL_MapRGB(screen->format, 128, 0, 0);
		SDL_FillRect(screen, &rect, col);
		SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
		SDL_Delay(20);
	}

	SSL_shutdown(ssl);
	SSL_CTX_free(ssl_ctx);
	return 0;
}
