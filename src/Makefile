TARGET:=
TARGET+=wmouse
TARGET+=rmouse
TARGET+=rmousesdl
TARGET+=cli
TARGET+=ser

SDL:=1
ifdef SDL
CFLAGS+=-DSDL `sdl2-config --cflags`
LDLIBS+=`sdl2-config --libs`
endif

all: $(TARGET)

ser: LDLIBS+=-lssl -lcrypto
cli: LDLIBS+=-lssl -lcrypto

key.pem:
	openssl genrsa > $@

certreq.pem: key.pem
	openssl req -new -key key.pem > $@

cert.pem: key.pem certreq.pem
	openssl x509 -req -days 365 -in certreq.pem -signkey key.pem -out $@

clean:
	$(RM) $(TARGET)
