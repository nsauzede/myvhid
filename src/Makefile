TARGET:=
TARGET+=wmouse
TARGET+=rmouse
TARGET+=rmousesdl

SDL:=1
ifdef SDL
CFLAGS+=-DSDL `sdl2-config --cflags`
LDLIBS+=`sdl2-config --libs`
endif

all: $(TARGET)

clean:
	$(RM) $(TARGET)
