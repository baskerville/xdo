VERSION = 0.1

CC       = gcc
LIBS     = -lm -lxcb -lxcb-icccm -lxcb-ewmh -lxcb-xtest
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -I$(PREFIX)/include
CFLAGS  += -D_POSIX_C_SOURCE=200112L -DVERSION=\"$(VERSION)\"
LDFLAGS  = -L$(PREFIX)/lib

PREFIX    ?= /usr/local
BINPREFIX  = $(PREFIX)/bin
MANPREFIX = $(PREFIX)/share/man

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
	mkdir -p "$(DESTDIR)$(MANPREFIX)/man1"
	cp -p xdo.1 "$(DESTDIR)$(MANPREFIX)/man1"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)/xdo"
	rm -f $(DESTDIR)$(MANPREFIX)/man1/xdo.1

doc:
	pandoc -t json doc/README | runhaskell doc/capitalize_headers.hs | pandoc -f json -t man --template doc/xdo.1.template -o xdo.1
	pandoc -f markdown -t rst doc/README -o README.rst
	patch -p 1 -i doc/missed_emph.patch

clean:
	rm -f $(OBJ) xdo

.PHONY: all debug install uninstall doc clean
