DESTDIR ?= /usr/local

CPPFLAGS += -MMD

SRCS = bfind.c

.PHONY: clean depclean install

all: bfind

bfind: $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS:.c=.o) $(LIBS)

clean: depclean
	- rm -f $(SRCS:.c=.o)

depclean:
	- rm -f $(SRCS:.c=.d)

install: bfind
	install -m 0755 bfind $(DESTDIR)/bin

-include $(SRCS:.c=.d)
