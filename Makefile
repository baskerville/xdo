VERSION = 0.1

CC       = gcc
LIBS     = -lm -lxcb -lxcb-icccm -lxcb-ewmh -lxcb-xtest
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -I$(PREFIX)/include
CFLAGS  += -D_POSIX_C_SOURCE=200112L -DVERSION=\"$(VERSION)\"
LDFLAGS  = -L$(PREFIX)/lib

PREFIX    ?= /usr/local
BINPREFIX  = $(PREFIX)/bin

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: CFLAGS += -Os
all: LDFLAGS += -s
all: xdo

debug: CFLAGS += -O0 -g -DDEBUG
debug: xdo

include Sourcedeps

$(OBJ): Makefile

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

xdo: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

install:
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p xdo "$(DESTDIR)$(BINPREFIX)"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)/xdo"

clean:
	rm -f $(OBJ) xdo

.PHONY: all debug clean install uninstall
